#pragma once

#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__


#include <memory>
#include <queue>
#include <vector>
#include <atomic>
#include <thread>
#include <future>
#include <mutex>
#include <iostream>

class NoneCopy {
public:
	~NoneCopy() {}

protected:
	NoneCopy() {}

private:
	// ɾ����������Ϳ�����ֵ
	NoneCopy(const NoneCopy&) = delete;

	NoneCopy& operator=(const NoneCopy&) = delete;
};

class ThreadPool : public NoneCopy {
public:
	using Task = std::packaged_task<void()>;

	~ThreadPool();

	// β���Ƶ�
	template <class F, class... Args>
	auto commit(F&& f, Args&&... args) ->
		std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))>;

	static ThreadPool& instance();

	int idleThreadCount() const;

private:
	ThreadPool(unsigned int num);
	void start();
	void stop();

private:
	std::atomic<int> _thread_num;	// �̳߳����̵߳�����
	std::queue<Task> _tasks;		// �������
	std::vector<std::thread> _pool;	// �̳߳�
	std::atomic<bool> _stop;		// �Ƿ�ֹͣ�̳߳�	
	std::mutex _cv_mtx;
	std::condition_variable _cv_lock;
};


template <class F, class... Args>
auto ThreadPool::commit(F&& f, Args&&... args) ->
std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))> {
	using _Ty = decltype(std::forward<F>(f)(std::forward<Args>(args)...));

	if (_stop.load()) {
		throw std::runtime_error("commit on ThreadPool is stopped.");
		return std::future<_Ty>();
	}

	auto task = std::make_shared<std::packaged_task<_Ty()>>(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...)
	);

	std::future<_Ty> res = task->get_future();

	{
		std::lock_guard<std::mutex> lock(_cv_mtx);
		_tasks.emplace([task]() {
			(*task)();	// ����task
			});

	}

	_cv_lock.notify_one();
	return res;
}


#endif // !__THREAD_POOL_H__