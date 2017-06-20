File Event Monitor
==================

The file event monitor (fem) framework allows kernel modules to intercept
vnode operation invocation with per-vnode granularity.  The original fem
design was described in PSARC/2003/172.  While the functionality remains in
unleashed, the design has changed in a number of ways.

(The following description talks about vnode specific ops vectors and fem
handling, but similar rules apply at the vfs level.)

The original implementation relied on being able to change vnode's ops
vector, to direct execution to its vhead functions which would invoke the
first pushed fem op.  This caused various headaches related to
const-ification and simplification of vnode ops invocation.

The new design *never* changes the vnode ops vector.  Instead it relies on
the `fop_*` functions to call the corresponding vhead function directly (via
the `fop_*_dispatch` helpers).  Once the vhead function has control, it does
similar processing as before - either it calls the first fem operation, or
it invokes the original vnode op (again, via the `fop_*_dispatch` helpers).

Each fem operation may or may not call the vnext function.  The vnext
function is still responsible for invoking the next fem operation, or
calling the original vnode op (yet again, accomplished via the
`fop_*_dispatch` helpers).
