# reflex-compiler

Welcome to the Reflex Compiler Project! This project aims to provide a statically typed language with single inheritance, multiple interfaces, and reference counting with periodic cycle detection. The frontend utilizes a robust and efficient approach, featuring a greedy deterministic regex tokenizer and a handwritten LL(1) recursive descent parser. On the backend, we leverage the power of LLVM to generate and optimize efficient machine code for various architectures.

## Documentation

The documentation can be found here
[https://mustartt.github.io/reflex-compiler-github-page/](https://mustartt.github.io/reflex-compiler-github-page/)

## Sample code

```
class String {
    private var buffer: char[];

    public func String() -> void {
        buffer = new char[0];
    }

    public func String(value: char[]) -> void {
        buffer = value;
    }

    public func size() -> int {
        return buffer.size();
    }

    public func charAt(index: int) -> char {
        return buffer[index];
    }
}
```

## Build

Install dependencies LLVM, Doxygen

```shell
mkdir build && cd build && cmake ../
cmake reflex # for compiler
cmake gtest # for tests
```

## Implementation details and Specifications

### GC Implementation

The compiler implements a mark and sweep garbage collector. All classes default
inherits from the parentStack Object. The base Object contains the following content

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

## V-Table implementation

The following code will generate the following type diagram. For the itable implementation, following `next*` will lead
to a level order. 

```
interface A {
    func method_A() -> void;
} 
interface B : A {
    func method_B() -> void;
} 
interface C : A {
    func method_C() -> void;
} 
interface D : B, C {
    func method_D() -> void;
} 
interface E {
    func method_other() -> void;
}

class Base {
    public var member_base: int;
    public func method_base() -> void {}
}

class Derived(Base) : D, E {
    public var member_derived: num;
    public func method_base() {}
    
    public func method_A() -> void {}
    public func method_B() -> void {}
    public func method_C() -> void {}
    public func method_D() -> void {}
    public func method_other() -> void {}
}
```

```
    Base                  Derived               itab_D.Derived    itab_C.Derived    itab_A.Derived
   ┌────────────┐        ┌──────────────┐      ┌─────────┐       ┌──────────┐      ┌─────────┐
┌──┤rtti*       │   ┌────┤rtti*         │  ┌───►next*    ├───────►next*     ├─┐  ┌─►next*    ├─┐
│  │member_base │   │    │member_base   │  │   │method_A*│       │method_A* │ │  │ │method_A*│ │
│  └────────────┘   │    │member_derived│  │   │method_B*│       │method_C* │ │  │ └─────────┘ │
│                   │    └──────────────┘  │   │method_C*│       └──────────┘ │  │             │
│                   │                      │   │method_D*│                    │  │             │
└─► vtab.Base       ├───► vtab.Derived     │   └─────────┘     ┌──────────────┘  │             │
   ┌────────────┐   │    ┌─────────────┐   │                   │  tab_B.Derived  │             │
   │type_info*  │   │    │type_info*   │   │                   │ ┌──────────┐    │             │
   │base_rtti*  ├───┘    │base_rtti*   ├─► │                   └─►next*     ├────┘             │
   │itable*     ├─►      │itable*      ├───┘                     │method_A* │                  │
   │method_base*│        │method_base* │                         │method_C* │                  │
   └────────────┘        │method_A*    │       itab_E.Derived    └──────────┘                  │
                         │method_B*    │      ┌─────────────┐                                  │
                         │method_C*    │    ┌─►next*        ├─►                                │
                         │method_D*    │    │ │method_other*│                                  │
                         │method_other*│    │ └─────────────┘                                  │
                         └─────────────┘    │                                                  │
                                            └──────────────────────────────────────────────────┘
```

## Language Specification

### Language Semantics Reference

#### Interfaces

An interface is an object reference type. It defines a collection of abstract
methods that can be implemented by a class. A class can implement multiple
interfaces; and an interface can derive methods from multiple interfaces.
Interfaces can be nested and accessed with qualifier `::` from parentStack type.
Furthermore, all members of interfaces are public.

#### Interface Syntax

```
InheritanceList -> Inheritance { "," Inheritance }
Inheritance     -> identifier { "::" identifier }

InterfaceDecl     -> "interface" identifier [ ":" InheritanceList ] InterfaceBody
InterfaceBody     -> "{" { InterfaceBodyDecl } "}"
InterfaceBodyDecl -> FunctionDecl | InterfaceDecl
```

##### Valid Interface Derivation

Interfaces can have nested interfaces. Use qualifier to access nested type.

1. There must not be cycles in the derivation tree; it must be a directed
   acyclic graph.
2. Parent interface cannot implement nested types.

```
interface IC : IA::IB {}

interface IA {
    interface IB {}
}
```

#### Classes

A class is a record type or a container for member variables with associated
member functions. Class can be instantiated with its own set of member values
via the `new` keyword. A class can extends functionality of the base class via
inheritance `class DerivedClass(BaseClass) : IA, IB`, and a single class can
only inherit from a single baseclass. However, a class can implement from
multiple different interfaces `IA` and `IB`. If a class does not implement all
interface methods, it is considered abstract and cannot be instantiated via
the `new` expression.

##### Class Syntax

```
modifier        -> "public" | "private" | "protected"
InheritanceList -> Inheritance { "," Inheritance }
Inheritance     -> identifier { "::" identifier }

ClassDecl -> "class" identifier
             [ "(" Inheritance ")" ]
             [ ":" InheritanceList ] ClassBody

ClassBody     -> "{" { [ modifier ] ClassBodyDecl } "}"
ClassBodyDecl -> Declaration | FunctionDecl
```

Derived class inherits all members of `Base` class methods and member variable.
Derived class also implements `IA` and `IB` interface methods.

```
interface IA {
   func imethod1() -> void;
}
interface IB {
   func imethod2() -> void;
}

class Base {
   protected var member1: int;
   public func method1() -> void {}
}

class Derived(Base) : IA, IB {
   private var member2: num;
   public func method2() -> void {}
   public func imethod2() -> void {}
   public func imethod2() -> void {}
}
```

### Functions

#### Function Syntax

```
FunctionDecl  -> "func" identifier Signature [ FunctionBody ]

Signature     -> Parameters [ "->" Type ]
Parameters    -> "(" ParameterList ")"
ParameterList -> Parameter { "," Parameter } | ϵ
Parameter     -> identifier ":" Type
FunctionBody  -> Block
```

### Expression

#### Primary Expression

```
PrimaryExpr -> Operand
             | NewExpr
             | Conversion
             | PrimaryExpr Selector
             | PrimaryExpr Index
             | PrimaryExpr Arguments

Operand      -> Literal | NamedOperand | "(" Expression ")"
Literal      -> BasicLit | ArrayLit | FunctionLit
BasicLit     -> bool_lit | number_lit | string_lit | null_lit
NamedOperand -> identifier { "::" identifier }

Conversion   -> "cast" "<" Type ">" "(" Expression ")"
NewExpr      -> "new" InstanceName
InstanceName -> identifier { "::" identifier }

Selector       -> "." identifier
Index          -> "[" Expression "]"
Arguments      -> "(" ExpressionList ")"
ExpressionList -> Expression { "," Expression } | ϵ
```

#### Expression

```
Expression  -> Expression bin_op Expression | UnaryExpr
UnaryExpr   -> PrimaryExpr | unary_op UnaryExpr

unary_op -> "-" | "!"
bin_op   -> "or"
          | "and"
          | "==" | "!=" | "<" | "<=" | ">" | ">="
          | "+" | "-" | "|"
          | "*" | "/" | "%" | "&"
```

#### Block

```
Block         -> "{" StatementList "}"
StatementList -> { Statement }
```

#### Statements

```
Statement -> Declaration ";"
           | SimpleStmt ";"
           | ReturnStmt ";"
           | BreakStmt ";"
           | ContinueStmt ";"
           | Block
           | IfStmt
           | ForStmt
           | WhileStmt

SimpleStmt -> EmptyStmt
            | ExpressionStmt
            | IncDecStmt
            | Assignment
            
EmptyStmt      -> ϵ
ExpressionStmt -> Expression
IncDecStmt     -> Expression postfix_op
postfix_op     -> "++" | "--"
Assignment     -> Expression "=" Expression

ReturnStmt   -> "return" Expression
BreakStmt    -> "break"
ContinueStmt -> "continue"

IfStmt       -> "if" "(" SimpleStmt ")" Block [ "else" Block ]
WhileStmt    -> "while" "(" SimpleStmt ")" Block
ForStmt      -> "for" "(" ForClause ")" Block

ForClause    -> ForClause | RangeClause
RangeClause  -> VarDecl "in" Expression
ForClause    -> InitClause ";" Expression ";" PostClause
InitClause   -> VarDecl | SimpleStmt
PostClause   -> SimpleStmt
```

#### Declarations

```
Declaration -> VarDecl | TypeDecl
VarDecl     -> "var" identifier [ ":" Type ] [ "=" Expression ]
TypeDecl    -> ClassDecl | InterfaceDecl | AnnotationDecl
```

#### Literal Values

```
Literal      -> BasicLit | ArrayLit | FunctionLit
BasicLit     -> bool_lit | number_lit | string_lit | null_lit
```

#### Array Literals

```
ArrayLit  -> "{" ElementList "}"
ElementList   -> Element { "," Element } | ϵ
Element       -> Expression | Literal
```

#### Function Literals

``` 
FunctionLit   -> "func" Signature FunctionBody

Signature     -> Parameters [ "->" Type ]
Parameters    -> "(" ParameterList ")"
ParameterList -> Parameter { "," Parameter } | ϵ
Parameter     -> identifier ":" Type
FunctionBody  -> Block
```

### Types

```
Type      -> TypeName | TypeLit | "(" Type ")"
TypeName  -> identifier { "::" identifier }
TypeLit   -> ArrayType | FunctionType

ArrayType   -> ElementType "[" ArrayLength "]"
ArrayLength -> number_lit
ElementType -> Type

FunctionType      -> "func" UnnamedSignature
UnnamedSignature  -> UnnamedParameters "->" Type
UnnamedParameters -> "(" ParameterTypeList ")"
ParameterTypeList -> Type { "," Type } | ϵ
```


