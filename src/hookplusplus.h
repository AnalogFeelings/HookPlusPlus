/*
 * HookPlusPlus - A lightweight 2-file C++ library for hooking in Windows-based systems.
 * Copyright (C) 2023 AestheticalZ
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <Windows.h>
#include <stdexcept>
#include <string>

#ifdef _M_AMD64
	#define PATCH_SIZE 13
#elif _M_IX86
	#define PATCH_SIZE 5
#endif

namespace HookPlusPlus
{
	class Hook
	{
	public:
		std::string LibraryName;
		std::string MethodName;
		BOOLEAN IsPatched = FALSE;

		Hook(std::string libraryName, std::string methodName, PVOID impostorFunction);
		~Hook();

		VOID PerformPatch();
		VOID UnperformPatch();

	private:
		PVOID ImpostorFunction = NULL;
		FARPROC OriginalFunction = NULL;

		CHAR OriginalHeader[PATCH_SIZE] = { 0 };
	};
}
