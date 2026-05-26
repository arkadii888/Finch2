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

*Exception:* Use parentheses `()` only for containers like `std::vector` when you intend to call a specific configuration constructor (e.g., size and initial value) rather than passing a list of elements.

```
// Creates a vector of 10 integers, all initialized to 20
std::vector<int> data(10, 20);

// Creates a vector of only 2 integers: [10, 20]
std::vector<int> wrong_data{10, 20};
```
