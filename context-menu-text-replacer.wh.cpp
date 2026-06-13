// ==WindhawkMod==
// @id              context-menu-text-replacer
// @name            Context Menu Text Replacer
// @description     Replaces shortened labels in the Windows context menu.
// @version         1.0
// @author          Phonkovick
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Replaces shortened labels in the classic Windows context menu.

Current replacements:

П&ереим.      -> Пере&именовать
Созд. &ярлык  -> Создать &ярлык
&Копир.       -> &Копировать

Tested on:
- Windows 11 25H2
*/
// ==/WindhawkModReadme==

#include <windows.h>

typedef BOOL(WINAPI* InsertMenuItemW_t)(
    HMENU,
    UINT,
    BOOL,
    LPCMENUITEMINFOW
);

typedef BOOL(WINAPI* AppendMenuW_t)(
    HMENU,
    UINT,
    UINT_PTR,
    LPCWSTR
);

InsertMenuItemW_t pInsertMenuItemW;
AppendMenuW_t pAppendMenuW;

LPCWSTR ReplaceString(LPCWSTR text)
{
    if (!text)
        return text;

    if (wcscmp(text, L"П&ереим.") == 0)
        return L"Пере&именовать";

    if (wcscmp(text, L"Созд. &ярлык") == 0)
        return L"Создать &ярлык";

    if (wcscmp(text, L"&Копир.") == 0)
        return L"&Копировать";

    return text;
}

BOOL WINAPI InsertMenuItemWHook(
    HMENU hMenu,
    UINT item,
    BOOL byPosition,
    LPCMENUITEMINFOW lpmi)
{
    if (!lpmi)
    {
        return pInsertMenuItemW(
            hMenu,
            item,
            byPosition,
            lpmi
        );
    }

    MENUITEMINFOW copy = *lpmi;

    if ((copy.fMask & MIIM_STRING) &&
        copy.dwTypeData)
    {
        copy.dwTypeData =
            (LPWSTR)ReplaceString(copy.dwTypeData);
    }

    return pInsertMenuItemW(
        hMenu,
        item,
        byPosition,
        &copy
    );
}

BOOL WINAPI AppendMenuWHook(
    HMENU hMenu,
    UINT flags,
    UINT_PTR id,
    LPCWSTR text)
{
    text = ReplaceString(text);

    return pAppendMenuW(
        hMenu,
        flags,
        id,
        text
    );
}

BOOL Wh_ModInit()
{
    HMODULE user32 =
        GetModuleHandleW(L"user32.dll");

    Wh_SetFunctionHook(
        (void*)GetProcAddress(
            user32,
            "InsertMenuItemW"
        ),
        (void*)InsertMenuItemWHook,
        (void**)&pInsertMenuItemW
    );

    Wh_SetFunctionHook(
        (void*)GetProcAddress(
            user32,
            "AppendMenuW"
        ),
        (void*)AppendMenuWHook,
        (void**)&pAppendMenuW
    );

    return TRUE;
}

void Wh_ModUninit()
{
}