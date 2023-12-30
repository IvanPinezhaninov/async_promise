# async::promise

[![Language C++11](https://img.shields.io/badge/language-C%2B%2B11-004482?style=flat-square
)](https://isocpp.org/wiki/faq/cpp11) [![MIT License](https://img.shields.io/github/license/IvanPinezhaninov/async_promise?label=license&style=flat-square)](http://opensource.org/licenses/MIT) [![Build status](https://img.shields.io/github/actions/workflow/status/IvanPinezhaninov/async_promise/build-and-test.yml?style=flat-square
)](https://github.com/IvanPinezhaninov/async_promise/actions/workflows/build-and-test.yml) [![Codecov](https://img.shields.io/codecov/c/github/IvanPinezhaninov/async_promise?style=flat-square)](https://codecov.io/gh/IvanPinezhaninov/async_promise)

## Overview

C++11 header-only library based on Promises/A+

The library is designed to create and run a chain of functions or class methods. The chain runs asynchronously and returns `std::future`. The library supports the methods `resolve`, `reject`, `then`, `all`, `all_settled`, `any`, `race`, `fail` and `finally`

## Documentation

To create an `async::promise` object use the static function `async::make_promise`, which takes as an argument a function with optional arguments
```cpp
auto promise = async::make_promise([] (int a, int b) { return a + b; }, 2, 2);
```

To start the execution of a chain of function calls use the `run` method which returns `std::future`
```cpp
auto future = async::make_promise([] { return 2; }).run();

std::cout << future.get() << std::endl; // prints 2
```

To add the next function to a chain use the `then` method which takes as an argument a function with an argument of the same type as the return value of the previous function in the chain
```cpp
auto future = async::make_promise([] { return 2; })
              .then([] (int x) { return x * 2; })
              .run();

std::cout << future.get() << std::endl; // prints 4
```

If the `then` method does not need to process the value returned by the previous function, then it is allowed to pass a function that does not take an argument
```cpp
auto future = async::make_promise([] { return 2; })
              .then([] { return 4; })
              .run();

std::cout << future.get() << std::endl; // prints 4
```

The `all` method accepts an iterable of functions. Each function asynchronously processes the return value of the previous function. When all functions have completed, the method will return an iterable with the results of functions in the same order as the functions in the incoming iterable. If an error occurs while executing functions it will be thrown
```cpp
std::vector<int(*)(int)> funcs
{
  [] (int x) { return x * 2; },
  [] (int x) { return x * 4; },
};

auto future = async::make_promise([] { return 2; })
              .all(funcs)
              .run();

auto results = future.get();
for (const auto& result : results)
  std::cout << result << std::endl;
```

It is also possible to create a promise object using the static function `async::make_promise_all`
```cpp
std::vector<int(*)(int)> funcs
{
  [] (int x) { return x * 2; },
  [] (int x) { return x * 4; },
};

auto future = async::make_promise_all(funcs, 2)
              .run();

auto results = future.get();
for (const auto& result : results)
  std::cout << result << std::endl;
```

The `all_settled` method accepts an iterable of functions. Each function asynchronously processes the return value of the previous function. When all functions have completed, the method will return an iterable with a special `settled` object that contains either a result or an error of functions in the same order as the functions in the incoming iterable
```cpp
std::vector<int(*)(int)> funcs
{
  [] (int x) { return x * 2; },
  [] (int x) -> int { throw std::runtime_error{"I'm an error'"}; },
};

auto future = async::make_promise([] { return 2; })
              .all_settled(funcs)
              .run();

auto results = future.get();
for (const auto& result : results)
{
  switch (result.type)
  {
    case async::settle_type::resolved:
      std::cout << "resolved: " << result.result << std::endl;
      break;
    case async::settle_type::rejected:
      try
      {
        std::rethrow_exception(result.error);
      }
      catch (const std::exception& e)
      {
        std::cout << "rejected: " << e.what() << std::endl;
      }
      break;
  }
}
```

It is also possible to create a promise object using the static function `async::make_promise_all_settled`
```cpp
std::vector<int(*)(int)> funcs
{
  [] (int x) { return x * 2; },
  [] (int x) -> int { throw std::runtime_error{"I'm an error'"}; },
};

auto future = async::make_promise_all_settled(funcs, 2)
              .run();

auto results = future.get();
for (const auto& result : results)
{
  switch (result.type)
  {
    case async::settle_type::resolved:
      std::cout << "resolved: " << result.result << std::endl;
      break;
    case async::settle_type::rejected:
      try
      {
        std::rethrow_exception(result.error);
      }
      catch (const std::exception& e)
      {
        std::cout << "rejected: " << e.what() << std::endl;
      }
      break;
  }
}
```

The `any` method accepts an iterable of functions. Each function asynchronously processes the return value of the previous function. The result of the method will be the result of the first function that completes successfully. If all functions fail, an `async::aggregate_error` exception will be thrown
```cpp
std::vector<int(*)(int)> funcs
{
  [] (int x) -> int { throw std::runtime_error{"I'm an error'"}; },
  [] (int x) { return x * 2; },
};

auto future = async::make_promise([] { return 2; })
              .any(funcs)
              .run();

std::cout << future.get() << std::endl; // prints 4
```

It is also possible to create a promise object using the static function `async::make_promise_any`
```cpp
std::vector<int(*)(int)> funcs
{
  [] (int x) -> int { throw std::runtime_error{"I'm an error'"}; },
  [] (int x) { return x * 2; },
};

auto future = async::make_promise_any(funcs, 2)
              .run();

std::cout << future.get() << std::endl; // prints 4
```

The `race` method accepts an iterable of functions. Each function asynchronously processes the return value of the previous function. The result of the method will be the result or error of the first completed function
```cpp
std::vector<int(*)(int)> funcs
{
  [] (int x) -> int { throw std::runtime_error{"I'm an error'"}; },
  [] (int x) { return x * 2; },
};

auto future = async::make_promise([] { return 2; })
              .race(funcs)
              .run();

try
{
  std::cout << future.get() << std::endl;
}
catch (const std::exception& e)
{
  std::cout << e.what() << std::endl;
}
```

It is also possible to create a promise object using the static function `async::make_promise_race`
```cpp
std::vector<int(*)(int)> funcs
{
  [] (int x) -> int { throw std::runtime_error{"I'm an error'"}; },
  [] (int x) { return x * 2; },
};

auto future = async::make_promise_race(funcs, 2)
              .run();

try
{
  std::cout << future.get() << std::endl;
}
catch (const std::exception& e)
{
  std::cout << e.what() << std::endl;
}
```

In the `all`, `all_settled`, `any` and `race` methods it is allowed to use iterable with functions without an argument if it is not necessary to process the value returned by the previous function
```cpp
std::vector<int(*)()> funcs
{
  [] () { return 1; },
  [] () { return 2; },
};

auto future = async::make_promise([] { return 2; })
              .all(funcs)
              .run();

auto results = future.get();
for (const auto& result : results)
  std::cout << result << std::endl;
```

Catching exceptions thrown in previously called functions is done by adding a `fail` method to the chain, which takes as an argument a function with an argument of type `std::exception_ptr` and returns a value of the same type as the previously called function. If the previous functions were executed without errors, then the method will return the result of the previous function
```cpp
static int error_handler(const std::exception_ptr& e)
{
  try
  {
    std::rethrow_exception(e);
  }
  catch(const std::exception& e)
  {
    return -1;
  }
  catch(...)
  {
    return -2;
  }
}

auto future = async::make_promise([] { throw std::runtime_error{"I'm an error!"}; })
              .fail(error_handler)
              .run();

std::cout << future.get() << std::endl; // prints -1
```

If the `fail` method does not need to handle a previously thrown exception, it is allowed to pass a function without an argument to the `fail` method
```cpp
static int error()
{
  throw std::runtime_error{"I'm an error"};
}

static int error_handler()
{
  return -1;
}

auto future = async::make_promise(error)
              .fail(error_handler)
              .run();

std::cout << future.get() << std::endl; // prints -1
```

To add the next function to a chain that will be called on both resolved and rejected states, use the `finally` method
```cpp
auto future1 = async::make_promise([] {})
               .finally([] { return "Hello World!"; })
               .run();

auto future2 = async::make_promise([] { throw std::runtime_error{"I'm an error!"}; })
               .finally([] { return "Hello World!"; })
               .run();

std::cout << future1.get() << std::endl; // prints "Hello World!"
std::cout << future2.get() << std::endl; // prints "Hello World!"
```

In all the above cases, you can use overloaded functions that take a class method or an iterable of class methods and a class object
```cpp
my_class obj;

std::vector<int(my_class::*)(int)> all_methods
{
  &my_class::method1,
  &my_class::method2,
};

auto future = async::make_promise(&my_class::method, &obj)
              .then(&my_class::then_method, &obj)
              .all(std::move(all_methods), &obj)
              .run();
```

To create `async::promise` object with resolved state, use the `async::make_resolved_promise` static function
```cpp
auto future = async::make_resolved_promise(2)
              .run();

std::cout << future.get() << std::endl; // prints 2
```

To create `async::promise` object with rejected state, use the `async::make_rejected_promise` static function
```cpp
auto future = async::make_rejected_promise<int>(std::runtime_error{"I'm an error!"})
              .run();

try
{
  std::cout << future.get() << std::endl;
}
catch (const std::runtime_error& e)
{
  std::cout << e.what() << std::endl;
}
```

## Build and test

```bash
git clone https://github.com/IvanPinezhaninov/async_promise.git
cd async_promise
mkdir build
cd build
cmake -GNinja .. -DCMAKE_BUILD_TYPE=Release -DASYNC_PROMISE_BUILD_TESTS=YES
cmake --build . -- -j4
ctest
```

## License
This code is distributed under the [MIT License](LICENSE)

## Author
[Ivan Pinezhaninov](mailto:ivan.pinezhaninov@gmail.com)
