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

// async_promise
#include <async_promise.hpp>

// catch2
#include <catch2/catch_test_macros.hpp>

// local
#include "funcs.h"

static constexpr auto str = "Hello World!";


TEST_CASE("Initial void void", "[initial]")
{
  auto future = async::promise<void>{void_void_func}.run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Initial error void void", "[initial]")
{
  auto future = async::promise<void>{error_void_void_func}.run();

  REQUIRE_THROWS_AS(future.get(), std::runtime_error);
}


TEST_CASE("Initial void string", "[initial]")
{
  auto future = async::promise<void>{void_string_func, str}.run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Initial error void string", "[initial]")
{
  auto future = async::promise<void>{error_void_string_func, str}.run();

  REQUIRE_THROWS_AS(future.get(), std::runtime_error);
}


TEST_CASE("Initial string void", "[initial]")
{
  auto future = async::promise<std::string>{string_void_func}.run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str);
}


TEST_CASE("Initial error string void", "[initial]")
{
  auto future = async::promise<std::string>{error_string_void_func}.run();

  REQUIRE_THROWS_AS(future.get(), std::runtime_error);
}


TEST_CASE("Initial string string", "[initial]")
{
  auto future = async::promise<std::string>{string_string_func, str}.run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str);
}


TEST_CASE("Initial error string string", "[initial]")
{
  auto future = async::promise<std::string>{error_string_string_func, str}.run();

  REQUIRE_THROWS_AS(future.get(), std::runtime_error);
}
