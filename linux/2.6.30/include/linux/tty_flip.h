#ifndef _LINUX_TTY_FLIP_H
#define _LINUX_TTY_FLIP_H
/*start: modify by j00127542 for Security red line, DTS2012050404171, 2012-05-04*/
#include "atpconfig.h"
#ifdef SUPPORT_ATP_SECURITY_REDLINE_CONSOLE
extern int g_TagConsole;
#endif
/*end: modify by j00127542 for Security red line, DTS2012050404171, 2012-05-04*/

extern int tty_buffer_request_room(struct tty_struct *tty, size_t size);
extern int tty_insert_flip_string(struct tty_struct *tty, const unsigned char *chars, size_t size);
extern int tty_insert_flip_string_flags(struct tty_struct *tty, const unsigned char *chars, const char *flags, size_t size);
extern int tty_prepare_flip_string(struct tty_struct *tty, unsigned char **chars, size_t size);
extern int tty_prepare_flip_string_flags(struct tty_struct *tty, unsigned char **chars, char **flags, size_t size);
void tty_schedule_flip(struct tty_struct *tty);

static inline int tty_insert_flip_char(struct tty_struct *tty,
					unsigned char ch, char flag)
{
	struct tty_buffer *tb = tty->buf.tail;
    /*start: modify by j00127542 for Security red line, when equipment is finished disable ttyS0 and ttyS1, DTS2012050404171,2012-05-04*/ 
#ifdef SUPPORT_ATP_SECURITY_REDLINE_CONSOLE
    /*Start for DTS2014062304432 HISI Console ttyAMA0/ttyAMA1 Disable by y00105954 2014-06-23*/
#ifdef CONFIG_HSAN
    /*ttyAMA0 ttyAMA1*/
    if ((('A' == tty->name[3]) && (('0' == tty->name[6]) || ('1' == tty->name[6]))) && (0 == g_TagConsole))
    {
        flag = TTY_BREAK;
    }
#else
    if ((('S' == tty->name[3]) && (('0' == tty->name[4]) || ('1' == tty->name[4]))) && (0 == g_TagConsole))
    {
        flag = TTY_BREAK;
    }
    /*End for DTS2014062304432 HISI Console ttyAMA0/ttyAMA1 Disable by y00105954 2014-06-23*/
#endif	
#endif
    /*end: modify by j00127542 for Security red line, when equipment is finished disable ttyS0 and ttyS1, DTS2012050404171, 2012-05-04*/
	if (tb && tb->used < tb->size) {
		tb->flag_buf_ptr[tb->used] = flag;
		tb->char_buf_ptr[tb->used++] = ch;
		return 1;
	}
	return tty_insert_flip_string_flags(tty, &ch, &flag, 1);
}

#endif /* _LINUX_TTY_FLIP_H */
