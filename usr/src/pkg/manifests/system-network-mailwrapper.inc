#
# This file and its contents are supplied under the terms of the
# Common Development and Distribution License ("CDDL"), version 1.0.
# You may only use this file in accordance with the terms of version
# 1.0 of the CDDL.
#
# A full copy of the text of the CDDL should have accompanied this
# source.  A copy of the CDDL is also available via the Internet
# at http://www.illumos.org/license/CDDL.
#

#
# Copyright 2014, Alexander Pyhalov
#

dir path=etc group=sys
dir path=etc/mail group=mail
dir path=usr/bin
dir path=usr/lib
dir path=usr/sbin
dir path=usr/share/man
dir path=usr/share/man/man8
dir path=usr/share/man/man4
file path=etc/mailer.conf group=sys preserve=true
file path=usr/lib/mailwrapper mode=0555
file path=usr/share/man/man8/mailwrapper.8
file path=usr/share/man/man4/mailer.conf.4
license usr/src/cmd/mailwrapper/THIRDPARTYLICENSE \
    license=usr/src/cmd/mailwrapper/THIRDPARTYLICENSE
link path=usr/bin/mailq mediator=mta mediator-implementation=mailwrapper \
    target=../lib/mailwrapper
link path=usr/lib/sendmail mediator=mta mediator-implementation=mailwrapper \
    target=mailwrapper
link path=usr/sbin/newaliases mediator=mta mediator-implementation=mailwrapper \
    target=../lib/mailwrapper
link path=usr/sbin/sendmail mediator=mta mediator-implementation=mailwrapper \
    target=../lib/mailwrapper
