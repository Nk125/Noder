#pragma once
#include <Include.pch>

class Threading : public httplib::TaskQueue {
public:
	static std::atomic<unsigned long long> id;

	typedef BS::thread_pool TP;

	static std::shared_ptr<TP> threader;

	virtual bool enqueue(std::function<void()>) override;

	virtual void shutdown() override;
};