#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <winreg.h>

class data
{
public:
	auto get_xbox_registry()
	{
		return xbox_registry_data;
	}
	auto get_guid_registry()
	{
		return guid_registry_data;
	}
	auto get_binary_registry()
	{
		return binary_registry_data;
	}
	auto get_credentials()
	{
		return cred_data;
	}
	auto get_processes()
	{
		return process_data;
	}
	auto get_telemetry()
	{
		return telemetry_data;
	}
private:
	std::vector<std::tuple<HKEY, std::wstring, std::wstring>> xbox_registry_data = {
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
	std::vector<std::tuple<HKEY, std::wstring, std::wstring, bool /* uppercase */, bool /* remove braces */>> guid_registry_data = {
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
	std::vector<std::tuple<HKEY, std::wstring, std::wstring>> binary_registry_data = {
		{
			HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\Tcpip6\\Parameters", L"Dhcpv6DUID"
		},
		{
			HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\TPM\\WMI", L"WindowsAIKHash"
		}
	};
	std::vector<std::wstring> cred_data = {
		L"XboxLive",
		L"Xbl|DeviceKey",
		L"XblGrts|DeviceKey",
		L"Xbl|1717113201|3a81b95622cbf047|Production",
		L"Xbl_Ticket|1717113201|Production|860a62fd4fd198c4",
		L"Xbl_Ticket|1717113201|Production|3a81b95622cbf047",
		L"Xbl_Ticket|1717113201|Production|6f16af02d5497acd",
		L"Xbl_Ticket|804980065|Production|3a81b95622cbf047"
	};
	std::vector<std::wstring> process_data = {
		L"XboxPcApp.exe",
		L"XboxPcAppFT.exe",
		L"XboxGameBarWidgets.exe"
	};
	std::vector<std::wstring> telemetry_data = {
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
};
