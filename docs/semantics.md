# Language Semantics Reference

## Interfaces

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

#### Valid Interface Derivation

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

## Classes

A class is a record type or a container for member variables with associated
member functions. Class can be instantiated with its own set of member values
via the `new` keyword. A class can extends functionality of the base class via
inheritance `class DerivedClass(BaseClass) : IA, IB`, and a single class can
only inherit from a single baseclass. However, a class can implement from
multiple different interfaces `IA` and `IB`. If a class does not implement all
interface methods, it is considered abstract and cannot be instantiated via
the `new` expression.

#### Class Syntax

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

## Functions

#### Function Syntax

```
FunctionDecl  -> "func" identifier Signature [ FunctionBody ]

Signature     -> Parameters [ "->" Type ]
Parameters    -> "(" ParameterList ")"
ParameterList -> Parameter { "," Parameter } | ϵ
Parameter     -> identifier ":" Type
FunctionBody  -> Block
```

## Expression

### Primary Expression

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

### Expression

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

### Block

```
Block         -> "{" StatementList "}"
StatementList -> { Statement }
```

### Statements

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

### Declarations

```
Declaration -> VarDecl | TypeDecl
VarDecl     -> "var" identifier [ ":" Type ] [ "=" Expression ]
TypeDecl    -> ClassDecl | InterfaceDecl | AnnotationDecl
```

## Literal Values

```
Literal      -> BasicLit | ArrayLit | FunctionLit
BasicLit     -> bool_lit | number_lit | string_lit | null_lit
```

### Array Literals

```
ArrayLit  -> "{" ElementList "}"
ElementList   -> Element { "," Element } | ϵ
Element       -> Expression | Literal
```

### Function Literals

``` 
FunctionLit   -> "func" Signature FunctionBody

Signature     -> Parameters [ "->" Type ]
Parameters    -> "(" ParameterList ")"
ParameterList -> Parameter { "," Parameter } | ϵ
Parameter     -> identifier ":" Type
FunctionBody  -> Block
```

## Types

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
