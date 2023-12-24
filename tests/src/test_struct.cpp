// stl
#include <stdexcept>

// local
#include "test_struct.h"

static constexpr auto str = "Hello World!";


void test_struct::void_void()
{}


void test_struct::error_void_void()
{
  throw std::runtime_error{str};
}


void test_struct::void_string(std::string str)
{}


void test_struct::error_void_string(std::string str)
{
  throw std::runtime_error{str};
}


void test_struct::void_exception(std::exception_ptr e)
{}


std::string test_struct::string_void()
{
  return str;
}


std::string test_struct::error_string_void()
{
  throw std::runtime_error{str};
}


std::string test_struct::string_string(std::string str)
{
  return str;
}


std::string test_struct::error_string_string(std::string str)
{
  throw std::runtime_error{str};
}


std::string test_struct::string_exception(std::exception_ptr e)
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
    return str;
  }
}
