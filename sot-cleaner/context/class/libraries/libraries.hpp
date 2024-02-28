#pragma once

#include <functional>
#include <vector>
#include <string>

class libraries
{
public:
	libraries();
	auto initialize() const -> void;
private:
	std::function<void()> _initialize;
	std::vector<std::wstring> _libraries = {
		L"advapi32.dll", L"kernel32.dll", L"ole32.dll", L"iphlpapi.dll"
	};
};