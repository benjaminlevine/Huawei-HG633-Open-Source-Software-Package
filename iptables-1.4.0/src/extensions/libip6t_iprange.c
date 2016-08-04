/* Shared library add-on to iptables to add IP range matching support. */
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <ip6tables.h>
#include <linux/netfilter.h>
#include <linux/netfilter/xt_iprange.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/* Function which prints out usage message. */
static void iprange_help(void)
{
	printf(
"iprange match v%s options:\n"
"[!] --src-range ip-ip        Match source IP in the specified range\n"
"[!] --dst-range ip-ip        Match destination IP in the specified range\n"
"\n",
IPTABLES_VERSION);
}

static const struct option iprange_opts[] = {
	{ "src-range", 1, NULL, '1' },
	{ "dst-range", 1, NULL, '2' },
	{0}
};

static struct in6_addr *
numeric_to_addr(const char *num)
{
	static struct in6_addr ap;
	int err;
	if ((err=inet_pton(AF_INET6, num, &ap)) > 0)
		return &ap;
#ifdef DEBUG
	fprintf(stderr, "\nnumeric2addr: %d\n", err);
#endif
	return (struct in6_addr *)NULL;
}

static char *
addr_to_numeric(const struct in6_addr *addrp)
{
	/* 0000:0000:0000:0000:0000:000.000.000.000
	 * 0000:0000:0000:0000:0000:0000:0000:0000 */
	static char buf[50+1];
	return (char *)inet_ntop(AF_INET6, addrp, buf, sizeof(buf));
}

static void
parse_iprange(char *arg, struct xt_iprange_mtinfo *info, __u8 flag)
{
	char *dash;
	const struct in6_addr *ia;

	dash = strchr(arg, '-');
	if (dash)
		*dash = '\0';
	else
	    exit_error(PARAMETER_PROBLEM, "iprange match: Bad IP6 address range\n");
		
	ia = numeric_to_addr(arg);
	printf("ip6_min=%s\n", arg);
	if (!ia)
		exit_error(PARAMETER_PROBLEM, "iprange match: Bad IP address `%s'\n", 
			   arg);
        if (flag & IPRANGE_SRC)
            memcpy(&info->src_min.in6, ia, sizeof(*ia));
        if (flag & IPRANGE_DST)
            memcpy(&info->dst_min.in6, ia, sizeof(*ia));
	 if (dash) {
		ia = numeric_to_addr(dash+1);
		printf("ip6_max=%s\n", dash+1);
		if (!ia)
			exit_error(PARAMETER_PROBLEM, "iprange match: Bad IP address `%s'\n",
				   dash+1);
        if (flag & IPRANGE_SRC)
		    memcpy(&info->src_max.in6, ia, sizeof(*ia));
	    if (flag & IPRANGE_DST)
	        memcpy(&info->dst_max.in6, ia, sizeof(*ia));
	}
}

/* Function which parses command options; returns true if it
   ate an option */
static int iprange_parse(int c, char **argv, int invert, unsigned int *flags,
                         const void *entry, struct xt_entry_match **match)
{
	struct xt_iprange_mtinfo *info = (struct xt_iprange_mtinfo *)(*match)->data;

	switch (c) {
	case '1':
		if (*flags & IPRANGE_SRC)
			exit_error(PARAMETER_PROBLEM,
				   "iprange match: Only use --src-range ONCE!");
		*flags |= IPRANGE_SRC;

		info->flags |= IPRANGE_SRC;
		check_inverse(optarg, &invert, &optind, 0);
		if (invert) {
			info->flags |= IPRANGE_SRC_INV;
		}
		parse_iprange(optarg, info, IPRANGE_SRC);			

		break;

	case '2':
		if (*flags & IPRANGE_DST)
			exit_error(PARAMETER_PROBLEM,
				   "iprange match: Only use --dst-range ONCE!");
		*flags |= IPRANGE_DST;

		info->flags |= IPRANGE_DST;
		check_inverse(optarg, &invert, &optind, 0);
		if (invert)
			info->flags |= IPRANGE_DST_INV;

		parse_iprange(optarg, info, IPRANGE_DST);		

		break;

	default:
		return 0;
	}
	return 1;
}

/* Final check; must have specified --src-range or --dst-range. */
static void iprange_check(unsigned int flags)
{
	if (!flags)
		exit_error(PARAMETER_PROBLEM,
			   "iprange match: You must specify `--src-range' or `--dst-range'");
}


/* Prints out the info. */
static void iprange_print(const void *ip, const struct xt_entry_match *match,
                          int numeric)
{
	struct xt_iprange_mtinfo *info = (struct xt_iprange_mtinfo *)match->data;

	if (info->flags & IPRANGE_SRC) {
		printf("source IP range ");
		if (info->flags & IPRANGE_SRC_INV)
			printf("! ");
		/*
		 * addr_to_numeric() uses a static buffer, so cannot
		 * combine the printf() calls.
		 */
		printf("%s", addr_to_numeric(&info->src_min.in6));
		printf("-%s", addr_to_numeric(&info->src_max.in6));
	}
	if (info->flags & IPRANGE_DST) {
		printf("destination IP range ");
		if (info->flags & IPRANGE_DST_INV)
			printf("! ");
		printf("%s", addr_to_numeric(&info->dst_min.in6));
		printf("-%s", addr_to_numeric(&info->dst_max.in6));
	}
}

/* Saves the union ipt_info in parsable form to stdout. */
static void iprange_save(const void *ip, const struct xt_entry_match *match)
{
	struct xt_iprange_mtinfo *info = (struct xt_iprange_mtinfo *)match->data;

	if (info->flags & IPRANGE_SRC) {
		if (info->flags & IPRANGE_SRC_INV)
			printf("! ");
		printf("--src-range ");
		printf("%s", addr_to_numeric(&info->src_min.in6));
		printf("-%s", addr_to_numeric(&info->src_max.in6));
		if (info->flags & IPRANGE_DST)
			fputc(' ', stdout);
	}
	if (info->flags & IPRANGE_DST) {
		if (info->flags & IPRANGE_DST_INV)
			printf("! ");
		printf("--dst-range ");
		printf("%s", addr_to_numeric(&info->dst_min.in6));
		printf("-%s", addr_to_numeric(&info->dst_max.in6));
	}
}

static struct ip6tables_match iprange_match = {
	.name		= "iprange",
	.version	= IPTABLES_VERSION,
	.size		= IP6T_ALIGN(sizeof(struct xt_iprange_mtinfo)),
	.userspacesize	= IP6T_ALIGN(sizeof(struct xt_iprange_mtinfo)),
	.help		= iprange_help,
	.parse		= iprange_parse,
	.final_check	= iprange_check,
	.print		= iprange_print,
	.save		= iprange_save,
	.extra_opts	= iprange_opts,
};

void _init(void)
{
	register_match6(&iprange_match);
}

