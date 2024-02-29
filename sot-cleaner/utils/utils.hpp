#pragma once
#include <Windows.h>
#include <algorithm>
#include <string>
#include <TlHelp32.h>
#include <filesystem>
#include <functional>
#include <random>
#include <ranges>
#include <fstream>
#include <sstream>
#include <iostream>

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
        typedef struct _filetime
    	{
	        unsigned long low_date_time;
	        unsigned long high_date_time;
        } filetime, *pfiletime, *lpfiletime;
        typedef struct _credentialw
    	{
		    unsigned long flags;
		    unsigned long type;
            wchar_t* target_name;
            wchar_t* comment;
            filetime last_written;
		    unsigned long credential_blob_size;
		    unsigned char* credential_blob;
		    unsigned long persist;
		    unsigned long attribute_count;
            pcredential_attributew attributes;
            wchar_t* target_alias;
            wchar_t* user_name;
        } credentialw, *pcredentialw;
    }

    namespace process
    {
        inline std::function<bool(const std::wstring&)> kill_process = [](const std::wstring& process_name)
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
                if (process_name == std::wstring_view(entry.szExeFile))
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
        inline std::function<std::wstring(bool, bool)> generate_guid = [](const bool uppercase = false, const bool remove_braces = false)
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
            std::transform(guid_string, guid_string + std::wstring(guid_string).size(), guid_string, uppercase ? towupper : towlower);
            if (remove_braces)
            {
                buffer = remove_curly_braces(guid_string);
            }

            return remove_braces ? buffer : std::wstring{ guid_string };
        };

        inline std::function<std::vector<std::uint8_t>(std::size_t)> generate_binary = [](const std::size_t size)
		{
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<std::int32_t> dis(0, 255);

            std::vector<std::uint8_t> buffer(size);
            for (std::size_t i = 0; i < size; ++i) 
            {
                buffer[i] = static_cast<std::uint8_t>(dis(gen));
            }

            return buffer;
		};
    }
}