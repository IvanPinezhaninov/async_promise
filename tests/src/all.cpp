/******************************************************************************
**
** Copyright (C) 2023 Ivan Pinezhaninov <ivan.pinezhaninov@gmail.com>
**
** This file is part of the async_promise project - which can be found at
** https://github.com/IvanPinezhaninov/async_promise/.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ALL KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ALL CLAIM,
** DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
** OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
** THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
******************************************************************************/

// local
#include "common.h"


TEST_CASE("All with class method void void", "[all]")
{
  test_struct obj;

  std::vector<void(test_struct::*)() const> methods
  {
    &test_struct::void_void,
    &test_struct::void_void,
  };

  auto future = async::make_resolved_promise().all(methods, &obj).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("All with class method error void void", "[all]")
{
  test_struct obj;

  std::vector<void(test_struct::*)() const> methods
  {
    &test_struct::void_void,
    &test_struct::error_void_void,
  };

  auto future = async::make_resolved_promise().all(methods, &obj).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("All with class method void string", "[all]")
{
  test_struct obj;

  std::vector<void(test_struct::*)(std::string) const> methods
  {
    &test_struct::void_string,
    &test_struct::void_string,
  };

  auto future = async::make_resolved_promise(str1).all(methods, &obj).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("All with class method error void string", "[all]")
{
  test_struct obj;

  std::vector<void(test_struct::*)(std::string) const> methods
  {
    &test_struct::void_string,
    &test_struct::error_void_string,
  };

  auto future = async::make_resolved_promise(str1).all(methods, &obj).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("All with class method string void", "[all]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)() const> methods
  {
    &test_struct::string_void1,
    &test_struct::string_void2,
  };

  auto future = async::make_resolved_promise().all(methods, &obj).run();

  std::vector<std::string> res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::RangeEquals(std::vector<std::string>{str1, str2}));
}


TEST_CASE("All with class method string void ignore arg", "[all]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)() const> methods
  {
    &test_struct::string_void1,
    &test_struct::string_void2,
  };

  auto future = async::make_resolved_promise(str1).all(methods, &obj).run();

  std::vector<std::string> res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::RangeEquals(std::vector<std::string>{str1, str2}));
}


TEST_CASE("All with class method error string void", "[all]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)() const> methods
  {
    &test_struct::string_void1,
    &test_struct::error_string_void,
  };

  auto future = async::make_resolved_promise().all(methods, &obj).run();

  std::vector<std::string> res;
  REQUIRE_THROWS_MATCHES(res = future.get(), std::runtime_error, Catch::Matchers::Message(str2));
  REQUIRE(res.empty());
}


TEST_CASE("All with class method string string", "[all]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)(std::string) const> methods
  {
    &test_struct::string_string1,
    &test_struct::string_string2,
  };

  auto future = async::make_resolved_promise(str1).all(methods, &obj).run();

  std::vector<std::string> res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::RangeEquals(std::vector<std::string>{str1, str2}));
}


TEST_CASE("All with class method error string string", "[all]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)(std::string) const> methods
  {
    &test_struct::string_string1,
    &test_struct::error_string_string
  };

  auto future = async::make_resolved_promise(str1).all(methods, &obj).run();

  std::vector<std::string> res;
  REQUIRE_THROWS_MATCHES(res = future.get(), std::runtime_error, Catch::Matchers::Message(str2));
  REQUIRE(res.empty());
}



TEST_CASE("All with function void void", "[all]")
{
  std::vector<void(*)()> funcs
  {
    void_void,
    void_void,
  };

  auto future = async::make_resolved_promise().all(funcs).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("All with function error void void", "[all]")
{
  std::vector<void(*)()> funcs
  {
    void_void,
    error_void_void,
  };

  auto future = async::make_resolved_promise().all(funcs).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("All with function void string", "[all]")
{
  std::vector<void(*)(std::string)> funcs
  {
    void_string,
    void_string,
  };

  auto future = async::make_resolved_promise(str1).all(funcs).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("All with function error void string", "[all]")
{
  std::vector<void(*)(std::string)> funcs
  {
    void_string,
    error_void_string,
  };

  auto future = async::make_resolved_promise(str1).all(funcs).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("All with function string void", "[all]")
{
  std::vector<std::string(*)()> funcs
  {
    string_void1,
    string_void2,
  };

  auto future = async::make_resolved_promise().all(funcs).run();

  std::vector<std::string> res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::RangeEquals(std::vector<std::string>{str1, str2}));
}


TEST_CASE("All with function string void ignore arg", "[all]")
{
  std::vector<std::string(*)()> funcs
  {
    string_void1,
    string_void2,
  };

  auto future = async::make_resolved_promise(str1).all(funcs).run();

  std::vector<std::string> res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::RangeEquals(std::vector<std::string>{str1, str2}));
}


TEST_CASE("All with function error string void", "[all]")
{
  std::vector<std::string(*)()> funcs
  {
    string_void1,
    error_string_void,
  };

  auto future = async::make_resolved_promise().all(funcs).run();

  std::vector<std::string> res;
  REQUIRE_THROWS_MATCHES(res = future.get(), std::runtime_error, Catch::Matchers::Message(str2));
  REQUIRE(res.empty());
}


TEST_CASE("All with function string string", "[all]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    string_string1,
    string_string2,
  };

  auto future = async::make_resolved_promise(str1).all(funcs).run();

  std::vector<std::string> res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::RangeEquals(std::vector<std::string>{str1, str2}));
}


TEST_CASE("All with function error string string", "[all]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    string_string1,
    error_string_string,
  };

  auto future = async::make_resolved_promise(str1).all(funcs).run();

  std::vector<std::string> res;
  REQUIRE_THROWS_MATCHES(res = future.get(), std::runtime_error, Catch::Matchers::Message(str2));
  REQUIRE(res.empty());
}
