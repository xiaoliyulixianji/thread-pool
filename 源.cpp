#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <queue> // ����
#include <mutex> // ������
#include <condition_variable> // ��������
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
					// ����
					std::unique_lock<std::mutex> lock(mtx);
					// �ж���������Ƿ�������
					condition.wait(lock, [this] {
						return !tasks.empty() || stop;
					});
					// �߳���ֹ��ֱ�ӷ���
					if (stop && tasks.empty())
					{
						return;
					}

					// ��������У�ȡ����
					std::function<void()> task(std::move(tasks.front()));
					// ��ȡ�ߵ��������������ɾ��
					tasks.pop();

					// ����
					lock.unlock();
					// ִ������
					task();

				}
			});
		}
	}


	~ThreadPool() {
		{
			// ����
			std::unique_lock<std::mutex> lock(mtx);
			stop = true;
		
		}
		// �����̣߳�����������������е���������
		condition.notify_all();
		for (auto & t :threads)
		{
			t.join();
		}
	}

	// ����ģ�����
	template<class F, class ... Args>
	void enqueue(F && f, Args&&... args) {
		// std::bind�����������������󶨣�std::forward����ת��,
		std::function<void()> task = std::bind(std::forward<F>(f), std::forward<Args> (args)...);
		{
			std::unique_lock<std::mutex> lock(mtx);
			tasks.emplace(std::move(task));
		}
		// ֪ͨ
		condition.notify_one();

	}

private:
	// �߳�����
	std::vector<std::thread> threads;
	// �������
	std::queue<std::function<void()>> tasks;
	// ������
	std::mutex mtx;
	// ��������
	std::condition_variable condition;

	bool stop;

};

int main() {

	ThreadPool threadPool(6);

	for (int i = 0; i < 10; i++)
	{
		threadPool.enqueue([i] {
			std::cout << "task:" << i <<"����"<< std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			std::cout << "task:" << i << "����" << std::endl;
			});
	}


	return 0;
}



