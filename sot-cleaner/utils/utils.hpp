#pragma once
#include <Windows.h>
#include <algorithm>
#include <print>
#include <string>
#include <codecvt>
#include <TlHelp32.h>
#include <filesystem>
#include <functional>
#include <random>
#include <ranges>
#include <fstream>
#include <sstream>
#include <iphlpapi.h>
#include <tchar.h>

#include "libraries/winreg/winreg.hpp"

namespace utils 
{
    /* dynamically resolve an import from a specified module */
    template <typename type>
    type dynamic_resolve(const std::string& module, const std::string& name)
    {
        const auto module_handle = GetModuleHandleA(module.c_str());
        const auto address = GetProcAddress(module_handle, name.c_str());
        return reinterpret_cast<type>(address);
    }

    namespace structs
    {
        typedef struct _credential_attributew
    	{
            wchar_t* keyword; /* e.g. "wininet" */
            unsigned long flags; /* 1 = encrypted, 2 = base64 encoded */
            unsigned long value_size; /* size of value in bytes */
            unsigned char* value; /* value data */
        } credential_attributew, *pcredential_attributew;
        typedef struct _credentialw
    	{
		    unsigned long flags;
		    unsigned long type;
            wchar_t* target_name;
            wchar_t* comment;
            FILETIME last_written;
		    unsigned long credential_blob_size;
		    unsigned char* credential_blob;
		    unsigned long persist;
		    unsigned long attribute_count;
            pcredential_attributew attributes;
            wchar_t* target_alias;
            wchar_t* user_name;
        } credentialw, *pcredentialw;
    }

    namespace network
    {
        inline std::function flush_dns_cache = []
        {
            /* command to flush dns cache */
            const std::wstring command = L"/c \"ipconfig /release && ipconfig /flushdns && ipconfig /renew && ipconfig /flushdns\"";

            /* create the process */
            SHELLEXECUTEINFOW shExInfo = { 0 };
            shExInfo.cbSize = sizeof(SHELLEXECUTEINFO);
            shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
            shExInfo.hwnd = nullptr;
            shExInfo.lpVerb = L"runas";
            shExInfo.lpFile = L"cmd.exe";
            shExInfo.lpParameters = command.c_str();
            shExInfo.lpDirectory = nullptr;
            shExInfo.nShow = SW_HIDE;
            shExInfo.hInstApp = nullptr;

            ShellExecuteExW(&shExInfo);
            WaitForSingleObject(shExInfo.hProcess, INFINITE);
            CloseHandle(shExInfo.hProcess);

            return true;
        };

        inline std::function restart_winmgmt = []
		{
			/* command to restart winmgmt service */
			const std::wstring command = L"/c \"net stop winmgmt && net start winmgmt\"";

			/* create the process */
            SHELLEXECUTEINFOW shExInfo = { 0 };
            shExInfo.cbSize = sizeof(SHELLEXECUTEINFO);
            shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
            shExInfo.hwnd = nullptr;
            shExInfo.lpVerb = L"runas";
            shExInfo.lpFile = L"cmd.exe";
            shExInfo.lpParameters = command.c_str();
            shExInfo.lpDirectory = nullptr;
            shExInfo.nShow = SW_HIDE;
            shExInfo.hInstApp = nullptr;

            ShellExecuteExW(&shExInfo);
            WaitForSingleObject(shExInfo.hProcess, INFINITE);
            CloseHandle(shExInfo.hProcess);

            return true;
		};

        inline std::function restart_adapters = []
        {
            /* command to restart network adapters */
            const std::wstring command = L"/c \"wmic path Win32_NetworkAdapter where PhysicalAdapter=TRUE call Disable && wmic path Win32_NetworkAdapter where PhysicalAdapter=TRUE call Enable\"";

            /* create the process */
            SHELLEXECUTEINFOW shExInfo = { 0 };
            shExInfo.cbSize = sizeof(SHELLEXECUTEINFO);
            shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
            shExInfo.hwnd = nullptr;
            shExInfo.lpVerb = L"runas";
            shExInfo.lpFile = L"cmd.exe";
            shExInfo.lpParameters = command.c_str();
            shExInfo.lpDirectory = nullptr;
            shExInfo.nShow = SW_HIDE;
            shExInfo.hInstApp = nullptr;

            ShellExecuteExW(&shExInfo);
            WaitForSingleObject(shExInfo.hProcess, INFINITE);
            CloseHandle(shExInfo.hProcess);

            return true;
        };

        inline std::function apply_hosts_filter = [](const std::vector<std::wstring>& block_list)
        {
            const auto get_system_root = []() -> std::wstring
            {
                wchar_t buffer[MAX_PATH];
                GetSystemDirectoryW(buffer,MAX_PATH);
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

            for (const auto& url : block_list)
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
                return true;
            }
            return false;
        };

        inline std::function spoof_adapters = []
        {
            /* initialize local lambdas */
            const auto generate_mac_address = []
			{
                std::wstringstream temp;
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<int> distribution(0, 253);

                for (int i = 0; i < 6; i++) 
                {
	                const int number = distribution(gen);
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
            static winreg::RegKey registry;
            auto registry_result = registry.TryOpen(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}");
            if (registry_result)
            {
	            const auto sub_keys = registry.TryEnumSubKeys().GetValue();
                for (const auto& sub_key : sub_keys)
                {
                    const std::wstring sub_key_name = L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}\\" + sub_key;
                    registry_result = registry.TryOpen(HKEY_LOCAL_MACHINE, sub_key_name);
                    if (registry_result)
                    {
                        auto enumerated_values = registry.TryEnumValues().GetValue();
                        for (const auto& key : enumerated_values | std::views::keys)
                        {
                            if (key == L"NetworkAddress")
							{
                                auto driver_desc = registry.TryGetStringValue(L"DriverDesc").GetValue();
                                std::wstring new_mac_address = generate_mac_address();
                                std::wstring new_mac_address_registry_buffer = new_mac_address;
                                new_mac_address_registry_buffer.erase(std::ranges::remove(new_mac_address_registry_buffer, '-').begin(), new_mac_address_registry_buffer.end());

                                registry_result = registry.TrySetStringValue(L"NetworkAddress", new_mac_address_registry_buffer);
                                if (registry_result)
                                {
                                    std::wcout << L"[+] successfully changed the mac address of " << driver_desc << " to " << new_mac_address << L"\n";
                                }
                                else
                                {
                                    std::wcout << L"[-] failed to change the mac address of " << driver_desc << L"\n";
                                }
                                registry.Close();
							}
                        }
                    }
                }
                registry.Close();
            }
            else
            {
                std::wcout << L"[-] failed to open Net class\n";
                return false;
            }
            return true;
        };
    }

    namespace process
    {
        inline std::function kill_process = [](const std::wstring& process_name)
        {
            PROCESSENTRY32W entry = {};
            entry.dwSize = sizeof(PROCESSENTRY32W);

            /* create a snapshot of all running processes */
            const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (snapshot == INVALID_HANDLE_VALUE)
            {
                std::wcout << L"[-] failed to create process snapshot\n";
                return false;
            }

            /* iterate through all processes */
            if (!Process32FirstW(snapshot, &entry))
            {
                CloseHandle(snapshot);
                std::wcout << L"[-] failed to get first process\n";
                return false;
            }

            do
            {
                /* check if the process name matches the one we want to kill */
                if (_wcsicmp(entry.szExeFile, process_name.c_str()) == 0)
                {
                    /* open the process and terminate it */
                    const auto process = OpenProcess(PROCESS_TERMINATE, FALSE, entry.th32ProcessID);
                    if (process != nullptr)
                    {
                        TerminateProcess(process, 0);
                        CloseHandle(process);
                        CloseHandle(snapshot);
                        return true;
                    }
                }
            } while (Process32NextW(snapshot, &entry)); /* iterate through all processes */

            /* close the snapshot handle */
			CloseHandle(snapshot);
            return false;
        };
    }

    namespace cryptography
    {
        inline std::function generate_guid = [](const bool uppercase = false, const bool remove_braces = false)
        {
            /* remove the curly braces from the guid string */
            const auto remove_curly_braces = [](const std::wstring& input)
            {
                std::wstring output = input;

                /* find and erase the curly braces */
                const size_t start = output.find_first_of(L'{');
                const size_t end = output.find_first_of(L'}');

                if (start != std::wstring::npos && end != std::wstring::npos && start < end)
                {
                    output.erase(start, 1);  /* erase the opening curly brace */
                    output.erase(end - 1, 1);  /* erase the closing curly brace */
                }
                return output;
            };

            /* initialize dynamic imports */
            const static auto co_create_guid = dynamic_resolve<long(__stdcall*)(GUID*)>("ole32.dll", "CoCreateGuid");
            const static auto string_from_guid = dynamic_resolve<int(__stdcall*)(const GUID&, wchar_t*, int)>("ole32.dll", "StringFromGUID2");

            /* generate a new guid */
            GUID guid = {};
            co_create_guid(&guid);

            /* convert the guid to a string */
            wchar_t guid_string[40];
            string_from_guid(guid, guid_string, std::size(guid_string));

            /* formatting */
            std::wstring buffer;
            std::transform(guid_string, guid_string + wcslen(guid_string), guid_string, uppercase ? ::towupper : ::towlower);
            if (remove_braces)
            {
                buffer = remove_curly_braces(guid_string);
            }

            return remove_braces ? buffer : std::wstring{ guid_string };
        };

        inline std::function generate_binary = [](const std::size_t size)
		{
        	std::srand(static_cast<unsigned int>(std::time(nullptr)));
			std::vector<unsigned char> buffer(size);
			std::ranges::generate(buffer, []
			{
				return static_cast<unsigned char>(std::rand() % 0x100);
			});
			return buffer;
		};
    }
}