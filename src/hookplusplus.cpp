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

#include "hookplusplus.h"

using namespace HookPlusPlus;

Hook::Hook(std::string libraryName, std::string methodName, PVOID impostorFunction)
{
	if (libraryName.length() == 0)
		throw new std::runtime_error("libraryName must not be empty.");
	else if (methodName.length() == 0)
		throw new std::runtime_error("methodName must not be empty");
	else if (impostorFunction == NULL)
		throw new std::runtime_error("impostorFunction must not be null.");

	this->LibraryName = libraryName;
	this->MethodName = methodName;
	this->ImpostorFunction = impostorFunction;
}

Hook::~Hook()
{
	UnperformPatch();
}

VOID Hook::PerformPatch()
{
	if (this->IsPatched) return;

	HINSTANCE loadedLibrary = LoadLibraryA(this->LibraryName.c_str());

	if (loadedLibrary == NULL)
	{
		DWORD lastError = GetLastError();

		throw new std::runtime_error("Failed to load library \"" + this->LibraryName + "\" correctly. Status code: " + std::to_string(lastError));
	}

	this->OriginalFunction = GetProcAddress(loadedLibrary, this->MethodName.c_str());

	if (this->OriginalFunction == NULL)
	{
		DWORD lastError = GetLastError();

		throw new std::runtime_error("Failed to seek method address for \"" + this->MethodName + "\" correctly. Status code: " + std::to_string(lastError));
	}

	if (!ReadProcessMemory(GetCurrentProcess(), (LPVOID)this->OriginalFunction, this->OriginalHeader, PATCH_SIZE, NULL))
	{
		DWORD lastError = GetLastError();

		throw new std::runtime_error("Failed to read method header for \"" + this->MethodName + "\" correctly. Status code: " + std::to_string(lastError));
	}

#ifdef _M_AMD64
	BYTE trampolinePatch[PATCH_SIZE] =
	{
		0x49, 0xba, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, // mov r10, address
		0x41, 0xff, 0xe2									// jmp r10
	};
#elif _M_IX86
	BYTE trampolinePatch[PATCH_SIZE] =
	{
		0xE9, 0x0, 0x0, 0x0, 0x0 // jmp address
	};
#endif

#ifdef _M_AMD64
	RtlCopyMemory(&trampolinePatch[2], &this->ImpostorFunction, sizeof(ULONG_PTR));
#elif _M_IX86
	DWORD source = (DWORD)this->OriginalFunction + PATCH_SIZE;
	DWORD dest = (DWORD)this->ImpostorFunction;
	PDWORD offset = (PDWORD)(dest - source);

	RtlCopyMemory(&trampolinePatch[1], &offset, sizeof(ULONG_PTR));
#endif

	if (!WriteProcessMemory(GetCurrentProcess(), (LPVOID)this->OriginalFunction, trampolinePatch, PATCH_SIZE, NULL))
	{
		DWORD lastError = GetLastError();

		throw new std::runtime_error("Failed to write trampoline patch for \"" + this->MethodName + "\" correctly. Status code: " + std::to_string(lastError));
	}

	this->IsPatched = TRUE;

	FreeLibrary(loadedLibrary);
}

VOID Hook::UnperformPatch()
{
	if (!this->IsPatched) return;

	if (!WriteProcessMemory(GetCurrentProcess(), (LPVOID)this->OriginalFunction, this->OriginalHeader, PATCH_SIZE, NULL))
	{
		DWORD lastError = GetLastError();

		throw new std::runtime_error("Failed to unpatch function \"" + this->MethodName + "\" correctly. Status code: " + std::to_string(lastError));
	}

	this->IsPatched = FALSE;
}