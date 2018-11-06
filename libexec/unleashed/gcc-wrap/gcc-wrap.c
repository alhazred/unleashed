/*
 * Copyright 2018 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
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

#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	char *new_argv[argc];
	bool skip_libc;
	int i;

	/* skip over argv[0] */
	argv++;
	argc--;

	skip_libc = false;

	for (i = 0; i < argc; i++) {
		new_argv[i] = argv[i];

		if (!strcmp(argv[i], "-ffreestanding") ||
		    !strcmp(argv[i], "-shared") ||
		    !strcmp(argv[i], "-c") ||
		    !strcmp(argv[i], "-S"))
			skip_libc = true;
	}

	if (skip_libc) {
		new_argv[argc] = NULL;
	} else {
		new_argv[argc] = "-lc";
		new_argv[argc + 1] = NULL;
	}

	if (execv(new_argv[0], new_argv))
		err("execv");
}
