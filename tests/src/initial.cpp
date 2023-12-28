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


TEST_CASE("Initial with class method void void", "[initial]")
{
  test_struct obj;
  auto future = async::promise<void>{&test_struct::void_void, &obj}.run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Initial with class method error void void", "[initial]")
{
  test_struct obj;
  auto future = async::promise<void>{&test_struct::error_void_void, &obj}.run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Initial with class method void string", "[initial]")
{
  test_struct obj;
  auto future = async::promise<void>{&test_struct::void_string, &obj, str1}.run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Initial with class method error void string", "[initial]")
{
  test_struct obj;
  auto future = async::promise<void>{&test_struct::error_void_string, &obj, str1}.run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Initial with class method string void", "[initial]")
{
  test_struct obj;
  auto future = async::promise<std::string>{&test_struct::string_void1, &obj}.run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Initial with class method error string void", "[initial]")
{
  test_struct obj;
  auto future = async::promise<std::string>{&test_struct::error_string_void, &obj}.run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Initial with class method string string", "[initial]")
{
  test_struct obj;
  auto future = async::promise<std::string>{&test_struct::string_string1, &obj, str1}.run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Initial with class method error string string", "[initial]")
{
  test_struct obj;
  auto future = async::promise<std::string>{&test_struct::error_string_string, &obj, str1}.run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Initial with func void void", "[initial]")
{
  auto future = async::promise<void>{void_void}.run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Initial with func error void void", "[initial]")
{
  auto future = async::promise<void>{error_void_void}.run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Initial with func void string", "[initial]")
{
  auto future = async::promise<void>{void_string, str1}.run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Initial with func error void string", "[initial]")
{
  auto future = async::promise<void>{error_void_string, str1}.run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Initial with func string void", "[initial]")
{
  auto future = async::promise<std::string>{string_void1}.run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Initial with func error string void", "[initial]")
{
  auto future = async::promise<std::string>{error_string_void}.run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Initial with func string string", "[initial]")
{
  auto future = async::promise<std::string>{string_string1, str1}.run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Initial with func error string string", "[initial]")
{
  auto future = async::promise<std::string>{error_string_string, str1}.run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}

