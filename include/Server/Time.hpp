#pragma once
#include <Include.pch>

namespace Time {
	typedef std::chrono::time_point<std::chrono::high_resolution_clock> timePoint;

	timePoint timer;

	timePoint now() {
		return std::chrono::high_resolution_clock::now();
	}

	inline std::chrono::milliseconds diff(const timePoint& a, const timePoint& b) {
		return std::chrono::duration_cast<std::chrono::milliseconds>(a - b);
	}
}