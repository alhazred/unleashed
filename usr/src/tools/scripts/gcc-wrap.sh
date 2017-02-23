#!/bin/sh

libc="-lc"

for a in "$@" ; do
	case "$a" in
		-ffreestanding|-shared|-c|-S)
			libc=""
			break
			;;
	esac
done

exec "$@" $libc
