
#include <windows.h>
#include <filesystem>
#include <string>
#include <atomic>
#include <thread>

namespace fs = std::filesystem;

std::atomic<bool> running{true};
std::atomic<bool> creating{true};
std::atomic<int> count{0};

void createFolders()
{
    try
    {
        fs::path path =
            fs::temp_directory_path() / "Mes_Farces";

        fs::create_directories(path);

        for (int i = 1; i <= 1000000000 && creating; ++i)
        {
            path /= "Dossier_" + std::to_string(i);
            fs::create_directory(path);
            count = i;
            Sleep(1);
        }
    }
    catch (...)
    {
        // Arrêt en cas d'erreur.
    }

    creating = false;
}

LRESULT CALLBACK WindowProc(
    HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_DESTROY)
    {
        running = false;
        creating = false;
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(
    HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    WNDCLASSA wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = "PetitCompagnon";
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
        "PetitCompagnon",
        "Petit Compagnon",
        WS_POPUP,
        100, 100, 180, 180,
        nullptr, nullptr, instance, nullptr
    );

    SetLayeredWindowAttributes(
        hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);

    ShowWindow(hwnd, SW_SHOW);

    std::thread worker(createFolders);

    MSG msg{};

    while (running)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                running = false;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (GetAsyncKeyState(VK_F9) & 1)
            creating = false;

        if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
            (GetAsyncKeyState(VK_SHIFT) & 0x8000) &&
            (GetAsyncKeyState('Q') & 1))
        {
            running = false;
            creating = false;
        }

        Sleep(10);
    }

    creating = false;

    if (worker.joinable())
        worker.join();

    DestroyWindow(hwnd);

    return 0;
}
