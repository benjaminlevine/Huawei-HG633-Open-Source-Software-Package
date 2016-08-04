/* Shared library add-on to iptables to add AH support. */
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <iptables.h>
#include <linux/netfilter/xt_iif.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>

/* Function which prints out usage message. */
static void iif_help(void)
{
	printf(
        "iif v%s options:\n"
        " --iif [!] device_name\n"
        "				match receive interface\n",
        IPTABLES_VERSION);
}

static const struct option iif_opts[] = {
	{ "iif", 1, NULL, '1' },
	{ }
};

static unsigned int get_dev_index(const char *pszIfname)
{
    struct ifreq stReq;
    int iSockfd = -1;
    unsigned int uiRet = 0;

    if (!pszIfname)
    {
        return uiRet;
    }

    iSockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (0 > iSockfd)
    {
        perror("socket");
        return uiRet;
    }

    memset(&stReq, 0, sizeof(stReq));
    strncpy(stReq.ifr_name, pszIfname, (IFNAMSIZ - 1));

    if (0 > ioctl(iSockfd, SIOCGIFINDEX, (void *)&stReq))
    {
        perror("ioctl");
        uiRet = 0;                
    }
    else
    {
        uiRet = stReq.ifr_ifindex;
    }
    
    close(iSockfd);
    
    return uiRet;
}

static void
parse_iif_str(const char *pszName, unsigned int *puiIndex)
{
    if (!pszName || !puiIndex)
    {
        exit_error(PARAMETER_PROBLEM,
               "args error");
    }
    
    *puiIndex = get_dev_index(pszName);

    if (0 >= *puiIndex)
    {
        exit_error(PARAMETER_PROBLEM, "args error");
    }

    return ;
}

/* Initialize the match. */
static void iif_init(struct xt_entry_match *m)
{
	struct xt_iif_info *pstinfo = (struct xt_iif_info *)m->data;

	memset(pstinfo, 0, sizeof(*pstinfo));
}

#define IIF_MATCH 0x01

/* Function which parses command options; returns true if it
   ate an option */
static int iif_parse(int c, char **argv, int invert, unsigned int *flags,
                    const void *entry, struct xt_entry_match **match)
{
	struct xt_iif_info *pstinfo = (struct xt_iif_info *)(*match)->data;

	switch (c) {
	case '1':
		if (*flags & IIF_MATCH)
			exit_error(PARAMETER_PROBLEM,
				   "Only one `--iif' allowed");
		check_inverse(optarg, &invert, &optind, 0);
		parse_iif_str(argv[optind-1], &pstinfo->uiIf);
		if (invert)
			pstinfo->invert |= 1;
		*flags |= IIF_MATCH;
		break;
        
	default:
		return 0;
	}

	return 1;
}

/* Prints out the union ipt_matchinfo. */
static void iif_print(const void *ip, const struct xt_entry_match *match,
                     int numeric)
{
	const struct xt_iif_info *pstIif = (struct xt_iif_info *)match->data;

	printf("iif ");
    if (pstIif->invert)
    {
        printf("! ");
    }
    printf("%d ", pstIif->uiIf);

    return ;
}

/* Saves the union ipt_matchinfo in parsable form to stdout. */
static void iif_save(const void *ip, const struct xt_entry_match *match)
{
	const struct xt_iif_info *iifinfo = (struct xt_iif_info *)match->data;

	printf("--iif %s", 
		(iifinfo->invert) ? "! " : "");
	printf("%u ", iifinfo->uiIf);

    return ;
}

static struct iptables_match iif_match = {
	.name 		= "iif",
	.version 	= IPTABLES_VERSION,
	.size		= IPT_ALIGN(sizeof(struct xt_iif_info)),
	.userspacesize 	= IPT_ALIGN(sizeof(struct xt_iif_info)),
	.help 		= iif_help,
	.init 		= iif_init,
	.parse 		= iif_parse,
	.print 		= iif_print,
	.save 		= iif_save,
	.extra_opts 	= iif_opts,
};

void
_init(void)
{
	register_match(&iif_match);
}
