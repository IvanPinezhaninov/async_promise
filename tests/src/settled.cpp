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


TEST_CASE("Resolved settled object copy constructor", "[settled]")
{
  async::settled<std::string> obj1{str1};
  REQUIRE(obj1.type == async::settle_type::resolved);
  REQUIRE(obj1.result == str1);

  auto obj2 = obj1;
  REQUIRE(obj2.type == async::settle_type::resolved);
  REQUIRE(obj2.result == str1);
  REQUIRE(obj2.type == obj1.type);
  REQUIRE(obj2.result == obj1.result);
}


TEST_CASE("Resolved settled object move constructor", "[settled]")
{
  async::settled<std::string> obj1{str1};
  REQUIRE(obj1.type == async::settle_type::resolved);
  REQUIRE(obj1.result == str1);

  auto obj2 = std::move(obj1);
  REQUIRE(obj2.type == async::settle_type::resolved);
  REQUIRE(obj2.result == str1);
}


TEST_CASE("Resolved settled object copy operator", "[settled]")
{
  async::settled<std::string> obj1{str1};
  REQUIRE(obj1.type == async::settle_type::resolved);
  REQUIRE(obj1.result == str1);

  async::settled<std::string> obj2{str2};
  REQUIRE(obj2.type == async::settle_type::resolved);
  REQUIRE(obj2.result == str2);

  obj2 = obj1;
  REQUIRE(obj2.type == async::settle_type::resolved);
  REQUIRE(obj2.result == str1);
  REQUIRE(obj2.type == obj1.type);
  REQUIRE(obj2.result == obj1.result);
}


TEST_CASE("Resolved settled object move operator", "[settled]")
{
  async::settled<std::string> obj1{str1};
  REQUIRE(obj1.type == async::settle_type::resolved);
  REQUIRE(obj1.result == str1);

  async::settled<std::string> obj2{str2};
  REQUIRE(obj2.type == async::settle_type::resolved);
  REQUIRE(obj2.result == str2);

  obj2 = std::move(obj1);
  REQUIRE(obj2.type == async::settle_type::resolved);
  REQUIRE(obj2.result == str1);
}


TEST_CASE("Rejected settled object copy constructor", "[settled]")
{
  async::settled<std::string> obj1{std::make_exception_ptr(std::runtime_error{str2})};
  REQUIRE(obj1.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj1.error), std::runtime_error, Catch::Matchers::Message(str2));

  auto obj2 = obj1;
  REQUIRE(obj2.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj2.error), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Rejected settled object move constructor", "[settled]")
{
  async::settled<std::string> obj1{std::make_exception_ptr(std::runtime_error{str2})};
  REQUIRE(obj1.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj1.error), std::runtime_error, Catch::Matchers::Message(str2));

  auto obj2 = std::move(obj1);
  REQUIRE(obj2.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj2.error), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Rejected settled object copy operator", "[settled]")
{
  async::settled<std::string> obj1{std::make_exception_ptr(std::runtime_error{str2})};
  REQUIRE(obj1.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj1.error), std::runtime_error, Catch::Matchers::Message(str2));

  async::settled<std::string> obj2{std::make_exception_ptr(std::runtime_error{str2})};
  REQUIRE(obj2.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj2.error), std::runtime_error, Catch::Matchers::Message(str2));

  obj2 = obj1;
  REQUIRE(obj2.type == obj1.type);
  REQUIRE(obj2.error == obj1.error);
}


TEST_CASE("Rejected settled object move operator", "[settled]")
{
  async::settled<std::string> obj1{std::make_exception_ptr(std::runtime_error{str2})};
  REQUIRE(obj1.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj1.error), std::runtime_error, Catch::Matchers::Message(str2));

  async::settled<std::string> obj2{std::make_exception_ptr(std::runtime_error{str2})};
  REQUIRE(obj2.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj2.error), std::runtime_error, Catch::Matchers::Message(str2));

  obj2 = std::move(obj1);
  REQUIRE(obj2.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj2.error), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Settled object swap method", "[settled]")
{
  async::settled<std::string> obj1{str1};
  REQUIRE(obj1.type == async::settle_type::resolved);
  REQUIRE(obj1.result == str1);

  async::settled<std::string> obj2{std::make_exception_ptr(std::runtime_error{str2})};
  REQUIRE(obj2.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj2.error), std::runtime_error, Catch::Matchers::Message(str2));

  obj1.swap(obj2);

  REQUIRE(obj1.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj1.error), std::runtime_error, Catch::Matchers::Message(str2));

  REQUIRE(obj2.type == async::settle_type::resolved);
  REQUIRE(obj2.result == str1);
}


TEST_CASE("Resolved settled void object copy constructor", "[settled]")
{
  async::settled<void> obj1;
  REQUIRE(obj1.type == async::settle_type::resolved);

  auto obj2 = obj1;
  REQUIRE(obj2.type == async::settle_type::resolved);
  REQUIRE(obj2.type == obj1.type);
}


TEST_CASE("Resolved settled void object move constructor", "[settled]")
{
  async::settled<void> obj1;
  REQUIRE(obj1.type == async::settle_type::resolved);

  auto obj2 = std::move(obj1);
  REQUIRE(obj2.type == async::settle_type::resolved);
}


TEST_CASE("Resolved settled void object copy operator", "[settled]")
{
  async::settled<void> obj1;
  REQUIRE(obj1.type == async::settle_type::resolved);

  async::settled<void> obj2;
  REQUIRE(obj2.type == async::settle_type::resolved);

  obj2 = obj1;
  REQUIRE(obj2.type == async::settle_type::resolved);
  REQUIRE(obj2.type == obj1.type);
}


TEST_CASE("Resolved settled void object move operator", "[settled]")
{
  async::settled<void> obj1;
  REQUIRE(obj1.type == async::settle_type::resolved);

  async::settled<void> obj2;
  REQUIRE(obj2.type == async::settle_type::resolved);

  obj2 = std::move(obj1);
  REQUIRE(obj2.type == async::settle_type::resolved);
}


TEST_CASE("Rejected settled void object copy constructor", "[settled]")
{
  async::settled<void> obj1{std::make_exception_ptr(std::runtime_error{str2})};
  REQUIRE(obj1.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj1.error), std::runtime_error, Catch::Matchers::Message(str2));

  auto obj2 = obj1;
  REQUIRE(obj2.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj2.error), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Rejected settled void object move constructor", "[settled]")
{
  async::settled<void> obj1{std::make_exception_ptr(std::runtime_error{str2})};
  REQUIRE(obj1.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj1.error), std::runtime_error, Catch::Matchers::Message(str2));

  auto obj2 = std::move(obj1);
  REQUIRE(obj2.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj2.error), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Rejected settled void object copy operator", "[settled]")
{
  async::settled<void> obj1{std::make_exception_ptr(std::runtime_error{str2})};
  REQUIRE(obj1.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj1.error), std::runtime_error, Catch::Matchers::Message(str2));

  async::settled<void> obj2{std::make_exception_ptr(std::runtime_error{str2})};
  REQUIRE(obj2.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj2.error), std::runtime_error, Catch::Matchers::Message(str2));

  obj2 = obj1;
  REQUIRE(obj2.type == obj1.type);
  REQUIRE(obj2.error == obj1.error);
}


TEST_CASE("Rejected settled void object move operator", "[settled]")
{
  async::settled<void> obj1{std::make_exception_ptr(std::runtime_error{str2})};
  REQUIRE(obj1.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj1.error), std::runtime_error, Catch::Matchers::Message(str2));

  async::settled<void> obj2{std::make_exception_ptr(std::runtime_error{str2})};
  REQUIRE(obj2.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj2.error), std::runtime_error, Catch::Matchers::Message(str2));

  obj2 = std::move(obj1);
  REQUIRE(obj2.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj2.error), std::runtime_error, Catch::Matchers::Message(str2));
}


TEST_CASE("Settled object void swap method", "[settled]")
{
  async::settled<void> obj1;
  REQUIRE(obj1.type == async::settle_type::resolved);

  async::settled<void> obj2{std::make_exception_ptr(std::runtime_error{str2})};
  REQUIRE(obj2.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj2.error), std::runtime_error, Catch::Matchers::Message(str2));

  obj1.swap(obj2);

  REQUIRE(obj1.type == async::settle_type::rejected);
  REQUIRE_THROWS_MATCHES(std::rethrow_exception(obj1.error), std::runtime_error, Catch::Matchers::Message(str2));

  REQUIRE(obj2.type == async::settle_type::resolved);
}
