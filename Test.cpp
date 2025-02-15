#include "Threadpool.hpp"
#include <list>

template <typename T>
std::list<T> pool_thread_quick_sort(std::list<T> input) {
	if (input.empty()) return input;

	std::list<T> result;
	// 直接移动	  dest			  resource	移动一个范围内的所有元素
	result.splice(result.begin(), input, input.begin());

	T const& pivot = *result.begin();
	auto divide_point = std::partition(input.begin(), input.end(),
		[&](T const& t) { return t < pivot; });

	std::list<T> lower_part;
	lower_part.splice(lower_part.end(), input, input.begin(), divide_point);

	std::future<std::list<T>> new_lower = ThreadPool::instance().commit(&pool_thread_quick_sort<T>, lower_part);

	std::list<T> new_higher = pool_thread_quick_sort(input);
	result.splice(result.end(), new_higher);
	result.splice(result.begin(), new_lower.get());
	return result;
}

int main() {
	std::list<int> nlist = { 6,1,0,5,2,9,11 };

	//ThreadPool::instance().commit([]() {
	//	std::cout << "Hello, World!" << std::endl;
	//	});

	auto sortlist = pool_thread_quick_sort<int>(nlist);

	for (auto& value : sortlist) {
		std::cout << value << " ";
	}

	std::cout << std::endl;
}