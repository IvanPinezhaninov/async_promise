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

#ifndef TEST_FUNCS_H
#define TEST_FUNCS_H

// stl
#include <string>


void void_void();

void void_void_delayed();

void error_void_void();

void error_void_void_delayed();

void void_string(std::string str);

void void_string_delayed(std::string str);

void error_void_string(std::string str);

void error_void_string_delayed(std::string str);

void void_exception(std::exception_ptr e);

std::string string_void1();

std::string string_void2();

std::string string_void_delayed();

std::string error_string_void();

std::string error_string_void_delayed();

std::string string_string1(std::string str);

std::string string_string2(std::string str);

std::string string_string_delayed(std::string str);

std::string error_string_string(std::string str);

std::string error_string_string_delayed(std::string str);

std::string string_exception(std::exception_ptr e);

#endif // TEST_FUNCS_H
