#pragma once

#include <functional>
#include "../../../utils/libraries/winreg/winreg.hpp"

class registry
{
public:
	registry();
	auto clean() const -> void;
	auto spoof_guid() const -> void;
	auto spoof_binary() const -> void;
private:
	std::function<void()> _clean;
	std::function<void()> _spoof_guid;
	std::function<void()> _spoof_binary;
	inline static auto m_winreg = winreg::RegKey();
};
