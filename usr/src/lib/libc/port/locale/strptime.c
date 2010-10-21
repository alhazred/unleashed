/*
 * Copyright 2010, Nexenta Systems, Inc.  All rights reserved.
 * Copyright (c) 1994 Powerdog Industries.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY POWERDOG INDUSTRIES ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE POWERDOG INDUSTRIES BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * official policies, either expressed or implied, of Powerdog Industries.
 */

#include "lint.h"
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <alloca.h>
#include "timelocal.h"

#define	asizeof(a)	(sizeof (a) / sizeof ((a)[0]))

static char *
__strptime(const char *buf, const char *fmt, struct tm *tm)
{
	char	c;
	const char *ptr;
	int	i, len;
	int Ealternative, Oalternative;
	struct lc_time_T *tptr = __get_current_time_locale();

	ptr = fmt;
	while (*ptr != 0) {
		if (*buf == 0)
			break;

		c = *ptr++;

		if (c != '%') {
			if (isspace((unsigned char)c))
				while (*buf != 0 &&
				    isspace((unsigned char)*buf))
					buf++;
			else if (c != *buf++)
				return (0);
			continue;
		}

		Ealternative = 0;
		Oalternative = 0;
label:
		c = *ptr++;
		switch (c) {
		case 0:
		case '%':
			if (*buf++ != '%')
				return (0);
			break;

		case '+':
			buf = __strptime(buf, tptr->date_fmt, tm);
			if (buf == 0)
				return (0);
			break;

		case 'C':
			if (!isdigit((unsigned char)*buf))
				return (0);

			/* XXX This will break for 3-digit centuries. */
			len = 2;
			for (i = 0;
			    len && isdigit((unsigned char)*buf);
			    buf++) {
				i *= 10;
				i += *buf - '0';
				len--;
			}
			if (i < 19)
				return (0);

			tm->tm_year = i * 100 - 1900;
			break;

		case 'c':
			buf = __strptime(buf, tptr->c_fmt, tm);
			if (buf == 0)
				return (0);
			break;

		case 'D':
			buf = __strptime(buf, "%m/%d/%y", tm);
			if (buf == 0)
				return (0);
			break;

		case 'E':
			if (Ealternative || Oalternative)
				break;
			Ealternative++;
			goto label;

		case 'O':
			if (Ealternative || Oalternative)
				break;
			Oalternative++;
			goto label;

		case 'F':
			buf = __strptime(buf, "%Y-%m-%d", tm);
			if (buf == 0)
				return (0);
			break;

		case 'R':
			buf = __strptime(buf, "%H:%M", tm);
			if (buf == 0)
				return (0);
			break;

		case 'r':
			buf = __strptime(buf, tptr->ampm_fmt, tm);
			if (buf == 0)
				return (0);
			break;

		case 'T':
			buf = __strptime(buf, "%H:%M:%S", tm);
			if (buf == 0)
				return (0);
			break;

		case 'X':
			buf = __strptime(buf, tptr->X_fmt, tm);
			if (buf == 0)
				return (0);
			break;

		case 'x':
			buf = __strptime(buf, tptr->x_fmt, tm);
			if (buf == 0)
				return (0);
			break;

		case 'j':
			if (!isdigit((unsigned char)*buf))
				return (0);

			len = 3;
			for (i = 0;
			    len && isdigit((unsigned char)*buf);
			    buf++) {
				i *= 10;
				i += *buf - '0';
				len--;
			}
			if (i < 1 || i > 366)
				return (0);

			tm->tm_yday = i - 1;
			break;

		case 'M':
		case 'S':
			if (*buf == 0 || isspace((unsigned char)*buf))
				break;

			if (!isdigit((unsigned char)*buf))
				return (0);

			len = 2;
			for (i = 0;
			    len && isdigit((unsigned char)*buf);
			    buf++) {
				i *= 10;
				i += *buf - '0';
				len--;
			}

			if (c == 'M') {
				if (i > 59)
					return (0);
				tm->tm_min = i;
			} else {
				if (i > 60)
					return (0);
				tm->tm_sec = i;
			}

			if (*buf != 0 && isspace((unsigned char)*buf))
				while (*ptr != 0 &&
				    !isspace((unsigned char)*ptr))
					ptr++;
			break;

		case 'H':
		case 'I':
		case 'k':
		case 'l':
			/*
			 * Of these, %l is the only specifier explicitly
			 * documented as not being zero-padded.  However,
			 * there is no harm in allowing zero-padding.
			 *
			 * XXX The %l specifier may gobble one too many
			 * digits if used incorrectly.
			 */
			if (!isdigit((unsigned char)*buf))
				return (0);

			len = 2;
			for (i = 0;
			    len && isdigit((unsigned char)*buf);
			    buf++) {
				i *= 10;
				i += *buf - '0';
				len--;
			}
			if (c == 'H' || c == 'k') {
				if (i > 23)
					return (0);
			} else if (i > 12)
				return (0);

			tm->tm_hour = i;

			if (*buf != 0 && isspace((unsigned char)*buf))
				while (*ptr != 0 &&
				    !isspace((unsigned char)*ptr))
					ptr++;
			break;

		case 'p':
			/*
			 * XXX This is bogus if parsed before hour-related
			 * specifiers.
			 */
			len = strlen(tptr->am);
			if (strncasecmp(buf, tptr->am, len) == 0) {
				if (tm->tm_hour > 12)
					return (0);
				if (tm->tm_hour == 12)
					tm->tm_hour = 0;
				buf += len;
				break;
			}

			len = strlen(tptr->pm);
			if (strncasecmp(buf, tptr->pm, len) == 0) {
				if (tm->tm_hour > 12)
					return (0);
				if (tm->tm_hour != 12)
					tm->tm_hour += 12;
				buf += len;
				break;
			}

			return (0);

		case 'A':
		case 'a':
			for (i = 0; i < asizeof(tptr->weekday); i++) {
				len = strlen(tptr->weekday[i]);
				if (strncasecmp(buf, tptr->weekday[i], len) ==
				    0)
					break;
				len = strlen(tptr->wday[i]);
				if (strncasecmp(buf, tptr->wday[i], len) == 0)
					break;
			}
			if (i == asizeof(tptr->weekday))
				return (0);

			tm->tm_wday = i;
			buf += len;
			break;

		case 'U':
		case 'W':
			/*
			 * XXX This is bogus, as we can not assume any valid
			 * information present in the tm structure at this
			 * point to calculate a real value, so just check the
			 * range for now.
			 */
			if (!isdigit((unsigned char)*buf))
				return (0);

			len = 2;
			for (i = 0;
			    len && isdigit((unsigned char)*buf);
			    buf++) {
				i *= 10;
				i += *buf - '0';
				len--;
			}
			if (i > 53)
				return (0);

			if (*buf != 0 && isspace((unsigned char)*buf))
				while (*ptr != 0 &&
				    !isspace((unsigned char)*ptr))
					ptr++;
			break;

		case 'w':
			if (!isdigit((unsigned char)*buf))
				return (0);

			i = *buf - '0';
			if (i > 6)
				return (0);

			tm->tm_wday = i;

			if (*buf != 0 && isspace((unsigned char)*buf))
				while (*ptr != 0 &&
				    !isspace((unsigned char)*ptr))
					ptr++;
			break;

		case 'd':
		case 'e':
			/*
			 * The %e specifier is explicitly documented as not
			 * being zero-padded but there is no harm in allowing
			 * such padding.
			 *
			 * XXX The %e specifier may gobble one too many
			 * digits if used incorrectly.
			 */
			if (!isdigit((unsigned char)*buf))
				return (0);

			len = 2;
			for (i = 0;
			    len && isdigit((unsigned char)*buf);
			    buf++) {
				i *= 10;
				i += *buf - '0';
				len--;
			}
			if (i > 31)
				return (0);

			tm->tm_mday = i;

			if (*buf != 0 && isspace((unsigned char)*buf))
				while (*ptr != 0 &&
				    !isspace((unsigned char)*ptr))
					ptr++;
			break;

		case 'B':
		case 'b':
		case 'h':
			for (i = 0; i < asizeof(tptr->month); i++) {
				len = strlen(tptr->month[i]);
				if (strncasecmp(buf, tptr->month[i], len) == 0)
					break;
			}
			/*
			 * Try the abbreviated month name if the full name
			 * wasn't found.
			 */
			if (i == asizeof(tptr->month)) {
				for (i = 0; i < asizeof(tptr->month); i++) {
					len = strlen(tptr->mon[i]);
					if (strncasecmp(buf, tptr->mon[i],
					    len) == 0)
						break;
				}
			}
			if (i == asizeof(tptr->month))
				return (0);

			tm->tm_mon = i;
			buf += len;
			break;

		case 'm':
			if (!isdigit((unsigned char)*buf))
				return (0);

			len = 2;
			for (i = 0;
			    len && isdigit((unsigned char)*buf);
			    buf++) {
				i *= 10;
				i += *buf - '0';
				len--;
			}
			if (i < 1 || i > 12)
				return (0);

			tm->tm_mon = i - 1;

			if (*buf != 0 && isspace((unsigned char)*buf))
				while (*ptr != 0 &&
				    !isspace((unsigned char)*ptr))
					ptr++;
			break;

		case 'Y':
		case 'y':
			if (*buf == 0 || isspace((unsigned char)*buf))
				break;

			if (!isdigit((unsigned char)*buf))
				return (0);

			len = (c == 'Y') ? 4 : 2;
			for (i = 0;
			    len && isdigit((unsigned char)*buf);
			    buf++) {
				i *= 10;
				i += *buf - '0';
				len--;
			}
			if (c == 'Y')
				i -= 1900;
			if (c == 'y' && i < 69)
				i += 100;
			if (i < 0)
				return (0);

			tm->tm_year = i;

			if (*buf != 0 && isspace((unsigned char)*buf))
				while (*ptr != 0 &&
				    !isspace((unsigned char)*ptr))
					ptr++;
			break;

		case 'Z':
			{
			const char *cp = buf;
			char *zonestr;

			while (isupper((unsigned char)*cp))
				++cp;
			if (cp - buf) {
				zonestr = alloca(cp - buf + 1);
				(void) strncpy(zonestr, buf, cp - buf);
				zonestr[cp - buf] = '\0';
				tzset();
				/*
				 * Once upon a time this supported "GMT",
				 * for GMT, but we removed this as Solaris
				 * doesn't have it, and we lack the needed
				 * timegm function.
				 */
				if (0 == strcmp(zonestr, tzname[0])) {
					tm->tm_isdst = 0;
				} else if (0 == strcmp(zonestr, tzname[1])) {
					tm->tm_isdst = 1;
				} else {
					return (0);
				}
				buf += cp - buf;
			}
			}
			break;

		/*
		 * Note that there used to be support %z and %s, but these
		 * are not supported by Solaris, so we have removed them.
		 * They would have required timegm() which is missing.
		 */
		}
	}
	return ((char *)buf);
}

char *
strptime(const char *buf, const char *fmt, struct tm *tm)
{
	/* Legacy Solaris strptime clears the incoming tm structure. */
	(void) memset(tm, 0, sizeof (*tm));

	return (__strptime(buf, fmt, tm));
}

/*
 * This is used by Solaris, and is a variant that does not clear the
 * incoming tm.  It is triggered by -D_STRPTIME_DONTZERO.
 */
char *
__strptime_dontzero(const char *buf, const char *fmt, struct tm *tm)
{
	return (__strptime(buf, fmt, tm));
}
