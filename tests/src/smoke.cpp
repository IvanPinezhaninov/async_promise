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
#include <numeric>

// async_promise
#include <async_promise.hpp>

// catch2
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_quantifiers.hpp>


template<typename T, template<typename, typename> class Container, typename Allocator>
static T sum(const Container<T, Allocator>& c)
{
  return std::accumulate(c.cbegin(), c.cend(), T{});
}


template<typename T, template<typename, typename> class Container, typename Allocator>
static T sum_settled(const Container<async::settled<T>, Allocator>& c)
{
  using namespace std::placeholders;
  return std::accumulate(c.cbegin(), c.cend(), T{},
                         std::bind(std::plus<T>(), _1, std::bind(&async::settled<T>::result, _2)));
}


TEST_CASE("Smoke make then", "[smoke]")
{
  auto future = async::make_promise([] (int val) { return val + 2; }, 1)
                .then([] (int val) { return val + 3; })
                .then([] (int val) { return val + 4; })
                .then([] (int val) { return val + 5; })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = 1 + 2 + 3 + 4 + 5;
  REQUIRE(res == exp);
}


TEST_CASE("Smoke make then no arg", "[smoke]")
{
  auto future = async::make_promise([] () { return 1; })
                .then([] (int val) { return val + 2; })
                .then([] (int val) { return val + 3; })
                .then([] (int val) { return val + 4; })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = 1 + 2 + 3 + 4;
  REQUIRE(res == exp);
}


TEST_CASE("Smoke then", "[smoke]")
{
  auto future = async::make_resolved_promise(1)
                .then([] (int val) { return val + 2; })
                .then([] (int val) { return val + 3; })
                .then([] (int val) { return val + 4; })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = 1 + 2 + 3 + 4;
  REQUIRE(res == exp);
}


TEST_CASE("Smoke then no arg", "[smoke]")
{
  auto future = async::make_resolved_promise(1)
                .then([] (int val) { return val + 2; })
                .then([] () { return 3; })
                .then([] (int val) { return val + 4; })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = 3 + 4;
  REQUIRE(res == exp);
}


TEST_CASE("Smoke all", "[smoke]")
{
  std::vector<int(*)(int)> funcs
  {
    [] (int val) { return val + 3; },
    [] (int val) { return val + 4; },
    [] (int val) { return val + 5; },
  };

  auto future = async::make_resolved_promise(1)
                .then([] (int val) { return val + 2; })
                .all(funcs)
                .then([] (const std::vector<int>& v) { return sum(v); })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = (1 + 2 + 3) + (1 + 2 + 4) + (1 + 2 + 5);
  REQUIRE(res == exp);
}


TEST_CASE("Smoke all no arg", "[smoke]")
{
  std::vector<int(*)()> funcs
  {
    [] () { return 3; },
    [] () { return 4; },
    [] () { return 5; },
  };

  auto future = async::make_resolved_promise(1)
                .then([] (int val) { return val + 2; })
                .all(funcs)
                .then([] (const std::vector<int>& v) { return sum(v); })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = 3 + 4 + 5;
  REQUIRE(res == exp);
}


TEST_CASE("Smoke all settled", "[smoke]")
{
  std::vector<int(*)(int)> funcs
  {
    [] (int val) { return val + 3; },
    [] (int val) { return val + 4; },
    [] (int val) { return val + 5; },
  };

  auto future = async::make_resolved_promise(1)
                .then([] (int val) { return val + 2; })
                .all_settled(funcs)
                .then([] (const std::vector<async::settled<int>>& v) { return sum_settled(v); })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = (1 + 2 + 3) + (1 + 2 + 4) + (1 + 2 + 5);
  REQUIRE(res == exp);
}


TEST_CASE("Smoke all settled no arg", "[smoke]")
{
  std::vector<int(*)()> funcs
  {
    [] () { return 3; },
    [] () { return 4; },
    [] () { return 5; },
  };

  auto future = async::make_resolved_promise(1)
                .then([] (int val) { return val + 2; })
                .all_settled(funcs)
                .then([] (const std::vector<async::settled<int>>& v) { return sum_settled(v); })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = 3 + 4 + 5;
  REQUIRE(res == exp);
}


TEST_CASE("Smoke any", "[smoke]")
{
  std::vector<int(*)(int)> funcs
  {
    [] (int val) { return val + 3; },
    [] (int val) { return val + 3; },
    [] (int val) { return val + 3; },
  };

  auto future = async::make_resolved_promise(1)
                .then([] (int val) { return val + 2; })
                .any(funcs)
                .then([] (int val) { return val + 4; })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = 1 + 2 + 3 + 4;
  REQUIRE(res == exp);
}


TEST_CASE("Smoke any no arg", "[smoke]")
{
  std::vector<int(*)()> funcs
  {
    [] () { return 3; },
    [] () { return 3; },
    [] () { return 3; },
  };

  auto future = async::make_resolved_promise(1)
                .then([] (int val) { return val + 2; })
                .any(funcs)
                .then([] (int val) { return val + 4; })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = 3 + 4;
  REQUIRE(res == exp);
}


TEST_CASE("Smoke race", "[smoke]")
{
  std::vector<int(*)(int)> funcs
  {
    [] (int val) { return val + 3; },
    [] (int val) { return val + 3; },
    [] (int val) { return val + 3; },
  };

  auto future = async::make_resolved_promise(1)
                .then([] (int val) { return val + 2; })
                .race(funcs)
                .then([] (int val) { return val + 4; })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = 1 + 2 + 3 + 4;
  REQUIRE(res == exp);
}


TEST_CASE("Smoke race no arg", "[smoke]")
{
  std::vector<int(*)()> funcs
  {
    [] () { return 3; },
    [] () { return 3; },
    [] () { return 3; },
  };

  auto future = async::make_resolved_promise(1)
                .then([] (int val) { return val + 2; })
                .race(funcs)
                .then([] (int val) { return val + 4; })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = 3 + 4;
  REQUIRE(res == exp);
}


TEST_CASE("Smoke static all", "[smoke]")
{
  std::vector<int(*)(int, int, int)> funcs
  {
    [] (int val1, int val2, int val3) { return val1 + val2 + val3 + 4; },
    [] (int val1, int val2, int val3) { return val1 + val2 + val3 + 5; },
    [] (int val1, int val2, int val3) { return val1 + val2 + val3 + 6; },
  };

  auto future = async::make_promise_all(funcs, 1, 2, 3)
                .then([] (const std::vector<int>& v) { return sum(v); })
                .then([] (int val) { return val + 7; })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = (1 + 2 + 3 + 4) + (1 + 2 + 3 + 5) + (1 + 2 + 3 + 6) + 7;
  REQUIRE(res == exp);
}


TEST_CASE("Smoke static all no args", "[smoke]")
{
  std::vector<int(*)()> funcs
  {
    [] () { return 1; },
    [] () { return 2; },
    [] () { return 3; },
  };

  auto future = async::make_promise_all(funcs)
                .then([] (const std::vector<int>& v) { return sum(v); })
                .then([] (int val) { return val + 4; })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = (1 + 2 + 3) + 4;
  REQUIRE(res == exp);
}


TEST_CASE("Smoke static all settled", "[smoke]")
{
  std::vector<int(*)(int, int, int)> funcs
  {
    [] (int val1, int val2, int val3) { return val1 + val2 + val3 + 4; },
    [] (int val1, int val2, int val3) { return val1 + val2 + val3 + 5; },
    [] (int val1, int val2, int val3) { return val1 + val2 + val3 + 6; },
  };

  auto future = async::make_promise_all_settled(funcs, 1, 2, 3)
                .then([] (const std::vector<async::settled<int>>& v) { return sum_settled(v); })
                .then([] (int val) { return val + 7; })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = (1 + 2 + 3 + 4) + (1 + 2 + 3 + 5) + (1 + 2 + 3 + 6) + 7;
  REQUIRE(res == exp);
}


TEST_CASE("Smoke static all settled no args", "[smoke]")
{
  std::vector<int(*)()> funcs
  {
    [] () { return 1; },
    [] () { return 2; },
    [] () { return 3; },
  };

  auto future = async::make_promise_all_settled(funcs)
                .then([] (const std::vector<async::settled<int>>& v) { return sum_settled(v); })
                .then([] (int val) { return val + 4; })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = (1 + 2 + 3) + 4;
  REQUIRE(res == exp);
}


TEST_CASE("Smoke static any", "[smoke]")
{
  std::vector<int(*)(int, int, int)> funcs
  {
    [] (int val1, int val2, int val3) { return val1 + val2 + val3 + 4; },
    [] (int val1, int val2, int val3) { return val1 + val2 + val3 + 4; },
    [] (int val1, int val2, int val3) { return val1 + val2 + val3 + 4; },
  };

  auto future = async::make_promise_any(funcs, 1, 2, 3)
                .then([] (int val) { return val + 5; })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = (1 + 2 + 3 + 4) + 5;
  REQUIRE(res == exp);
}


TEST_CASE("Smoke static any no args", "[smoke]")
{
  std::vector<int(*)()> funcs
  {
    [] () { return 1; },
    [] () { return 1; },
    [] () { return 1; },
  };

  auto future = async::make_promise_any(funcs)
                .then([] (int val) { return val + 2; })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = 1 + 2;
  REQUIRE(res == exp);
}


TEST_CASE("Smoke static race", "[smoke]")
{
  std::vector<int(*)(int, int, int)> funcs
  {
    [] (int val1, int val2, int val3) { return val1 + val2 + val3 + 4; },
    [] (int val1, int val2, int val3) { return val1 + val2 + val3 + 4; },
    [] (int val1, int val2, int val3) { return val1 + val2 + val3 + 4; },
  };

  auto future = async::make_promise_race(funcs, 1, 2, 3)
                .then([] (int val) { return val + 5; })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = (1 + 2 + 3 + 4) + 5;
  REQUIRE(res == exp);
}


TEST_CASE("Smoke static race no args", "[smoke]")
{
  std::vector<int(*)()> funcs
  {
    [] () { return 1; },
    [] () { return 1; },
    [] () { return 1; },
  };

  auto future = async::make_promise_race(funcs)
                .then([] (int val) { return val + 2; })
                .run();

  auto res = 0;
  REQUIRE_NOTHROW(res = future.get());
  static constexpr auto exp = 1 + 2;
  REQUIRE(res == exp);
}
