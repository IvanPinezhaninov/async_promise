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

// local
#include "common.h"


TEST_CASE("Then with class method void void", "[then]")
{
  test_struct obj;
  auto future = async::make_resolved_promise().then(&test_struct::void_void, &obj).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Then with class method error void void", "[then]")
{
  test_struct obj;
  auto future = async::make_resolved_promise().then(&test_struct::error_void_void, &obj).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Then with class method void void ignore arg", "[then]")
{
  test_struct obj;
  auto future = async::make_resolved_promise(str1).then(&test_struct::void_void, &obj).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Then with class method error void void ignore arg", "[then]")
{
  test_struct obj;
  auto future = async::make_resolved_promise(str1).then(&test_struct::error_void_void, &obj).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Then with class method void string", "[then]")
{
  test_struct obj;
  auto future = async::make_resolved_promise(str1).then(&test_struct::void_string, &obj).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Then with class method error void string", "[then]")
{
  test_struct obj;
  auto future = async::make_resolved_promise(str1).then(&test_struct::error_void_string, &obj).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Then with class method string void", "[then]")
{
  test_struct obj;
  auto future = async::make_resolved_promise().then(&test_struct::string_void1, &obj).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Then with class method error string void", "[then]")
{
  test_struct obj;
  auto future = async::make_resolved_promise().then(&test_struct::error_string_void, &obj).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Then with class method string void ignore arg", "[then]")
{
  test_struct obj;
  auto future = async::make_resolved_promise(str1).then(&test_struct::string_void1, &obj).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Then with class method error string void ignore arg", "[then]")
{
  test_struct obj;
  auto future = async::make_resolved_promise(str1).then(&test_struct::error_string_void, &obj).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Then with class method string string", "[then]")
{
  test_struct obj;
  auto future = async::make_resolved_promise(str1).then(&test_struct::string_string1, &obj).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Then with class method error string string", "[then]")
{
  test_struct obj;
  auto future = async::make_resolved_promise(str1).then(&test_struct::error_string_string, &obj).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Then with func void void", "[then]")
{
  auto future = async::make_resolved_promise().then(void_void).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Then with func error void void", "[then]")
{
  auto future = async::make_resolved_promise().then(error_void_void).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Then with func void void ignore arg", "[then]")
{
  auto future = async::make_resolved_promise(str1).then(void_void).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Then with func error void void ignore arg", "[then]")
{
  auto future = async::make_resolved_promise(str1).then(error_void_void).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Then with func void string", "[then]")
{
  auto future = async::make_resolved_promise(str1).then(void_string).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Then with func error void string", "[then]")
{
  auto future = async::make_resolved_promise(str1).then(error_void_string).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Then with func string void", "[then]")
{
  auto future = async::make_resolved_promise().then(string_void1).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Then with func error string void", "[then]")
{
  auto future = async::make_resolved_promise().then(error_string_void).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Then with func string void ignore arg", "[then]")
{
  auto future = async::make_resolved_promise(str1).then(string_void1).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Then with func error string void ignore arg", "[then]")
{
  auto future = async::make_resolved_promise(str1).then(error_string_void).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Then with func string string", "[then]")
{
  auto future = async::make_resolved_promise(str1).then(string_string1).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Then with func error string string", "[then]")
{
  auto future = async::make_resolved_promise(str1).then(error_string_string).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}
