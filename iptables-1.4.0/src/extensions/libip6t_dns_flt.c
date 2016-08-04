#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <ip6tables.h>
#include <linux/netfilter_ipv6/ip6_tables.h>

#include <linux/netfilter/xt_dns_flt.h>



/* Function which prints out usage message. */
static void help(void)
{
	printf(
        "dns_flt match v%s options:\n"
        "[!] --str string	Match DNS name.\n",
        IPTABLES_VERSION);
}

static struct option opts[] = {
	{ "str", 1, 0, '1' },
	{0}
};

/* Initialize the match. */
static void init(struct xt_entry_match *m)
{

}


static void parse_dns_str(const char *dnsString, char *pstResult, int len)
{
    int iLen = 0;

    if (!dnsString || !pstResult || ('\0' == *dnsString))
    {
        exit_error(PARAMETER_PROBLEM, "--str must with a string");
    }

    iLen = strlen(dnsString);
    if (iLen >= len)
    {
        exit_error(PARAMETER_PROBLEM, "string too long, must not longer than 256");
    }

    memset(pstResult, 0, len);
    strncpy(pstResult, dnsString, iLen);
}

/* Function which parses command options; returns true if it
   ate an option */
static int parse(int c, char **argv, int invert, unsigned int *flags,
      const void *entry,
      struct xt_entry_match **match)
{
	 URL_STRING_ST *dnsinfo =
		(URL_STRING_ST *)(*match)->data;

	switch (c) {
	case '1':
		if (*flags)
			exit_error(PARAMETER_PROBLEM,"Only one `--str' allowed");
		check_inverse(optarg, &invert, &optind, 0);
		parse_dns_str(argv[optind-1], (char *)(&dnsinfo->acURL), URL_STRING_LEN);
		if (invert)
			dnsinfo->u16Inv = 1;
		*flags = 1;
		break;
	default:
		return 0;
	}
	return 1;
}

static void print_dns_flt(char *pstStr, int invert)
{
	if (invert)
		printf("! ");

    if (pstStr)
		printf("%s ", pstStr);
}

/* Final check; must have specified --mss. */
static void final_check(unsigned int flags)
{
	if (!flags)
		exit_error(PARAMETER_PROBLEM,
			   "dns_flt match: You must specify `--str'");
}

/* Prints out the matchinfo. */
static void
print(const void *ip,
      const struct xt_entry_match *match,
      int numeric)
{
	URL_STRING_ST *dnsinfo =
		(URL_STRING_ST *)match->data;

	printf("dns_flt match ");
	print_dns_flt((char *)(dnsinfo->acURL), dnsinfo->u16Inv);
}

/* Saves the union ipt_matchinfo in parsable form to stdout. */
static void
save(const void *ip, const struct xt_entry_match *match)
{
	const  URL_STRING_ST *dnsinfo =
		(const URL_STRING_ST *)match->data;

	printf("--str ");
	print_dns_flt((char *)(dnsinfo->acURL), dnsinfo->u16Inv);
}

static
struct ip6tables_match dns_flt = {
    .name          = "dns_flt",
    .version	   = IPTABLES_VERSION,
    .size		   = IP6T_ALIGN(sizeof(URL_STRING_ST)),
    .userspacesize = IP6T_ALIGN(sizeof(URL_STRING_ST)),
    .help		   = &help,
    .init          = &init,
    .parse		   = &parse,
    .final_check   = &final_check,
    .print         = &print,
    .save          = &save,
    .extra_opts    = opts
};

void _init(void)
{
	register_match6(&dns_flt);
}


