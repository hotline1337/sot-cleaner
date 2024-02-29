#pragma once

#include <functional>
#include "../../../utils/libraries/winreg/winreg.hpp"

class network
{
public:
	network();
	auto apply_hosts_filter() const -> void;
	auto spoof_adapters() const -> void;
	auto restart_adapters() const -> void;
	auto flush_dns_cache() const -> void;
	auto restart_winmgmt() const -> void;
private:
	std::function<void()> _apply_hosts_filter;
	std::function<void()> _spoof_adapters;
	std::function<void()> _restart_adapters;
	std::function<void()> _flush_dns_cache;
	std::function<void()> _restart_winmgmt;
	inline static auto m_winreg = winreg::RegKey();
};
