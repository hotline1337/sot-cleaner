#include <iostream>

#include "network.hpp"
#include "../../ctx.hpp"

network::network() : _apply_hosts_filter([this]
{
	std::wcout << L"\n[*] applying telemetry filters\n";

	/* initialize local lambdas */
	const auto get_system_root = []() -> std::wstring
	{
		wchar_t buffer[MAX_PATH];
		GetSystemDirectoryW(buffer, MAX_PATH);
		const std::filesystem::path path(buffer);
		return path.wstring();
	};

	std::filesystem::path hosts_file = std::filesystem::path(get_system_root()) / R"(drivers\etc\hosts)";
	std::wifstream file_input(hosts_file);
	std::vector<std::wstring> all_host_lines;

	if (file_input.is_open())
	{
		std::wstring line;
		while (std::getline(file_input, line))
		{
			all_host_lines.push_back(line);
		}
		file_input.close();
	}

	for (const auto& url : ctx::data->get_telemetry())
	{
		bool line_exists = false;
		for (const auto& line : all_host_lines)
		{
			if (line.contains(url))
			{
				line_exists = true;
				break;
			}
		}
		if (!line_exists)
		{
			all_host_lines.push_back(L"0.0.0.0            " + url);
			std::wcout << L"[+] successfully blocked " << url << L"\n";
		}
	}

	std::wofstream file_output(hosts_file);
	if (file_output.is_open())
	{
		for (const auto& line : all_host_lines)
		{
			file_output << line << L"\n";
		}
		file_output.close();
		std::wcout << L"[+] successfully applied telemetry filters\n";
		return;
	}
	std::wcout << L"[-] failed to apply telemetry filters\n";
}), _spoof_adapters([this]
{
	std::wcout << L"\n[*] spoofing network adapters\n";

	/* initialize local lambdas */
	const auto generate_mac_address = []() -> std::wstring
	{
		std::wstringstream temp;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution distribution(0, 253);

		for (std::int32_t i = 0; i < 6; i++)
		{
			const std::int32_t number = distribution(gen);
			if (i != 0)
			{
				temp << std::setfill(L'0') << std::setw(2) << std::hex << number;
			}
			else
			{
				temp << L"02";
			}

			if (i != 5)
			{
				temp << L"-";
			}
		}
		std::wstring result = temp.str();
		std::ranges::transform(result, result.begin(), [](const wchar_t c)
		{
			return std::toupper(c);
		});

		return result;
	};

	/* spoof mac */
	auto registry_result = m_winreg.TryOpen(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}");
	if (registry_result)
	{
		const auto sub_keys = m_winreg.TryEnumSubKeys().GetValue();
		for (const auto& sub_key : sub_keys)
		{
			const std::wstring sub_key_name = L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}\\" + sub_key;
			registry_result = m_winreg.TryOpen(HKEY_LOCAL_MACHINE, sub_key_name);
			if (registry_result)
			{
				const auto enumerated_values = m_winreg.TryEnumValues().GetValue();
				for (const auto& key : enumerated_values | std::views::keys)
				{
					if (key == L"NetworkAddress")
					{
						const auto driver_desc = m_winreg.TryGetStringValue(L"DriverDesc").GetValue();
						const auto new_mac_address = generate_mac_address();
						auto new_mac_address_registry_buffer = new_mac_address;
						new_mac_address_registry_buffer.erase(std::ranges::remove(new_mac_address_registry_buffer, '-').begin(), new_mac_address_registry_buffer.end());

						registry_result = m_winreg.TrySetStringValue(L"NetworkAddress", new_mac_address_registry_buffer);
						if (registry_result)
						{
							std::wcout << L"[+] successfully changed the mac address of " << driver_desc << " to " << new_mac_address << L"\n";
						}
						else
						{
							std::wcout << L"[-] failed to change the mac address of " << driver_desc << L"\n";
						}
						m_winreg.Close();
					}
				}
			}
		}
		m_winreg.Close();
	}
	else
	{
		std::wcout << L"[-] failed to open registry Net class\n";
		std::wcout << L"[-] failed to spoof network adapters\n";
		return;
	}
	std::wcout << L"[+] successfully spoofed network adapters\n";
}), _restart_adapters([this]
{
	std::wcout << L"\n[*] restarting network adapters\n";

	/* command to restart network adapters */
	const std::wstring command = L"/c \"wmic path Win32_NetworkAdapter where PhysicalAdapter=TRUE call Disable && wmic path Win32_NetworkAdapter where PhysicalAdapter=TRUE call Enable\"";

	/* create the process */
	SHELLEXECUTEINFOW execute_info = {};
	execute_info.cbSize = sizeof(SHELLEXECUTEINFO);
	execute_info.fMask = SEE_MASK_NOCLOSEPROCESS;
	execute_info.hwnd = nullptr;
	execute_info.lpVerb = L"runas";
	execute_info.lpFile = L"cmd.exe";
	execute_info.lpParameters = command.c_str();
	execute_info.lpDirectory = nullptr;
	execute_info.nShow = SW_HIDE;
	execute_info.hInstApp = nullptr;

	ShellExecuteExW(&execute_info);
	WaitForSingleObject(execute_info.hProcess, INFINITE);
	CloseHandle(execute_info.hProcess);
	std::wcout << L"[+] successfully restarted network adapters\n";
}), _flush_dns_cache([this]
{
	std::wcout << L"\n[*] flushing dns\n";

	/* command to flush dns cache */
	const std::wstring command = L"/c \"ipconfig /release && ipconfig /flushdns && ipconfig /renew && ipconfig /flushdns\"";

	/* create the process */
	SHELLEXECUTEINFOW execute_info = {};
	execute_info.cbSize = sizeof(SHELLEXECUTEINFO);
	execute_info.fMask = SEE_MASK_NOCLOSEPROCESS;
	execute_info.hwnd = nullptr;
	execute_info.lpVerb = L"runas";
	execute_info.lpFile = L"cmd.exe";
	execute_info.lpParameters = command.c_str();
	execute_info.lpDirectory = nullptr;
	execute_info.nShow = SW_HIDE;
	execute_info.hInstApp = nullptr;

	ShellExecuteExW(&execute_info);
	WaitForSingleObject(execute_info.hProcess, INFINITE);
	CloseHandle(execute_info.hProcess);
	std::wcout << L"[+] successfully flushed dns\n";
}), _restart_winmgmt([this]
{
	std::wcout << L"\n[*] restarting winmgmt\n";

	/* command to restart winmgmt service */
	const std::wstring command = L"/c \"net stop winmgmt && net start winmgmt\"";

	/* create the process */
	SHELLEXECUTEINFOW execute_info = {};
	execute_info.cbSize = sizeof(SHELLEXECUTEINFO);
	execute_info.fMask = SEE_MASK_NOCLOSEPROCESS;
	execute_info.hwnd = nullptr;
	execute_info.lpVerb = L"runas";
	execute_info.lpFile = L"cmd.exe";
	execute_info.lpParameters = command.c_str();
	execute_info.lpDirectory = nullptr;
	execute_info.nShow = SW_HIDE;
	execute_info.hInstApp = nullptr;

	ShellExecuteExW(&execute_info);
	WaitForSingleObject(execute_info.hProcess, INFINITE);
	CloseHandle(execute_info.hProcess);
	std::wcout << L"[+] successfully restarted winmgmt\n";
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