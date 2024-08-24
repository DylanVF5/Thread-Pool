#include "threadpool/threadpool.hpp"

void printNumber(int number) {
    std::printf("Thread ID: %d, Number: %d\n", std::this_thread::get_id(), number);
}

int main() {
    ThreadPool pool(4);

    // enqueue 4 tasks, creating a temporary lambda capturing the variable i to pass to the function.
    for (int i = 0; i < 4; ++i) 
    {
        pool.enqueue([i]() { printNumber(i); });
    }

    return 0;
}