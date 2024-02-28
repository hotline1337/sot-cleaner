#include <Windows.h>

#include "libraries.hpp"
#include "../../ctx.hpp"

libraries::libraries() : _initialize([this]
{
	for (const auto& library : _libraries)
	{
		LoadLibraryW(library.data());
	}
}){}

auto libraries::initialize() const -> void
{
	_initialize();
}
