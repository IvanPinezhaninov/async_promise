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

// stl
#include <chrono>
#include <thread>

// async_promise
#include <async_promise.hpp>

// catch2
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

// local
#include "common.h"


TEST_CASE("Make race void void", "[make race]")
{
  std::vector<void(*)()> void_void
  {
    [] () {},
    [] () {},
  };

  auto future = async::make_promise_race(void_void).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make race error won void void", "[make race]")
{
  std::vector<void(*)()> void_void
  {
    [] () { throw std::runtime_error{str1}; },
    [] () { std::this_thread::sleep_for(std::chrono::milliseconds(100)); },
  };

  auto future = async::make_promise_race(void_void).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str1));
}


TEST_CASE("Make race error lose void void", "[make race]")
{
  std::vector<void(*)()> void_void
  {
    [] () {},
    [] () { std::this_thread::sleep_for(std::chrono::milliseconds(100)); throw std::runtime_error{str2}; },
  };

  auto future = async::make_promise_race(void_void).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make race void string", "[make race]")
{
  std::vector<void(*)(std::string)> void_str
  {
    [] (std::string) {},
    [] (std::string) {},
  };

  auto future = async::make_promise_race(void_str, str1).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make race error won void string", "[make race]")
{
  std::vector<void(*)(std::string)> void_str
  {
    [] (std::string) { throw std::runtime_error{str1}; },
    [] (std::string) { std::this_thread::sleep_for(std::chrono::milliseconds(100)); },
  };

  auto future = async::make_promise_race(void_str, str1).run();

  REQUIRE_THROWS_MATCHES(future.get(), std::runtime_error, Catch::Matchers::Message(str1));
}


TEST_CASE("Make race error lose void string", "[make race]")
{
  std::vector<void(*)(std::string)> void_str
  {
    [] (std::string) {},
    [] (std::string) { std::this_thread::sleep_for(std::chrono::milliseconds(100)); throw std::runtime_error{str2}; },
  };

  auto future = async::make_promise_race(void_str, str1).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make race string void", "[make race]")
{
  std::vector<std::string(*)()> str_void
  {
    [] () { return std::string{str1}; },
    [] () { return std::string{str2}; },
  };

  auto future = async::make_promise_race(str_void).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Make race error won string void", "[make race]")
{
  std::vector<std::string(*)()> str_void
  {
    [] () -> std::string { throw std::runtime_error{str1}; },
    [] () { std::this_thread::sleep_for(std::chrono::milliseconds(100)); return std::string{str2}; },
  };

  auto future = async::make_promise_race(str_void).run();

  std::string res;
  REQUIRE_THROWS_MATCHES(res = future.get(), std::runtime_error, Catch::Matchers::Message(str1));
  REQUIRE(res.empty());
}


TEST_CASE("Make race error lose string void", "[make race]")
{
  std::vector<std::string(*)()> str_void
  {
    [] () { return std::string{str1}; },
    [] () -> std::string { std::this_thread::sleep_for(std::chrono::milliseconds(100)); throw std::runtime_error{str2}; },
  };

  auto future = async::make_promise_race(str_void).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Make race string string", "[make race]")
{
  std::vector<std::string(*)(std::string)> str_str
  {
    [] (std::string str) { return std::string{str1}; },
    [] (std::string str) { return std::string{str2}; },
  };

  auto future = async::make_promise_race(str_str, str1).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE_THAT(res, Catch::Matchers::Equals(str1) || Catch::Matchers::Equals(str2));
}


TEST_CASE("Make race error won string string", "[make race]")
{
  std::vector<std::string(*)(std::string)> str_str
  {
    [] (std::string str) -> std::string { throw std::runtime_error{str1}; },
    [] (std::string str) { std::this_thread::sleep_for(std::chrono::milliseconds(100)); return std::string{str2}; },
  };

  auto future = async::make_promise_race(str_str, str1).run();

  std::string res;
  REQUIRE_THROWS_MATCHES(res = future.get(), std::runtime_error, Catch::Matchers::Message(str1));
  REQUIRE(res.empty());
}


TEST_CASE("Make race error lose string string", "[make race]")
{
  std::vector<std::string(*)(std::string)> str_str
  {
    [] (std::string str) { return std::string{str1}; },
    [] (std::string str) -> std::string { std::this_thread::sleep_for(std::chrono::milliseconds(100)); throw std::runtime_error{str2}; },
  };

  auto future = async::make_promise_race(str_str, str1).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}
