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

// async_promise
#include <async_promise.hpp>

// catch2
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

// local
#include "common.h"


TEST_CASE("Any with class method void void", "[any]")
{
  test_struct test;

  std::vector<void(test_struct::*)() const> methods
  {
    &test_struct::void_void,
    &test_struct::void_void,
  };

  auto future = async::make_resolved_promise().any(methods, &test).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Any with class method error void void", "[any]")
{
  test_struct test;

  std::vector<void(test_struct::*)() const> methods
  {
    &test_struct::error_void_void,
    &test_struct::void_void,
  };

  auto future = async::make_resolved_promise().any(methods, &test).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Any with class method all error void void", "[any]")
{
  test_struct test;

  std::vector<void(test_struct::*)() const> methods
  {
    &test_struct::error_void_void,
    &test_struct::error_void_void,
  };

  auto future = async::make_resolved_promise().any(methods, &test).run();

  REQUIRE_THROWS_MATCHES(future.get(), async::aggregate_error, Catch::Matchers::Message(aggregate_error_message));
}


TEST_CASE("Any with class method void string", "[any]")
{
  test_struct test;

  std::vector<void(test_struct::*)(std::string) const> methods
  {
    &test_struct::void_string,
    &test_struct::void_string,
  };

  auto future = async::make_resolved_promise(str1).any(methods, &test).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Any with class method error void string", "[any]")
{
  test_struct test;

  std::vector<void(test_struct::*)(std::string) const> methods
  {
    &test_struct::error_void_string,
    &test_struct::void_string,
  };

  auto future = async::make_resolved_promise(str1).any(methods, &test).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Any with class method all error void string", "[any]")
{
  test_struct test;

  std::vector<void(test_struct::*)(std::string) const> methods
  {
    &test_struct::error_void_string,
    &test_struct::error_void_string,
  };

  auto future = async::make_resolved_promise(str1).any(methods, &test).run();

  REQUIRE_THROWS_MATCHES(future.get(), async::aggregate_error, Catch::Matchers::Message(aggregate_error_message));
}


TEST_CASE("Any with class method string void", "[any]")
{
  test_struct test;

  std::vector<std::string(test_struct::*)() const> methods
  {
    &test_struct::string_void1,
    &test_struct::string_void2,
  };

  auto future = async::make_resolved_promise().any(methods, &test).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Any with class method string void ignore arg", "[any]")
{
  test_struct test;

  std::vector<std::string(test_struct::*)() const> methods
  {
    &test_struct::string_void1,
    &test_struct::string_void2,
  };

  auto future = async::make_resolved_promise(str1).any(methods, &test).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Any with class method error string void", "[any]")
{
  test_struct test;

  std::vector<std::string(test_struct::*)() const> methods
  {
    &test_struct::error_string_void,
    &test_struct::string_void1,
  };

  auto future = async::make_resolved_promise().any(methods, &test).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Any with class method all error string void", "[any]")
{
  test_struct test;

  std::vector<std::string(test_struct::*)() const> methods
  {
    &test_struct::error_string_void,
    &test_struct::error_string_void,
  };

  auto future = async::make_resolved_promise().any(methods, &test).run();
  REQUIRE_THROWS_MATCHES(future.get(), async::aggregate_error, Catch::Matchers::Message(aggregate_error_message));
}


TEST_CASE("Any with class method string string", "[any]")
{
  test_struct test;

  std::vector<std::string(test_struct::*)(std::string) const> methods
  {
    &test_struct::string_string1,
    &test_struct::string_string2,
  };

  auto future = async::make_resolved_promise(str1).any(methods, &test).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Any with class method error string string", "[any]")
{
  test_struct test;

  std::vector<std::string(test_struct::*)(std::string) const> methods
  {
    &test_struct::error_string_string,
    &test_struct::string_string,
  };

  auto future = async::make_resolved_promise(str1).any(methods, &test).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Any with class method all error string string", "[any]")
{
  test_struct test;

  std::vector<std::string(test_struct::*)(std::string) const> methods
  {
    &test_struct::error_string_string,
    &test_struct::error_string_string,
  };

  auto future = async::make_resolved_promise(str1).any(methods, &test).run();
  REQUIRE_THROWS_MATCHES(future.get(), async::aggregate_error, Catch::Matchers::Message(aggregate_error_message));
}



TEST_CASE("Any with func void void", "[any]")
{
  std::vector<void(*)()> funcs
  {
    [] () {},
    [] () {},
  };

  auto future = async::make_resolved_promise().any(funcs).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Any with func error void void", "[any]")
{
  std::vector<void(*)()> funcs
  {
    [] () { throw std::runtime_error{str2}; },
    [] () {},
  };

  auto future = async::make_resolved_promise().any(funcs).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Any with func all error void void", "[any]")
{
  std::vector<void(*)()> funcs
  {
    [] () { throw std::runtime_error{str1}; },
    [] () { throw std::runtime_error{str2}; },
  };

  auto future = async::make_resolved_promise().any(funcs).run();

  REQUIRE_THROWS_MATCHES(future.get(), async::aggregate_error, Catch::Matchers::Message(aggregate_error_message));
}


TEST_CASE("Any with func void string", "[any]")
{
  std::vector<void(*)(std::string)> funcs
  {
    [] (std::string) {},
    [] (std::string) {},
  };

  auto future = async::make_resolved_promise(str1).any(funcs).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Any with func error void string", "[any]")
{
  std::vector<void(*)(std::string)> funcs
  {
    [] (std::string) { throw std::runtime_error{str2}; },
    [] (std::string) {},
  };

  auto future = async::make_resolved_promise(str1).any(funcs).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Any with func all error void string", "[any]")
{
  std::vector<void(*)(std::string)> funcs
  {
    [] (std::string) { throw std::runtime_error{str1}; },
    [] (std::string) { throw std::runtime_error{str2}; },
  };

  auto future = async::make_resolved_promise(str1).any(funcs).run();

  REQUIRE_THROWS_MATCHES(future.get(), async::aggregate_error, Catch::Matchers::Message(aggregate_error_message));
}


TEST_CASE("Any with func string void", "[any]")
{
  std::vector<std::string(*)()> funcs
  {
    [] () { return std::string{str1}; },
    [] () { return std::string{str2}; },
  };

  auto future = async::make_resolved_promise().any(funcs).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Any with func string void ignore arg", "[any]")
{
  std::vector<std::string(*)()> funcs
  {
    [] () { return std::string{str1}; },
    [] () { return std::string{str2}; },
  };

  auto future = async::make_resolved_promise(str1).any(funcs).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Any with func error string void", "[any]")
{
  std::vector<std::string(*)()> funcs
  {
    [] () -> std::string { throw std::runtime_error{str2}; },
    [] () { return std::string{str1}; },
  };

  auto future = async::make_resolved_promise().any(funcs).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Any with func all error string void", "[any]")
{
  std::vector<std::string(*)()> funcs
  {
    [] () -> std::string { throw std::runtime_error{str1}; },
    [] () -> std::string { throw std::runtime_error{str2}; },
  };

  auto future = async::make_resolved_promise().any(funcs).run();
  REQUIRE_THROWS_MATCHES(future.get(), async::aggregate_error, Catch::Matchers::Message(aggregate_error_message));
}


TEST_CASE("Any with func string string", "[any]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    [] (std::string str) { return std::string{str1}; },
    [] (std::string str) { return std::string{str2}; },
  };

  auto future = async::make_resolved_promise(str1).any(funcs).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Any with func error string string", "[any]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    [] (std::string str) -> std::string { throw std::runtime_error{str2}; },
    [] (std::string str) { return std::string{str1}; },
  };

  auto future = async::make_resolved_promise(str1).any(funcs).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Any with func all error string string", "[any]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    [] (std::string str) -> std::string { throw std::runtime_error{str1}; },
    [] (std::string str) -> std::string { throw std::runtime_error{str2}; },
  };

  auto future = async::make_resolved_promise(str1).any(funcs).run();
  REQUIRE_THROWS_MATCHES(future.get(), async::aggregate_error, Catch::Matchers::Message(aggregate_error_message));
}
