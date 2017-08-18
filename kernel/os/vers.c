/*
 * Copyright 2017 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * UTS_VERSION and  UTS_PLATFORM must defined by
 * the build system.
 */

#include <sys/utsname.h>
#include <sys/sunddi.h>

struct utsname utsname = {
	.sysname = "Unleashed",
	.nodename = "",
	.release = "1.0",
	.version = UTS_VERSION,
	.machine = UTS_PLATFORM,
};

const struct utsname *
utsname_get(void)
{
	return &utsname;
}

void utsname_set_machine(const char *machine)
{
	strncpy(utsname.machine, machine, _SYS_NMLN);
	utsname.machine[_SYS_NMLN - 1] = '\0';
}
