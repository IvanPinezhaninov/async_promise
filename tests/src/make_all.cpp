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
#include <catch2/matchers/catch_matchers_range_equals.hpp>

// local
#include "common.h"


TEST_CASE("Make all void void", "[make all]")
{
  std::vector<void(*)()> funcs
  {
    [] () {},
    [] () {},
  };

  auto future = async::make_promise_all(funcs).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make all error void void", "[make all]")
{
  std::vector<void(*)()> funcs
  {
    [] () {},
    [] () { throw std::runtime_error{str2}; },
  };

  auto future = async::make_promise_all(funcs).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Make all void string", "[make all]")
{
  std::vector<void(*)(std::string)> funcs
  {
    [] (std::string) {},
    [] (std::string) {},
  };

  auto future = async::make_promise_all(funcs, str1).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make all error void string", "[make all]")
{
  std::vector<void(*)(std::string)> funcs
  {
    [] (std::string) {},
    [] (std::string) { throw std::runtime_error{str2}; },
  };

  auto future = async::make_promise_all(funcs, str1).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Make all string void", "[make all]")
{
  std::vector<std::string(*)()> funcs
  {
    [] () { return std::string{str1}; },
    [] () { return std::string{str2}; },
  };

  auto future = async::make_promise_all(funcs).run();

  std::vector<std::string> res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::RangeEquals(std::vector<std::string>{str1, str2}));
}


TEST_CASE("Make all error string void", "[make all]")
{
  std::vector<std::string(*)()> funcs
  {
    [] () { return std::string{str1}; },
    [] () -> std::string { throw std::runtime_error{str2}; },
  };

  auto future = async::make_promise_all(funcs).run();

  std::vector<std::string> res;
  REQUIRE_THROWS_MATCHES(res = future.get(), std::runtime_error, Catch::Matchers::Message(str2));
  REQUIRE(res.empty());
}


TEST_CASE("Make all string string", "[make all]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    [] (std::string str) { return std::string{str1}; },
    [] (std::string str) { return std::string{str2}; },
  };

  auto future = async::make_promise_all(funcs, str1).run();

  std::vector<std::string> res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::RangeEquals(std::vector<std::string>{str1, str2}));
}


TEST_CASE("Make all error string string", "[make all]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    [] (std::string str) { return std::string{str1}; },
    [] (std::string str) -> std::string { throw std::runtime_error{str2}; },
  };

  auto future = async::make_promise_all(funcs, str1).run();

  std::vector<std::string> res;
  REQUIRE_THROWS_MATCHES(res = future.get(), std::runtime_error, Catch::Matchers::Message(str2));
  REQUIRE(res.empty());
}
