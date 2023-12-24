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

static constexpr auto str = "Hello World!";


TEST_CASE("Func then void void", "[then]")
{
  auto future = async::make_resolved_promise().then(void_void).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Func then error void void", "[then]")
{
  auto future = async::make_resolved_promise().then(error_void_void).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str));
}


TEST_CASE("Func then void void ignore arg", "[then]")
{
  auto future = async::make_resolved_promise(str).then(void_void).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Func then error void void ignore arg", "[then]")
{
  auto future = async::make_resolved_promise(str).then(error_void_void).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str));
}


TEST_CASE("Func then void string", "[then]")
{
  auto future = async::make_resolved_promise(str).then(void_string).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Func then error void string", "[then]")
{
  auto future = async::make_resolved_promise(str).then(error_void_string).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str));
}


TEST_CASE("Func then string void", "[then]")
{
  auto future = async::make_resolved_promise().then(string_void).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str);
}


TEST_CASE("Func then error string void", "[then]")
{
  auto future = async::make_resolved_promise().then(error_string_void).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str));
}


TEST_CASE("Func then string void ignore arg", "[then]")
{
  auto future = async::make_resolved_promise(str).then(string_void).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str);
}


TEST_CASE("Func then error string void ignore arg", "[then]")
{
  auto future = async::make_resolved_promise(str).then(error_string_void).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str));
}


TEST_CASE("Func then string string", "[then]")
{
  auto future = async::make_resolved_promise(str).then(string_string).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str);
}


TEST_CASE("Func then error string string", "[then]")
{
  auto future = async::make_resolved_promise(str).then(error_string_string).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str));
}
