#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <queue> // 队列
#include <mutex> // 互斥锁
#include <condition_variable> // 条件变量
#include <functional>


class  ThreadPool
{
public:
	ThreadPool(int numThread) :stop(false)
	{
		for (int i = 0; i < numThread; i++)
		{
			threads.emplace_back([this] {
				while (1) {
					// 加锁
					std::unique_lock<std::mutex> lock(mtx);
					// 判断任务队列是否有任务
					condition.wait(lock, [this] {
						return !tasks.empty() || stop;
					});
					// 线程终止，直接返回
					if (stop && tasks.empty())
					{
						return;
					}

					// 从任务队列，取任务
					std::function<void()> task(std::move(tasks.front()));
					// 将取走的任务，在任务队列删除
					tasks.pop();

					// 解锁
					lock.unlock();
					// 执行任务
					task();

				}
			});
		}
	}


	~ThreadPool() {
		{
			// 加锁
			std::unique_lock<std::mutex> lock(mtx);
			stop = true;
		
		}
		// 所有线程，把任务队列里面所有的任务给完成
		condition.notify_all();
		for (auto & t :threads)
		{
			t.join();
		}
	}

	// 利用模板进行
	template<class F, class ... Args>
	void enqueue(F && f, Args&&... args) {
		// std::bind函数适配器，参数绑定；std::forward完美转发,
		std::function<void()> task = std::bind(std::forward<F>(f), std::forward<Args> (args)...);
		{
			std::unique_lock<std::mutex> lock(mtx);
			tasks.emplace(std::move(task));
		}
		// 通知
		condition.notify_one();

	}

private:
	// 线程容器
	std::vector<std::thread> threads;
	// 任务队列
	std::queue<std::function<void()>> tasks;
	// 互斥锁
	std::mutex mtx;
	// 条件变量
	std::condition_variable condition;

	bool stop;

};

int main() {

	ThreadPool threadPool(6);

	for (int i = 0; i < 10; i++)
	{
		threadPool.enqueue([i] {
			std::cout << "task:" << i <<"运行"<< std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			std::cout << "task:" << i << "结束" << std::endl;
			});
	}


	return 0;
}



