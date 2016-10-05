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

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include <jeffpc/sock.h>

int connect_ip(const char *host, uint16_t port, bool v4, bool v6, enum ip_type type)
{
	struct addrinfo hints, *res, *p;
	char strport[6];
	int sock;

	if (!host || !port || (!v4 && !v6) || (type != IP_TCP))
		return -EINVAL;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	snprintf(strport, sizeof(strport), "%hu", port);

	switch (getaddrinfo(host, strport, &hints, &res)) {
		case 0:
			/* success */
			break;
		case EAI_ADDRFAMILY:
		case EAI_FAMILY:
		case EAI_SERVICE:
		case EAI_SOCKTYPE:
			return -ENOTSUP;
		case EAI_AGAIN:
			return -EAGAIN;
		case EAI_BADFLAGS:
			return -EINVAL;
		case EAI_MEMORY:
			return -ENOMEM;
		case EAI_NODATA:
		case EAI_NONAME:
			return -ENOENT;
		case EAI_OVERFLOW:
			return -EOVERFLOW;
		case EAI_SYSTEM:
			return -errno;
		default:
			/* TODO: is this the best errno we can return? */
			return -ENOENT;
	}

	sock = -ENOENT;

	for (p = res; p; p = p->ai_next) {
		switch (p->ai_family) {
			case AF_INET:
				if (!v4)
					continue;
				break;
			case AF_INET6:
				if (!v6)
					continue;
				break;
			default:
				continue;
		}

		sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sock == -1) {
			sock = -errno;
			continue;
		}

		if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
			close(sock);
			sock = -errno;
			continue;
		}

		/* success! */
		break;
	}

	freeaddrinfo(res);

	return sock;
}
