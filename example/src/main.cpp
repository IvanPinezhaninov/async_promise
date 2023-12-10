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
#include <iostream>

// async_promise
#include <async_promise.hpp>


template<typename T>
void print(const T& val) {
  std::cout << val << std::endl;
}


template<typename First, typename... Rest>
static void print(const First& first, const Rest&... rest)
{
  std::cout << first;
  print(rest...);
}


template<typename First, typename Container,
         typename = typename Container::value_type>
static void print_container(const First& first, const Container& c)
{
  std::cout << first << ": ";
  for (std::size_t i = 0; i < c.size(); ++i)
  {
    if (0 < i) std::cout << ", ";
    std::cout << c[i];
  }
  std::cout << std::endl;
}


static void print_error(const std::exception_ptr& e)
{
  std::cout << "error: ";

  try
  {
      std::rethrow_exception(e);
  }
  catch(const std::exception& e)
  {
      std::cout << e.what() << std::endl;
  }
  catch(...)
  {
      std::cout << "unknown error" << std::endl;
  }
}


static int sum(int a, int b)
{
  return a + b;
}


static void error()
{
  throw std::runtime_error("I'm error");
}


int main(int, char**)
{
  using namespace std::placeholders;

  std::vector<int(*)(int)> funcs
  {
    [] (int val)
    {
      return val * 2;
    },
    [] (int val)
    {
      return val * 4;
    },
    [] (int val)
    {
      return val * 8;
    },
  };

  auto future = async::make_promise(sum, 2, 2)
                .then([] (int val) { print("sum: ", val); return val; })
                .any(funcs)
                .then([] (int val) { print("any: ", val); return val; })
                .race(funcs)
                .then([] (int val) { print("race: ", val); return val; })
                .all(funcs)
                .then([] (const std::vector<int>& val) { print_container("all", val); return val; })
                .then(error)
                .fail(print_error)
                .then([] { return 42; })
                .run();

  print("result: ", future.get());

  return EXIT_SUCCESS;
}
