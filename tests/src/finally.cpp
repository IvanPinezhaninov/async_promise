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


TEST_CASE("Finally with class method resolve void void", "[finally]")
{
  test_struct obj;
  auto future = async::make_resolved_promise().finally(&test_struct::void_void, &obj).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Finally with class method resolve string void", "[finally]")
{
  test_struct obj;
  auto future = async::make_resolved_promise(str1).finally(&test_struct::string_void1, &obj).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Finally with class method reject void void", "[finally]")
{
  test_struct obj;
  auto future = async::make_rejected_promise(std::runtime_error{str2}).finally(&test_struct::void_void, &obj).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Finally with class method reject string void", "[finally]")
{
  test_struct obj;
  auto future = async::make_rejected_promise(std::runtime_error{str2}).finally(&test_struct::string_void1, &obj).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Finally with func resolve void void", "[finally]")
{
  auto future = async::make_resolved_promise().finally(void_void).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Finally with func resolve string void", "[finally]")
{
  auto future = async::make_resolved_promise(str1).finally(string_void1).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}


TEST_CASE("Finally with func reject void void", "[finally]")
{
  auto future = async::make_rejected_promise(std::runtime_error{str2}).finally(void_void).run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Finally with func reject string void", "[finally]")
{
  auto future = async::make_rejected_promise(std::runtime_error{str2}).finally(string_void1).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}
