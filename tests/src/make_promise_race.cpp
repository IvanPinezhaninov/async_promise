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


TEST_CASE("Make race with class method void void", "[make promise race]")
{
  test_struct obj;

  std::vector<void(test_struct::*)() const> methods
  {
    &test_struct::void_void,
    &test_struct::void_void,
  };

  auto future = async::make_promise_race(methods, &obj).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make race with class method error won void void", "[make promise race]")
{
  test_struct obj;

  std::vector<void(test_struct::*)() const> methods
  {
    &test_struct::error_void_void,
    &test_struct::void_void_delayed,
  };

  auto future = async::make_promise_race(methods, &obj).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Make race with class method error lose void void", "[make promise race]")
{
  test_struct obj;

  std::vector<void(test_struct::*)() const> methods
  {
    &test_struct::void_void,
    &test_struct::error_void_void_delayed,
  };

  auto future = async::make_promise_race(methods, &obj).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make race with class method void string", "[make promise race]")
{
  test_struct obj;

  std::vector<void(test_struct::*)(std::string) const> methods
  {
    &test_struct::void_string,
    &test_struct::void_string,
  };

  auto future = async::make_promise_race(methods, &obj, str1).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make race with class method error won void string", "[make promise race]")
{
  test_struct obj;

  std::vector<void(test_struct::*)(std::string) const> methods
  {
    &test_struct::error_void_string,
    &test_struct::void_string_delayed,
  };

  auto future = async::make_promise_race(methods, &obj, str1).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Make race with class method error lose void string", "[make promise race]")
{
  test_struct obj;

  std::vector<void(test_struct::*)(std::string) const> methods
  {
    &test_struct::void_string,
    &test_struct::error_void_string_delayed,
  };

  auto future = async::make_promise_race(methods, &obj, str1).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make race with class method string void", "[make promise race]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)() const> methods
  {
    &test_struct::string_void1,
    &test_struct::string_void2,
  };

  auto future = async::make_promise_race(methods, &obj).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Make race with class method error won string void", "[make promise race]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)() const> methods
  {
    &test_struct::error_string_void,
        &test_struct::string_void_delayed,
  };

  auto future = async::make_promise_race(methods, &obj).run();

  std::string res;
  REQUIRE_THROWS_MATCHES(res = future.get(), std::runtime_error, Catch::Matchers::Message(str2));
  REQUIRE(res.empty());
}


TEST_CASE("Make race with class method error lose string void", "[make promise race]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)() const> methods
  {
    &test_struct::string_void1,
    &test_struct::error_string_void_delayed,
  };

  auto future = async::make_promise_race(methods, &obj).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Make race with class method string string", "[make promise race]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)(std::string) const> methods
  {
    &test_struct::string_string1,
    &test_struct::string_string2,
  };

  auto future = async::make_promise_race(methods, &obj, str1).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Make race with class method error won string string", "[make promise race]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)(std::string) const> methods
  {
    &test_struct::error_string_string,
        &test_struct::string_string_delayed,
  };

  auto future = async::make_promise_race(methods, &obj, str1).run();

  std::string res;
  REQUIRE_THROWS_MATCHES(res = future.get(), std::runtime_error, Catch::Matchers::Message(str2));
  REQUIRE(res.empty());
}


TEST_CASE("Make race with class method error lose string string", "[make promise race]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)(std::string) const> methods
  {
    &test_struct::string_string1,
    &test_struct::error_string_string_delayed,
  };

  auto future = async::make_promise_race(methods, &obj, str1).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}



TEST_CASE("Make race with func void void", "[make promise race]")
{
  std::vector<void(*)()> funcs
  {
    void_void,
    void_void,
  };

  auto future = async::make_promise_race(funcs).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make race with func error won void void", "[make promise race]")
{
  std::vector<void(*)()> funcs
  {
    void_void_delayed,
    error_void_void,
  };

  auto future = async::make_promise_race(funcs).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Make race with func error lose void void", "[make promise race]")
{
  std::vector<void(*)()> funcs
  {
    void_void,
    error_void_void_delayed,
  };

  auto future = async::make_promise_race(funcs).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make race with func void string", "[make promise race]")
{
  std::vector<void(*)(std::string)> funcs
  {
    void_string,
    void_string,
  };

  auto future = async::make_promise_race(funcs, str1).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make race with func error won void string", "[make promise race]")
{
  std::vector<void(*)(std::string)> funcs
  {
    void_string_delayed,
    error_void_string,
  };

  auto future = async::make_promise_race(funcs, str1).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Make race with func error lose void string", "[make promise race]")
{
  std::vector<void(*)(std::string)> funcs
  {
    void_string,
    error_void_string_delayed,
  };

  auto future = async::make_promise_race(funcs, str1).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make race with func string void", "[make promise race]")
{
  std::vector<std::string(*)()> funcs
  {
    string_void1,
    string_void2,
  };

  auto future = async::make_promise_race(funcs).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Make race with func error won string void", "[make promise race]")
{
  std::vector<std::string(*)()> funcs
  {
    string_void_delayed,
    error_string_void,
  };

  auto future = async::make_promise_race(funcs).run();

  std::string res;
  REQUIRE_THROWS_MATCHES(res = future.get(), std::runtime_error, Catch::Matchers::Message(str2));
  REQUIRE(res.empty());
}


TEST_CASE("Make race with func error lose string void", "[make promise race]")
{
  std::vector<std::string(*)()> funcs
  {
    string_void1,
    error_string_void_delayed,
  };

  auto future = async::make_promise_race(funcs).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Make race with func string string", "[make promise race]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    string_string1,
    string_string2,
  };

  auto future = async::make_promise_race(funcs, str1).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Make race with func error won string string", "[make promise race]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    string_string_delayed,
    error_string_string,
  };

  auto future = async::make_promise_race(funcs, str1).run();

  std::string res;
  REQUIRE_THROWS_MATCHES(res = future.get(), std::runtime_error, Catch::Matchers::Message(str2));
  REQUIRE(res.empty());
}


TEST_CASE("Make race with func error lose string string", "[make promise race]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    string_string1,
    error_string_string_delayed,
  };

  auto future = async::make_promise_race(funcs, str1).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}
