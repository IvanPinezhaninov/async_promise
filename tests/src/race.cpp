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


TEST_CASE("Race with class method void void", "[race]")
{
  test_struct obj;

  std::vector<void(test_struct::*)() const> methods
  {
    &test_struct::void_void,
    &test_struct::void_void,
  };

  auto future = async::make_resolved_promise().race(methods, &obj).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Race with class method error won void void", "[race]")
{
  test_struct obj;

  std::vector<void(test_struct::*)() const> methods
  {
    &test_struct::error_void_void,
    &test_struct::void_void_delayed,
  };

  auto future = async::make_resolved_promise().race(methods, &obj).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Race with class method error lose void void", "[race]")
{
  test_struct obj;

  std::vector<void(test_struct::*)() const> methods
  {
    &test_struct::void_void,
    &test_struct::error_void_void_delayed,
  };

  auto future = async::make_resolved_promise().race(methods, &obj).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Race with class method void string", "[race]")
{
  test_struct obj;

  std::vector<void(test_struct::*)(std::string) const> methods
  {
    &test_struct::void_string,
    &test_struct::void_string,
  };

  auto future = async::make_resolved_promise(str1).race(methods, &obj).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Race with class method error won void string", "[race]")
{
  test_struct obj;

  std::vector<void(test_struct::*)(std::string) const> methods
  {
    &test_struct::error_void_string,
    &test_struct::void_string_delayed,
  };

  auto future = async::make_resolved_promise(str1).race(methods, &obj).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Race with class method error lose void string", "[race]")
{
  test_struct obj;

  std::vector<void(test_struct::*)(std::string) const> methods
  {
    &test_struct::void_string,
    &test_struct::error_void_string_delayed,
  };

  auto future = async::make_resolved_promise(str1).race(methods, &obj).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Race with class method string void", "[race]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)() const> methods
  {
    &test_struct::string_void1,
    &test_struct::string_void2,
  };

  auto future = async::make_resolved_promise().race(methods, &obj).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Race with class method string void ignore arg", "[race]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)() const> methods
  {
    &test_struct::string_void1,
    &test_struct::string_void2,
  };

  auto future = async::make_resolved_promise(str1).race(methods, &obj).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Race with class method error won string void", "[race]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)() const> methods
  {
    &test_struct::error_string_void,
        &test_struct::string_void_delayed,
  };

  auto future = async::make_resolved_promise().race(methods, &obj).run();

  std::string res;
  REQUIRE_THROWS_MATCHES(res = future.get(), std::runtime_error, Catch::Matchers::Message(str2));
  REQUIRE(res.empty());
}


TEST_CASE("Race with class method error lose string void", "[race]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)() const> methods
  {
    &test_struct::string_void1,
    &test_struct::error_string_void_delayed,
  };

  auto future = async::make_resolved_promise().race(methods, &obj).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Race with class method string string", "[race]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)(std::string) const> methods
  {
    &test_struct::string_string1,
    &test_struct::string_string2,
  };

  auto future = async::make_resolved_promise(str1).race(methods, &obj).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Race with class method error won string string", "[race]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)(std::string) const> methods
  {
    &test_struct::error_string_string,
        &test_struct::string_string_delayed,
  };

  auto future = async::make_resolved_promise(str1).race(methods, &obj).run();

  std::string res;
  REQUIRE_THROWS_MATCHES(res = future.get(), std::runtime_error, Catch::Matchers::Message(str2));
  REQUIRE(res.empty());
}


TEST_CASE("Race with class method error lose string string", "[race]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)(std::string) const> methods
  {
    &test_struct::string_string1,
    &test_struct::error_string_string_delayed,
  };

  auto future = async::make_resolved_promise(str1).race(methods, &obj).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}



TEST_CASE("Race with func void void", "[race]")
{
  std::vector<void(*)()> funcs
  {
    void_void,
    void_void,
  };

  auto future = async::make_resolved_promise().race(funcs).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Race with func error won void void", "[race]")
{
  std::vector<void(*)()> funcs
  {
    error_void_void,
    void_void_delayed,
  };

  auto future = async::make_resolved_promise().race(funcs).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Race with func error lose void void", "[race]")
{
  std::vector<void(*)()> funcs
  {
    void_void,
    error_void_void_delayed,
  };

  auto future = async::make_resolved_promise().race(funcs).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Race with func void string", "[race]")
{
  std::vector<void(*)(std::string)> funcs
  {
    void_string,
    void_string,
  };

  auto future = async::make_resolved_promise(str1).race(funcs).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Race with func error won void string", "[race]")
{
  std::vector<void(*)(std::string)> funcs
  {
    error_void_string,
    void_string_delayed,
  };

  auto future = async::make_resolved_promise(str1).race(funcs).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Race with func error lose void string", "[race]")
{
  std::vector<void(*)(std::string)> funcs
  {
    void_string,
    error_void_string_delayed,
  };

  auto future = async::make_resolved_promise(str1).race(funcs).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Race with func string void", "[race]")
{
  std::vector<std::string(*)()> funcs
  {
    string_void1,
    string_void2,
  };

  auto future = async::make_resolved_promise().race(funcs).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Race with func string void ignore arg", "[race]")
{
  std::vector<std::string(*)()> funcs
  {
    string_void1,
    string_void2,
  };

  auto future = async::make_resolved_promise(str1).race(funcs).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Race with func error won string void", "[race]")
{
  std::vector<std::string(*)()> funcs
  {
    error_string_void,
        string_void_delayed,
  };

  auto future = async::make_resolved_promise().race(funcs).run();

  std::string res;
  REQUIRE_THROWS_MATCHES(res = future.get(), std::runtime_error, Catch::Matchers::Message(str2));
  REQUIRE(res.empty());
}


TEST_CASE("Race with func error lose string void", "[race]")
{
  std::vector<std::string(*)()> funcs
  {
    string_void1,
    error_string_void_delayed,
  };

  auto future = async::make_resolved_promise().race(funcs).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Race with func string string", "[race]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    string_string1,
    string_string2,
  };

  auto future = async::make_resolved_promise(str1).race(funcs).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Race with func error won string string", "[race]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    error_string_string,
    string_string_delayed,
  };

  auto future = async::make_resolved_promise(str1).race(funcs).run();

  std::string res;
  REQUIRE_THROWS_MATCHES(res = future.get(), std::runtime_error, Catch::Matchers::Message(str2));
  REQUIRE(res.empty());
}


TEST_CASE("Race with func error lose string string", "[race]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    string_string1,
    error_string_string_delayed,
  };

  auto future = async::make_resolved_promise(str1).race(funcs).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}
