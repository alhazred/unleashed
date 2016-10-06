/*
 * Copyright (c) 2016 Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <jeffpc/jeffpc.h>

#include "init.h"
#include "error_impl.h"

/*
 * We initialize these to the defaults to allow cmn_err to work even before
 * the library is initialized.
 */
struct jeffpc_ops libops = {
	.print		= default_print,
	.log		= default_log,
	.assfail	= default_assfail,
	.assfail3	= default_assfail3,
};

#define SET_OP(mem, def)				\
	do {						\
		libops.mem = ops->mem ? ops->mem : def;	\
	} while (0)

void jeffpc_init(struct jeffpc_ops *ops)
{
	struct jeffpc_ops dummy;

	if (!ops) {
		memset(&dummy, 0, sizeof(dummy));
		ops = &dummy;
	}

	SET_OP(print,    default_print);
	SET_OP(log,      default_log);
	SET_OP(assfail,  default_assfail);
	SET_OP(assfail3, default_assfail3);

	init_str_subsys();
	init_val_subsys();

}
