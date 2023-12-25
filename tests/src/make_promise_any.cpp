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

// async_promise
#include <async_promise.hpp>

// catch2
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

// local
#include "common.h"


TEST_CASE("Make any void void", "[make promise any]")
{
  std::vector<void(*)()> funcs
  {
    [] () {},
    [] () {},
  };

  auto future = async::make_promise_any(funcs).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make any error void void", "[make promise any]")
{
  std::vector<void(*)()> funcs
  {
    [] () { throw std::runtime_error{str2}; },
    [] () {},
  };

  auto future = async::make_promise_any(funcs).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make any all error void void", "[make promise any]")
{
  std::vector<void(*)()> funcs
  {
    [] () { throw std::runtime_error{str1}; },
    [] () { throw std::runtime_error{str2}; },
  };

  auto future = async::make_promise_any(funcs).run();

  REQUIRE_THROWS_MATCHES(future.get(), async::aggregate_error, Catch::Matchers::Message(aggregate_error_message));
}


TEST_CASE("Make any void string", "[make promise any]")
{
  std::vector<void(*)(std::string)> funcs
  {
    [] (std::string) {},
    [] (std::string) {},
  };

  auto future = async::make_promise_any(funcs, str1).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make any error void string", "[make promise any]")
{
  std::vector<void(*)(std::string)> funcs
  {
    [] (std::string) { throw std::runtime_error{str2}; },
    [] (std::string) {},
  };

  auto future = async::make_promise_any(funcs, str1).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make any all error void string", "[make promise any]")
{
  std::vector<void(*)(std::string)> funcs
  {
    [] (std::string) { throw std::runtime_error{str1}; },
    [] (std::string) { throw std::runtime_error{str2}; },
  };

  auto future = async::make_promise_any(funcs, str1).run();

  REQUIRE_THROWS_MATCHES(future.get(), async::aggregate_error, Catch::Matchers::Message(aggregate_error_message));
}


TEST_CASE("Make any string void", "[make promise any]")
{
  std::vector<std::string(*)()> funcs
  {
    [] () { return std::string{str1}; },
    [] () { return std::string{str2}; },
  };

  auto future = async::make_promise_any(funcs).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Make any error string void", "[make promise any]")
{
  std::vector<std::string(*)()> funcs
  {
    [] () -> std::string { throw std::runtime_error{str2}; },
    [] () { return std::string{str1}; },
  };

  auto future = async::make_promise_any(funcs).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Make any all error string void", "[make promise any]")
{
  std::vector<std::string(*)()> funcs
  {
    [] () -> std::string { throw std::runtime_error{str1}; },
    [] () -> std::string { throw std::runtime_error{str2}; },
  };

  auto future = async::make_promise_any(funcs).run();
  REQUIRE_THROWS_MATCHES(future.get(), async::aggregate_error, Catch::Matchers::Message(aggregate_error_message));
}


TEST_CASE("Make any string string", "[make promise any]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    [] (std::string str) { return std::string{str1}; },
    [] (std::string str) { return std::string{str2}; },
  };

  auto future = async::make_promise_any(funcs, str1).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Make any error string string", "[make promise any]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    [] (std::string str) -> std::string { throw std::runtime_error{str2}; },
    [] (std::string str) { return std::string{str1}; },
  };

  auto future = async::make_promise_any(funcs, str1).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Make any all error string string", "[make promise any]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    [] (std::string str) -> std::string { throw std::runtime_error{str1}; },
    [] (std::string str) -> std::string { throw std::runtime_error{str2}; },
  };

  auto future = async::make_promise_any(funcs, str1).run();
  REQUIRE_THROWS_MATCHES(future.get(), async::aggregate_error, Catch::Matchers::Message(aggregate_error_message));
}
