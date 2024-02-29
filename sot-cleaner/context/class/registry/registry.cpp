#include <Windows.h>
#include <iostream>

#include "registry.hpp"

#include "../../ctx.hpp"

registry::registry() : _clean([this]
{
	/* clean registry */
	std::wcout << L"\n[*] cleaning registry\n";
	for (const auto& key : ctx::data->get_xbox_registry())
	{
		auto registry_result = m_winreg.TryOpen(std::get<0>(key), std::get<1>(key));
		if (!registry_result)
		{
			std::wcout << L"[-] failed to open " << std::get<1>(key) << L"\n";
		}

		registry_result = m_winreg.TryDeleteTree(std::get<2>(key));
		if (!registry_result)
		{
			std::wcout << L"[-] failed to delete " << std::get<1>(key) << L"\\" << std::get<2>(key) << L"\n";
		}
		else
		{
			std::wcout << L"[+] successfully deleted " << std::get<1>(key) << L"\\" << std::get<2>(key) << L"\n";
		}
		m_winreg.Close();
	}
}), _spoof_guid([this]
{
	/* spoof registry (guid) */
	std::wcout << L"\n[*] spoofing registry\n";
	for (const auto& key : ctx::data->get_guid_registry())
	{
		auto registry_result = m_winreg.TryOpen(std::get<0>(key), std::get<1>(key));
		if (!registry_result)
		{
			std::wcout << L"[-] failed to open " << std::get<1>(key) << L"\n";
		}

		registry_result = m_winreg.TrySetStringValue(std::get<2>(key), utils::cryptography::generate_guid(std::get<3>(key), std::get<4>(key)));
		if (!registry_result)
		{
			std::wcout << L"[-] failed to set " << std::get<1>(key) << L"\\" << std::get<2>(key) << L"\n";
		}
		else
		{
			std::wcout << L"[+] successfully spoofed " << std::get<1>(key) << L"\\" << std::get<2>(key) << L"\n";
		}
		m_winreg.Close();
	}
}), _spoof_binary([this]
{
	/* spoof registry (binary) */
	for (const auto& key : ctx::data->get_binary_registry())
	{
		auto registry_result = m_winreg.TryOpen(std::get<0>(key), std::get<1>(key));
		if (!registry_result)
		{
			std::wcout << L"[-] failed to open " << std::get<1>(key) << L"\n";
		}

		const auto value_size = m_winreg.TryGetBinaryValue(std::get<2>(key)).GetValue().size();
		registry_result = m_winreg.TrySetBinaryValue(std::get<2>(key), utils::cryptography::generate_binary(value_size));
		if (!registry_result)
		{
			std::wcout << L"[-] failed to set " << std::get<1>(key) << L"\\" << std::get<2>(key) << L"\n";
		}
		else
		{
			std::wcout << L"[+] successfully spoofed " << std::get<1>(key) << L"\\" << std::get<2>(key) << L"\n";
		}
		m_winreg.Close();
	}
}){}

auto registry::clean() const -> void
{
	_clean();
}

auto registry::spoof_guid() const -> void
{
	_spoof_guid();
}

auto registry::spoof_binary() const -> void
{
	_spoof_binary();
}