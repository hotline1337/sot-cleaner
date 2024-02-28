#include <Windows.h>
#include <iostream>

#include "credentials.hpp"
#include "../../ctx.hpp"

credentials::credentials() : _clean([this]
{
	/* clean cmdkeys */
	std::wcout << L"\n[*] cleaning credentials\n";
	const static auto cred_delete = utils::dynamic_resolve<int(__stdcall*)(const wchar_t*, unsigned long, unsigned long)>("advapi32.dll", "CredDeleteW");
	const static auto cred_read = utils::dynamic_resolve<int(__stdcall*)(const wchar_t*, unsigned long, unsigned long, utils::structs::pcredentialw*)>("advapi32.dll", "CredReadW");
	for (const auto& cred : ctx::data->get_credentials())
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
}){}

auto credentials::clean() const -> void
{
	_clean();
}
