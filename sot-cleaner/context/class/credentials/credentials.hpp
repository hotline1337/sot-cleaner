#pragma once

#include <functional>

class credentials
{
public:
	credentials();
	auto clean() const -> void;
private:
	std::function<void()> _clean;
};
