#include <windows.h>

#pragma comment(lib, "user32.lib")

HHOOK g_hook = nullptr;
bool  g_enabled = false;
HWND  g_button = nullptr;

bool IsOnlyWinPressed()
{
    return
        ((GetAsyncKeyState(VK_LWIN) & 0x8000) ||
         (GetAsyncKeyState(VK_RWIN) & 0x8000)) &&
        !(GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
        !(GetAsyncKeyState(VK_SHIFT) & 0x8000) &&
        !(GetAsyncKeyState(VK_MENU) & 0x8000);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && g_enabled)
    {
        auto* kbd = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) &&
            kbd->vkCode == 'H' &&
            IsOnlyWinPressed())
        {
            // Блокируем ТОЛЬКО Win+H
            return 1;
        }
    }

    return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

void ToggleHook()
{
    if (!g_enabled)
    {
        g_hook = SetWindowsHookEx(
            WH_KEYBOARD_LL,
            KeyboardProc,
            GetModuleHandle(nullptr),
            0
        );

        if (g_hook)
        {
            g_enabled = true;
            SetWindowText(g_button, L"Выключить Win + H");
        }
    }
    else
    {
        UnhookWindowsHookEx(g_hook);
        g_hook = nullptr;
        g_enabled = false;
        SetWindowText(g_button, L"Включить Win + H");
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        g_button = CreateWindowW(
            L"BUTTON",
            L"Включить Win + H",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            50, 40, 200, 40,
            hwnd,
            (HMENU)1,
            nullptr,
            nullptr
        );
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1)
            ToggleHook();
        break;

    case WM_DESTROY:
        if (g_enabled && g_hook)
            UnhookWindowsHookEx(g_hook);
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, PWSTR, int)
{
    const wchar_t CLASS_NAME[] = L"WinHBlocker";

    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Win + H Controller",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        300, 160,
        nullptr,
        nullptr,
        hInst,
        nullptr
    );

    ShowWindow(hwnd, SW_SHOW);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}