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
#include "test_funcs.h"
#include "test_struct.h"

static constexpr auto str = "Hello World!";


TEST_CASE("Then with class method void void", "[then]")
{
  test_struct test;
  auto future = async::make_resolved_promise().then(&test_struct::void_void, &test).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Then with class method error void void", "[then]")
{
  test_struct test;
  auto future = async::make_resolved_promise().then(&test_struct::error_void_void, &test).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str));
}


TEST_CASE("Then with class method void void ignore arg", "[then]")
{
  test_struct test;
  auto future = async::make_resolved_promise(str).then(&test_struct::void_void, &test).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Then with class method error void void ignore arg", "[then]")
{
  test_struct test;
  auto future = async::make_resolved_promise(str).then(&test_struct::error_void_void, &test).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str));
}


TEST_CASE("Then with class method void string", "[then]")
{
  test_struct test;
  auto future = async::make_resolved_promise(str).then(&test_struct::void_string, &test).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Then with class method error void string", "[then]")
{
  test_struct test;
  auto future = async::make_resolved_promise(str).then(&test_struct::error_void_string, &test).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str));
}


TEST_CASE("Then with class method string void", "[then]")
{
  test_struct test;
  auto future = async::make_resolved_promise().then(&test_struct::string_void, &test).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str);
}


TEST_CASE("Then with class method error string void", "[then]")
{
  test_struct test;
  auto future = async::make_resolved_promise().then(&test_struct::error_string_void, &test).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str));
}


TEST_CASE("Then with class method string void ignore arg", "[then]")
{
  test_struct test;
  auto future = async::make_resolved_promise(str).then(&test_struct::string_void, &test).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str);
}


TEST_CASE("Then with class method error string void ignore arg", "[then]")
{
  test_struct test;
  auto future = async::make_resolved_promise(str).then(&test_struct::error_string_void, &test).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str));
}


TEST_CASE("Then with class method string string", "[then]")
{
  test_struct test;
  auto future = async::make_resolved_promise(str).then(&test_struct::string_string, &test).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str);
}


TEST_CASE("Then with class method error string string", "[then]")
{
  test_struct test;
  auto future = async::make_resolved_promise(str).then(&test_struct::error_string_string, &test).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str));
}


TEST_CASE("Then with func void void", "[then]")
{
  auto future = async::make_resolved_promise().then(void_void).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Then with func error void void", "[then]")
{
  auto future = async::make_resolved_promise().then(error_void_void).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str));
}


TEST_CASE("Then with func void void ignore arg", "[then]")
{
  auto future = async::make_resolved_promise(str).then(void_void).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Then with func error void void ignore arg", "[then]")
{
  auto future = async::make_resolved_promise(str).then(error_void_void).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str));
}


TEST_CASE("Then with func void string", "[then]")
{
  auto future = async::make_resolved_promise(str).then(void_string).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Then with func error void string", "[then]")
{
  auto future = async::make_resolved_promise(str).then(error_void_string).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str));
}


TEST_CASE("Then with func string void", "[then]")
{
  auto future = async::make_resolved_promise().then(string_void).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str);
}


TEST_CASE("Then with func error string void", "[then]")
{
  auto future = async::make_resolved_promise().then(error_string_void).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str));
}


TEST_CASE("Then with func string void ignore arg", "[then]")
{
  auto future = async::make_resolved_promise(str).then(string_void).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str);
}


TEST_CASE("Then with func error string void ignore arg", "[then]")
{
  auto future = async::make_resolved_promise(str).then(error_string_void).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str));
}


TEST_CASE("Then with func string string", "[then]")
{
  auto future = async::make_resolved_promise(str).then(string_string).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str);
}


TEST_CASE("Then with func error string string", "[then]")
{
  auto future = async::make_resolved_promise(str).then(error_string_string).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str));
}
