#include <Server/Threading.hpp>

bool Threading::enqueue(std::function<void()> fn) {
	try {
		threader->detach_task(fn);
		return true;
	}
	catch (...) {
		return false;
	}
}

void Threading::shutdown() {
	threader->purge();
}

std::atomic<unsigned long long> Threading::id = 0;
std::shared_ptr<Threading::TP> Threading::threader = nullptr;