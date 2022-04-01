# GC Implementation

The compiler implements a mark and sweep garbage collector. All classes default
inherits from the parent Object. The base Object contains the following content

```
class Object {
    protected var marked: bool;
    ...
    protected func __gc_mark() -> void;
    protected func __gc_is_alive() -> void;
    ...
}
```

Upon each garbage collection invocation, each object's `__gc_mark`
function will mark self as a reachable object, and will not be collected by gc.
Furthermore, the invocation to `__gc_mark` will call member's `__gc_mark
` if the member is not a null reference and only `__gc_is_alive` is false.
Before each collection cycle, the GC will set all `marked` as false.

The compiler is responsible for inserting default `__gc_mark` which will mark
all non-null member references, unless a custom `__gc_mark` is provided.
