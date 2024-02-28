#include <iostream>

#include "process.hpp"
#include "../../ctx.hpp"

process::process() : _kill_microsoft_apps([this]
{
	/* kill xbox processes */
	std::wcout << L"[*] killing processes\n";
	for (const auto& process : ctx::data->get_processes())
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
}){}

auto process::kill_microsoft_apps() const -> void
{
	_kill_microsoft_apps();
}