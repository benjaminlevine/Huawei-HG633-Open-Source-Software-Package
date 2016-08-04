/*
 *  ebt_ip6
 *
 *	Authors:
 * Kuo-Lang Tseng <kuo-lang.tseng@intel.com>
 * Manohar Castelino <manohar.r.castelino@intel.com>
 *
 *  Jan 11, 2008
 *
 */

#ifndef __LINUX_BRIDGE_EBT_IP6_H
#define __LINUX_BRIDGE_EBT_IP6_H

/* BEGIN: Added by h00169677, 2013/4/19   PN:DTS2013031909624*/
#include "atpconfig.h"
/* END:   Added by h00169677, 2013/4/19 */

#define EBT_IP6_SOURCE 0x01
#define EBT_IP6_DEST 0x02
#define EBT_IP6_TCLASS 0x04
#define EBT_IP6_PROTO 0x08
#define EBT_IP6_SPORT 0x10
#define EBT_IP6_DPORT 0x20
#define EBT_IP6_FLOWLBL 0x40  // IPv6 Flow Label

/* BEGIN: Added by h00169677, 2013/4/19   PN:DTS2013031909624*/
#ifdef BRIDGE_EBT_IP_IPRANGE   
#define EBT_IP6_SRANGE 0x0100
#define EBT_IP6_DRANGE 0x0200
#endif
/* END:   Added by h00169677, 2013/4/19 */

/* BEGIN: Modified by h00169677, 2013/4/19   PN:DTS2013031909624*/
#ifdef BRIDGE_EBT_IP_IPRANGE 
#define EBT_IP6_MASK (EBT_IP6_SOURCE | EBT_IP6_DEST | EBT_IP6_TCLASS |\
		      EBT_IP6_PROTO | EBT_IP6_SPORT | EBT_IP6_DPORT | EBT_IP6_FLOWLBL |\
		      EBT_IP6_SRANGE | EBT_IP6_DRANGE)
#else
#define EBT_IP6_MASK (EBT_IP6_SOURCE | EBT_IP6_DEST | EBT_IP6_TCLASS |\
		      EBT_IP6_PROTO | EBT_IP6_SPORT | EBT_IP6_DPORT | EBT_IP6_FLOWLBL)
#endif
/* END:   Modified by h00169677, 2013/4/19 */
		      
#define EBT_IP6_MATCH "ip6"


/* BEGIN: Added by h00169677, 2013/4/19   PN:DTS2013031909624*/
#ifdef BRIDGE_EBT_IP_IPRANGE
struct ebt_ip6range
{
    struct in6_addr min_ip6;
    struct in6_addr max_ip6;
};
#endif
/* END:   Added by h00169677, 2013/4/19 */

/* the same values are used for the invflags */
struct ebt_ip6_info
{
	struct in6_addr saddr;
	struct in6_addr daddr;
	struct in6_addr smsk;
	struct in6_addr dmsk;
	uint8_t  tclass;
	uint8_t  protocol;
/* BEGIN: Modified by h00169677, 2013/4/19   PN:DTS2013031909624*/
#ifdef BRIDGE_EBT_IP_IPRANGE  
	uint16_t  bitmask;
	uint16_t  invflags;
#else
	uint8_t  bitmask;
	uint8_t  invflags;
#endif
/* END:   Modified by h00169677, 2013/4/19 */
	uint16_t sport[2];
	uint16_t dport[2];
    uint32_t flowlbl;  // IPv6 Flow Label
/* BEGIN: Added by h00169677, 2013/4/19   PN:DTS2013031909624*/
#ifdef BRIDGE_EBT_IP_IPRANGE  
    struct ebt_ip6range ip6range_src;
    struct ebt_ip6range ip6range_dst; 
#endif  
/* END:   Added by h00169677, 2013/4/19 */
};

#endif
