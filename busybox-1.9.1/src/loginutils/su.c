/* vi: set sw=4 ts=4: */
/*
 *  Mini su implementation for busybox
 *
 *  Licensed under the GPL v2 or later, see the file LICENSE in this tarball.
 */

#include "libbb.h"
#include <syslog.h>
#include "atptypes.h"
#include "atputil.h"
#include "atpconfig.h"
#define SU_OPT_mp (3)
#define SU_OPT_l (4)

static VOS_UINT32 InEquipMode()
{
    VOS_UINT32 ulEquipMode = 0;
    VOS_UINT32        ulRetVal = 0;
    ATP_MSG_HEADER_ST    stMsg;
    ATP_MSG_HEADER_ST   *pstResp = VOS_NULL; 
    
    ATP_MSG_Init("busybox");
    
    /*向cms发送获取生产模式的消息*/
    ATP_MSG_SimpleBuilder(&stMsg, ATP_CBBID_CMS_NAME, ATP_MSG_CMS_EQUIP_GET, 0);
    ulRetVal = ATP_MSG_SendAndRecv(&stMsg, &pstResp, 2 * 1000);
    if (VOS_OK != ulRetVal)
    {
        printf("get equip mode  failed %x .\n",ulRetVal);
    }
    else
    {
        if (VOS_NULL != pstResp)
        {
            ulEquipMode = pstResp->ulMsgData;
            free((VOS_VOID *)pstResp);
        }
    }
    
    return ulEquipMode;

}

int su_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int su_main(int argc, char **argv)
{
	unsigned flags;
	char *opt_shell = NULL;
	char *opt_command = NULL;
	const char *opt_username = "root";
	struct passwd *pw;
	uid_t cur_uid = getuid();
	const char *tty;
	char *old_user;
	//克罗地亚定制开放su权限
#ifndef SUPPORT_ATP_SHELL_OPEN_SU
    /** if not in equipmode and the image is debug image , we use the su command, otherwise don't use it*/
    if (!InEquipMode())
    {
#ifdef SUPPORT_ATP_ATP_DEBUG_IAMGE     
            printf("\n");
#else           
            printf("release image, you do not have root previlege  \n"
                    "please choose debug image if you want to use su \n");
           return 0;
#endif
    }
#endif
	flags = getopt32(argv, "mplc:s:", &opt_command, &opt_shell);
	argc -= optind;
	argv += optind;

	if (argc && LONE_DASH(argv[0])) {
		flags |= SU_OPT_l;
		argc--;
		argv++;
	}

	/* get user if specified */
	if (argc) {
		opt_username = argv[0];
		//argc--; - not used below anyway
		argv++;
	}

	if (ENABLE_FEATURE_SU_SYSLOG) {
		/* The utmp entry (via getlogin) is probably the best way to identify
		the user, especially if someone su's from a su-shell.
		But getlogin can fail -- usually due to lack of utmp entry.
		in this case resort to getpwuid.  */
		old_user = xstrdup(USE_FEATURE_UTMP(getlogin() ? : ) (pw = getpwuid(cur_uid)) ? pw->pw_name : "");
		tty = ttyname(2) ? : "none";
		openlog(applet_name, 0, LOG_AUTH);
	}

	pw = getpwnam(opt_username);
	if (!pw)
		bb_error_msg_and_die("unknown id: %s", opt_username);

	/* Make sure pw->pw_shell is non-NULL.  It may be NULL when NEW_USER
	   is a username that is retrieved via NIS (YP), but that doesn't have
	   a default shell listed.  */
	if (!pw->pw_shell || !pw->pw_shell[0])
		pw->pw_shell = (char *)DEFAULT_SHELL;

	if ((cur_uid == 0) || correct_password(pw)) {
		if (ENABLE_FEATURE_SU_SYSLOG)
			syslog(LOG_NOTICE, "%c %s %s:%s",
				'+', tty, old_user, opt_username);
	} else {
		if (ENABLE_FEATURE_SU_SYSLOG)
			syslog(LOG_NOTICE, "%c %s %s:%s",
				'-', tty, old_user, opt_username);
		bb_error_msg_and_die("incorrect password");
	}

	if (ENABLE_FEATURE_CLEAN_UP && ENABLE_FEATURE_SU_SYSLOG) {
		closelog();
		free(old_user);
	}

	if (!opt_shell && (flags & SU_OPT_mp))
		opt_shell = getenv("SHELL");

#if ENABLE_FEATURE_SU_CHECKS_SHELLS
	if (opt_shell && cur_uid && restricted_shell(pw->pw_shell)) {
		/* The user being su'd to has a nonstandard shell, and so is
		   probably a uucp account or has restricted access.  Don't
		   compromise the account by allowing access with a standard
		   shell.  */
		bb_error_msg("using restricted shell");
		opt_shell = NULL;
	}
#endif
	if (!opt_shell)
		opt_shell = pw->pw_shell;

	change_identity(pw);
	/* setup_environment params: shell, loginshell, changeenv, pw */
	setup_environment(opt_shell, flags & SU_OPT_l, !(flags & SU_OPT_mp), pw);
	USE_SELINUX(set_current_security_context(NULL);)

	/* Never returns */
	run_shell(opt_shell, flags & SU_OPT_l, opt_command, (const char**)argv);

	/* return EXIT_FAILURE; - not reached */
}
