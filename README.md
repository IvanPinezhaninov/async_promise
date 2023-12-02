# async::promise

![Build and test](https://github.com/IvanPinezhaninov/async_promise/actions/workflows/build-and-test.yml/badge.svg)
[![MIT License](https://img.shields.io/badge/license-mit-blue.svg?style=flat)](http://opensource.org/licenses/MIT)

## Overview

Promises/A+ asynchronous C++11 header-only library

The library is designed to create and run a chain of function. The chain runs asynchronously and returns `std::future`. The library supports the `then`, `all`, `all_settled`, `any`, `race`, `fail` and `finally` methods

## Documentation

The constructor for creating an `async::promise` object takes a function with optional arguments

```cpp
auto promise = async::promise<int>{[] (int a, int b) { return a + b; }, 2, 2};
```

To start the execution of a chain of function calls, use the `run` method, which returns `std::future`

```cpp
auto future = async::promise<int>{[] { return 2; }}.run();

std::cout << future.get() << std::endl; // prints 2
```

To add the next function to a chain, the `then` method can be used, which takes as an argument a function whose argument is of the same type as that returned by the previous function in the chain

```cpp
auto future = async::promise<int>{[] { return 2; }}
              .then([] (int x) { return x * 2; })
              .run();

std::cout << future.get() << std::endl; // prints 4
```

If the `then` method does not need to process the value returned by the previous function, then it is allowed to pass a function that does not take an argument

```cpp
auto future = async::promise<int>{[] { return 2; }}
              .then([] { return 4; })
              .run();

std::cout << future.get() << std::endl; // prints 4
```

The `all` method accepts an iterable of functions. Each function asynchronously processes the return value of the previous function. When all functions have completed, the method will return an iterable with the results of functions in the same order as the functions in the incoming iterable. If an error occurs while executing functions, it will be thrown

```cpp
std::vector<int(*)(int)> funcs
{
  [] (int x) { return x * 2; },
  [] (int x) { return x * 4; },
};

auto future = async::promise<int>{[] { return 2; }}
              .all(funcs)
              .run();

auto results = future.get();
for (const auto& result : results)
  std::cout << result << std::endl;
```

The `all_settled` method accepts an iterable of functions. Each function asynchronously processes the return value of the previous function. When all functions have completed, the method will return an iterable with a special settled object that contains either a result or an error of functions in the same order as the functions in the incoming iterable.

```cpp
std::vector<int(*)(int)> funcs
{
  [] (int x) { return x * 2; },
  [] (int x) -> int { throw std::runtime_error{"I'm an error'"}; },
};

auto future = async::promise<int>{[] { return 2; }}
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
      try {
        std::rethrow_exception(result.error);
      } catch (const std::exception& e) {
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

auto future = async::promise<int>{[] { return 2; }}
              .any(funcs)
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

auto future = async::promise<int>{[] { return 2; }}
              .race(funcs)
              .run();

try {
  std::cout << future.get() << std::endl;
} catch (const std::exception& e) {
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

auto future = async::promise<int>{[] { return 2; }}
              .all(funcs)
              .run();

auto results = future.get();
for (const auto& result : results)
  std::cout << result << std::endl;
```

Catching exceptions thrown in previously called functions is done by adding a `fail` method to the chain, which takes as an argument a function whose argument is of type `std::exception_ptr` and returns a value of the same type as the previously called function. If the previous functions were executed without errors, then the method is free to pass through itself the value calculated by the previous function. If an exception was thrown in one of the previous functions, it can be handled using the function passed to the `fail` method

```cpp
static int error()
{
  throw std::runtime_error{"I'm an error"};
}

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

auto future = async::promise<int>{error}
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

auto future = async::promise<int>{error}
              .fail(error_handler)
              .run();

std::cout << future.get() << std::endl; // prints -1
```

To add the next function to a chain that will be called on both resolved and rejected, the `finally` method can be used

```cpp
auto future1 = async::static_promise<void>::resolve()
               .finally([] { return "Hello World!"; })
               .run();

auto future2 = async::static_promise<void>::reject(std::runtime_error{"I'm an error"})
               .finally([] { return "Hello World!"; })
               .run();

std::cout << future1.get() << std::endl; // prints "Hello World!"
std::cout << future2.get() << std::endl; // prints "Hello World!"
```

A set of static methods `resolve`, `reject`, `all`, `all_settled`, `any`, `race` are available in the special class `async::static_promise`

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
