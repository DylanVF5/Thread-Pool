# ThreadPool
- A C++ implementation of a simple thread pool for parallel task execution.

# Features
- Create a pool of worker threads
- Enqueue tasks for parallel execution
- Automatic task distribution among worker threads

# Usage
```cpp
#include "threadpool/threadpool.hpp"

// Create a thread pool with 4 worker threads
ThreadPool pool(4);

// Enqueue tasks
for (int i = 0; i < 8; ++i) {
    pool.enqueue([i]() { 
        std::printf("Thread ID: %d, Number: %d\n", std::this_thread::get_id(), i);
    });
}
```

# Purpose
- Designed for educational purposes to understand multithreading and task parallelization in C++.

# Requirements
- C++11 or later
- Compiler with C++11 support (tested on GCC and MSVC)

# Note
- This is a learning project and may not be suitable for production environments.
- The implementation here may not be the most efficient or optimal way to create a thread pool. It's a first attempt at understanding and implementing these concepts, and there's likely room for improvement.
