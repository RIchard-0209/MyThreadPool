#pragma once

#include <memory>

class NoneCopy {
public:
	~NoneCopy() {}

protected:
	NoneCopy() {}

private:
	// É¾³ý¿½±´¹¹ÔìºÍ¿½±´¸³Öµ
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