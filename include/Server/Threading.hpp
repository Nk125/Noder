#pragma once
#include <Include.pch>

// Moved from Precompiled Header due to thread_info_index/_pool that didn't possible compiling
#include <BS_thread_pool.hpp>

namespace Threading {
	std::atomic<unsigned long long int> id = 0;

	typedef BS::thread_pool TP;

	std::shared_ptr<TP> threader;
}