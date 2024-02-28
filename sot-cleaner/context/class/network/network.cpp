#include <iostream>

#include "network.hpp"
#include "../../ctx.hpp"

network::network() : _apply_hosts_filter([this]
{
	std::wcout << L"\n[*] applying telemetry filters\n";
	if (utils::network::apply_hosts_filter(ctx::data->get_telemetry()))
	{
		std::wcout << L"[+] successfully applied telemetry filters\n";
	}
	else
	{
		std::wcout << L"[-] failed to apply telemetry filters\n";
	}
}), _spoof_adapters([this]
{
	std::wcout << L"\n[*] spoofing network adapters\n";
	if (utils::network::spoof_adapters())
	{
		std::wcout << L"[+] successfully spoofed network adapters\n";
	}
	else
	{
		std::wcout << L"[-] failed to spoof network adapters\n";
	}
}), _restart_adapters([this]
{
	std::wcout << L"\n[*] restarting network adapters\n";
	if (utils::network::restart_adapters())
	{
		std::wcout << L"[+] successfully restarted network adapters\n";
	}
	else
	{
		std::wcout << L"[-] failed to restart network adapters\n";
	}
}), _flush_dns_cache([this]
{
	std::wcout << L"\n[*] flushing dns\n";
	if (utils::network::flush_dns_cache())
	{
		std::wcout << L"[+] successfully flushed dns\n";
	}
	else
	{
		std::wcout << L"[-] failed to flush dns\n";
	}
}), _restart_winmgmt([this]
{
	std::wcout << L"\n[*] restarting winmgmt\n";
	if (utils::network::restart_winmgmt())
	{
		std::wcout << L"[+] successfully restarted winmgmt\n";
	}
	else
	{
		std::wcout << L"[-] failed to restart winmgmt\n";
	}
}){}

auto network::apply_hosts_filter() const -> void
{
	_apply_hosts_filter();
}

auto network::spoof_adapters() const -> void
{
	_spoof_adapters();
}

auto network::restart_adapters() const -> void
{
	_restart_adapters();
}

auto network::flush_dns_cache() const -> void
{
	_flush_dns_cache();
}

auto network::restart_winmgmt() const -> void
{
	_restart_winmgmt();
}