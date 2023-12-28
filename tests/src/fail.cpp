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


TEST_CASE("Fail with class method void void", "[fail]")
{
  test_struct obj;
  auto future = async::make_rejected_promise(std::runtime_error{str2})
                .fail(&test_struct::void_void, &obj).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Fail with class method void exception", "[fail]")
{
  test_struct obj;
  auto future = async::make_rejected_promise(std::runtime_error{str2})
                .fail(&test_struct::void_exception, &obj).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Fail with class method string void", "[fail]")
{
  test_struct obj;
  auto future = async::make_rejected_promise<std::string>(std::runtime_error{str2})
                .fail(&test_struct::string_void1, &obj).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Fail with class method string exception", "[fail]")
{
  test_struct obj;
  auto future = async::make_rejected_promise<std::string>(std::runtime_error{str2})
                .fail(&test_struct::string_exception, &obj).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Fail with func void void", "[fail]")
{
  auto future = async::make_rejected_promise(std::runtime_error{str2}).fail(void_void).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Fail with func void exception", "[fail]")
{
  auto future = async::make_rejected_promise(std::runtime_error{str2}).fail(void_exception).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Fail with func string void", "[fail]")
{
  auto future = async::make_rejected_promise<std::string>(std::runtime_error{str2}).fail(string_void1).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Fail with func string exception", "[fail]")
{
  auto future = async::make_rejected_promise<std::string>(std::runtime_error{str2}).fail(string_exception).run();

  REQUIRE_NOTHROW(future.get());
}

