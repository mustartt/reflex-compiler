# V-Table implementation

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

The following code will generate the following type diagram. For the itable implementation, following `next*` will lead
to a level order. 


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

```
https://asciiflow.com/#/share/eJy9lV2KgzAQx68S8iilLPtQaN%2Bq7i0CojRLBauLmy2V0luUPcyyp%2BlJNn40mZhEYysbfNCZyfx%2FySTjGefxgeJN%2FpVlC5zFFS3xBp8JPhG8Wa9WC4Ir%2Fva6fuFvjJ4Y%2FyAY8eHHnxRpI6RleqS7njVlcRKFS%2BBsLIFm2d4thOTcdrv%2BOj53JfcZcJ5r%2Bhkz8gXCz5Kx1JMqxgwwpotSIr5%2Fcl4gb1AexMgg8WrK0II2hgM9JLSMkrryArILhj4Jd%2FexfbGLtp6yk8LaBIoZ%2FXgVwb2sGtquPVlGNN%2BIFuhoI7IAVhsSaNKjwwYqrBOVkIc4Y6g2h0QJPUvosGR71tCRX%2Fkl6CO9k9r6YUcR8qPVf3yf%2B7iTUj1YZuuWN9a6MfpyH4YwWfVBozR%2FLzywWUj3oTFRR%2BpBmLoXRF3TMk%2BFEbIH2bFuV0sHU0tvgql%2FLxn11EwiDHpNrCYYtX%2BZIoR4F1ovF16Ynsd%2B7ZTYwEHQ%2Fdzpq0Gwqs1f%2BQ1ewil5DXBjq%2FNVgKnXaGQ4YwQQw3DykHaKZhANe6LAVbA9LW3N9kE5JafrpZ91s6epOYlOhHiW658fgi%2F48gfs5F2%2F)
```