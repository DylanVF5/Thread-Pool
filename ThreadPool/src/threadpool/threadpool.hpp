#pragma once
#include "../dependencies/common.h"

class ThreadPool
{
public:
	// initializer for ThreadPool class, decides how many worker threads should be created for handling the queue of tasks
	ThreadPool(SIZE_T threadCount)
	{
		for (SIZE_T i = 0; i < threadCount; i++)
		{
			// emplace_back() constructs an element to place into the vector, so what this is essentially doing is calling the constructor 
			// of std::thread like this: std::thread([this] { workerFunction(); });
			// i believe this is called an implicit construction
			workers.emplace_back([this] { workerFunction(); });
		}
	}

	// deconstructor for ThreadPool class, ensures we have control of the mutex to avoid undefined behaviour, and we join the 
	// worker threads together to essentially re-merge the section of the program that is allocated elsewhere to do the task
	// (basically how threads work)
	~ThreadPool()
	{
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			stop = true;
		}
		condition.notify_all();
		for (std::thread& worker : workers)
		{
			worker.join();
		}
	}

	// function that handles adding a new function or task to the queue, makes it possible to make the worker threads have a task
	// to complete, && is called a universal reference, and what this means is that it will take the LVALUE and RVALUE,
	// which is important to ensure that any type of function can be passed (function, functors, lambdas)
	// the reason we use std::forward is to ensure if the input function is an LVALUE (function object) then we copy it to the queue,
	// whereas if its an RVALUE (temporary lambda for example) then it can also pass it, its essentially for compatibility
	template<typename T>
	void enqueue(T&& function)
	{
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			tasks.emplace(std::forward<T>(function));
		}
		condition.notify_one();
	}

private:
	// so this function is in a while true loop, and all the loop does is waits for a new task to complete, it works by first locking the mutex
	// to ensure no other threads attempt to take the current task to prevent undefined behaviour, and then ensures that the stop bool is not true
	// or the task queue isnt empty, and if both of those conditions are met, then we take a task from the task list, and pop to remove it, then
	// we call task, (std::function<void()> task; essentially creates a variable that can hold any no argument function)
	// the condition.wait() call can wake up sporadically, which is why we use a lambda predictate to double check (something to do with some OS bug).
	// NOTE: if task throws an exception the worker thread will terminate, so if you plan on using this make sure you modify to fit your needs.
	void workerFunction()
	{
		while (true)
		{
			std::function<void()> task;
			{
				std::unique_lock<std::mutex> lock(queueMutex);

				condition.wait(lock, [this] {return stop || !tasks.empty(); });

				if (stop && tasks.empty()) return;

				task = std::move(tasks.front());
				tasks.pop();
			}
			task();
		}
	}

	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;
	std::mutex queueMutex;
	std::condition_variable condition;
	bool stop = false;
};