#pragma once
#include <chrono>
#include <memory>
#include <Server/ThreadPool/BSThreadPoolLight.hpp>
#include <thread>

namespace Threading {
	std::atomic<unsigned long long int> id = 0;

	typedef BS::thread_pool_light TP;

	std::shared_ptr<TP> threader;
}