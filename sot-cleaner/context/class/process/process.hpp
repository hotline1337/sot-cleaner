#pragma once

#include <functional>

class process
{
public:
	process();
	auto kill_microsoft_apps() const -> void;
private:
	std::function<void()> _kill_microsoft_apps;
};