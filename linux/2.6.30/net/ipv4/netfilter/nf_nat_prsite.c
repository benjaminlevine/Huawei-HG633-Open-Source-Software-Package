/* (C) 2001-2002 Magnus Boden <mb@ozaba.mine.nu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Version: 0.0.7
 *
 * Thu 21 Mar 2002 Harald Welte <laforge@gnumonks.org>
 * 	- Port to newnat API
 *
 * This module currently supports DNAT:
 * iptables -t nat -A PREROUTING -d x.x.x.x -j DNAT --to-dest x.x.x.y
 *
 * and SNAT:
 * iptables -t nat -A POSTROUTING { -j MASQUERADE , -j SNAT --to-source x.x.x.x }
 *
 * It has not been tested with
 * -j SNAT --to-source x.x.x.x-x.x.x.y since I only have one external ip
 * If you do test this please let me know if it works or not.
 *
 */

#include <linux/module.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>

#include <linux/netfilter.h>

#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_expect.h>
#include <net/netfilter/nf_conntrack_ecache.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <net/netfilter/nf_conntrack_tuple.h>
#include <linux/netfilter/nf_conntrack_prsite.h>
#include <net/checksum.h>
#include <net/tcp.h>
#include <net/udp.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("China c60023298 <c60023298@huawei.com>");
MODULE_DESCRIPTION("http NAT helper");
MODULE_ALIAS("nf_nat_http");

struct list_head g_allSite;
struct list_head g_allAddr[AFFINED_ADDR_BUF_MAX];
/*start of HG_maintance by z67625 2007.06.19 HG550V100R002C01B010 A36D03605 */
int g_port_hashkey[AFFINED_ADDR_BUF_MAX];
EXPORT_SYMBOL(g_port_hashkey);

/*end of HG_maintance by z67625 2007.06.19 HG550V100R002C01B010 A36D03605 */
static struct http_response g_response[] = 
{
{HTTP_RSPNS_OK, 200, "OK"},
{HTTP_RSPNS_REDIR_PMNT, 301, "Moved Permanently"},
{HTTP_RSPNS_REDIR_TEMP, 302, "Redirection"},
/*start of HG_maintance by z67625 2007.07.27 HG550V100R002C01B013  A36D03605 */ 
{HTTP_RSPNS_REDIR_UNMODIFIED,304,"Not Modified"},
/*end of HG_maintance by z67625 2007.07.27 HG550V100R002C01B013  A36D03605 */ 
/*start of HG_maintance by c00126165 2009.05.31 ATP V100R003C01 */
{HTTP_RSPNS_REDIR_FORBIDDEN,403,"Forbidden"},
/*end of HG_maintance by c00126165 2009.05.31 ATP V100R003C01 */
{HTTP_RSPNS_BUTT, 0, NULL}
};

#if 0
#define PRSITE_DEBUGP(format, args...) printk(format, ## args)
#else
#define PRSITE_DEBUGP(format, args...)
#endif
extern int (*prsite_add_hook)(char __user *optval);
extern int (*prsite_del_hook)(char __user *optval);
extern int (*prsite_modi_hook)(char __user *optval);

void http_nat_comm_init(void);
struct power_site *http_get_power_site(char *url);
int http_add_power_site(struct power_site *psite);
void http_put_affined_addr(struct affined_addr* paddr);
void http_put_power_site(struct power_site* psite);
void http_dump_affined_addrs(void);
void http_dump_power_sites(void);

/*************** start of 添加静态IP地址，默认Computer。add by c60023298 ***************/

extern int (*prsite_init_hook)(char __user *optval);
extern int (*prsite_dump_hook)(char __user *optval);


struct list_head g_stAllStaticAddr[AFFINED_ADDR_BUF_MAX];
int g_lStaticPortHashKey[AFFINED_ADDR_BUF_MAX];
struct prsite_url_info g_stPrsiteUrlInfo;
unsigned int g_lPortToAddr[AFFINED_ADDR_BUF_MAX];
int lPrsiteDebug = 0;
int lPrsiteSwitch = 0;

#if 0
#define PRSITE_STATIC_DEBUGP(format, args...) do{printk(format, ## args);}while(0)
#else
#define PRSITE_STATIC_DEBUGP(format, args...)
#endif

int prsite_init_info(char __user *optval)
{
    int ret = 0;
    PRSITE_STATIC_DEBUGP("prsite init start...\n");
    memset(&g_stPrsiteUrlInfo, 0, sizeof(struct prsite_url_info));
    if (copy_from_user(&g_stPrsiteUrlInfo, optval, sizeof(struct prsite_url_info))) 
    {
          ret = 1;
          return ret;
    }
    return ret;
}

int prsite_dump(char __user *optval)
{
    unsigned int dump_flag = 0;
    int ret = 1;  
    copy_from_user(&dump_flag, optval, sizeof(unsigned int));
    if(!lPrsiteSwitch)
    {
        lPrsiteDebug = 1;
        lPrsiteSwitch = 1;
    }
    else
    {
        lPrsiteDebug = 0;
        lPrsiteSwitch = 0;
    }
    http_dump_affined_addrs();
    http_dump_power_sites();

    return ret;
}

struct affined_addr* http_get_affined_addr_static(unsigned int addr)
{
    struct affined_addr *paddr = NULL;
    list_for_each_entry_rcu(paddr, &g_stAllStaticAddr[AFFINED_ADDR_HASH_KEY(addr)], list)
    {
        if(addr == paddr->addr)
        {
            atomic_inc(&paddr->refcnt);
            return paddr;
        }
    }
    return NULL;
}

int http_add_affined_addr_static(struct affined_addr *paddr)
{
    if(NULL == paddr)
        return 0;
    list_add_rcu(&paddr->list, &g_stAllStaticAddr[AFFINED_ADDR_HASH_KEY(paddr->addr)]);   
    return 1;
}

struct affined_addr* http_get_affined_addr_by_port_static(unsigned long port_no)
{
    struct affined_addr *paddr = NULL;
    if(g_lStaticPortHashKey[port_no] == AFFINED_ADDR_BUF_MAX)
    {
        PRSITE_STATIC_DEBUGP("prsite static ip port is null:%d\n",(int)port_no);
        return NULL;
    }
    else
    {
        list_for_each_entry_rcu(paddr, &g_stAllStaticAddr[g_lStaticPortHashKey[port_no]], list)
        {
            if(port_no == paddr->port_no)
            {
                atomic_inc(&paddr->refcnt);
                return paddr;
            }
        }
    }
    return NULL;
}

struct affined_addr *http_get_affined_static_port(int port_no,int addr)
{
    struct affined_addr *paddr = NULL;
    int i = 0;
    for(i = 0; i < AFFINED_ADDR_BUF_MAX; i++)
    {
        list_for_each_entry_rcu(paddr, &g_stAllStaticAddr[i], list)
        {
            if((addr == paddr->addr) && paddr->port_no == port_no)
            {
                PRSITE_STATIC_DEBUGP("find same port in prsite list\n");
                return paddr;
            }
        }
    }
    return NULL;
}

/* START of edit  for HG526v3 强制门户 by z00195436 at 2013.1.4*/
int prsite_add_static(struct sk_buff *skb)
{
    int ret = 0;
    struct affined_addr *paddr = NULL;
    struct power_site   *psite = NULL;  
    struct iphdr *iph = ip_hdr(skb);
    unsigned int addr = iph->daddr;
    unsigned int mask = g_stPrsiteUrlInfo.ul_lan_mask;
    
    http_nat_comm_init();

    PRSITE_STATIC_DEBUGP("prsite_add_static add static\n");

    if(NULL == (psite = http_get_power_site(g_stPrsiteUrlInfo.ac_computer_url)))
    {
        psite = kmalloc(sizeof(struct power_site), GFP_ATOMIC);
        if(NULL == psite)
        {
               ret = 1;
               goto out;
        }
        memset(psite, 0, sizeof(struct power_site));
        spin_lock_init(&psite->lock);
        strcpy(psite->url, g_stPrsiteUrlInfo.ac_computer_url);
        psite->addr = 0;
        atomic_set(&psite->refcnt, 0);            
        atomic_set(&psite->dirty, 0);
        http_add_power_site(psite);
        atomic_inc(&psite->refcnt);
    }

    paddr = kmalloc(sizeof(struct affined_addr), GFP_ATOMIC);
    if(NULL == paddr)
    {
        http_put_power_site(psite);
        ret = 1;
        goto out;
    }

    memset(paddr, 0, sizeof(struct affined_addr));
    spin_lock_init(&paddr->lock);   
    init_timer(&paddr->timer);
    paddr->state = AFFINED_ADDR_INIT;
    paddr->idle = 0;
    atomic_set(&paddr->refcnt, 0);            
    atomic_set(&paddr->dirty, 0);
    atomic_inc(&paddr->refcnt);
    paddr->addr = addr;
    paddr->mask = mask;
    paddr->force_to = psite;
    atomic_inc(&psite->refcnt);
    http_add_affined_addr_static(paddr);
    http_put_affined_addr(paddr);
    http_put_power_site(psite);
out:
    return ret;    
}
/* END of edit  for HG526v3 强制门户 by z00195436 at 2013.1.4*/
/***************  end of 添加静态IP地址，默认Computer。add by c60023298  ***************/

void http_nat_comm_init(void)
{
    int i;
    static int inited = 0;

    if(inited)
        return;

    inited = 1;
    
    PRSITE_DEBUGP("HTTP ALG: http comm inited\n");    
    
    INIT_LIST_HEAD(&g_allSite);
    
    for(i = 0; i < AFFINED_ADDR_BUF_MAX; i++)
    {
        INIT_LIST_HEAD(&g_allAddr[i]);
        /*add by heary*/
        INIT_LIST_HEAD(&g_stAllStaticAddr[i]);
        g_port_hashkey[i] = AFFINED_ADDR_BUF_MAX;
        g_lStaticPortHashKey[i] = AFFINED_ADDR_BUF_MAX;
        g_lPortToAddr[i] = 0;
    }

}

struct http_response* http_get_response_define(unsigned int index)
{
    if(index >= HTTP_RSPNS_BUTT)
        return NULL;

    return &g_response[index];
}

void http_dump_power_sites(void)
{
    struct power_site *psite = NULL;

    printk("\nPrsite Url: dump all url");
    list_for_each_entry_rcu(psite, &g_allSite, list)
    {
        printk("\n\t url <%s>", psite->url);
        printk("\n\t url <%u.%u.%u.%u>", NIPQUAD(psite->addr));
        printk("\n");
    }
    printk("\nGlobal Prsite Info:");
    printk("\n\t prsite <%s>",g_stPrsiteUrlInfo.lEnable?"Enable":"Disable");
    printk("\n\t lan_addr <%u.%u.%u.%u>", NIPQUAD(g_stPrsiteUrlInfo.ul_lan_addr));
    printk("\n\t lan_mask <%u.%u.%u.%u>", NIPQUAD(g_stPrsiteUrlInfo.ul_lan_mask));
    printk("\n\t stb_url <%s>", (g_stPrsiteUrlInfo.ac_stb_url));
    printk("\n\t computer_url <%s>", (g_stPrsiteUrlInfo.ac_computer_url));
    printk("\n\t wifi_url <%s>\r\n", (g_stPrsiteUrlInfo.ac_wifi_url));
}

void http_dump_affined_addrs(void)
{
    struct affined_addr *paddr = NULL;
    int i;

    printk("\nPrsite Ip: dump all address");
    for(i = 0; i < AFFINED_ADDR_BUF_MAX; i++)
    {
        list_for_each_entry_rcu(paddr, &g_allAddr[i], list)
        {
            printk("\n\t site url <%s>", paddr->force_to->url);
            printk("\n\t lan ip <%u.%u.%u.%u>", NIPQUAD(paddr->addr));
            printk("\n\t lan mask <%u.%u.%u.%u>", NIPQUAD(paddr->mask));
            printk("\n\t port_no <%lu>", paddr->port_no);
            printk("\n\t prsite status <%s>", ((paddr->state>AFFINED_ADDR_UNFORCE)?"used":"unused"));
            printk("\n\t ip_type <%s>","DHCP");
            printk("\n");
        }
    }

    for(i = 0; i < AFFINED_ADDR_BUF_MAX; i++)
    {
        list_for_each_entry_rcu(paddr, &g_stAllStaticAddr[i], list)
        {
            printk("\n\t site url <%s>", paddr->force_to->url);
            printk("\n\t lan ip <%u.%u.%u.%u>", NIPQUAD(paddr->addr));
            printk("\n\t lan mask <%u.%u.%u.%u>", NIPQUAD(paddr->mask));
            printk("\n\t port_no <%lu>", paddr->port_no);
            printk("\n\t prsite status <%s>", ((paddr->state>AFFINED_ADDR_UNFORCE)?"used":"unused"));
            printk("\n\t ip_type <%s>","Static");
            printk("\n");
        }
    }
}

int http_add_power_site(struct power_site *psite)
{
    if(NULL == psite)
        return 0;
    list_add_rcu(&psite->list, &g_allSite);
    return 1;
    
}

struct power_site* http_get_power_site(char *url)
{
    struct power_site *psite = NULL;
    
    if(NULL == url)
        return NULL;

    list_for_each_entry_rcu(psite, &g_allSite, list)
    {
        if(strcmp(psite->url, url) == 0)
        {
            atomic_inc(&psite->refcnt);
            return psite;
        }
    }

    return NULL;
}

void http_put_power_site(struct power_site *psite )
{
    if(NULL == psite)
        return;

    atomic_dec(&psite->refcnt);

    if(atomic_read(&psite->dirty) && !atomic_read(&psite->refcnt))
    {
        spin_lock_bh(&psite->lock);
        list_del_rcu(&psite->list);
        spin_unlock_bh(&psite->lock);
        
        kfree(psite);
    }
}

void http_update_power_site(char *url, unsigned int addr)
{
    struct power_site *psite = NULL;
    
    if(NULL == url || 0 == addr)
        return;

    if(NULL == (psite = http_get_power_site(url)))
        return;

    spin_lock_bh(&psite->lock);
    psite->addr = addr;
    spin_unlock_bh(&psite->lock);

    http_put_power_site(psite);
}

int http_add_affined_addr(struct affined_addr *paddr)
{
    if(NULL == paddr)
        return 0;
    list_add_rcu(&paddr->list, &g_allAddr[AFFINED_ADDR_HASH_KEY(paddr->addr)]);   
    return 1;
}

struct affined_addr* http_get_affined_addr(unsigned int addr)
{
    struct affined_addr *paddr = NULL;
    list_for_each_entry_rcu(paddr, &g_allAddr[AFFINED_ADDR_HASH_KEY(addr)], list)
    {
        if(addr == paddr->addr)
        {
            atomic_inc(&paddr->refcnt);
            return paddr;
        }
    }
    return NULL;
}

/*start of HG_maintance by z67625 2007.06.19 HG550V100R002C01B010 A36D03605 */
struct affined_addr* http_get_affined_addr_by_port(unsigned long port_no)
{
    struct affined_addr *paddr = NULL;
    if(g_port_hashkey[port_no] == AFFINED_ADDR_BUF_MAX)
    {
        PRSITE_DEBUGP("prsite dhcp ip port is null:%d\n",(int)port_no);
        return NULL;
    }
    else
    {
        list_for_each_entry_rcu(paddr, &g_allAddr[g_port_hashkey[port_no]], list)
        {
            PRSITE_DEBUGP("port_no [%d] == addr [%d]\n", (int)(paddr->port_no), paddr->addr);
            if(port_no == paddr->port_no)
            {
                atomic_inc(&paddr->refcnt);
                return paddr;
            }
        }
    }
    return NULL;
}
/*end of HG_maintance by z67625 2007.06.19 HG550V100R002C01B010 A36D03605 */

void http_put_affined_addr(struct affined_addr *paddr)
{
    if(NULL == paddr)
        return;
    
    atomic_dec(&paddr->refcnt);

    if(atomic_read(&paddr->dirty) && !atomic_read(&paddr->refcnt))
    {
        spin_lock_bh(&paddr->lock);
        http_put_power_site(paddr->force_to);
        paddr->force_to = NULL;
        
        if(AFFINED_ADDR_FORCED == paddr->state)
        {
            del_timer(&paddr->timer);
            paddr->state = AFFINED_ADDR_INIT;
        }
        
        list_del_rcu(&paddr->list);
        spin_unlock_bh(&paddr->lock);
        
        kfree(paddr);
    }
}

int http_isdigit(char *cp)
{
    char *p = NULL;

    if(NULL == cp)
        return 0;

    /*'0' ascii is 0x30, '9' ascii is 0x39*/
    for(p = cp; (0 != *p)&&(*p >= 0x30) && (*p <= 0x39); p++)
    {;}

    if((0 == *p) && (p != cp))
        return 1;
    else
        return 0;
}

int http_atoi(char *cp)
{
    char *tmp = NULL;
    int a, ret;

    if(NULL == cp)
        return 0;
        
    tmp = cp + strlen(cp);

    for(a = 1, ret = 0; tmp != cp; a*=10, tmp--)
    {
        /*'0' ascii is 0x30*/
        ret += (*(tmp - 1) - 0x30) * a;
    }
    
    return ret;
}

void http_tracecheck_timeout(unsigned long ptr)
{
    struct affined_addr *lan_addr = (struct affined_addr*)ptr;
    PRSITE_DEBUGP("http_tracecheck_timeout ======>\n");
    spin_lock_bh(&lan_addr->lock);
    if(lan_addr->idle > HTTP_TRACE_TIMEOUT)
    {
        lan_addr->state = AFFINED_ADDR_UNFORCE;
        del_timer(&lan_addr->timer);
    }
    else
    {
        lan_addr->idle += HTTP_TRACE_CHECK_TIMEOUT;
        mod_timer(&lan_addr->timer, jiffies + HTTP_TRACE_CHECK_TIMEOUT*60*HZ);
    }
    spin_unlock_bh(&lan_addr->lock);
}

/* START of edit  for HG526v3 强制门户 by z00195436 at 2013.1.4*/
int http_set_appdata(struct sk_buff *pskb, void *pdata, int len)
{
    struct iphdr *iph = NULL;
    struct tcphdr *tcph = NULL;
    u_int32_t tcplen;
    u_int32_t datalen;
    struct sk_buff *newskb = NULL;

    PRSITE_DEBUGP("http_set_appdata ======>\n");
 
    if(NULL == (pskb) || NULL == pdata)
    {
        return 0;
    }

    if (skb_shinfo((pskb))->frag_list)
    {
        newskb = skb_copy((pskb), GFP_ATOMIC); 
        if (!newskb)
        {
            return 0;
        }
        kfree_skb((pskb));
        pskb = newskb;
    }
    
    if (((pskb)->end - (pskb)->data) < len)
    {
        struct sk_buff * skb2 = skb_copy_expand(pskb, 0, len, GFP_ATOMIC);
        if (NULL == skb2)
        {
            return 0;
        }
        kfree_skb(pskb);
        pskb = skb2;
    }

    memset(pskb->data + ip_hdr(pskb)->ihl * 4 + sizeof(struct tcphdr),0,
    pskb->len - ip_hdr(pskb)->ihl * 4 - sizeof(struct tcphdr));

    memcpy(pskb->data + ip_hdr(pskb)->ihl * 4 + sizeof(struct tcphdr),pdata,len);

    iph = ip_hdr(pskb);
    tcph = (void *)iph + iph->ihl*4;
    tcph->doff = 5;

    pskb->tail = pskb->data + ip_hdr(pskb)->ihl * 4 + sizeof(struct tcphdr) + len;

    ip_hdr(pskb)->tot_len = htons(ip_hdr(pskb)->ihl * 4 + sizeof(struct tcphdr) + len);

    ip_hdr(pskb)->check = 0;

    ip_hdr(pskb)->check = ip_fast_csum((unsigned char *)ip_hdr(pskb),ip_hdr(pskb)->ihl);

    (pskb)->len = (pskb)->tail - (pskb)->data;

    /* fix checksum information */
    tcplen = (pskb)->len - iph->ihl*4;
    datalen = tcplen - tcph->doff*4;
    (pskb)->csum = csum_partial((char *)tcph + tcph->doff*4, datalen, 0);

    tcph->check = 0;
    tcph->check = tcp_v4_check(tcplen, iph->saddr, iph->daddr,
                   csum_partial((char *)tcph, tcph->doff*4,
                       (pskb)->csum));

    return 1;    
}
/* END of edit  for HG526v3 强制门户 by z00195436 at 2013.1.4*/

int http_get_httphead(struct sk_buff *skb, char *pdata)
{
    struct tcphdr *tcph = NULL;
    struct iphdr *iph = NULL;
    
    if(NULL == skb || NULL == pdata)
        return 0;

    memset(pdata,0,HTTP_PROTO_HEAD_BUF_MAX);
    iph = ip_hdr(skb);
    tcph = (void *)iph + iph->ihl*4;

     PRSITE_DEBUGP("HTTP TcpHdrLength: <%d>\n", tcph->doff * 4);
    /* get tcp header */
    if(0 != skb_copy_bits(skb, iph->ihl*4 + tcph->doff * 4, pdata, HTTP_PROTO_HEAD_BUF_MAX))
        return 0;

    return 1;
}

int http_get_response_code(struct sk_buff *skb, int *code)
{
    char http_data[HTTP_PROTO_HEAD_BUF_MAX] = "";
    char *code_head = NULL;
    char *code_tail = NULL;
    int  ret = 0;
    
    if((NULL == skb) || (NULL == code))
        return ret;

     PRSITE_DEBUGP("http_get_response_code ======>\n");
    if(!http_get_httphead(skb, http_data))
        return ret;
    
    PRSITE_DEBUGP("HTTP ALG: <%s>\n", http_data);
    
    if(strncmp(http_data, HTTP_PROTO_NAME, strlen(HTTP_PROTO_NAME)) != 0)
    {
        PRSITE_DEBUGP("HTTP ALG: http_get_response_code===>1");
        goto local_return;
    }

    /* 'HTTP/1.1 200 OK' */
    if(NULL == (code_head = strchr(http_data, ' ')))
    {
        PRSITE_DEBUGP("HTTP ALG: http_get_response_code===>2");
        goto local_return;
    }

    code_head++;
    if(NULL == (code_tail = strchr(code_head, ' ')))
        goto local_return;
    else
        *code_tail = 0;

    if(http_isdigit(code_head))
    {
        *code = http_atoi(code_head);
        ret = 1;
    }
    PRSITE_DEBUGP("HTTP ALG: http response code <%d>\n", *code);
    
local_return:
    return ret;
}


int http_changeto_newurl(struct sk_buff *pskb, char *url)
{
    char http_data[HTTP_PROTO_HEAD_BUF_MAX + 1]     = "";
    char *httpver_head   = NULL;
    char *httpver_tail   = NULL;
    char http_newdata[HTTP_RESPONSE_BUF_MAX + 1]  = "";
    int  ret = 0;
    
     PRSITE_DEBUGP("http_changeto_newurl ======>\n");
    if(NULL == (pskb) || NULL == url)
        return ret;

    memset(http_data, 0, sizeof(http_data));
    memset(http_newdata, 0, sizeof(http_newdata));
    
    if(!http_get_httphead((pskb), http_data))
        return ret;

    PRSITE_DEBUGP("HTTP ALG: http head<%s>\n", http_data);

    /*
    HTTP/1.1 302 Redirection\r\n
    Content-length: 0\r\n
    Location: http://www.huawei.com/index.html\r\n
    Connection: Close\r\n
    \r\n
    */

    if(NULL == (httpver_head = strstr(http_data, HTTP_PROTO_NAME))
     || NULL == (httpver_tail = strchr(httpver_head, ' ')))
        goto local_return;

    *httpver_tail = 0;
    sprintf(http_newdata, "%s %d %s\r\nContent-length: 0\r\nLocation: %s\r\nConnection: Close\r\n\r\n",
                            httpver_head, 
                            g_response[HTTP_RSPNS_REDIR_TEMP].code, 
                            g_response[HTTP_RSPNS_REDIR_TEMP].desc,
                            url);

    PRSITE_DEBUGP("HTTP ALG: new http response \n%s\n", http_newdata);
    
    ret = http_set_appdata(pskb, http_newdata, strlen(http_newdata));

local_return:
    return ret;        
}

int http_do_redirect(struct sk_buff *pskb)
{
    int ret = 0;
    struct affined_addr *lan_addr = NULL;
     PRSITE_STATIC_DEBUGP("http_do_redirect ======>1\n");

    PRSITE_STATIC_DEBUGP("http_do_redirect ======>dest:<%u.%u.%u.%u>\n", NIPQUAD(ip_hdr(pskb)->daddr));
    if(NULL == (lan_addr = http_get_affined_addr(ip_hdr(pskb)->daddr)))
    {
        PRSITE_STATIC_DEBUGP("http_do_redirect ======>3\n");

        /*添加静态IP地址，默认设备类型为Computer,但是不能为LAN维护地址*/
         if((ip_hdr(pskb)->saddr == g_stPrsiteUrlInfo.ul_lan_addr) 
            || (ip_hdr(pskb)->daddr == g_stPrsiteUrlInfo.ul_lan_addr))
        {
            PRSITE_STATIC_DEBUGP("prsite is lan addr\n");
            return ret;
        }
         PRSITE_STATIC_DEBUGP("http_do_redirect ======>4\n");
        if((ip_hdr(pskb)->daddr & g_stPrsiteUrlInfo.ul_lan_mask) == (g_stPrsiteUrlInfo.ul_lan_addr & g_stPrsiteUrlInfo.ul_lan_mask))
        {
            if(NULL == (lan_addr = http_get_affined_addr_static(ip_hdr(pskb)->daddr)))
            {
                /*添加静态IP地址*/
                if(prsite_add_static((pskb)))
                {
                    /*添加失败*/
                    PRSITE_STATIC_DEBUGP("prsite add static ip is fail\n");
                    return ret;
                }
                PRSITE_STATIC_DEBUGP("prsite add static ip Success!\n");
                /*重新获取静态IP地址信息*/
                if(NULL == (lan_addr = http_get_affined_addr_static(ip_hdr(pskb)->daddr)))
                {
                    PRSITE_STATIC_DEBUGP("get prsite static ip is NULL\n");
                    return ret;
                }
            }
        }
        else
        {
            PRSITE_STATIC_DEBUGP("\n\t prsite ip <%u.%u.%u.%u>", NIPQUAD(ip_hdr(pskb)->daddr));
            return ret;
        }
    } 

    PRSITE_STATIC_DEBUGP("http_do_redirect===>Url:%s\n",lan_addr->force_to->url);
    
    /*the same subnet, pass*/
    if((ip_hdr(pskb)->saddr & lan_addr->mask) == (lan_addr->addr & lan_addr->mask))
    {
        PRSITE_STATIC_DEBUGP("prsite ipaddr is local address(lan)\n");
        goto local_return;
    }
    
    switch(lan_addr->state)
    {
    case AFFINED_ADDR_INIT:
    case AFFINED_ADDR_UNFORCE:
            
        if(http_changeto_newurl((pskb), lan_addr->force_to->url))
        {
            spin_lock_bh(&lan_addr->lock);
            lan_addr->state = AFFINED_ADDR_FORCING;
            spin_unlock_bh(&lan_addr->lock);
        }
        break;
    case AFFINED_ADDR_FORCING:       
        //if(skb->nh.iph->saddr == lan_addr->force_to->addr)
        //{
            spin_lock_bh(&lan_addr->lock);
            lan_addr->idle = 0;
            lan_addr->state = AFFINED_ADDR_FORCED;
            lan_addr->timer.expires = jiffies + HTTP_TRACE_CHECK_TIMEOUT*60*HZ;
            lan_addr->timer.function = http_tracecheck_timeout;
            lan_addr->timer.data = (unsigned long)lan_addr;
            mod_timer(&lan_addr->timer, lan_addr->timer.expires);
            spin_unlock_bh(&lan_addr->lock);
        //}
        //else
        //    http_changeto_newurl(skb, lan_addr->force_to->url);
        
        break;
    case AFFINED_ADDR_FORCED:
        spin_lock_bh(&lan_addr->lock);
        lan_addr->idle = 0;
        spin_unlock_bh(&lan_addr->lock);
        break;
    default:;
    }

local_return:
    PRSITE_STATIC_DEBUGP("Go to Local Return\n");
    http_put_affined_addr(lan_addr);
    return ret;
}

/*增加强制门户节点*/
int prsite_add(char __user *optval)
{
    int ret = 0;
    struct affined_bind bind;
    struct affined_addr *paddr = NULL;
    struct power_site   *psite = NULL;            

     PRSITE_DEBUGP("prsite_add ======>\n");

     if (copy_from_user(&bind, optval, sizeof(struct affined_bind))) 
    {
          ret = 1;
          return ret;
     }
     
     http_nat_comm_init();

      /*modify by heary*/
      bind.mask = g_stPrsiteUrlInfo.ul_lan_mask;
      
      PRSITE_STATIC_DEBUGP("\n\t add ipaddr <%u.%u.%u.%u>", NIPQUAD( bind.addr));
      PRSITE_STATIC_DEBUGP("\n\t add mask <%u.%u.%u.%u>", NIPQUAD( bind.mask));
     
     if(NULL != (paddr = http_get_affined_addr(bind.addr)))
    {
/*start of PC重起也要进行强制门户 for hg522 by c60023298 2008-1-25*/        
        spin_lock_bh(&paddr->lock);    
        paddr->state = AFFINED_ADDR_INIT;    
        spin_unlock_bh(&paddr->lock); 
        http_put_affined_addr(paddr);
/*end of PC重起也要进行强制门户 for hg522 by c60023298 2008-1-25*/
          ret = 1;
        return ret;
    }

     if(NULL == (psite = http_get_power_site(bind.url)))
    {
        psite = kmalloc(sizeof(struct power_site), GFP_ATOMIC);
        if(NULL == psite)
        {
               ret = 1;
               return ret;
        }
        memset(psite, 0, sizeof(struct power_site));
        spin_lock_init(&psite->lock);
        strcpy(psite->url, bind.url);
        psite->addr = 0;
        atomic_set(&psite->refcnt, 0);            
        atomic_set(&psite->dirty, 0);
        http_add_power_site(psite);
        atomic_inc(&psite->refcnt);
    }

    paddr = kmalloc(sizeof(struct affined_addr), GFP_ATOMIC);
    if(NULL == paddr)
    {
        http_put_power_site(psite);
        ret = 1;
          return ret;
    }
    memset(paddr, 0, sizeof(struct affined_addr));
    spin_lock_init(&paddr->lock);   
    init_timer(&paddr->timer);
    paddr->state = AFFINED_ADDR_INIT;
    paddr->idle = 0;
    atomic_set(&paddr->refcnt, 0);            
    atomic_set(&paddr->dirty, 0);
    atomic_inc(&paddr->refcnt);
    paddr->addr = bind.addr;
    paddr->mask = bind.mask;
    paddr->force_to = psite;
    atomic_inc(&psite->refcnt);
    http_add_affined_addr(paddr);
    http_put_affined_addr(paddr);
    http_put_power_site(psite);
    return ret;
}

/*删除强制门户节点*/
int prsite_del(char __user *optval)
{
    int ret = 0;
    struct affined_bind bind;
    struct affined_addr *paddr = NULL;
    struct power_site   *psite = NULL; 

     PRSITE_DEBUGP("prsite_del ======>\n");
     if (copy_from_user(&bind, optval, sizeof(struct affined_bind)))
    {
          ret =1;
          return ret;
     }

     http_nat_comm_init();

    if((0 != bind.addr) && (0 == bind.url[0]))/* del lan dev */
    {
        if(NULL != (paddr = http_get_affined_addr(bind.addr)))
            atomic_set(&paddr->dirty, 1);

        http_put_affined_addr(paddr);
    }            
     else if((0 == bind.addr) && (0 != bind.url[0]))/* del site */
    {
        if(NULL != (psite = http_get_power_site(bind.url)))
            atomic_set(&psite->dirty, 1);

        http_put_power_site(psite);
    }
     else if((0 != bind.addr) && (0 != bind.url[0]))/* del site url and lan dev */
    {

        if(NULL != (paddr = http_get_affined_addr(bind.addr)))
            atomic_set(&paddr->dirty, 1);
        
        if(NULL != (psite = http_get_power_site(bind.url)))
            atomic_set(&psite->dirty, 1);

        http_put_affined_addr(paddr);
        http_put_power_site(psite);
    }
     else
    {
          ret = 1;
          return ret;
    }
    return ret;
}

/*修改强制门户节点*/
int prsite_modi(char __user *optval)
{
    unsigned long port_no;
    int ret = 1;
    struct affined_addr *paddr = NULL;
     PRSITE_DEBUGP("prsite_modi ======>\n");
    if (copy_from_user(&port_no, optval, sizeof(int))) 
    {
          PRSITE_DEBUGP("***copy_from_user Fail!***\n");
          return ret;
    }
    
    http_nat_comm_init();
    PRSITE_DEBUGP("\n\t prsite_modi port no:[%u]\n",port_no);

    if(NULL != (paddr = http_get_affined_addr_by_port(port_no)))
    {
        PRSITE_DEBUGP("prsite modify dhcp ip\n");
        atomic_set(&paddr->dirty, 1);
        atomic_set(&paddr->refcnt, 1);
        http_put_affined_addr(paddr);
        ret = 0;
    }

    /*静态IP地址,add by heary,删除该静态IP地址*/
    if(NULL != (paddr = http_get_affined_addr_by_port_static(port_no)))
    {
        PRSITE_DEBUGP("prsite modify static ip\n");
        atomic_set(&paddr->dirty, 1);
        atomic_set(&paddr->refcnt, 1);
        http_put_affined_addr(paddr);
        ret = 0;
    }
    PRSITE_DEBUGP("prsite_modi return with [%d].\n", ret);
    return ret;
         
}

/*start modified by t00191685 for 强制门户移植根据linux2.6.30内核修改 2011-11-30*/
/*进行强制门户处理*/    
void http_help(struct nf_conn *ct,int dir,unsigned int hooknum,struct sk_buff *pskb)
{    
    int http_code = 0;
    //z67625
     struct http_response *prespok = NULL;
    struct http_response *prespmodi = NULL;
    struct http_response *pforbidden = NULL;
    /*start of HG_maintance by z67625 2007.06.19 HG550V100R002C01B010 A36D03605 */
    struct affined_addr *lan_addr = NULL;
    /*end of HG_maintance by z67625 2007.06.19 HG550V100R002C01B010 A36D03605 */

    /*start of static ip for prsite by c60023298 2007-12-25*/
    struct affined_addr *paddr = NULL;
    /*start of static ip for prsite by c60023298 2007-12-25*/
    struct nf_conntrack_tuple_hash *tuple = &(ct->tuplehash[IP_CT_DIR_ORIGINAL]);
    
    unsigned int ulSrcIpAddress = tuple->tuple.src.u3.ip;

    PRSITE_DEBUGP("http_help ======>1\n");

    if(!g_stPrsiteUrlInfo.lEnable)
    {
        PRSITE_STATIC_DEBUGP("prsite is not enable\n");
        return ;
    }

    //PRSITE_STATIC_DEBUGP("Http_help====><***1***>dir:%d,saddr:<%u.%u.%u.%u>\n", dir,NIPQUAD((pskb)->nh.iph->saddr));
    //PRSITE_STATIC_DEBUGP("Nat Src:<%u.%u.%u.%u>,Nat Dst:<%u.%u.%u.%u>\n",NIPQUAD(ulSrcIpAddress),NIPQUAD(tuple->tuple.dst.u3.ip));
    //PRSITE_STATIC_DEBUGP("Packet Src:<%u.%u.%u.%u>,Nat Dst:<%u.%u.%u.%u>\n",NIPQUAD((pskb)->nh.iph->saddr),NIPQUAD((pskb)->nh.iph->daddr));

//     if (hooknum == NF_IP_POST_ROUTING && dir == IP_CT_DIR_REPLY) 
     if (dir == IP_CT_DIR_REPLY) 
     {
         PRSITE_DEBUGP("http_help ======>2\n");
          if(http_get_response_code(pskb, &http_code))
         {
            PRSITE_DEBUGP("http_help ======>3\n");            
            prespok = http_get_response_define(HTTP_RSPNS_OK);
            prespmodi = http_get_response_define(HTTP_RSPNS_REDIR_UNMODIFIED);
            pforbidden = http_get_response_define(HTTP_RSPNS_REDIR_FORBIDDEN);
            PRSITE_DEBUGP("http_help ======>4\n");  
               if(((NULL != prespok) && (http_code == prespok->code)) 
                    || ((NULL != prespmodi) && (http_code == prespmodi->code))
                    || ((NULL != pforbidden) && (http_code == pforbidden->code)))
               {
                 PRSITE_DEBUGP("http_help ======>5\n");  
                 http_do_redirect(pskb);
               }                 
         }          
     } 
    /*start of HG_maintance by z67625 2007.06.19 HG550V100R002C01B010 A36D03605 */
   else if(NULL != (lan_addr = http_get_affined_addr(ulSrcIpAddress)))
   {
//        if(hooknum == NF_IP_PRE_ROUTING && dir == IP_CT_DIR_ORIGINAL && lan_addr->state == AFFINED_ADDR_FORCING)
         if(dir == IP_CT_DIR_ORIGINAL && lan_addr->state == AFFINED_ADDR_FORCING)
          {
            
            /* START of edit  for HG526v3 强制门户 by z00195436 at 2013.1.4*/
            /*get lanindevice*/
            if (pskb->lanindev)
            {
                PRSITE_DEBUGP("pskb->lanindevice [%s]\n", pskb->lanindev->name);  
                spin_lock_bh(&lan_addr->lock);
                if (0 == (strncmp(pskb->lanindev->name, LAN_PORT_1, strlen(LAN_PORT_1))))
                {
                    lan_addr->port_no = 5;
                }
                else if (0 == (strncmp(pskb->lanindev->name, LAN_PORT_2, strlen(LAN_PORT_2))))
                {
                    lan_addr->port_no = 4;
                }
                else if (0 == (strncmp(pskb->lanindev->name, LAN_PORT_3, strlen(LAN_PORT_3))))
                {
                    lan_addr->port_no = 3;
                }
                else if (0 == (strncmp(pskb->lanindev->name, LAN_PORT_4, strlen(LAN_PORT_4))))
                {
                    lan_addr->port_no = 2;
                }                
                spin_unlock_bh(&lan_addr->lock);
                PRSITE_DEBUGP("g_port_hashkey [%u] [%u]\n", lan_addr->port_no, (ulSrcIpAddress));  
                g_port_hashkey[lan_addr->port_no] = AFFINED_ADDR_HASH_KEY(ulSrcIpAddress);
            }
            else
            {
                printk("nfmark error");
            }
            /* END of edit  for HG526v3 强制门户 by z00195436 at 2013.1.4*/
        }
        http_put_affined_addr(lan_addr);
    }
    /*end of HG_maintance by z67625 2007.06.19 HG550V100R002C01B010 A36D03605 */
   /*modify by heary*/
   else if(NULL != (lan_addr = http_get_affined_addr_static(ulSrcIpAddress)))
   {
//        if(hooknum == NF_IP_PRE_ROUTING && dir == IP_CT_DIR_ORIGINAL && lan_addr->state == AFFINED_ADDR_FORCING)
        if(dir == IP_CT_DIR_ORIGINAL && lan_addr->state == AFFINED_ADDR_FORCING)
        {
            /* START of edit  for HG526v3 强制门户 by z00195436 at 2013.1.4*/
            /*get lanindevice*/
            if (pskb->lanindev)
            {
                PRSITE_DEBUGP("pskb->lanindevice [%s]\n", pskb->lanindev->name);  
                spin_lock_bh(&lan_addr->lock);
                if (0 == (strncmp(pskb->lanindev->name, LAN_PORT_1, strlen(LAN_PORT_1))))
                {
                    lan_addr->port_no = 5;
                }
                else if (0 == (strncmp(pskb->lanindev->name, LAN_PORT_2, strlen(LAN_PORT_2))))
                {
                    lan_addr->port_no = 4;
                }
                else if (0 == (strncmp(pskb->lanindev->name, LAN_PORT_3, strlen(LAN_PORT_3))))
                {
                    lan_addr->port_no = 3;
                }
                else if (0 == (strncmp(pskb->lanindev->name, LAN_PORT_4, strlen(LAN_PORT_4))))
                {
                    lan_addr->port_no = 2;
                }                
                spin_unlock_bh(&lan_addr->lock);
                /*该端口以前就有一个静态IP地址强制上网了。*/
                if(g_lPortToAddr[lan_addr->port_no] != 0 &&
                    g_lPortToAddr[lan_addr->port_no] != ulSrcIpAddress)
                {
                    PRSITE_STATIC_DEBUGP("The same port have two ip addr\n");
                    if(NULL != (paddr = http_get_affined_static_port(lan_addr->port_no,g_lPortToAddr[lan_addr->port_no])))
                    {
                        atomic_set(&paddr->dirty, 1);
                        atomic_set(&paddr->refcnt, 1);
                        http_put_affined_addr(paddr);
                    }
                }                
                g_lStaticPortHashKey[lan_addr->port_no] = AFFINED_ADDR_HASH_KEY(ulSrcIpAddress);
                PRSITE_STATIC_DEBUGP("g_lStaticPortHashKey[%d]: %d\n",(int)(lan_addr->port_no), g_lStaticPortHashKey[lan_addr->port_no]);
                g_lPortToAddr[lan_addr->port_no] = ulSrcIpAddress;
            }
            else
            {
                printk("nfmark error");
            }
            /* END of edit  for HG526v3 强制门户 by z00195436 at 2013.1.4*/
        }
        http_put_affined_addr(lan_addr);
    }
}
/*end modified by t00191685 for 强制门户移植根据linux2.6.30内核修改 2011-11-30*/
static void __exit nf_nat_http_fini(void)
{
    rcu_assign_pointer(prsite_add_hook, NULL);
    rcu_assign_pointer(prsite_del_hook, NULL);
    rcu_assign_pointer(prsite_modi_hook, NULL);
    rcu_assign_pointer(prsite_dump_hook, NULL);
    rcu_assign_pointer(prsite_init_hook, NULL);
    rcu_assign_pointer(http_nat_help_hook, NULL);

    synchronize_rcu();
}

static int __init nf_nat_http_init(void)
{
    PRSITE_DEBUGP("nf_nat_http_init <*********** start ************>\n");

    http_nat_comm_init();
    BUG_ON(rcu_dereference(prsite_add_hook) != NULL);
    BUG_ON(rcu_dereference(prsite_del_hook) != NULL);
    BUG_ON(rcu_dereference(prsite_modi_hook) != NULL);
    BUG_ON(rcu_dereference(prsite_dump_hook) != NULL);
    BUG_ON(rcu_dereference(prsite_init_hook) != NULL);
    BUG_ON(rcu_dereference(http_nat_help_hook) != NULL);

    rcu_assign_pointer(prsite_add_hook, prsite_add);
    rcu_assign_pointer(prsite_del_hook, prsite_del);
    rcu_assign_pointer(prsite_modi_hook, prsite_modi);
    rcu_assign_pointer(prsite_dump_hook, prsite_dump);
    rcu_assign_pointer(prsite_init_hook, prsite_init_info);
    rcu_assign_pointer(http_nat_help_hook, http_help);
    return 0;
}

module_init(nf_nat_http_init);
module_exit(nf_nat_http_fini);

