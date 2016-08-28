#!/bin/sh

libc="-lc"

for a in "$@" ; do
	case "$a" in
		-ffreestanding|-shared|-c|-S)
			libc=""
			;;
	esac
done

exec "$@" $libc
