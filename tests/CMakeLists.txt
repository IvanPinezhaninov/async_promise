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


cmake_minimum_required(VERSION 3.11)

include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_SHALLOW    YES
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)

set(HEADERS
  src/common.h
  src/test_funcs.h
  src/test_struct.h
)

set(SOURCES
  src/all_settled.cpp
  src/all.cpp
  src/any.cpp
  src/fail.cpp
  src/finally.cpp
  src/initial.cpp
  src/make_promise_all_settled.cpp
  src/make_promise_all.cpp
  src/make_promise_any.cpp
  src/make_promise_race.cpp
  src/make_promise.cpp
  src/make_rejected_promise.cpp
  src/make_resolved_promise.cpp
  src/race.cpp
  src/settled.cpp
  src/smoke.cpp
  src/test_funcs.cpp
  src/test_struct.cpp
  src/then.cpp
)

set(TARGET async_promise_tests)

add_executable(${TARGET}
  ${HEADERS}
  ${SOURCES}
)

set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

target_link_libraries(${TARGET} PRIVATE
  Catch2::Catch2WithMain
  async::promise
)

catch_discover_tests(${TARGET})
