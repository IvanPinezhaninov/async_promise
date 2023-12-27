// stl
#include <stdexcept>

// local
#include "constants.h"
#include "test_struct.h"


void test_struct::void_void() const
{}


void test_struct::error_void_void() const
{
  throw std::runtime_error{str2};
}


void test_struct::void_string(std::string str) const
{}


void test_struct::error_void_string(std::string str) const
{
  throw std::runtime_error{str2};
}


void test_struct::void_exception(std::exception_ptr e) const
{}


std::string test_struct::string_void1() const
{
  return str1;
}


std::string test_struct::string_void2() const
{
  return str2;
}


std::string test_struct::error_string_void() const
{
  throw std::runtime_error{str2};
}


std::string test_struct::string_string(std::string str) const
{
  return str;
}


std::string test_struct::string_string1(std::string str) const
{
  return str1;
}


std::string test_struct::string_string2(std::string str) const
{
  return str2;
}


std::string test_struct::error_string_string(std::string str) const
{
  throw std::runtime_error{str2};
}


std::string test_struct::string_exception(std::exception_ptr e) const
{
  try
  {
    std::rethrow_exception(e);
  }
  catch(const std::exception& e)
  {
    return e.what();
  }
  catch(...)
  {
    return str1;
  }
}
