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

// async_promise
#include <async_promise.hpp>

// catch2
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

// local
#include "common.h"


TEST_CASE("All settled void void", "[all_settled]")
{
  std::vector<void(*)()> funcs
  {
    [] () {},
    [] () {},
  };

  auto future = async::make_resolved_promise().all_settled(funcs).run();

  std::vector<async::settled<void>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == funcs.size());
  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.back().type == async::settle_type::resolved);
}


TEST_CASE("All settled error void void", "[all_settled]")
{
  std::vector<void(*)()> funcs
  {
    [] () {},
    [] () { throw std::runtime_error{str2}; },
  };

  auto future = async::make_resolved_promise().all_settled(funcs).run();

  std::vector<async::settled<void>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == funcs.size());
  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.back().type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(res.back().error), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("All settled void string", "[all_settled]")
{
  std::vector<void(*)(std::string)> funcs
  {
    [] (std::string) {},
    [] (std::string) {},
  };

  auto future = async::make_resolved_promise(str1).all_settled(funcs).run();

  std::vector<async::settled<void>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == funcs.size());
  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.back().type == async::settle_type::resolved);
}


TEST_CASE("All settled error void string", "[all_settled]")
{
  std::vector<void(*)(std::string)> funcs
  {
    [] (std::string) {},
    [] (std::string) { throw std::runtime_error{str2}; },
  };

  auto future = async::make_resolved_promise(str1).all_settled(funcs).run();

  std::vector<async::settled<void>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == funcs.size());
  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.back().type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(res.back().error), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("All settled string void", "[all_settled]")
{
  std::vector<std::string(*)()> funcs
  {
    [] () { return std::string{str1}; },
    [] () { return std::string{str2}; },
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


TEST_CASE("All settled string void ignore arg", "[all_settled]")
{
  std::vector<std::string(*)()> funcs
  {
    [] () { return std::string{str1}; },
    [] () { return std::string{str2}; },
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


TEST_CASE("All settled error string void", "[all_settled]")
{
  std::vector<std::string(*)()> funcs
  {
    [] () { return std::string{str1}; },
    [] () -> std::string { throw std::runtime_error{str2}; },
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


TEST_CASE("All settled string string", "[all_settled]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    [] (std::string str) { return str; },
    [] (std::string str) { return str; },
  };

  auto future = async::make_resolved_promise(str1).all_settled(funcs).run();

  std::vector<async::settled<std::string>> res;
  REQUIRE_NOTHROW(res = future.get());

  REQUIRE(res.size() == funcs.size());
  REQUIRE(res.front().type == async::settle_type::resolved);
  REQUIRE(res.front().result == str1);
  REQUIRE(res.back().type == async::settle_type::resolved);
  REQUIRE(res.back().result == str1);
}


TEST_CASE("All settled error string string", "[all_settled]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    [] (std::string str) { return std::string{str1}; },
    [] (std::string str) -> std::string { throw std::runtime_error{str2}; },
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
