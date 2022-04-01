## Interface and Class AST annotation

```
interface IA {
    interface IB {
        interface IC {}
    }
    interface ID {}
    
    func imethod(a: IA, b: IB, c: IB::IC) -> ID;
}

class A {
    public class B {
        public class C {}
    }
    pubilc class D {}
    
    public func cmethod(a: A, b: B, c: B::C) -> D {}
    
    public var cvar: A;
}

func function(a: A, b: A::B, c: A::B::C) -> A::D {}

var variable: IA;
```
