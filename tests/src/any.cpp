/******************************************************************************
**
** Copyright (C) 2023 Ivan Pinezhaninov <ivan.pinezhaninov@gmail.com>
**
** This file is part of the async_promise project - which can be found at
** https://github.com/IvanPinezhaninov/async_promise/.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
** DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
** OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
** THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
******************************************************************************/

// stl
#include <cstring>

// async_promise
#include <async_promise.hpp>

// catch2
#include <catch2/catch_test_macros.hpp>

static constexpr auto str = "Hello World!";
static constexpr auto aggregate_error = "All promises rejected";


TEST_CASE("Any void void", "[any]")
{
  std::vector<void(*)()> void_void
  {
    [] () {},
    [] () {},
  };

  auto future = async::promise<void>::resolve().any(void_void).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Any error void void", "[any]")
{
  std::vector<void(*)()> void_void
  {
    [] () { throw std::runtime_error{str}; },
    [] () {},
  };

  auto future = async::promise<void>::resolve().any(void_void).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Any all error void void", "[any]")
{
  std::vector<void(*)()> void_void
  {
    [] () { throw std::runtime_error{str}; },
    [] () { throw std::runtime_error{str}; },
  };

  auto future = async::promise<void>::resolve().any(void_void).run();

  try
  {
    future.get();
  }
  catch(const async::aggregate_error& e)
  {
    REQUIRE(0 == std::strcmp(e.what(), aggregate_error));
    for (const auto& error : e.errors)
      REQUIRE_THROWS_AS(std::rethrow_exception(error), std::runtime_error);
  }
  catch(...)
  {
    REQUIRE(false);
  }
}


TEST_CASE("Any void string", "[any]")
{
  std::vector<void(*)(std::string)> void_str
  {
    [] (std::string) {},
    [] (std::string) {},
  };

  auto future = async::promise<std::string>::resolve(str).any(void_str).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Any error void string", "[any]")
{
  std::vector<void(*)(std::string)> void_str
  {
    [] (std::string) { throw std::runtime_error{str}; },
    [] (std::string) {},
  };

  auto future = async::promise<std::string>::resolve(str).any(void_str).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Any all error void string", "[any]")
{
  std::vector<void(*)(std::string)> void_str
  {
    [] (std::string) { throw std::runtime_error{str}; },
    [] (std::string) { throw std::runtime_error{str}; },
  };

  auto future = async::promise<std::string>::resolve(str).any(void_str).run();

  try
  {
    future.get();
  }
  catch(const async::aggregate_error& e)
  {
    REQUIRE(0 == std::strcmp(e.what(), aggregate_error));
    for (const auto& error : e.errors)
      REQUIRE_THROWS_AS(std::rethrow_exception(error), std::runtime_error);
  }
  catch(...)
  {
    REQUIRE(false);
  }
}


TEST_CASE("Any string void", "[any]")
{
  std::vector<std::string(*)()> str_void
  {
    [] () { return std::string{str}; },
    [] () { return std::string{str}; },
  };

  auto future = async::promise<void>::resolve().any(str_void).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str);
}


TEST_CASE("Any error string void", "[any]")
{
  std::vector<std::string(*)()> str_void
  {
    [] () -> std::string { throw std::runtime_error{str}; },
    [] () { return std::string{str}; },
  };

  auto future = async::promise<void>::resolve().any(str_void).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str);
}


TEST_CASE("Any all error string void", "[any]")
{
  std::vector<std::string(*)()> str_void
  {
    [] () -> std::string { throw std::runtime_error{str}; },
    [] () -> std::string { throw std::runtime_error{str}; },
  };

  auto future = async::promise<void>::resolve().any(str_void).run();

  try
  {
    future.get();
  }
  catch(const async::aggregate_error& e)
  {
    REQUIRE(0 == std::strcmp(e.what(), aggregate_error));
    for (const auto& error : e.errors)
      REQUIRE_THROWS_AS(std::rethrow_exception(error), std::runtime_error);
  }
  catch(...)
  {
    REQUIRE(false);
  }
}


TEST_CASE("Any string string", "[any]")
{
  std::vector<std::string(*)(std::string)> str_str
  {
    [] (std::string str) { return str; },
    [] (std::string str) { return str; },
  };

  auto future = async::promise<std::string>::resolve(str).any(str_str).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str);
}


TEST_CASE("Any error string string", "[any]")
{
  std::vector<std::string(*)(std::string)> str_str
  {
    [] (std::string str) -> std::string { throw std::runtime_error{str}; },
    [] (std::string str) { return str; },
  };

  auto future = async::promise<std::string>::resolve(str).any(str_str).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str);
}


TEST_CASE("Any all error string string", "[any]")
{
  std::vector<std::string(*)(std::string)> str_str
  {
    [] (std::string str) -> std::string { throw std::runtime_error{str}; },
    [] (std::string str) -> std::string { throw std::runtime_error{str}; },
  };

  auto future = async::promise<std::string>::resolve(str).any(str_str).run();

  try
  {
    future.get();
  }
  catch(const async::aggregate_error& e)
  {
    REQUIRE(0 == std::strcmp(e.what(), aggregate_error));
    for (const auto& error : e.errors)
      REQUIRE_THROWS_AS(std::rethrow_exception(error), std::runtime_error);
  }
  catch(...)
  {
    REQUIRE(false);
  }
}
