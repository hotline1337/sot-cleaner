#include <iostream>
#include <format>
#include <fstream>
#include <vector>
#include <future>
#include <tuple>

#include "utils/libraries/winreg/winreg.hpp"
#include "utils/utils.hpp"

auto main() -> std::int32_t
{
	/* load libraries (just in case lol cuz i dont add the libraries to the linker) */
	LoadLibraryW(L"advapi32.dll");
	LoadLibraryW(L"kernel32.dll");
	LoadLibraryW(L"ole32.dll");
	LoadLibraryW(L"iphlpapi.dll");

	std::wcout << L"[~] sot-cleaner\n";
	std::wcout << L"[~] made with <3 by hotline\n\n";
	try
	{
		/* initialize data */
		const std::vector<std::tuple<HKEY, std::wstring, std::wstring>> xbox_registry_data = {
			{
				HKEY_CURRENT_USER, L"Software\\Microsoft", L"IdentityCRL"
			},
			{
				HKEY_USERS, L".DEFAULT\\Software\\Microsoft", L"IdentityCRL"
			},
			{
				HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\OneSettings", L"xbox"
			},
			{
				HKEY_LOCAL_MACHINE, L"Software\\Microsoft", L"XboxLive"
			},
			{
				HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft", L"Xbox"
			}
		};
		const std::vector<std::tuple<HKEY, std::wstring, std::wstring, bool /* uppercase */, bool /* remove braces */>> guid_registry_data = {
			{
				HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\IDConfigDB\\Hardware Profiles\\0001", L"HwProfileGUID", false, false
			},
			{
				HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\SQMClient", L"MachineId", false, false
			},
			{
				HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Cryptography", L"MachineGuid", false, true
			},
			{
				HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate", L"SusClientId", false, true
			},
			{
				HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"BuildGUID", false, true
			},
			{
				HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\0000", L"UserModeDriverGUID", true, false
			}
		};
		const std::vector<std::tuple<HKEY, std::wstring, std::wstring>> binary_registry_data = {
			{
				HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\Tcpip6\\Parameters", L"Dhcpv6DUID"
			},
			{
				HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\TPM\\WMI", L"WindowsAIKHash"
			},
			{
				HKEY_CURRENT_USER, L"Software\\Microsoft\\Direct3D", L"WHQLClass"
			}
		};
		const std::vector<std::wstring> cred_data = {
			L"XboxLive",
			L"Xbl|DeviceKey",
			L"XblGrts|DeviceKey",
			L"Xbl|1717113201|3a81b95622cbf047|Production",
			L"Xbl_Ticket|1717113201|Production|860a62fd4fd198c4",
			L"Xbl_Ticket|1717113201|Production|3a81b95622cbf047",
			L"Xbl_Ticket|1717113201|Production|6f16af02d5497acd",
			L"Xbl_Ticket|804980065|Production|3a81b95622cbf047"
		};
		const std::vector<std::wstring> process_data = {
			L"XboxPcApp.exe",
			L"XboxPcAppFT.exe",
			L"XboxGameBarWidgets.exe"
		};
		const std::vector<std::wstring> telemetry_data = {
			/* xbox */
			L"cdn.optimizely.com",
			L"analytics.xboxlive.com",
			L"cdf-anon.xboxlive.com",
			L"settings-ssl.xboxlive.com",
			/* microsoft */
			L"v20.events.data.microsoft.com",
			/* sea of thieves */
			L"athenaprod.maelstrom.gameservices.xboxlive.com",
		};

		/* kill xbox processes */
		std::wcout << L"[*] killing processes\n";
		for (const auto& process : process_data)
		{
			if (utils::process::kill_process(process))
			{
				std::wcout << L"[+] successfully killed " << process << L"\n";
			}
			else
			{
				std::wcout << L"[-] failed to kill " << process << L" : it might have been already terminated\n";
			}
		}

		/* clean registry */
		std::wcout << L"\n[*] cleaning registry\n";
		static winreg::RegKey registry;
		for (const auto& key : xbox_registry_data)
		{
			auto registry_result = registry.TryOpen(std::get<0>(key), std::get<1>(key));
			if (!registry_result)
			{
				std::wcout << L"[-] failed to open " << std::get<1>(key) << L"\n";
			}

			registry_result = registry.TryDeleteTree(std::get<2>(key));
			if (!registry_result)
			{
				std::wcout << L"[-] failed to delete " << std::get<1>(key) << L"\\" << std::get<2>(key) << L"\n";
			}
			else
			{
				std::wcout << L"[+] successfully deleted " << std::get<1>(key) << L"\\" << std::get<2>(key) << L"\n";
			}
			registry.Close();
		}

		/* spoof registry (guid) */
		std::wcout << L"\n[*] spoofing registry\n";
		for (const auto& key : guid_registry_data)
		{
			auto registry_result = registry.TryOpen(std::get<0>(key), std::get<1>(key));
			if (!registry_result)
			{
				std::wcout << L"[-] failed to open " << std::get<1>(key) << L"\n";
			}
			
			registry_result = registry.TrySetStringValue(std::get<2>(key), utils::cryptography::generate_guid(std::get<3>(key), std::get<4>(key)));
			if (!registry_result)
			{
				std::wcout << L"[-] failed to set " << std::get<1>(key) << L"\\" << std::get<2>(key) << L"\n";
			}
			else
			{
				std::wcout << L"[+] successfully spoofed " << std::get<1>(key) << L"\\" << std::get<2>(key) << L"\n";
			}
			registry.Close();
		}

		/* spoof registry (binary) */
		for (const auto& key : binary_registry_data)
		{
			auto registry_result = registry.TryOpen(std::get<0>(key), std::get<1>(key));
			if (!registry_result)
			{
				std::wcout << L"[-] failed to open " << std::get<1>(key) << L"\n";
			}

			auto value_size = registry.TryGetBinaryValue(std::get<2>(key)).GetValue().size();
			registry_result = registry.TrySetBinaryValue(std::get<2>(key), utils::cryptography::generate_binary(value_size));
			if (!registry_result)
			{
				std::wcout << L"[-] failed to set " << std::get<1>(key) << L"\\" << std::get<2>(key) << L"\n";
			}
			else
			{
				std::wcout << L"[+] successfully spoofed " << std::get<1>(key) << L"\\" << std::get<2>(key) << L"\n";
			}
			registry.Close();
		}

		/* clean cmdkeys */
		std::wcout << L"\n[*] cleaning credentials\n";
		const static auto cred_delete = utils::dynamic_resolve<int(__stdcall*)(const wchar_t*, unsigned long, unsigned long)>("advapi32.dll", "CredDeleteW");
		const static auto cred_read = utils::dynamic_resolve<int(__stdcall*)(const wchar_t*, unsigned long, unsigned long, utils::structs::pcredentialw*)>("advapi32.dll", "CredReadW");
		for (const auto& cred : cred_data)
		{
			cred_read(cred.data(), 1, 0, nullptr);
			if (GetLastError() != ERROR_NOT_FOUND)
			{
				const auto cred_delete_result = cred_delete(cred.data(), 1, 0);
				if (!cred_delete_result)
				{
					std::wcout << L"[-] failed to delete " << cred << L"\n";
				}
				else
				{
					std::wcout << L"[+] successfully deleted " << cred << L"\n";
				}
			}
			else
			{
				std::wcout << L"[-] failed to find " << cred << L"\n";
			}
		}

		/* apply telemetry filters */
		std::wcout << L"\n[*] applying telemetry filters\n";
		if (utils::network::apply_hosts_filter(telemetry_data))
		{
			std::wcout << L"[+] successfully applied telemetry filters\n";
		}
		else
		{
			std::wcout << L"[-] failed to apply telemetry filters\n";
		}

		/* spoof mac address */
		std::wcout << L"\n[*] spoofing network adapters\n";
		if (utils::network::spoof_adapters())
		{
			std::wcout << L"[+] successfully spoofed network adapters\n";
		}
		else
		{
			std::wcout << L"[-] failed to spoof network adapters\n";
		}

		/* flush dns */
		std::wcout << L"\n[*] flushing dns\n";
		if (utils::network::flush_dns_cache())
		{
			std::wcout << L"[+] successfully flushed dns\n";
		}
		else
		{
			std::wcout << L"[-] failed to flush dns\n";
		}

		/* restart winmgmt */
		std::wcout << L"\n[*] restarting winmgmt\n";
		if (utils::network::restart_winmgmt())
		{
			std::wcout << L"[+] successfully restarted winmgmt\n";
		}
		else
		{
			std::wcout << L"[-] failed to restart winmgmt\n";
		}

		/* restart network adapters */
		std::wcout << L"\n[*] restarting network adapters\n";
		if (utils::network::restart_adapters())
		{
			std::wcout << L"[+] successfully restarted network adapters\n";
		}
		else
		{
			std::wcout << L"[-] failed to restart network adapters\n";
		}
	}
	catch (const std::runtime_error& error)
	{
		std::wcout << L"\n\n[!] fatal error : " << error.what() << L"\n";
		std::wcin.get();
		return EXIT_FAILURE;
	}

	std::wcout << L"\n[~] finalized";
	std::wcin.get();
	return EXIT_SUCCESS;
}
