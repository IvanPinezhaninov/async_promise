/******************************************************************************
**
** Copyright (C) 2023 Ivan Pinezhaninov <ivan.pinezhaninov@gmail.com>
**
** This file is part of the async_promise - which can be found at
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

#ifndef TEST_STRUCT_H
#define TEST_STRUCT_H

// stl
#include <string>


struct test_struct final
{
  void void_void() const;

  void error_void_void() const;

  void void_string(std::string str) const;

  void error_void_string(std::string str) const;

  void void_exception(std::exception_ptr e) const;

  std::string string_void1() const;

  std::string string_void2() const;

  std::string error_string_void() const;

  std::string string_string(std::string str) const;

  std::string string_string1(std::string str) const;

  std::string string_string2(std::string str) const;

  std::string error_string_string(std::string str) const;

  std::string string_exception(std::exception_ptr e) const;
};

#endif // TEST_STRUCT_H
