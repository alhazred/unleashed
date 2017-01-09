We think that operating systems are very exciting to work on; In particular,
we enjoy working with descendants of OpenSolaris.  Unfortunately, we feel
that the major OpenSolaris fork - illumos - has adopted a contribution
process that hinders development and discourages new contributors from
coming back with more code changes.  Therefore, we created this fork to try
to address these issues.

Goals
=====

While the full list of goals is constantly evolving, the following is a list
of the basic goals which we always strive for.

1. The contribution process must be simple and must encourage new
   contributors to repeatedly contribute.
2. We aim to have periodic releases (roughly one every 6 months).  Security
   patches will be provided between releases.
3. Unleashed aims to be a modern operating system base.
  1. Modern compiler support.  Currently, we support only gcc 4.4.4, however
     we hope to allow a wider range of gcc versions.  Eventually, we would
     like to be able to use clang as well.
  2. The UNIX environment has changed drastically over the past 20 years.
     As a result, we hope to ship an environment that provides the comforts
     of modern UNIX, yet maintain the unique features we inherited that set
     us apart from other UNIX systems out there.
  3. POSIX has won.  Therefore, when it does not hinder usability (see item
     3.2), we want a system that is POSIX compliant without having to jump
     through special hoops (e.g., setting $PATH, or providing extra compiler
     flags).
  4. We do *not* support "extreme legacy".  While support for legacy
     interfaces and binaries is important, it must be done in moderation.
     Therefore, old interfaces may be removed from time to time.  Interface
     deprecation will be clearly communicated through release notes.
4. Maintaining code is hard enough when the code is squeaky clean.  To make
   our job easier, we try to get the code clean when first committing it -
   even if it delays the commit a little bit.  In other words, we care about
   more than just that the code works - we want code we can (for the most
   part) be proud of.
5. XXX: describe the amount of self-contained-ness

Rules
=====

The community organization is based on the FreeBSD community.  (See
docs/organization.md for a more thorough description.)  This style of
community reflects our belief that our community members can behave
responsibly both when communicating with others as well as when committing
code to the repository.  To help guide newcomers, we have created a Code of
Conduct (see docs/code-of-conduct.md) that we expect everyone to abide by.

Commits
-------

Commits are cheap.  Modern revision control systems (e.g., git) handle large
numbers of commits very well.  Therefore small changes are encouraged
(instead of "mega commits" that seem to touch half the code base).  Smaller
commits make it easier to search through commit history to see what other
parts of the repository were changed as part of the change.

Each commit should build and boot.  Obviously, running nightly and a full
set of tests for each commit is not necessarily practical, however one
should try to avoid commits that break the build.  (Commits that don't build
or boot make it harder to bisect the history to find bad commits.)

For the most part, we use a Linux kernel-style commit messages.  If there is
a bug number reference it.  For example:

    subsys: frob the input 7 times

    frobbing less than 7 times leads to information disclosure
    vulnerability.

    illumos bug #123

    Spelling of comments fixed up by: Committer Developer <c.d@example.com>
