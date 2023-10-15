// stl
#include <stdexcept>

// local
#include "funcs.h"

static constexpr auto str = "Hello World!";


void void_void_func()
{}


void error_void_void_func()
{
  throw std::runtime_error{str};
}


void void_string_func(std::string str)
{}


void error_void_string_func(std::string str)
{
  throw std::runtime_error{str};
}


void void_exception_func(std::exception_ptr e)
{}


std::string string_void_func()
{
  return str;
}


std::string error_string_void_func()
{
  throw std::runtime_error{str};
}


std::string string_string_func(std::string str)
{
  return str;
}


std::string error_string_string_func(std::string str)
{
  throw std::runtime_error{str};
}


std::string string_exception_func(std::exception_ptr e)
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
