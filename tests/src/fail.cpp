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
#include "test_funcs.h"

static constexpr auto str = "Hello World!";


TEST_CASE("Fail void void", "[fail]")
{
  auto future = async::make_rejected_promise(std::runtime_error{str}).fail(void_void).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Fail void exception", "[fail]")
{
  auto future = async::make_rejected_promise(std::runtime_error{str}).fail(void_exception).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Fail string void", "[fail]")
{
  auto future = async::make_rejected_promise<std::string>(std::runtime_error{str}).fail(string_void).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Fail string exception", "[fail]")
{
  auto future = async::make_rejected_promise<std::string>(std::runtime_error{str}).fail(string_exception).run();

  REQUIRE_NOTHROW(future.get());
}

