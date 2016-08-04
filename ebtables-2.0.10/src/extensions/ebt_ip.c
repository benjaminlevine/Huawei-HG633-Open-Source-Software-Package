/* ebt_ip
 * 
 * Authors:
 * Bart De Schuymer <bdschuym@pandora.be>
 *
 * Changes:
 *    added ip-sport and ip-dport; parsing of port arguments is
 *    based on code from iptables-1.2.7a
 *    Innominate Security Technologies AG <mhopf@innominate.com>
 *    September, 2002
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <netdb.h>
#include "../include/ebtables_u.h"
#include "atpconfig.h"
#include <linux/netfilter_bridge/ebt_ip.h>

#define IP_SOURCE '1'
#define IP_DEST   '2'
#define IP_myTOS  '3' /* include/bits/in.h seems to already define IP_TOS */
#define IP_PROTO  '4'
#define IP_SPORT  '5'
#define IP_DPORT  '6'
#define IP_8021P  '7'
#define IP_8021Q  '8'
#ifdef BRIDGE_EBT_IP_IPRANGE   
#define IP_SRANGE '9'
#define IP_DRANGE 'a'
#endif

static struct option opts[] =
{
	{ "ip-source"           , required_argument, 0, IP_SOURCE },
	{ "ip-src"              , required_argument, 0, IP_SOURCE },
	{ "ip-destination"      , required_argument, 0, IP_DEST   },
	{ "ip-dst"              , required_argument, 0, IP_DEST   },
	{ "ip-tos"              , required_argument, 0, IP_myTOS  },
	{ "ip-protocol"         , required_argument, 0, IP_PROTO  },
	{ "ip-proto"            , required_argument, 0, IP_PROTO  },
	{ "ip-source-port"      , required_argument, 0, IP_SPORT  },
	{ "ip-sport"            , required_argument, 0, IP_SPORT  },
	{ "ip-destination-port" , required_argument, 0, IP_DPORT  },
	{ "ip-dport"            , required_argument, 0, IP_DPORT  },
	{ "ip-vlanprio"         , required_argument, 0, IP_8021P  },
	{ "ip-vlanid"           , required_argument, 0, IP_8021Q  },
#ifdef BRIDGE_EBT_IP_IPRANGE   	
    { "ip-srcrange"         , required_argument, 0, IP_SRANGE },
    { "ip-dstrange"         , required_argument, 0, IP_DRANGE },
#endif	
	{ 0 }
};

/* put the ip string into 4 bytes */
static int undot_ip(char *ip, unsigned char *ip2)
{
	char *p, *q, *end;
	long int onebyte;
	int i;
	char buf[20];

	strncpy(buf, ip, sizeof(buf) - 1);

	p = buf;
	for (i = 0; i < 3; i++) {
		if ((q = strchr(p, '.')) == NULL)
			return -1;
		*q = '\0';
		onebyte = strtol(p, &end, 10);
		if (*end != '\0' || onebyte > 255 || onebyte < 0)
			return -1;
		ip2[i] = (unsigned char)onebyte;
		p = q + 1;
	}

	onebyte = strtol(p, &end, 10);
	if (*end != '\0' || onebyte > 255 || onebyte < 0)
		return -1;
	ip2[3] = (unsigned char)onebyte;

	return 0;
}

/* put the mask into 4 bytes */
/* transform a protocol and service name into a port number */
static uint16_t parse_port(const char *protocol, const char *name)
{
	struct servent *service;
	char *end;
	int port;

	port = strtol(name, &end, 10);
	if (*end != '\0') {
		if (protocol && 
		    (service = getservbyname(name, protocol)) != NULL)
			return ntohs(service->s_port);
	}
	else if (port >= 0 || port <= 0xFFFF) {
		return port;
	}
	ebt_print_error("Problem with specified %s port '%s'", 
			protocol?protocol:"", name);
	return 0;
}

static void
parse_port_range(const char *protocol, const char *portstring, uint16_t *ports)
{
	char *buffer;
	char *cp;
	
	buffer = strdup(portstring);
	if ((cp = strchr(buffer, ':')) == NULL)
		ports[0] = ports[1] = parse_port(protocol, buffer);
	else {
		*cp = '\0';
		cp++;
		ports[0] = buffer[0] ? parse_port(protocol, buffer) : 0;
		if (ebt_errormsg[0] != '\0')
			return;
		ports[1] = cp[0] ? parse_port(protocol, cp) : 0xFFFF;
		if (ebt_errormsg[0] != '\0')
			return;
		
		if (ports[0] > ports[1])
			ebt_print_error("Invalid portrange (min > max)");
	}
	free(buffer);
}

static void print_port_range(uint16_t *ports)
{
	if (ports[0] == ports[1])
		printf("%d ", ports[0]);
	else
		printf("%d:%d ", ports[0], ports[1]);
}

static void print_help()
{
	printf(
"ip options:\n"
"--ip-src    [!] address[/mask]: ip source specification\n"
"--ip-dst    [!] address[/mask]: ip destination specification\n"
"--ip-tos    [!] tos           : ip tos specification\n"
"--ip-proto  [!] protocol      : ip protocol specification\n"
"--ip-sport  [!] port[:port]   : tcp/udp source port or port range\n"
"--ip-dport  [!] port[:port]   : tcp/udp destination port or port range\n"
"--ip-vlanprio   vlanprio      : ip vlan 802.1p\n"
"--ip-vlanid     vlanid        : ip vlan 802.1q\n"
#ifdef BRIDGE_EBT_IP_IPRANGE   
"--ip-srcrange   ip-ip         : ip source range\n"
"--ip-dstrange   ip-ip         : ip destination range\n"
#endif
);
}

static void init(struct ebt_entry_match *match)
{
	struct ebt_ip_info *ipinfo = (struct ebt_ip_info *)match->data;

	ipinfo->invflags = 0;
	ipinfo->bitmask = 0;
}

#ifdef BRIDGE_EBT_IP_IPRANGE   
#define OPT_SOURCE 0x0001
#define OPT_DEST   0x0002
#define OPT_TOS    0x0004
#define OPT_PROTO  0x0008
#define OPT_SPORT  0x0010
#define OPT_DPORT  0x0020
#define OPT_8021P  0x0040
#define OPT_8021Q  0x0080
#define OPT_SRANGE 0x0100
#define OPT_DRANGE 0x0200
#else
#define OPT_SOURCE 0x01
#define OPT_DEST   0x02
#define OPT_TOS    0x04
#define OPT_PROTO  0x08
#define OPT_SPORT  0x10
#define OPT_DPORT  0x20
#define OPT_8021P  0x40
#define OPT_8021Q  0x80
#endif
#ifdef BRIDGE_EBT_IP_IPRANGE   
static void
parse_iprange(char *arg, struct ebt_iprange *range)
{
	char *dash;
    uint32_t ip;

	dash = strchr(arg, '-');
	if (dash)
		*dash = '\0';

	if (undot_ip(arg, (unsigned char *)&ip))
		print_error("iprange match: Bad IP address `%s'\n", 
			   arg);
	range->min_ip = htonl(ip);
    printf("line:%d min_ip:%x\n",__LINE__,htonl(range->min_ip));

	if (dash) {
		if (undot_ip(dash+1, (unsigned char *)&ip))
			print_error("iprange match: Bad IP address `%s'\n",
				   dash+1);
		range->max_ip = htonl(ip);
        printf("line:%d max_ip:%x\n",__LINE__,htonl(range->max_ip));
	} else
    {   
		range->max_ip = range->min_ip;
        printf("line:%d max_ip:%x\n",__LINE__,htonl(range->max_ip));
     }
}
#endif

static int parse(int c, char **argv, int argc, const struct ebt_u_entry *entry,
   unsigned int *flags, struct ebt_entry_match **match)
{
	struct ebt_ip_info *ipinfo = (struct ebt_ip_info *)(*match)->data;
	char *end;
	long int i;

	switch (c) {
	case IP_SOURCE:
		ebt_check_option2(flags, OPT_SOURCE);
		ipinfo->bitmask |= EBT_IP_SOURCE;

	case IP_DEST:
		if (c == IP_DEST) {
			ebt_check_option2(flags, OPT_DEST);
			ipinfo->bitmask |= EBT_IP_DEST;
		}
		if (ebt_check_inverse2(optarg)) {
			if (c == IP_SOURCE)
				ipinfo->invflags |= EBT_IP_SOURCE;
			else
				ipinfo->invflags |= EBT_IP_DEST;
		}
		if (c == IP_SOURCE)
			ebt_parse_ip_address(optarg, &ipinfo->saddr, &ipinfo->smsk);
		else
			ebt_parse_ip_address(optarg, &ipinfo->daddr, &ipinfo->dmsk);
		break;

	case IP_SPORT:
	case IP_DPORT:
		if (c == IP_SPORT) {
			ebt_check_option2(flags, OPT_SPORT);
			ipinfo->bitmask |= EBT_IP_SPORT;
			if (ebt_check_inverse2(optarg))
				ipinfo->invflags |= EBT_IP_SPORT;
		} else {
			ebt_check_option2(flags, OPT_DPORT);
			ipinfo->bitmask |= EBT_IP_DPORT;
			if (ebt_check_inverse2(optarg))
				ipinfo->invflags |= EBT_IP_DPORT;
		}
		if (c == IP_SPORT)
			parse_port_range(NULL, optarg, ipinfo->sport);
		else
			parse_port_range(NULL, optarg, ipinfo->dport);
		break;

	case IP_myTOS:
		ebt_check_option2(flags, OPT_TOS);
		if (ebt_check_inverse2(optarg))
			ipinfo->invflags |= EBT_IP_TOS;
		i = strtol(optarg, &end, 16);
		/*Start of modified by f00120964 for qos ipp problem 20120221 */
		//if (i < 0 || i > 255 || *end != '\0')
		if (i < 0 || i > 1023 || *end != '\0')
		/*End of modified by f00120964 for qos ipp problem 20120221 */
			ebt_print_error2("Problem with specified IP tos");
		ipinfo->tos = i;
		ipinfo->bitmask |= EBT_IP_TOS;
		break;

	case IP_PROTO:
		ebt_check_option2(flags, OPT_PROTO);
		if (ebt_check_inverse2(optarg))
			ipinfo->invflags |= EBT_IP_PROTO;
		i = strtoul(optarg, &end, 10);
		if (*end != '\0') {
			struct protoent *pe;

			pe = getprotobyname(optarg);
			if (pe == NULL)
				ebt_print_error("Unknown specified IP protocol - %s", argv[optind - 1]);
			ipinfo->protocol = pe->p_proto;
		} else {
			ipinfo->protocol = (unsigned char) i;
		}
		ipinfo->bitmask |= EBT_IP_PROTO;
		break;
	case IP_8021P:
	case IP_8021Q:
		if (c == IP_8021P) {
			ebt_check_option2(flags, OPT_8021P);
    		i = strtol(argv[optind - 1], &end, 10);
    		if (i < 0 || i > 7 || *end != '\0')
    			print_error("Problem with specified 802.1p");
            ipinfo->vlan_8021p = i;
			ipinfo->bitmask |= EBT_IP_8021P;
		} else {
			ebt_check_option2(flags, OPT_8021Q);
    		i = strtol(argv[optind - 1], &end, 10);
    		if (i < 0 || i > 4094 || *end != '\0')
    			print_error("Problem with specified 802.1q");
            ipinfo->vlan_8021q = i;            
			ipinfo->bitmask |= EBT_IP_8021Q;
		} 
        break;
#ifdef BRIDGE_EBT_IP_IPRANGE   		
	case IP_SRANGE:
	case IP_DRANGE:
		if (c == IP_SRANGE) {
			ebt_check_option2(flags, OPT_SRANGE);
            parse_iprange(optarg,&(ipinfo->src));
			ipinfo->bitmask |= EBT_IP_SRANGE;
		} else {
			ebt_check_option2(flags, OPT_DRANGE);
            parse_iprange(optarg,&(ipinfo->dst));
			ipinfo->bitmask |= EBT_IP_DRANGE;
		} 
        break;
#endif		
	default:
		return 0;
	}
	return 1;
}

static void final_check(const struct ebt_u_entry *entry,
   const struct ebt_entry_match *match, const char *name,
   unsigned int hookmask, unsigned int time)
{
	struct ebt_ip_info *ipinfo = (struct ebt_ip_info *)match->data;

	if (entry->ethproto != ETH_P_IP || entry->invflags & EBT_IPROTO) {
		ebt_print_error("For IP filtering the protocol must be "
		            "specified as IPv4");
	} else if (ipinfo->bitmask & (EBT_IP_SPORT|EBT_IP_DPORT) &&
		(!(ipinfo->bitmask & EBT_IP_PROTO) ||
		ipinfo->invflags & EBT_IP_PROTO ||
		(ipinfo->protocol!=IPPROTO_TCP &&
		 ipinfo->protocol!=IPPROTO_UDP &&
		 ipinfo->protocol!=IPPROTO_SCTP &&
		 ipinfo->protocol!=IPPROTO_DCCP)))
		ebt_print_error("For port filtering the IP protocol must be "
				"either 6 (tcp), 17 (udp), 33 (dccp) or "
				"132 (sctp)");
}

#ifdef BRIDGE_EBT_IP_IPRANGE   
static void
print_iprange(const struct ebt_iprange *range)
{
	const unsigned char *byte_min, *byte_max;

	byte_min = (const unsigned char *) &(range->min_ip);
	byte_max = (const unsigned char *) &(range->max_ip);
	printf("%d.%d.%d.%d-%d.%d.%d.%d ", 
		byte_min[3], byte_min[2], byte_min[1], byte_min[0],
		byte_max[3], byte_max[2], byte_max[1], byte_max[0]);
}
#endif

static void print(const struct ebt_u_entry *entry,
   const struct ebt_entry_match *match)
{
	struct ebt_ip_info *ipinfo = (struct ebt_ip_info *)match->data;
	int j;

	if (ipinfo->bitmask & EBT_IP_SOURCE) {
		printf("--ip-src ");
		if (ipinfo->invflags & EBT_IP_SOURCE)
			printf("! ");
		for (j = 0; j < 4; j++)
			printf("%d%s",((unsigned char *)&ipinfo->saddr)[j],
			   (j == 3) ? "" : ".");
		printf("%s ", ebt_mask_to_dotted(ipinfo->smsk));
	}
	if (ipinfo->bitmask & EBT_IP_DEST) {
		printf("--ip-dst ");
		if (ipinfo->invflags & EBT_IP_DEST)
			printf("! ");
		for (j = 0; j < 4; j++)
			printf("%d%s", ((unsigned char *)&ipinfo->daddr)[j],
			   (j == 3) ? "" : ".");
		printf("%s ", ebt_mask_to_dotted(ipinfo->dmsk));
	}
	if (ipinfo->bitmask & EBT_IP_TOS) {
		printf("--ip-tos ");
		if (ipinfo->invflags & EBT_IP_TOS)
			printf("! ");
		printf("0x%02X ", ipinfo->tos);
	}
	if (ipinfo->bitmask & EBT_IP_PROTO) {
		struct protoent *pe;

		printf("--ip-proto ");
		if (ipinfo->invflags & EBT_IP_PROTO)
			printf("! ");
		pe = getprotobynumber(ipinfo->protocol);
		if (pe == NULL) {
			printf("%d ", ipinfo->protocol);
		} else {
			printf("%s ", pe->p_name);
		}
	}
	if (ipinfo->bitmask & EBT_IP_SPORT) {
		printf("--ip-sport ");
		if (ipinfo->invflags & EBT_IP_SPORT)
			printf("! ");
		print_port_range(ipinfo->sport);
	}
	if (ipinfo->bitmask & EBT_IP_DPORT) {
		printf("--ip-dport ");
		if (ipinfo->invflags & EBT_IP_DPORT)
			printf("! ");
		print_port_range(ipinfo->dport);
	}
	if (ipinfo->bitmask & EBT_IP_8021P) {
		printf("--ip-vlanprio ");
		if (ipinfo->invflags & EBT_IP_8021P)
			printf("! ");
		printf("%d ", ipinfo->vlan_8021p);
	}
	if (ipinfo->bitmask & EBT_IP_8021Q) {
		printf("--ip-vlanid ");
		if (ipinfo->invflags & EBT_IP_8021Q)
			printf("! ");
		printf("%d ", ipinfo->vlan_8021q);
	}
#ifdef BRIDGE_EBT_IP_IPRANGE   	
	if (ipinfo->bitmask & EBT_IP_SRANGE) {
		printf("--ip-srcrange ");
		if (ipinfo->invflags & EBT_IP_SRANGE)
			printf("! ");
		print_iprange(&ipinfo->src);
	}
	if (ipinfo->bitmask & EBT_IP_DRANGE) {
		printf("--ip-dstrange ");
		if (ipinfo->invflags & EBT_IP_DRANGE)
			printf("! ");
		print_iprange(&ipinfo->dst);
	}   
#endif	
}

static int compare(const struct ebt_entry_match *m1,
   const struct ebt_entry_match *m2)
{
	struct ebt_ip_info *ipinfo1 = (struct ebt_ip_info *)m1->data;
	struct ebt_ip_info *ipinfo2 = (struct ebt_ip_info *)m2->data;

	if (ipinfo1->bitmask != ipinfo2->bitmask)
		return 0;
	if (ipinfo1->invflags != ipinfo2->invflags)
		return 0;
	if (ipinfo1->bitmask & EBT_IP_SOURCE) {
		if (ipinfo1->saddr != ipinfo2->saddr)
			return 0;
		if (ipinfo1->smsk != ipinfo2->smsk)
			return 0;
	}
	if (ipinfo1->bitmask & EBT_IP_DEST) {
		if (ipinfo1->daddr != ipinfo2->daddr)
			return 0;
		if (ipinfo1->dmsk != ipinfo2->dmsk)
			return 0;
	}
	if (ipinfo1->bitmask & EBT_IP_TOS) {
		if (ipinfo1->tos != ipinfo2->tos)
			return 0;
	}
	if (ipinfo1->bitmask & EBT_IP_PROTO) {
		if (ipinfo1->protocol != ipinfo2->protocol)
			return 0;
	}
	if (ipinfo1->bitmask & EBT_IP_SPORT) {
		if (ipinfo1->sport[0] != ipinfo2->sport[0] ||
		   ipinfo1->sport[1] != ipinfo2->sport[1])
			return 0;
	}
	if (ipinfo1->bitmask & EBT_IP_DPORT) {
		if (ipinfo1->dport[0] != ipinfo2->dport[0] ||
		   ipinfo1->dport[1] != ipinfo2->dport[1])
			return 0;
	}

	if (ipinfo1->bitmask & EBT_IP_8021P) {
		if (ipinfo1->vlan_8021p != ipinfo2->vlan_8021p)
			return 0;
	}
    
	if (ipinfo1->bitmask & EBT_IP_8021Q) {
		if (ipinfo1->vlan_8021q != ipinfo2->vlan_8021q)
			return 0;
	}
#ifdef BRIDGE_EBT_IP_IPRANGE   	
	if (ipinfo1->bitmask & EBT_IP_SRANGE) {
		if (((ipinfo1->src).min_ip != (ipinfo2->src).min_ip) || 
            ((ipinfo1->src).max_ip != (ipinfo2->src).max_ip))
			return 0;
	}

	if (ipinfo1->bitmask & EBT_IP_DRANGE) {
		if (((ipinfo1->dst).min_ip != (ipinfo2->dst).min_ip) || 
            ((ipinfo1->dst).max_ip != (ipinfo2->dst).max_ip))
			return 0;
	}    
#endif	
	return 1;
}

static struct ebt_u_match ip_match =
{
	.name		= "ip",
	.size		= sizeof(struct ebt_ip_info),
	.help		= print_help,
	.init		= init,
	.parse		= parse,
	.final_check	= final_check,
	.print		= print,
	.compare	= compare,
	.extra_ops	= opts,
};

void _init(void)
{
	ebt_register_match(&ip_match);
}
