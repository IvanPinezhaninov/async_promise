#============================================================================
#
# Copyright (C) 2023 Ivan Pinezhaninov <ivan.pinezhaninov@gmail.com>
#
# This file is part of the async_promise which can be found at
# https://github.com/IvanPinezhaninov/async_promise/.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
# THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
#============================================================================

cmake_minimum_required(VERSION 3.1)

project(async_promise LANGUAGES CXX VERSION 1.0.0)

option(ASYNC_PROMISE_BUILD_EXAMPLE "Build example" OFF)
option(ASYNC_PROMISE_BUILD_TESTS "Build tests" OFF)
option(ASYNC_PROMISE_CODECOV "Add test coverage" OFF)

if(ASYNC_PROMISE_BUILD_EXAMPLE)
  add_subdirectory(example)
endif()

if(ASYNC_PROMISE_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

find_package(Threads REQUIRED)

set(HEADERS
  include/async_promise.hpp
)

set(TARGET async_promise)

add_library(${TARGET} INTERFACE)

add_library("async::promise" ALIAS ${TARGET})

target_include_directories(${TARGET} INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${TARGET} INTERFACE
  Threads::Threads
)

if(ASYNC_PROMISE_CODECOV AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(${TARGET} INTERFACE -O0 -g --coverage)
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(${TARGET} INTERFACE --coverage)
  else()
    target_link_libraries(${TARGET} INTERFACE --coverage)
  endif()
endif()

install(FILES ${HEADERS} DESTINATION include)
