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


TEST_CASE("All settled with class method void void", "[all_settled]")
{
  test_struct obj;

  std::vector<void(test_struct::*)() const> methods
  {
    &test_struct::void_void,
    &test_struct::void_void,
  };

  auto future = async::make_resolved_promise().all_settled(methods, &obj).run();

  std::vector<async::settled<void>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == methods.size());
  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.back().type == async::settle_type::resolved);
}


TEST_CASE("All settled with class method error void void", "[all_settled]")
{
  test_struct obj;

  std::vector<void(test_struct::*)() const> methods
  {
    &test_struct::void_void,
    &test_struct::error_void_void,
  };

  auto future = async::make_resolved_promise().all_settled(methods, &obj).run();

  std::vector<async::settled<void>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == methods.size());
  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.back().type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(res.back().error), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("All settled with class method void string", "[all_settled]")
{
  test_struct obj;

  std::vector<void(test_struct::*)(std::string) const> methods
  {
    &test_struct::void_string,
    &test_struct::void_string,
  };

  auto future = async::make_resolved_promise(str1).all_settled(methods, &obj).run();

  std::vector<async::settled<void>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == methods.size());
  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.back().type == async::settle_type::resolved);
}


TEST_CASE("All settled with class method error void string", "[all_settled]")
{
  test_struct obj;

  std::vector<void(test_struct::*)(std::string) const> methods
  {
    &test_struct::void_string,
    &test_struct::error_void_string,
  };

  auto future = async::make_resolved_promise(str1).all_settled(methods, &obj).run();

  std::vector<async::settled<void>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == methods.size());
  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.back().type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(res.back().error), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("All settled with class method string void", "[all_settled]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)() const> methods
  {
    &test_struct::string_void1,
    &test_struct::string_void2,
  };

  auto future = async::make_resolved_promise().all_settled(methods, &obj).run();

  std::vector<async::settled<std::string>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == methods.size());
  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.front().result == str1);
  REQUIRE(res.back().type == async::settle_type::resolved);
  REQUIRE(res.back().result == str2);
}


TEST_CASE("All settled with class method string void ignore arg", "[all_settled]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)() const> methods
  {
    &test_struct::string_void1,
    &test_struct::string_void2,
  };

  auto future = async::make_resolved_promise(str1).all_settled(methods, &obj).run();

  std::vector<async::settled<std::string>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == methods.size());
  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.front().result == str1);
  REQUIRE(res.back().type == async::settle_type::resolved);
  REQUIRE(res.back().result == str2);
}


TEST_CASE("All settled with class method error string void", "[all_settled]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)() const> methods
  {
    &test_struct::string_void1,
    &test_struct::error_string_void,
  };

  auto future = async::make_resolved_promise().all_settled(methods, &obj).run();

  std::vector<async::settled<std::string>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == methods.size());

  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.front().result == str1);
  REQUIRE(res.back().type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(res.back().error), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("All settled with class method string string", "[all_settled]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)(std::string) const> methods
  {
    &test_struct::string_string1,
    &test_struct::string_string2,
  };

  auto future = async::make_resolved_promise(str1).all_settled(methods, &obj).run();

  std::vector<async::settled<std::string>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == methods.size());
  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.front().result == str1);
  REQUIRE(res.back().type == async::settle_type::resolved);
  REQUIRE(res.back().result == str2);
}


TEST_CASE("All settled with class method error string string", "[all_settled]")
{
  test_struct obj;

  std::vector<std::string(test_struct::*)(std::string) const> methods
  {
    &test_struct::string_string1,
    &test_struct::error_string_string
  };

  auto future = async::make_resolved_promise(str1).all_settled(methods, &obj).run();

  std::vector<async::settled<std::string>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == methods.size());

  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.front().result == str1);
  REQUIRE(res.back().type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(res.back().error), std::runtime_error, Catch::Matchers::Message(str2));
}



TEST_CASE("All settled with func void void", "[all_settled]")
{
  std::vector<void(*)()> funcs
  {
    void_void,
    void_void,
  };

  auto future = async::make_resolved_promise().all_settled(funcs).run();

  std::vector<async::settled<void>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == funcs.size());
  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.back().type == async::settle_type::resolved);
}


TEST_CASE("All settled with func error void void", "[all_settled]")
{
  std::vector<void(*)()> funcs
  {
    void_void,
    error_void_void,
  };

  auto future = async::make_resolved_promise().all_settled(funcs).run();

  std::vector<async::settled<void>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == funcs.size());
  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.back().type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(res.back().error), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("All settled with func void string", "[all_settled]")
{
  std::vector<void(*)(std::string)> funcs
  {
    void_string,
    void_string,
  };

  auto future = async::make_resolved_promise(str1).all_settled(funcs).run();

  std::vector<async::settled<void>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == funcs.size());
  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.back().type == async::settle_type::resolved);
}


TEST_CASE("All settled with func error void string", "[all_settled]")
{
  std::vector<void(*)(std::string)> funcs
  {
    void_string,
    error_void_string,
  };

  auto future = async::make_resolved_promise(str1).all_settled(funcs).run();

  std::vector<async::settled<void>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == funcs.size());
  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.back().type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(res.back().error), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("All settled with func string void", "[all_settled]")
{
  std::vector<std::string(*)()> funcs
  {
    string_void1,
    string_void2,
  };

  auto future = async::make_resolved_promise().all_settled(funcs).run();

  std::vector<async::settled<std::string>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == funcs.size());
  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.front().result == str1);
  REQUIRE(res.back().type == async::settle_type::resolved);
  REQUIRE(res.back().result == str2);
}


TEST_CASE("All settled with func string void ignore arg", "[all_settled]")
{
  std::vector<std::string(*)()> funcs
  {
    string_void1,
    string_void2,
  };

  auto future = async::make_resolved_promise(str1).all_settled(funcs).run();

  std::vector<async::settled<std::string>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == funcs.size());
  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.front().result == str1);
  REQUIRE(res.back().type == async::settle_type::resolved);
  REQUIRE(res.back().result == str2);
}


TEST_CASE("All settled with func error string void", "[all_settled]")
{
  std::vector<std::string(*)()> funcs
  {
    string_void1,
    error_string_void,
  };

  auto future = async::make_resolved_promise().all_settled(funcs).run();

  std::vector<async::settled<std::string>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == funcs.size());

  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.front().result == str1);
  REQUIRE(res.back().type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(res.back().error), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("All settled with func string string", "[all_settled]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    string_string1,
    string_string2,
  };

  auto future = async::make_resolved_promise(str1).all_settled(funcs).run();

  std::vector<async::settled<std::string>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == funcs.size());
  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.front().result == str1);
  REQUIRE(res.back().type == async::settle_type::resolved);
  REQUIRE(res.back().result == str2);
}


TEST_CASE("All settled with func error string string", "[all_settled]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    string_string1,
    error_string_string,
  };

  auto future = async::make_resolved_promise(str1).all_settled(funcs).run();

  std::vector<async::settled<std::string>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == funcs.size());

  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.front().result == str1);
  REQUIRE(res.back().type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(res.back().error), std::runtime_error, Catch::Matchers::Message(str2));
}
