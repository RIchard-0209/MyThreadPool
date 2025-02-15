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
	@brief: ����ģʽ
				�����ھֲ��ľ�̬���������������ںͽ���ͬ�������ǿɼ��Ƚ��ں����ڲ�
*/
ThreadPool& ThreadPool::instance() {
	static ThreadPool ins;
	return ins;
}

/*
	@brief: �߳̿�ʼ����
	@note: 1. �����������ȡ������
		   2. ִ������
		   3. ����ִ����ϣ��߳�����һ
			
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
	@brief: ֹͣ�̳߳�
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
	@brief: ����̳߳��п����̵߳�����
*/
int ThreadPool::idleThreadCount() const {
	return _thread_num;
}



