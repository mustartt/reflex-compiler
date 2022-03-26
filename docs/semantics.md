# Language Semantics Reference

## Interfaces

An interface is an object reference type. It defines a collection of abstract
methods that can be implemented by a class. A class can implement multiple
interfaces; and an interface can derive methods from multiple interfaces.
Interfaces can be nested and accessed with qualifier `::` from parent type.
Furthermore, all members of interfaces are public.

#### Interface Syntax

```
InheritanceList -> Inheritance { "," Inheritance }
Inheritance     -> identifier | ModuleIdentifier

InterfaceDecl     -> "interface" identifier [ ":" InheritanceList ] InterfaceBody
InterfaceBody     -> "{" { InterfaceBodyDecl } "}"
InterfaceBodyDecl -> FunctionDecl | InterfaceDecl
```

#### Valid Interface Derivation

Nested Interfaces.

```
interface IA {
    interface IB : IA {}
}
```

Use qualifier to access nested interfaces.

```
interface IC {
    interface ID : IC {}
}
interface IA : IC {
    interface IB : IC::ID {}
}
```

Note: there must not be cycles in the derivation tree; it must be a directed
acyclic graph.

