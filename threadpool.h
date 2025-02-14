#pragma once

#include <memory>

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
	~ThreadPool();

	static ThreadPool& instance() {
		static ThreadPool ins;
		return ins;
	}

private:
	ThreadPool();
};