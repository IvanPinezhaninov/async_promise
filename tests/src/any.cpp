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
#include <cstring>

// async_promise
#include <async_promise.hpp>

// catch2
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/catch_test_macros.hpp>

static constexpr auto str = "Hello World!";
static constexpr auto aggregate_error_message = "All functions rejected";


TEST_CASE("Any void void", "[any]")
{
  std::vector<void(*)()> funcs
  {
    [] () {},
    [] () {},
  };

  auto future = async::promise<void>::resolve().any(funcs).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Any error void void", "[any]")
{
  std::vector<void(*)()> funcs
  {
    [] () { throw std::runtime_error{str}; },
    [] () {},
  };

  auto future = async::promise<void>::resolve().any(funcs).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Any all error void void", "[any]")
{
  std::vector<void(*)()> funcs
  {
    [] () { throw std::runtime_error{str}; },
    [] () { throw std::runtime_error{str}; },
  };

  auto future = async::promise<void>::resolve().any(funcs).run();

  REQUIRE_THROWS_MATCHES(future.get(), async::aggregate_error, Catch::Matchers::Message(aggregate_error_message));
}


TEST_CASE("Any void string", "[any]")
{
  std::vector<void(*)(std::string)> funcs
  {
    [] (std::string) {},
    [] (std::string) {},
  };

  auto future = async::promise<std::string>::resolve(str).any(funcs).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Any error void string", "[any]")
{
  std::vector<void(*)(std::string)> funcs
  {
    [] (std::string) { throw std::runtime_error{str}; },
    [] (std::string) {},
  };

  auto future = async::promise<std::string>::resolve(str).any(funcs).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Any all error void string", "[any]")
{
  std::vector<void(*)(std::string)> funcs
  {
    [] (std::string) { throw std::runtime_error{str}; },
    [] (std::string) { throw std::runtime_error{str}; },
  };

  auto future = async::promise<std::string>::resolve(str).any(funcs).run();

  REQUIRE_THROWS_MATCHES(future.get(), async::aggregate_error, Catch::Matchers::Message(aggregate_error_message));
}


TEST_CASE("Any string void", "[any]")
{
  std::vector<std::string(*)()> funcs
  {
    [] () { return std::string{str}; },
    [] () { return std::string{str}; },
  };

  auto future = async::promise<void>::resolve().any(funcs).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str);
}


TEST_CASE("Any error string void", "[any]")
{
  std::vector<std::string(*)()> funcs
  {
    [] () -> std::string { throw std::runtime_error{str}; },
    [] () { return std::string{str}; },
  };

  auto future = async::promise<void>::resolve().any(funcs).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str);
}


TEST_CASE("Any all error string void", "[any]")
{
  std::vector<std::string(*)()> funcs
  {
    [] () -> std::string { throw std::runtime_error{str}; },
    [] () -> std::string { throw std::runtime_error{str}; },
  };

  auto future = async::promise<void>::resolve().any(funcs).run();
  REQUIRE_THROWS_MATCHES(future.get(), async::aggregate_error, Catch::Matchers::Message(aggregate_error_message));
}


TEST_CASE("Any string string", "[any]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    [] (std::string str) { return str; },
    [] (std::string str) { return str; },
  };

  auto future = async::promise<std::string>::resolve(str).any(funcs).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str);
}


TEST_CASE("Any error string string", "[any]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    [] (std::string str) -> std::string { throw std::runtime_error{str}; },
    [] (std::string str) { return str; },
  };

  auto future = async::promise<std::string>::resolve(str).any(funcs).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str);
}


TEST_CASE("Any all error string string", "[any]")
{
  std::vector<std::string(*)(std::string)> funcs
  {
    [] (std::string str) -> std::string { throw std::runtime_error{str}; },
    [] (std::string str) -> std::string { throw std::runtime_error{str}; },
  };

  auto future = async::promise<std::string>::resolve(str).any(funcs).run();
  REQUIRE_THROWS_MATCHES(future.get(), async::aggregate_error, Catch::Matchers::Message(aggregate_error_message));
}
