#pragma once
#include <chrono>
#include <memory>
#include <Server/ThreadPool/BSThreadPool.hpp>
#include <thread>

namespace Threading {
	std::atomic<unsigned long long int> id = 0;

	typedef BS::thread_pool TP;

	std::shared_ptr<TP> threader;
}