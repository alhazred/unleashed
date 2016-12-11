Clean Code Best Practices
=========================

This document describes some of the practices we use to maintain high code
quality.

Debug-only build variables
--------------------------

If you have any variables that are used only in a debug build, don't leave
them unused outside of debug builds.

The following is *bad* because it leaves an unused variable, which forces
the entire build to use `-Wno-unused-variable`:

```
int foobar(int arg)
{
	int foo;

#ifdef DEBUG
	foo = checkarg(arg);
	if (foo != 42)
		return -1;
#endif

	bar(arg);

	return 0;
}
```

To solve it, make the definition of `foo` part of the `#ifdef` or in this
case eliminate it completely and check the return value of `checkarg`
directly:

```
int foobar(int arg)
{
#ifdef DEBUG
	if (checkarg(arg) != 42)
		return -1;
#endif

	bar(arg);

	return 0;
}
```

ISA & platform #ifdef (ab)use
-----------------------------

At times it is tempting to use ISA and platform defines (e.g., `__amd64`,
`__i386`, and `__x86`) to conditionally compile code.  Before you do so,
use the following questions to guide your determination if using these
defines is use or abuse.

1. Is the feature/driver/code/etc. cross-platform?
2. Is it cross-platform in theory but not fully implemented everywhere?

A 'yes' answer to any of these means that using ISA or platform defs is
abuse and you should add a config variable instead.

For example, support for Linux core files in libproc is implemented only for
x86 but in theory it is a cross-platform feature.  As a result libproc uses
`CONFIG_LINUX_CORE_SUPPORT` to no-op the feature on sparc.  At the same
time while building on x86, the code needs to decide which register names to
use so it also has a `#ifdef __amd64`.  (Note that this check is inside a
code block guarded by `CONFIG_LINUX_CORE_SUPPORT`.)

### `__i386 || __amd64` vs. `__x86`

Throughout the code you will see checks such as:

```
#if defined(__i386) || defined(__amd64)
```

and

```
#ifdef __x86
```

While they may appear to be identical, they are not equally expressive.
(Note, *currently* `__x86` is defined as essentially `__i386 || __amd64`
which is why developers get confused.)

* `__i386` means 32-bit x86 ISA.
* `__amd64` means 64-bit x86 ISA.
* `__x86` means any x86 ISA.

Given these definitions, checking for `__x86` reads as "the following code
is meant for *any* x86 ISA,"  while checking for `__i386 || __amd64` reads
as "the following code is meant for 32-bit or 64-bit x86 ISA, but it may
break on other (future) x86 ISAs."

While there are currently only two x86 ISAs, use the expression which better
describes the condition you actually want to check for.  It makes the code
more future proof, and it lets future developers know what the true intent
was.
