# Finch Code Convention

## Naming

- **Files** - snake_case `my_file.hpp`, `my_file.cpp`
- **Folders** - snake_case `my_folder`
- **Types** - PascalCase `MyClass`
- **Variables** - snake_case `my_variable`
- **Class Members** - snake_case_ `my_variable_`
- **Constants** - UPPER_CASE `MY_VALUE`
- **Enums** - PascalCase `MyEnum`
- **Functions** - PascalCase `MyFunction()`

## Formatting

**Looping and branching statements**

```cpp
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

```cpp
int first{1};
int second{2};
double temperature{26.5};
std::string name{"Alex"};
```

**Class**

```cpp
class MyClass {
 public:
  MyClass();

  void SomeFunction();

 private:
  void SomePrivateFunction();

  int some_variable_;
};
```

**Includes**

```cpp
#include <system.h>

#include <string>

#include "third_party/hello.h"

#include "my_utils/util.hpp"
```

**Literals**

```cpp
int max_users{5'000};
float delta{0.000'1f};
double alpha{0.05};
```

**Logging**

```cpp
std::cout << "MyClass::MyFunction: Houston, we have a problem." << std::endl;
```
