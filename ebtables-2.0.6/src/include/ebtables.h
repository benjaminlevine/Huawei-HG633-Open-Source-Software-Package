/*
 *  ebtables
 *
 *	Authors:
 *	Bart De Schuymer		<bdschuym@pandora.be>
 *
 *  ebtables.c,v 2.0, April, 2002
 *
 *  This code is stongly inspired on the iptables code which is
 *  Copyright (C) 1999 Paul `Rusty' Russell & Michael J. Neuling
 */

/* Local copy of the kernel file, needed for Sparc64 support */
#ifndef __LINUX_BRIDGE_EFF_H
#define __LINUX_BRIDGE_EFF_H
#include <linux/if.h>
#include <linux/netfilter_bridge.h>
#include <linux/if_ether.h>

#define EBT_TABLE_MAXNAMELEN 32
#define EBT_CHAIN_MAXNAMELEN EBT_TABLE_MAXNAMELEN
#define EBT_FUNCTION_MAXNAMELEN EBT_TABLE_MAXNAMELEN

/* verdicts >0 are "branches" */
#define EBT_ACCEPT   -1
#define EBT_DROP     -2
#define EBT_CONTINUE -3
#define EBT_RETURN   -4
#define NUM_STANDARD_TARGETS   4

struct ebt_counter
{
	uint64_t pcnt;
	uint64_t bcnt;
};

struct ebt_replace
{
	char name[EBT_TABLE_MAXNAMELEN];
	unsigned int valid_hooks;           /* 该表在哪些hook上可用 */
	/* nr of rules in the table */
	unsigned int nentries;
	/* total size of the entries */
	unsigned int entries_size;
	/* start of the chains */
#ifdef KERNEL_64_USERSPACE_32
	uint64_t hook_entry[NF_BR_NUMHOOKS];
#else
	struct ebt_entries *hook_entry[NF_BR_NUMHOOKS];
#endif
	/* nr of counters userspace expects back */
	unsigned int num_counters;                  /* 该规则的计数器个数，比如匹配的报文字节数等，一般每一个规则一个 */
	/* where the kernel will put the old counters */
#ifdef KERNEL_64_USERSPACE_32
	uint64_t counters;
	uint64_t entries;
#else
	struct ebt_counter *counters;
	char *entries;
#endif
};

struct ebt_entries {
	/* this field is always set to zero
	 * See EBT_ENTRY_OR_ENTRIES.
	 * Must be same size as ebt_entry.bitmask */
	unsigned int distinguisher;
	/* the chain name */
	char name[EBT_CHAIN_MAXNAMELEN];
	/* counter offset for this chain */
	unsigned int counter_offset;
	/* one standard (accept, drop, return) per hook */
	int policy;
	/* nr. of entries */
	unsigned int nentries;
	/* entry list */
	char data[0] __attribute__ ((aligned (__alignof__(struct ebt_replace))));
};

/* used for the bitmask of struct ebt_entry */

/* This is a hack to make a difference between an ebt_entry struct and an
 * ebt_entries struct when traversing the entries from start to end.
 * Using this simplifies the code alot, while still being able to use
 * ebt_entries.
 * Contrary, iptables doesn't use something like ebt_entries and therefore uses
 * different techniques for naming the policy and such. So, iptables doesn't
 * need a hack like this.
 */
#define EBT_ENTRY_OR_ENTRIES 0x01
/* these are the normal masks */
#define EBT_NOPROTO 0x02
#define EBT_802_3 0x04
#define EBT_SOURCEMAC 0x08
#define EBT_DESTMAC 0x10
#define EBT_F_MASK (EBT_NOPROTO | EBT_802_3 | EBT_SOURCEMAC | EBT_DESTMAC \
   | EBT_ENTRY_OR_ENTRIES)

#define EBT_IPROTO 0x01
#define EBT_IIN 0x02
#define EBT_IOUT 0x04
#define EBT_ISOURCE 0x8
#define EBT_IDEST 0x10
#define EBT_ILOGICALIN 0x20
#define EBT_ILOGICALOUT 0x40
#define EBT_INV_MASK (EBT_IPROTO | EBT_IIN | EBT_IOUT | EBT_ILOGICALIN \
   | EBT_ILOGICALOUT | EBT_ISOURCE | EBT_IDEST)

struct ebt_entry_match
{
	union {
		char name[EBT_FUNCTION_MAXNAMELEN];
		struct ebt_match *match;
	} u;
	/* size of data */
	unsigned int match_size;
#ifdef KERNEL_64_USERSPACE_32
	unsigned int pad;
#endif
	unsigned char data[0] __attribute__ ((aligned (__alignof__(struct ebt_replace))));
};

struct ebt_entry_watcher
{
	union {
		char name[EBT_FUNCTION_MAXNAMELEN];
		struct ebt_watcher *watcher;
	} u;
	/* size of data */
	unsigned int watcher_size;
#ifdef KERNEL_64_USERSPACE_32
	unsigned int pad;
#endif
	unsigned char data[0] __attribute__ ((aligned (__alignof__(struct ebt_replace))));
};


/* 表示一个具体的target，注意一个具体的target总是与一个通用的头部连接起来的 */
struct ebt_entry_target
{
	union {
		char name[EBT_FUNCTION_MAXNAMELEN];
		struct ebt_target *target;
	} u;
	/* size of data */
	unsigned int target_size;           /* 该target总的大小,应该包括了通用的target部分 */
#ifdef KERNEL_64_USERSPACE_32
	unsigned int pad;
#endif
	unsigned char data[0] __attribute__ ((aligned (__alignof__(struct ebt_replace))));
};

#define EBT_STANDARD_TARGET "standard"
struct ebt_standard_target
{
	struct ebt_entry_target target;
	int verdict;
#ifdef KERNEL_64_USERSPACE_32
	unsigned int pad;
#endif
};

/* one entry */
struct ebt_entry {
	/* this needs to be the first field */
	unsigned int bitmask;
	unsigned int invflags;
	uint16_t ethproto;
	/* the physical in-dev */
	char in[IFNAMSIZ];
	/* the logical in-dev */
	char logical_in[IFNAMSIZ];
	/* the physical out-dev */
	char out[IFNAMSIZ];
	/* the logical out-dev */
	char logical_out[IFNAMSIZ];
	unsigned char sourcemac[ETH_ALEN];
	unsigned char sourcemsk[ETH_ALEN];
	unsigned char destmac[ETH_ALEN];
	unsigned char destmsk[ETH_ALEN];
	/* sizeof ebt_entry + matches */
	unsigned int watchers_offset;
	/* sizeof ebt_entry + matches + watchers */
	unsigned int target_offset;
	/* sizeof ebt_entry + matches + watchers + target */
	unsigned int next_offset;
	unsigned char elems[0] __attribute__ ((aligned (__alignof__(struct ebt_replace))));
};

/* {g,s}etsockopt numbers */
#define EBT_BASE_CTL            128

#define EBT_SO_SET_ENTRIES      (EBT_BASE_CTL)
#define EBT_SO_SET_COUNTERS     (EBT_SO_SET_ENTRIES+1)
#define EBT_SO_SET_MAX          (EBT_SO_SET_COUNTERS+1)

#define EBT_SO_GET_INFO         (EBT_BASE_CTL)
#define EBT_SO_GET_ENTRIES      (EBT_SO_GET_INFO+1)
#define EBT_SO_GET_INIT_INFO    (EBT_SO_GET_ENTRIES+1)
#define EBT_SO_GET_INIT_ENTRIES (EBT_SO_GET_INIT_INFO+1)
#define EBT_SO_GET_MAX          (EBT_SO_GET_INIT_ENTRIES+1)

/* blatently stolen from ip_tables.h
 * fn returns 0 to continue iteration */
#define EBT_MATCH_ITERATE(e, fn, args...)                   \
({                                                          \
	unsigned int __i;                                   \
	int __ret = 0;                                      \
	struct ebt_entry_match *__match;                    \
	                                                    \
	for (__i = sizeof(struct ebt_entry);                \
	     __i < (e)->watchers_offset;                    \
	     __i += __match->match_size +                   \
	     sizeof(struct ebt_entry_match)) {              \
		__match = (void *)(e) + __i;                \
		                                            \
		__ret = fn(__match , ## args);              \
		if (__ret != 0)                             \
			break;                              \
	}                                                   \
	if (__ret == 0) {                                   \
		if (__i != (e)->watchers_offset)            \
			__ret = -EINVAL;                    \
	}                                                   \
	__ret;                                              \
})

#define EBT_WATCHER_ITERATE(e, fn, args...)                 \
({                                                          \
	unsigned int __i;                                   \
	int __ret = 0;                                      \
	struct ebt_entry_watcher *__watcher;                \
	                                                    \
	for (__i = e->watchers_offset;                      \
	     __i < (e)->target_offset;                      \
	     __i += __watcher->watcher_size +               \
	     sizeof(struct ebt_entry_watcher)) {            \
		__watcher = (void *)(e) + __i;              \
		                                            \
		__ret = fn(__watcher , ## args);            \
		if (__ret != 0)                             \
			break;                              \
	}                                                   \
	if (__ret == 0) {                                   \
		if (__i != (e)->target_offset)              \
			__ret = -EINVAL;                    \
	}                                                   \
	__ret;                                              \
})

#define EBT_ENTRY_ITERATE(entries, size, fn, args...)       \
({                                                          \
	unsigned int __i;                                   \
	int __ret = 0;                                      \
	struct ebt_entry *__entry;                          \
	                                                    \
	for (__i = 0; __i < (size);) {                      \
		__entry = (void *)(entries) + __i;          \
		__ret = fn(__entry , ## args);              \
		if (__ret != 0)                             \
			break;                              \
		if (__entry->bitmask != 0)                  \
			__i += __entry->next_offset;        \
		else                                        \
			__i += sizeof(struct ebt_entries);  \
	}                                                   \
	if (__ret == 0) {                                   \
		if (__i != (size))                          \
			__ret = -EINVAL;                    \
	}                                                   \
	__ret;                                              \
})

#endif
