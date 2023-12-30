#pragma once
#include <Include.pch>

namespace Time {
	typedef std::chrono::time_point<std::chrono::high_resolution_clock> timePoint;

	static timePoint timer;

#if SHOW_RPS
	// Used to get a hint with RPS
	static timePoint tp;
#endif

	static timePoint now() {
		return std::chrono::high_resolution_clock::now();
	}

	static inline std::chrono::milliseconds diff(const Time::timePoint& a, const Time::timePoint& b) {
		return std::chrono::duration_cast<std::chrono::milliseconds>(a - b);
	}
}