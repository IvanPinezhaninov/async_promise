// stl
#include <stdexcept>

// local
#include "constants.h"
#include "test_funcs.h"


void void_void()
{}


void error_void_void()
{
  throw std::runtime_error{str2};
}


void void_string(std::string str)
{}


void error_void_string(std::string str)
{
  throw std::runtime_error{str2};
}


void void_exception(std::exception_ptr e)
{}


std::string string_void()
{
  return str1;
}


std::string error_string_void()
{
  throw std::runtime_error{str2};
}


std::string string_string(std::string str)
{
  return str;
}


std::string error_string_string(std::string str)
{
  throw std::runtime_error{str2};
}


std::string string_exception(std::exception_ptr e)
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
