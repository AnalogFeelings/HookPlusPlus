# HookPlusPlus - Hooking made easy.
![Lines of code](https://img.shields.io/tokei/lines/github/AestheticalZ/hookplusplus?label=Lines%20of%20Code&style=flat-square)
![GitHub repo file count (custom path)](https://img.shields.io/github/directory-file-count/aestheticalz/hookplusplus/src?color=brightgreen&label=Library%20Files&style=flat-square)
![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/aestheticalz/hookplusplus?color=green&label=Code%20Size&style=flat-square)
![GitHub issues](https://img.shields.io/github/issues/aestheticalz/hookplusplus?label=Issues&style=flat-square)
![GitHub pull requests](https://img.shields.io/github/issues-pr/aestheticalz/hookplusplus?label=Pull%20Requests&style=flat-square)
![GitHub](https://img.shields.io/github/license/aestheticalz/hookplusplus?label=License&style=flat-square)
![GitHub Repo stars](https://img.shields.io/github/stars/aestheticalz/hookplusplus?label=Stargazers&style=flat-square)

HookPlusPlus is a lightweight 2-file C++ library for hooking in Windows-based systems! No more looking around Google for any way to hook under both x86_64 and x86, because HookPlusPlus does both.

:warning: **Important**: Please read [SECURITY.md](SECURITY.md) before using.

Usage is very simple, and below you can find some example code.

## Example
Say you have a DLL you want to attach whenever you want to debug something. In this case, here's the example code, with comments for your leisure.

```cpp
#include "hookplusplus.h"

// Define hooks.
HookPlusPlus::Hook* MessageBoxHookW = NULL;
HookPlusPlus::Hook* MessageBoxHookA = NULL;

// REMEMBER! The impostor function's signature must match 1:1 the function your trying to hook!
DWORD WINAPI HookedMessageBowW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
    // Unperform patch to be able to call the original function.
    MessageBoxHookW->UnperformPatch();

    DWORD returnCode = MessageBoxW(hWnd, L"Ah sorry mate, I just hooked this message box!\n- HookPlusPlus", L"Sorry!", uType);

    // Perform it again.
    MessageBoxHookW->PerformPatch();
    
    return returnCode;
}

// REMEMBER! The impostor function's signature must match 1:1 the function your trying to hook!
DWORD WINAPI HookedMessageBowA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
    // Unperform patch to be able to call the original function.
    MessageBoxHookA->UnperformPatch();

    DWORD returnCode = MessageBoxA(hWnd, "Ah sorry mate, I just hooked this message box!\n- HookPlusPlus", "Sorry!", uType);

    // Perform it again.
    MessageBoxHookA->PerformPatch();

    return returnCode;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD callReason, LPVOID lpReserved)
{
    switch (callReason)
    {
    case DLL_PROCESS_ATTACH:
        // Create the hooks.
        MessageBoxHookW = new HookPlusPlus::Hook("user32.dll", "MessageBoxW", &HookedMessageBowW);
        MessageBoxHookA = new HookPlusPlus::Hook("user32.dll", "MessageBoxA", &HookedMessageBowA);

        // Perform the patches.
        MessageBoxHookW->PerformPatch();
        MessageBoxHookA->PerformPatch();

        break;
    case DLL_PROCESS_DETACH:
        // Remember: deleting hooks automatically unperforms patches.
        delete MessageBoxHookW;
        delete MessageBoxHookA;

        break;
    }

    return TRUE;
}
```
