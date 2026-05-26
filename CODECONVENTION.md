# Finch Code Convention

## Naming

- **Files** - snake_case `my_file.hpp`, `my_file.cpp`
- **Types** - PascalCase `MyClass`
- **Variables** - snake_case `my_variable`
- **Class Members** - snake_case_ `my_variable_`
- **Constants** - UPPER_CASE `MY_VALUE`
- **Enums** - PascalCase `MyEnum`
- **Functions** - PascalCase `MyFunction()`

## Formatting

**Looping and branching statements**

```
if (condition) {
  DoOneThing();
} else if (condition) {
  DoAnotherThing();
} else {
  DoNothing();
}
```

**Initialization**

Use brace initialization `{}` by default for all variables and class object creations.

```
int first{1};
int second{2};
double temperature{26.5};
std::string name{"Alex"};
```
