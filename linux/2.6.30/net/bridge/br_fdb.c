/*
 *	Forwarding database
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/rculist.h>
#include <linux/spinlock.h>
#include <linux/times.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#ifdef CONFIG_ATH_WRAP
#include <linux/netfilter_bridge.h>
#endif
#include <linux/jhash.h>
#include <linux/random.h>
#include <asm/atomic.h>
#include <asm/unaligned.h>
#include "br_private.h"

#if defined(CONFIG_MIPS_BRCM)
#include "br_igmp.h"
#include <linux/blog.h>
#include <linux/bcm_log.h>
#endif
#include "atp_interface.h"

#if defined(CONFIG_MIPS_BRCM)
int (*fdb_check_expired_wl_hook)(unsigned char *addr) = NULL;
#endif

static struct kmem_cache *br_fdb_cache __read_mostly;
static int fdb_insert(struct net_bridge *br, struct net_bridge_port *source,
		      const unsigned char *addr);

static u32 fdb_salt __read_mostly;

int __init br_fdb_init(void)
{
	br_fdb_cache = kmem_cache_create("bridge_fdb_cache",
					 sizeof(struct net_bridge_fdb_entry),
					 0,
					 SLAB_HWCACHE_ALIGN, NULL);
	if (!br_fdb_cache)
		return -ENOMEM;

	get_random_bytes(&fdb_salt, sizeof(fdb_salt));
	return 0;
}

void br_fdb_fini(void)
{
	kmem_cache_destroy(br_fdb_cache);
}


/* if topology_changing then use forward_delay (default 15 sec)
 * otherwise keep longer (default 5 minutes)
 */
static inline unsigned long hold_time(const struct net_bridge *br)
{
#if 1//defined(CONFIG_MIPS_BRCM)
	/* Seems one timer constant in bridge code can serve several different purposes. As we use forward_delay=0,
	if the code left unchanged, every entry in fdb will expire immidately after a topology change and every packet
	will flood the local ports for a period of bridge_max_age. This will result in low throughput after boot up. 
	So we decoulpe this timer from forward_delay. */
	return br->topology_change ? (15*HZ) : br->ageing_time;
#else
	return br->topology_change ? br->forward_delay : br->ageing_time;
#endif
}

static inline int has_expired(const struct net_bridge *br,
				  const struct net_bridge_fdb_entry *fdb)
{
	return !fdb->is_static 
		&& time_before_eq(fdb->ageing_timer + hold_time(br), jiffies);
}

static inline int br_mac_hash(const unsigned char *mac)
{
	/* use 1 byte of OUI cnd 3 bytes of NIC */
	u32 key = get_unaligned((u32 *)(mac + 2));
	return jhash_1word(key, fdb_salt) & (BR_HASH_SIZE - 1);
}

/*START OF MODIFY by y00181549 for HOMEGW-1438 wds wlroaming problem at 2013/9/11*/
#ifdef CONFIG_ATP_HYBRID
void fdb_delete(struct net_bridge *br, struct net_bridge_fdb_entry *f)
#else
static inline void fdb_delete(struct net_bridge *br, 
	                          struct net_bridge_fdb_entry *f)
#endif
{
	br->num_fdb_entries--;

	hlist_del_rcu(&f->hlist);
#if defined(CONFIG_MIPS_BRCM) && defined(CONFIG_BLOG)
	blog_notify(DESTROY_BRIDGEFDB, (void*)f, 0, 0);
#endif
	br_fdb_put(f);
}
#ifdef CONFIG_ATP_HYBRID
EXPORT_SYMBOL(fdb_delete);
#endif
/*END OF MODIFY by y00181549 for HOMEGW-1438 wds wlroaming problem at 2013/9/11*/

void br_fdb_changeaddr(struct net_bridge_port *p, const unsigned char *newaddr)
{
	struct net_bridge *br = p->br;
	int i;
	
	spin_lock_bh(&br->hash_lock);

	/* Search all chains since old address/hash is unknown */
	for (i = 0; i < BR_HASH_SIZE; i++) {
		struct hlist_node *h;
		hlist_for_each(h, &br->hash[i]) {
			struct net_bridge_fdb_entry *f;

			f = hlist_entry(h, struct net_bridge_fdb_entry, hlist);
			if (f->dst == p && f->is_local) {
				/* maybe another port has same hw addr? */
				struct net_bridge_port *op;
				list_for_each_entry(op, &br->port_list, list) {
					if (op != p && 
					    !compare_ether_addr(op->dev->dev_addr,
								f->addr.addr)) {
						f->dst = op;
						goto insert;
					}
				}

				/* delete old one */
				fdb_delete(br, f);
				goto insert;
			}
		}
	}
 insert:
	/* insert new address,  may fail if invalid address or dup. */
	fdb_insert(br, p, newaddr);

	spin_unlock_bh(&br->hash_lock);
}

void br_fdb_cleanup(unsigned long _data)
{
	struct net_bridge *br = (struct net_bridge *)_data;
	unsigned long delay = hold_time(br);
	unsigned long next_timer = jiffies + br->forward_delay;
	int i;

	spin_lock_bh(&br->hash_lock);
	for (i = 0; i < BR_HASH_SIZE; i++) {
		struct net_bridge_fdb_entry *f;
		struct hlist_node *h, *n;

		hlist_for_each_entry_safe(f, h, n, &br->hash[i], hlist) {
			unsigned long this_timer;
			if (f->is_static)
				continue;
			this_timer = f->ageing_timer + delay;
			if (time_before_eq(this_timer, jiffies))
#if defined(CONFIG_MIPS_BRCM)
			{
				if (fdb_check_expired_wl_hook && (fdb_check_expired_wl_hook(f->addr.addr) == 0))
					f->ageing_timer = jiffies;  /* refresh */
				else
					fdb_delete(br, f);
			}
#else
				fdb_delete(br, f);
#endif /* CONFIG_MIPS_BRCM */
			else if (time_before(this_timer, next_timer))
				next_timer = this_timer;
		}
	}
	spin_unlock_bh(&br->hash_lock);

	/* Add HZ/4 to ensure we round the jiffies upwards to be after the next
	 * timer, otherwise we might round down and will have no-op run. */
	mod_timer(&br->gc_timer, round_jiffies(next_timer + HZ/4));
}

/* Completely flush all dynamic entries in forwarding database.*/
void br_fdb_flush(struct net_bridge *br)
{
	int i;

	spin_lock_bh(&br->hash_lock);
	for (i = 0; i < BR_HASH_SIZE; i++) {
		struct net_bridge_fdb_entry *f;
		struct hlist_node *h, *n;
		hlist_for_each_entry_safe(f, h, n, &br->hash[i], hlist) {
			if (!f->is_static)
				fdb_delete(br, f);
		}
	}
	spin_unlock_bh(&br->hash_lock);
}

/* Flush all entries refering to a specific port.
 * if do_all is set also flush static entries
 */
void br_fdb_delete_by_port(struct net_bridge *br,
			   const struct net_bridge_port *p,
			   int do_all)
{
	int i;

	spin_lock_bh(&br->hash_lock);
	for (i = 0; i < BR_HASH_SIZE; i++) {
		struct hlist_node *h, *g;
		
		hlist_for_each_safe(h, g, &br->hash[i]) {
			struct net_bridge_fdb_entry *f
				= hlist_entry(h, struct net_bridge_fdb_entry, hlist);
			if (f->dst != p) 
				continue;

			if (f->is_static && !do_all)
				continue;
			/*
			 * if multiple ports all have the same device address
			 * then when one port is deleted, assign
			 * the local entry to other port
			 */
			if (f->is_local) {
				struct net_bridge_port *op;
				list_for_each_entry(op, &br->port_list, list) {
					if (op != p && 
					    !compare_ether_addr(op->dev->dev_addr,
								f->addr.addr)) {
						f->dst = op;
						goto skip_delete;
					}
				}
			}
			fdb_delete(br, f);
		skip_delete: ;
		}
	}
	spin_unlock_bh(&br->hash_lock);
}

#ifdef CONFIG_WAN_LAN_BIND
/*遍历其它bridge的端口寻找相应的fdb*/
struct net_bridge_fdb_entry *__br_fdb_get_other_bridge(struct net_bridge *br,
					  const unsigned char *addr)
{
    struct hlist_node *h;
	struct net_bridge_fdb_entry *fdb;
    struct net_device *dev;
    struct net_bridge *brtmp;
    
    for_each_netdev(&init_net,dev)
    {
        brtmp = netdev_priv(dev);
        if(IS_BRIDGE_DEV(dev->name) && brtmp != br)
        {           
            hlist_for_each_entry_rcu(fdb, h, &brtmp->hash[br_mac_hash(addr)], hlist) {
        		if (!compare_ether_addr(fdb->addr.addr, addr)) {
        			if (unlikely(has_expired(brtmp, fdb)))
        				break;
        			return fdb;
        		}
        	}
        }
    }

    return NULL;
}

static void __br_fdb_clear_other_bridge(struct net_bridge *br,
					  const unsigned char *addr)
{
    struct hlist_node *h;
    struct net_bridge_fdb_entry *fdb;
    struct net_device *dev;
    struct net_bridge *brtmp;
    
    for_each_netdev(&init_net,dev)
    {
        brtmp = netdev_priv(dev);
        if(IS_BRIDGE_DEV(dev->name) && brtmp != br)
        {           
            spin_lock(&brtmp->hash_lock);
            hlist_for_each_entry_rcu(fdb, h, &brtmp->hash[br_mac_hash(addr)], hlist) 
            {
                if (!compare_ether_addr(fdb->addr.addr, addr)) 
                {
                    fdb_delete(brtmp, fdb);
                    spin_unlock(&brtmp->hash_lock);
                    return;
                }
            }
            spin_unlock(&brtmp->hash_lock);
        }
    }

    return;
}
#endif

/* No locking or refcounting, assumes caller has no preempt (rcu_read_lock) */
struct net_bridge_fdb_entry *__br_fdb_get(struct net_bridge *br,
					  const unsigned char *addr)
{
	struct hlist_node *h;
	struct net_bridge_fdb_entry *fdb;

	hlist_for_each_entry_rcu(fdb, h, &br->hash[br_mac_hash(addr)], hlist) {
		if (!compare_ether_addr(fdb->addr.addr, addr)) {
			if (unlikely(has_expired(br, fdb)))
				break;
			return fdb;
		}
	}

#ifdef CONFIG_WAN_LAN_BIND
    /*遍历其它bridge的端口寻找相应的fdb*/
    return __br_fdb_get_other_bridge(br, addr);
#endif

	return NULL;
}

/* Interface used by ATM hook that keeps a ref count */
struct net_bridge_fdb_entry *br_fdb_get(struct net_bridge *br, 
					unsigned char *addr)
{
	struct net_bridge_fdb_entry *fdb;

	rcu_read_lock();
	fdb = __br_fdb_get(br, addr);
	if (fdb && !atomic_inc_not_zero(&fdb->use_count))
		fdb = NULL;
	rcu_read_unlock();
	return fdb;
}

static void fdb_rcu_free(struct rcu_head *head)
{
	struct net_bridge_fdb_entry *ent
		= container_of(head, struct net_bridge_fdb_entry, rcu);
	kmem_cache_free(br_fdb_cache, ent);
}

/* Set entry up for deletion with RCU  */
void br_fdb_put(struct net_bridge_fdb_entry *ent)
{
	if (atomic_dec_and_test(&ent->use_count))
		call_rcu(&ent->rcu, fdb_rcu_free);
}

/*
 * Fill buffer with forwarding table records in 
 * the API format.
 */
int br_fdb_fillbuf(struct net_bridge *br, void *buf,
		   unsigned long maxnum, unsigned long skip)
{
	struct __fdb_entry *fe = buf;
	int i, num = 0;
	struct hlist_node *h;
	struct net_bridge_fdb_entry *f;

	memset(buf, 0, maxnum*sizeof(struct __fdb_entry));

	rcu_read_lock();
	for (i = 0; i < BR_HASH_SIZE; i++) {
		hlist_for_each_entry_rcu(f, h, &br->hash[i], hlist) {
			if (num >= maxnum)
				goto out;

			if (has_expired(br, f)) 
				continue;

			if (skip) {
				--skip;
				continue;
			}

			/* convert from internal format to API */
			memcpy(fe->mac_addr, f->addr.addr, ETH_ALEN);

			/* due to ABI compat need to split into hi/lo */
			fe->port_no = f->dst->port_no;
			fe->port_hi = f->dst->port_no >> 8;

			fe->is_local = f->is_local;
			if (!f->is_static)
				fe->ageing_timer_value = jiffies_to_clock_t(jiffies - f->ageing_timer);
			++fe;
			++num;
		}
	}

 out:
	rcu_read_unlock();

	return num;
}


#ifdef CONFIG_SUPPORT_ATP
int br_fdb_filllanbuf(struct net_bridge *br, void *buf,
		   unsigned long maxnum, unsigned long skip)
{
	struct __fdb_lan *fe = buf;
	int i, num = 0;
	struct hlist_node *h;
	struct net_bridge_fdb_entry *f;

	memset(buf, 0, maxnum*sizeof(struct __fdb_lan));

	rcu_read_lock();
	for (i = 0; i < BR_HASH_SIZE; i++) {
		hlist_for_each_entry_rcu(f, h, &br->hash[i], hlist) {
			if (num >= maxnum)
				goto out;

			if (has_expired(br, f)) 
				continue;

			if (skip) {
				--skip;
				continue;
			}

			/* convert from internal format to API */
			memcpy(fe->mac_addr, f->addr.addr, ETH_ALEN);
			fe->port_no = f->dst->port_no;
			memcpy(fe->lanname,f->dst->dev->name,16);
			++fe;
			++num;
		}
	}

 out:
	rcu_read_unlock();

	return num;
}

#endif

static inline struct net_bridge_fdb_entry *fdb_find(struct hlist_head *head,
						    const unsigned char *addr)
{
	struct hlist_node *h;
	struct net_bridge_fdb_entry *fdb;

	hlist_for_each_entry_rcu(fdb, h, head, hlist) {
		if (!compare_ether_addr(fdb->addr.addr, addr))
			return fdb;
	}
	return NULL;
}

static struct net_bridge_fdb_entry *fdb_create(struct net_bridge *br, 
					       struct hlist_head *head,
					       struct net_bridge_port *source,
					       const unsigned char *addr,
					       int is_local,
					       int is_static)
{
	struct net_bridge_fdb_entry *fdb;

	/* Open this condition filter to avoid out of memory, d00176003@2012/12/24 */ 
	if(br->num_fdb_entries >= BR_MAX_FDB_ENTRIES)
		return NULL;

#if defined(CONFIG_MIPS_BRCM)
	/* some users want to always flood. */
	if (hold_time(br) == 0 && !is_local && !is_static)
		return NULL;
#endif

	fdb = kmem_cache_alloc(br_fdb_cache, GFP_ATOMIC);
	if (fdb) {
		memcpy(fdb->addr.addr, addr, ETH_ALEN);
		atomic_set(&fdb->use_count, 1);
		hlist_add_head_rcu(&fdb->hlist, head);

		fdb->dst = source;
#ifdef CONFIG_ATH_WRAP
		if(is_local 
			&& (((source->type & PTYPE_MASK) == WRAP_PTYPE_PETH) 
				|| ((source->type & PTYPE_MASK) == WRAP_PTYPE_PVAP))){
			fdb->is_local = 0;
			fdb->is_static = 0;
		}else
#endif
        {
			fdb->is_local = is_local;
#if defined(CONFIG_MIPS_BRCM)
			fdb->is_static = is_static;
#else
			fdb->is_static = is_local;
#endif /* CONFIG_MIPS_BRCM */
        }
		fdb->ageing_timer = jiffies;

		br->num_fdb_entries++;
	}
	return fdb;
}

#if defined(CONFIG_MIPS_BRCM)
static int fdb_adddel_static(struct net_bridge *br,
                             struct net_bridge_port *source,
                             const unsigned char *addr, 
                             int addEntry)
{
	struct hlist_head *head;
	struct net_bridge_fdb_entry *fdb;

	if (!is_valid_ether_addr(addr))
		return -EINVAL;

	head = &br->hash[br_mac_hash(addr)];

	rcu_read_lock();
	fdb = fdb_find(head, addr);
	if (fdb)
	{
		/* if the entry exists and it is not static then we will delete it
		   and then add it back as static. If we are not adding an entry
		   then just delete it */
		if ( (0 == addEntry) || (0 == fdb->is_static) )
		{
			fdb_delete(br, fdb);
		}
	}
	rcu_read_unlock();
   
	if ( 1 == addEntry )
	{
		if (!fdb_create(br, head, source, addr, 0, 1))
			return -ENOMEM;
	}

	return 0;
}
#endif

static int fdb_insert(struct net_bridge *br, struct net_bridge_port *source,
		  const unsigned char *addr)
{
	struct hlist_head *head = &br->hash[br_mac_hash(addr)];
	struct net_bridge_fdb_entry *fdb;

	if (!is_valid_ether_addr(addr))
		return -EINVAL;

	fdb = fdb_find(head, addr);
	if (fdb) {
		/* it is okay to have multiple ports with same
		 * address, just use the first one.
				 */
		if (fdb->is_local)
					return 0;

				printk(KERN_WARNING "%s adding interface with same address "
				       "as a received packet\n",
				       source->dev->name);
		fdb_delete(br, fdb);
			}

	if (!fdb_create(br, head, source, addr, 1, 1))
		return -ENOMEM;

	return 0;
}

int br_fdb_insert(struct net_bridge *br, struct net_bridge_port *source,
		  const unsigned char *addr)
{
	int ret;

	spin_lock_bh(&br->hash_lock);
	ret = fdb_insert(br, source, addr);
	spin_unlock_bh(&br->hash_lock);
	return ret;
}
/*Start: modify by j00127542 for DTS2011111007882, 2011-12-12*/
#if CONFIG_BCM_EXT_SWITCH
extern int port_stp_flag;
#endif
/*End: modify by j00127542 for DTS2011111007882, 2011-12-12*/

void br_fdb_update(struct net_bridge *br, struct net_bridge_port *source,
		   const unsigned char *addr)
{
	struct hlist_head *head = &br->hash[br_mac_hash(addr)];
	struct net_bridge_fdb_entry *fdb;

	/* some users want to always flood. */
	if (hold_time(br) == 0)
		return;

	/* ignore packets unless we are using this port */
	if (!(source->state == BR_STATE_LEARNING ||
	      source->state == BR_STATE_FORWARDING))
		return;

	fdb = fdb_find(head, addr);
	if (likely(fdb)) {
		/* attempt to update an entry for a local interface */
		if (unlikely(fdb->is_local)) {
/*Start: modify by j00127542 for DTS2011111007882, 2011-12-12*/
#if CONFIG_BCM_EXT_SWITCH
			if (net_ratelimit()){
			/*Added by l00135113@20120225 for support 257*/
                #if 0
                port_stp_flag |= (1<<('5' - source->dev->name[5]));
                #endif
				printk(KERN_WARNING "%s: received packet with "
				       " own address as source address\n",
				       source->dev->name);
			}
#else
			if (net_ratelimit())
				printk(KERN_WARNING "%s: received packet with "
				       " own address as source address\n",
				       source->dev->name);
#endif
/*End: modify by j00127542 for DTS2011111007882, 2011-12-12*/
		} else {
#if defined(CONFIG_MIPS_BRCM)
            /* In case of MAC move - let ethernet driver clear switch ARL */
			if (fdb->dst && fdb->dst->port_no != source->port_no) {
				bcmFun_t *ethswClearArlFun;
				/* Get the switch clear ARL function pointer */
				ethswClearArlFun =  bcmFun_get(BCM_FUN_IN_ENET_CLEAR_ARL_ENTRY);
				if ( ethswClearArlFun ) {
					ethswClearArlFun((void*)addr);
				}
#if defined(CONFIG_BLOG)
				/* Also flush the associated entries in accelerators */
				blog_notify(DESTROY_BRIDGEFDB, (void*)fdb, 0, 0);
#endif
			}
#endif /* BRCM_MIPS */
			/* fastpath: update of existing entry */
			fdb->dst = source;
			fdb->ageing_timer = jiffies;
		}
	} 
        else
        {
#ifdef CONFIG_WAN_LAN_BIND
            __br_fdb_clear_other_bridge(br, addr);
#endif /* CONFIG_WAN_LAN_BIND */

            spin_lock(&br->hash_lock);
            if (!fdb_find(head, addr))
            fdb_create(br, head, source, addr, 0, 0);
            /* else  we lose race and someone else inserts
            * it first, don't bother updating
            */
            spin_unlock(&br->hash_lock);
        }
}

#if defined(CONFIG_MIPS_BRCM) && defined(CONFIG_BLOG)
extern void br_fdb_refresh( struct net_bridge_fdb_entry *fdb );
void br_fdb_refresh( struct net_bridge_fdb_entry *fdb )
{
	fdb->ageing_timer = jiffies;
	return;
}
#endif


#if defined(CONFIG_MIPS_BRCM)
int br_fdb_adddel_static(struct net_bridge *br, struct net_bridge_port *source,
                         const unsigned char *addr, int bInsert)
{
	int ret = 0;

	spin_lock_bh(&br->hash_lock);

	ret = fdb_adddel_static(br, source, addr, bInsert);

	spin_unlock_bh(&br->hash_lock);
   
	return ret;
}

EXPORT_SYMBOL(fdb_check_expired_wl_hook);

#endif

#ifdef CONFIG_ATP_HYBRID_GREACCEL
int hi_kenrel_br_get_port(int ifindex, unsigned char *addr)
{
	struct net_bridge *br = NULL;
	struct net_bridge_fdb_entry *dst; 
    struct net_device *dev = NULL; 

    dev = dev_get_by_index(&init_net, ifindex); 
    if (NULL == dev)
    {
        return -1; 
    }

    dev_put(dev); 
    if ((IFF_EBRIDGE & dev->priv_flags) != IFF_EBRIDGE)
    {
        return -1; 
    }
    br = netdev_priv(dev); 
    dst = __br_fdb_get(br, addr); 
    if ((NULL != dst) && (!dst->is_local))
    {
        return dst->dst->dev->ifindex; 
    }
    return -1; 
}
EXPORT_SYMBOL(hi_kenrel_br_get_port); 
#endif
