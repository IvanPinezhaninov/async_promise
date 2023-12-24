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
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/catch_test_macros.hpp>

// local
#include "common.h"


TEST_CASE("Make promise with class method void void", "[make promise]")
{
  test_struct test;
  auto future = async::make_promise(&test_struct::void_void, &test).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make promise with class method error void void", "[make promise]")
{
  test_struct test;
  auto future = async::make_promise(&test_struct::error_void_void, &test).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str1));
}


TEST_CASE("Make promise with class method void string", "[make promise]")
{
  test_struct test;
  auto future = async::make_promise(&test_struct::void_string, &test, str1).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make promise with class method error void string", "[make promise]")
{
  test_struct test;
  auto future = async::make_promise(&test_struct::error_void_string, &test, str1).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str1));
}


TEST_CASE("Make promise with class method string void", "[make promise]")
{
  test_struct test;
  auto future = async::make_promise(&test_struct::string_void, &test).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Make promise with class method error string void", "[make promise]")
{
  test_struct test;
  auto future = async::make_promise(&test_struct::error_string_void, &test).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str1));
}


TEST_CASE("Make promise with class method string string", "[make promise]")
{
  test_struct test;
  auto future = async::make_promise(&test_struct::string_string, &test, str1).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Make promise with class method error string string", "[make promise]")
{
  test_struct test;
  auto future = async::make_promise(&test_struct::error_string_string, &test, str1).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str1));
}


TEST_CASE("Make promise with func void void", "[make promise]")
{
  auto future = async::make_promise(void_void).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make promise with func error void void", "[make promise]")
{
  auto future = async::make_promise(error_void_void).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str1));
}


TEST_CASE("Make promise with func void string", "[make promise]")
{
  auto future = async::make_promise(void_string, str1).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make promise with func error void string", "[make promise]")
{
  auto future = async::make_promise(error_void_string, str1).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str1));
}


TEST_CASE("Make promise with func string void", "[make promise]")
{
  auto future = async::make_promise(string_void).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Make promise with func error string void", "[make promise]")
{
  auto future = async::make_promise(error_string_void).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str1));
}


TEST_CASE("Make promise with func string string", "[make promise]")
{
  auto future = async::make_promise(string_string, str1).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Make promise with func error string string", "[make promise]")
{
  auto future = async::make_promise(error_string_string, str1).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str1));
}
