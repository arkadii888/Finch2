# Finch Code Convention

## Naming

- **Files** - snake_case. 
*Example:* `my_file.hpp`, `my_file.cpp`
- **Folders** - snake_case. 
*Example:* `my_folder`
- **Types** - PascalCase. 
*Example:* `MyClass`
- **Variables** - snake_case. 
*Example:* `my_variable`
- **Class Members** - snake_case_. 
*Example:* `my_variable_`
- **Constants** - UPPER_CASE. 
*Example:* `MY_VALUE`
- **Enums** - PascalCase. 
*Example:* `MyEnum`
- **Functions** - PascalCase. 
*Example:*`MyFunction()`

## Formatting

**Looping and branching statements**

*Example:*

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

*Example:*

```cpp
int first {1};
int second {2};
double temperature {26.5};
std::string name {"Alex"};
```

**Class**

*Example:*

```cpp
class MyClass {
 public: // 1 space
    MyClass(); // 4 spaces

    void SomeFunction(); 

 private:
    void SomePrivateFunction();

    int some_variable_;
};
```

All logically grouped lines (variables, dependencies, functions, etc.) should be alphabetically sorted with case sensitivity.

```cpp
struct Telemetry {
    double latitude_deg {0.0f};
    double longitude_deg {0.0f};
    float absolute_altitude_m {0.0f};
    float current_battery_a {0.0f};
    float relative_altitude_m {0.0f};
    float remaining_percent {0.0f};
    float voltage_v {0.0f};
    float yaw_deg {0.0f};
};
```

**Includes**

*Example:*

```cpp
#include <system.h>

#include <string>

#include "third_party/hello.h"

#include "my_utils/util.hpp"
```

**Literals**

*Example:*

```cpp
int max_users {5'000};
float delta {0.000'1f};
double alpha {0.05};
```

**Logging**

*Example:*

```cpp
std::cout << "MyClass::MyFunction: Houston, we have a problem." << std::endl;
```
