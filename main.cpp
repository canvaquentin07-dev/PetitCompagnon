
#include <windows.h>
#include <cmath>

const wchar_t CLASS_NAME[] = L"PetitCompagnon";

float px = 500, py = 300;
float animation = 0;
float direction = 1;

void DrawCharacter(HDC dc)
{
    HBRUSH black = CreateSolidBrush(RGB(0,0,0));
    HBRUSH skin = CreateSolidBrush(RGB(255,205,160));
    HBRUSH white = CreateSolidBrush(RGB(245,245,245));
    HBRUSH blue = CreateSolidBrush(RGB(70,150,220));
    HBRUSH dark = CreateSolidBrush(RGB(30,35,45));

    HPEN outline = CreatePen(PS_SOLID, 3, RGB(20,20,25));
    HPEN limbs = CreatePen(PS_SOLID, 7, RGB(245,245,245));

    RECT r = {0,0,100,130};
    FillRect(dc, &r, black);

    HGDIOBJ oldBrush = SelectObject(dc, white);
    HGDIOBJ oldPen = SelectObject(dc, outline);

    int walk = (int)(sin(animation) * 12);

    // Jambes
    SelectObject(dc, dark);
    MoveToEx(dc, 42, 85, NULL);
    LineTo(dc, 38 + walk, 113);

    MoveToEx(dc, 58, 85, NULL);
    LineTo(dc, 62 - walk, 113);

    // Pieds
    SelectObject(dc, white);
    MoveToEx(dc, 38 + walk, 113, NULL);
    LineTo(dc, 27 + walk, 117);

    MoveToEx(dc, 62 - walk, 113, NULL);
    LineTo(dc, 73 - walk, 117);

    // Bras
    SelectObject(dc, limbs);
    MoveToEx(dc, 30, 55, NULL);
    LineTo(dc, 12, 72 - walk/2);

    MoveToEx(dc, 70, 55, NULL);
    LineTo(dc, 88, 72 + walk/2);

    // Corps
    SelectObject(dc, blue);
    RoundRect(dc, 30, 48, 70, 91, 12, 12);

    // Tête
    SelectObject(dc, skin);
    Ellipse(dc, 29, 7, 71, 49);

    // Cheveux
    SelectObject(dc, dark);
    Pie(dc, 28, 1, 72, 45, 28, 25, 72, 25);

    // Yeux
    SelectObject(dc, white);
    Ellipse(dc, 36, 22, 46, 33);
    Ellipse(dc, 54, 22, 64, 33);

    SelectObject(dc, dark);
    Ellipse(dc, 40, 25, 45, 31);
    Ellipse(dc, 55, 25, 60, 31);

    // Sourire
    SelectObject(dc, outline);
    Arc(dc, 40, 29, 60, 44, 40, 35, 60, 35);

    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);

    DeleteObject(black);
    DeleteObject(skin);
    DeleteObject(white);
    DeleteObject(blue);
    DeleteObject(dark);
    DeleteObject(outline);
    DeleteObject(limbs);
}

LRESULT CALLBACK WindowProc(
    HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch(msg)
    {
    case WM_CREATE:
        SetTimer(hwnd, 1, 16, NULL);
        RegisterHotKey(hwnd, 1, MOD_CONTROL | MOD_SHIFT, 'Q');
        return 0;

    case WM_TIMER:
    {
        POINT mouse;
        GetCursorPos(&mouse);

        float dx = mouse.x + 75 - px;
        float dy = mouse.y - 35 - py;

        if (fabs(dx) > 3 || fabs(dy) > 3)
        {
            px += dx * 0.055f;
            py += dy * 0.055f;
            animation += 0.35f;

            if (fabs(dx) > 2)
                direction = dx > 0 ? 1 : -1;
        }

        SetWindowPos(hwnd, HWND_TOPMOST,
            (int)px - 50, (int)py - 65,
            100, 130,
            SWP_NOACTIVATE);

        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);
        DrawCharacter(dc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_NCHITTEST:
        return HTTRANSPARENT;

    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;

    case WM_HOTKEY:
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        KillTimer(hwnd, 1);
        UnregisterHotKey(hwnd, 1);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE instance,
    HINSTANCE, LPSTR, int)
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED |
        WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        CLASS_NAME,
        L"Petit Compagnon",
        WS_POPUP,
        500, 300, 100, 130,
        NULL, NULL, instance, NULL
    );

    if (!hwnd) return 1;

    SetLayeredWindowAttributes(
        hwnd, RGB(0,0,0), 255, LWA_COLORKEY
    );

    ShowWindow(hwnd, SW_SHOWNOACTIVATE);

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

