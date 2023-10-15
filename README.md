# async::promise

[![MIT License](https://img.shields.io/badge/license-mit-blue.svg?style=flat)](http://opensource.org/licenses/MIT)

## Overview

Promises/A+ asynchronous C++11 header-only library


## Build and test

```bash
git clone https://github.com/IvanPinezhaninov/async_promise.git
cd async_promise
mkdir build
cd build
cmake -GNinja .. -DCMAKE_BUILD_TYPE=Release -DASYNC_PROMISE_BUILD_TESTS=YES
cmake --build . -- -j4
ctest
```

## License
This code is distributed under the [MIT License](LICENSE)

## Author
[Ivan Pinezhaninov](mailto:ivan.pinezhaninov@gmail.com)
