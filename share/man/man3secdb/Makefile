MAN = getauthattr.3secdb \
      getexecattr.3secdb \
      getprofattr.3secdb \
      getuserattr.3secdb \
      kva_match.3secdb

MLINKS+=	getauthattr.3secdb chkauthattr.3secdb
MLINKS+=	getauthattr.3secdb endauthattr.3secdb
MLINKS+=	getauthattr.3secdb free_authattr.3secdb
MLINKS+=	getauthattr.3secdb getauthnam.3secdb
MLINKS+=	getauthattr.3secdb setauthattr.3secdb
MLINKS+=	getexecattr.3secdb endexecattr.3secdb
MLINKS+=	getexecattr.3secdb free_execattr.3secdb
MLINKS+=	getexecattr.3secdb getexecprof.3secdb
MLINKS+=	getexecattr.3secdb getexecuser.3secdb
MLINKS+=	getexecattr.3secdb match_execattr.3secdb
MLINKS+=	getexecattr.3secdb setexecattr.3secdb
MLINKS+=	getprofattr.3secdb endprofattr.3secdb
MLINKS+=	getprofattr.3secdb free_profattr.3secdb
MLINKS+=	getprofattr.3secdb free_proflist.3secdb
MLINKS+=	getprofattr.3secdb getproflist.3secdb
MLINKS+=	getprofattr.3secdb getprofnam.3secdb
MLINKS+=	getprofattr.3secdb setprofattr.3secdb
MLINKS+=	getuserattr.3secdb enduserattr.3secdb
MLINKS+=	getuserattr.3secdb fgetuserattr.3secdb
MLINKS+=	getuserattr.3secdb free_userattr.3secdb
MLINKS+=	getuserattr.3secdb getusernam.3secdb
MLINKS+=	getuserattr.3secdb getuseruid.3secdb
MLINKS+=	getuserattr.3secdb setuserattr.3secdb

# We want to just install manpages but there is no manpage-only mk file so
# we use prog.mk.  This is ugly, but so is the entire share/man subtree.
# Eventually, it will go away and with it this hack as well.
.include <prog.mk>
