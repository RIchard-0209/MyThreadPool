#include "threadpool.hpp"


ThreadPool::ThreadPool(unsigned int num = std::thread::hardware_concurrency())
	: _stop(false) {
	_thread_num = num <= 1 ? 2 : num;

	start();
}


ThreadPool::~ThreadPool() {
	stop();
}


/*
	@brief: 单例模式
				函数内局部的静态变量，其生命周期和进程同步，但是可见度仅在函数内部
*/
ThreadPool& ThreadPool::instance() {
	static ThreadPool ins;
	return ins;
}

/*
	@brief: 线程开始函数
	@note: 1. 从任务队列中取出任务
		   2. 执行任务
		   3. 任务执行完毕，线程数减一
			
*/
void ThreadPool::start() {
	for (int i = 0; i < _thread_num; ++ i) {
		_pool.emplace_back([this]() {
			while (!this->_stop.load()) {
				Task task;
				{
					std::unique_lock<std::mutex> lock(this->_cv_mtx);
					this->_cv_lock.wait(lock, [this]() {
						return this->_stop.load() || !this->_tasks.empty();
						});
					if (this->_tasks.empty()) return;

					task = std::move(this->_tasks.front());
					this->_tasks.pop();

				}
				--this->_thread_num;
				task();
				++this->_thread_num;
			}
		});
	}
}

/*
	@brief: 停止线程池
*/

void ThreadPool::stop() {
	_stop.store(true);
	_cv_lock.notify_all();
	for (auto& t : _pool) {
		if (t.joinable()) {
			std::cout << "join thread " << t.get_id() << std::endl;
			t.join();
		}
	}
}

/*
	@brief: 获得线程池中空闲线程的数量
*/
int ThreadPool::idleThreadCount() const {
	return _thread_num;
}



