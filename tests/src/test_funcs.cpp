// stl
#include <chrono>
#include <stdexcept>
#include <thread>

// local
#include "constants.h"
#include "test_funcs.h"


void void_void()
{}


void void_void_delayed()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(delay_length));
}


void error_void_void()
{
  throw std::runtime_error{str2};
}


void error_void_void_delayed()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(delay_length));
  throw std::runtime_error{str2};
}


void void_string(std::string str)
{}


void void_string_delayed(std::string str)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(delay_length));
}


void error_void_string(std::string str)
{
  throw std::runtime_error{str2};
}


void error_void_string_delayed(std::string str)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(delay_length));
  throw std::runtime_error{str2};
}


void void_exception(std::exception_ptr e)
{}


std::string string_void1()
{
  return str1;
}


std::string string_void2()
{
  return str2;
}


std::string string_void_delayed()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(delay_length));
  return str1;
}


std::string error_string_void()
{
  throw std::runtime_error{str2};
}


std::string error_string_void_delayed()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(delay_length));
  throw std::runtime_error{str2};
}


std::string string_string1(std::string str)
{
  return str1;
}


std::string string_string2(std::string str)
{
  return str2;
}


std::string string_string_delayed(std::string str)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(delay_length));
  return str1;
}


std::string error_string_string(std::string str)
{
  throw std::runtime_error{str2};
}


std::string error_string_string_delayed(std::string str)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(delay_length));
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
