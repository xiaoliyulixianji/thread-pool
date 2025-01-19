#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>

// gpt

template <typename T>
class ThreadPool {
public:
	ThreadPool(size_t numThreads) : stop(false), tasksRemaining(0) {
		for (size_t i = 0; i < numThreads; ++i) {
			workers.emplace_back([this]() { this->workerThread(); });
		}
	}

	~ThreadPool() {
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			stop = true;
		}
		cv.notify_all();
		for (std::thread &worker : workers) {
			worker.join();
		}
	}

	// 提交任务到线程池
	void enqueue(std::function<void()> task) {
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			if (stop) {
				throw std::runtime_error("enqueue on stopped ThreadPool");
			}
			taskQueue.push(task);
			++tasksRemaining;  // 增加任务计数
		}
		cv.notify_one();
	}

	// 等待所有任务完成
	void waitForCompletion() {
		std::unique_lock<std::mutex> lock(cvMutex);
		cvFinished.wait(lock, [this]() { return tasksRemaining == 0; });
	}

private:
	void workerThread() {
		while (true) {
			std::function<void()> task;
			{
				std::unique_lock<std::mutex> lock(queueMutex);
				cv.wait(lock, [this]() { return stop || !taskQueue.empty(); });
				if (stop && taskQueue.empty()) return;
				task = std::move(taskQueue.front());
				taskQueue.pop();
			}

			task();
			{
				std::unique_lock<std::mutex> lock(cvMutex);
				--tasksRemaining;  // 执行完任务后减少计数
			}
			cvFinished.notify_one();  // 通知主线程，任务已经完成
		}
	}

	std::vector<std::thread> workers;
	std::queue<std::function<void()>> taskQueue;
	std::mutex queueMutex;
	std::condition_variable cv;
	std::mutex cvMutex;  // 用于任务计数器的条件变量
	std::condition_variable cvFinished;  // 用于等待所有任务完成
	bool stop;
	int tasksRemaining;  // 任务计数器
};

class TaskExecutor {
public:
	TaskExecutor(std::vector<int>& data) : data(data) {}

	void v1() {
		std::cout << "Executing v1 for object " << data[0] << "\n";
	}
	void v2() {
		std::cout << "Executing v2 for object " << data[0] << "\n";
	}
	void v3() {
		std::cout << "Executing v3 for object " << data[0] << "\n";
	}
	void v4() {
		std::cout << "Executing v4 for object " << data[0] << "\n";
	}
	void v5() {
		std::cout << "Executing v5 for object " << data[0] << "\n";
	}
	void v6() {
		std::cout << "Executing v6 for object " << data[0] << "\n";
	}

	// 调度任务
/*	void execute(ThreadPool<TaskExecutor>& pool) {
		pool.enqueue([&executor = *this]() { executor.v1(); });
		pool.enqueue([&executor = *this]() { executor.v2(); });
		pool.enqueue([&executor = *this]() { executor.v3(); });
		pool.enqueue([&executor = *this]() { executor.v4(); });
		pool.enqueue([&executor = *this]() { executor.v5(); });
		pool.enqueue([&executor = *this]() { executor.v6(); });
	}*/

	void execute(ThreadPool<TaskExecutor>& pool) {
		pool.enqueue([executor = std::make_shared<TaskExecutor>(*this)]() { executor->v1(); });
		pool.enqueue([executor = std::make_shared<TaskExecutor>(*this)]() { executor->v2(); });
		pool.enqueue([executor = std::make_shared<TaskExecutor>(*this)]() { executor->v3(); });
		pool.enqueue([executor = std::make_shared<TaskExecutor>(*this)]() { executor->v4(); });
		pool.enqueue([executor = std::make_shared<TaskExecutor>(*this)]() { executor->v5(); });
		pool.enqueue([executor = std::make_shared<TaskExecutor>(*this)]() { executor->v6(); });
	}


private:
	std::vector<int>& data;
};

int main() {
	// 假设每个对象是一个带有标识的 vector
	std::vector<std::vector<int>> objects = { {1}, {2}, {3}, {4}, {5} };

	// 创建线程池，假设我们使用 4 个线程
	ThreadPool<TaskExecutor> pool(4);

	// 为每个对象执行任务
	for (auto& obj : objects) {
		TaskExecutor executor(obj);
		executor.execute(pool);
	}

	// 等待直到所有任务完成
	pool.waitForCompletion();

	std::cout << "All tasks are complete.\n";

	return 0;
}
