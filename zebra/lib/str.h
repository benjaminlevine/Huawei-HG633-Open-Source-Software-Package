/*
 * $Id: str.h,v 1.1 2008/12/15 08:53:53 l63336 Exp $
 */

#ifndef _ZEBRA_STR_H
#define _ZEBRA_STR_H

#ifndef HAVE_SNPRINTF
int snprintf(char *, size_t, const char *, ...);
#endif

#ifndef HAVE_VSNPRINTF
#define vsnprintf(buf, size, format, args) vsprintf(buf, format, args)
#endif

#ifndef HAVE_STRLCPY
size_t strlcpy(char *, const char *, size_t);
#endif

#ifndef HAVE_STRLCAT
size_t strlcat(char *, const char *, size_t);
#endif

#endif
