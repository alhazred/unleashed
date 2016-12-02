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
