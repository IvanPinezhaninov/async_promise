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


TEST_CASE("Make resolved void", "[make resolved promise]")
{
  auto future = async::make_resolved_promise().run();

  REQUIRE_NOTHROW(future.get());
}


TEST_CASE("Make resolved string", "[make resolved promise]")
{
  auto future = async::make_resolved_promise(str1).run();

  std::string res;
  REQUIRE_NOTHROW(res = future.get());
  REQUIRE(res == str1);
}
