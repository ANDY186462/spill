// Spill.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "RacingGame.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name


// Bil 1 (WASD)
int car1X = 450;
int car1Y = 120;

// Bil 2 (Piltaster)
int car2X = 450;
int car2Y = 160;

//Felles fart
int carSpeed = 5;

//Styring
bool wDown = false;
bool sDown = false;
bool aDown = false;
bool dDown = false;

bool upDown = false;
bool downDown = false;
bool leftDown = false;
bool rightDown = false;

// Løpslogikk
int lapsToWin = 3;
int car1Laps = 0;
int car2Laps = 0;
bool car1CanCountLap = false;
bool car2CanCountLap = false;
//Målstrek
RECT startLine = { 500, 100, 600, 200 };

struct Obstacle
{
    int x, y;
    int width, height;
    int dx, dy; // fart og retning
};

Obstacle obstacles[] = {
    // Rektangulære hindringer
    {200, 300, 70, 30, 10, 0},
    {1050, 300, 70, 30, -5, 0},
    {1100, 360, 70, 30, -5, 0},
    {600, 500, 30, 70, 0, 5},
    {680, 500, 30, 70, 0, 5},
    {800, 500, 30, 70, 0, 5},

    //Kvadratiske hindringer
    {500, 550, 16, 16, 0, 0},
     {500, 500, 16, 16, 0, 0 },

     {450, 525, 16, 16, 0, 0},
     {450, 575, 16, 16, 0, 0 },

     {400, 550, 16, 16, 0, 0},
     {400, 500, 16, 16, 0, 0 },

     {350, 525, 16, 16, 0, 0},
     {350, 575, 16, 16, 0, 0 },

     {300, 500, 16, 16, 0, 0},
     {300, 550, 16, 16, 0, 0 },

     {250, 525, 16, 16, 0, 0},
     {250, 575, 16, 16, 0, 0 },

};

int numObstacles = sizeof(obstacles) / sizeof(obstacles[0]);

// Lag to arrays globalt for å huske hindringenes originale hastighet
int obstaclesOriginalDx[] = { 10, -10, 10, 0, 0, 1, // Rektangulære hindringer 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Kvadratiske hindringer
int obstaclesOriginalDy[] = { 0, 0, 10, 10, 5, 10, // Rektangulære hindringer
0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0 }; // Kvadratiske hindringer

// Funksjon for å sjekke om bilen er på banen (beige område)
bool isOnTrack(int x, int y, int width, int height)
{
    // Definer alle fire baneseksjoner
    RECT leftRect = { 100, 100, 250, 501 };       // venstre rektangel
    RECT rightRect = { 1050, 100, 1200, 501 };    // høyre rektangel
    RECT topRect = { 100, 100, 1200, 200 };     // øvre rektangel
    RECT bottomRect = { 100, 500, 1200, 600 };     // nedre rektangel

    RECT carRect = { x, y, x + width, y + height };

    auto overlaps = [](RECT a, RECT b) -> bool {
        return !(a.right < b.left || a.left > b.right || a.bottom < b.top || a.top > b.bottom);
        };

    // Sjekk om bilen overlapper minst ett rektangel på banen
    return overlaps(carRect, leftRect) ||
        overlaps(carRect, rightRect) ||
        overlaps(carRect, topRect) ||
        overlaps(carRect, bottomRect);
}

// Sjekk om bilen er på brun kantlinje
bool isOnEdge(int x, int y, int width, int height)
{
    // ---- Indre kanter ----
    RECT rightInner = { 1025, 200, 1050, 501 };
    RECT leftInner = { 250, 200, 275, 501 };
    RECT topInner = { 270, 200, 1050, 225 };
    RECT bottomInner = { 270, 475, 1050, 501 };

    // ---- Ytre kanter ----
    RECT rightOuter = { 1200, 75, 1225, 625 };
    RECT leftOuter = { 75, 75, 100, 625 };
    RECT topOuter = { 99, 75, 1200, 100 };
    RECT bottomOuter = { 99, 600, 1200, 625 };

    RECT carRect = { x, y, x + width, y + height };

    auto overlaps = [](RECT a, RECT b) -> bool {
        return !(a.right <= b.left ||
            a.left >= b.right ||
            a.bottom <= b.top ||
            a.top >= b.bottom);
        };

    return overlaps(carRect, rightInner) ||
        overlaps(carRect, leftInner) ||
        overlaps(carRect, topInner) ||
        overlaps(carRect, bottomInner) ||
        overlaps(carRect, rightOuter) ||
        overlaps(carRect, leftOuter) ||
        overlaps(carRect, topOuter) ||
        overlaps(carRect, bottomOuter);
}

bool overlaps(RECT a, RECT b)
{
    return !(a.right <= b.left ||
        a.left >= b.right ||
        a.bottom <= b.top ||
        a.top >= b.bottom);
}

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_RACINGGAME, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RACINGGAME));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RACINGGAME));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_RACINGGAME);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    SetTimer(hWnd, 1, 16, NULL); // ca 60 FPS

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // ---- Gress ----
        HBRUSH grassBrush = CreateSolidBrush(RGB(0, 170, 0));
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, grassBrush);
        Rectangle(hdc, 0, 0, 1300, 700);
        SelectObject(hdc, oldBrush);
        DeleteObject(grassBrush);

        // Fjern kantlinjer (kun én gang!)
        HPEN oldPen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));

        // ---- Bane ----
        HBRUSH baneBrush = CreateSolidBrush(RGB(200, 180, 180));
        oldBrush = (HBRUSH)SelectObject(hdc, baneBrush);

        Rectangle(hdc, 1050, 100, 1200, 501); //høyre rektangel
        Rectangle(hdc, 100, 100, 250, 501); //venstre rektangel
        Rectangle(hdc, 100, 100, 1200, 200); //øvre rektangel
        Rectangle(hdc, 100, 500, 1200, 600); //nedre rektangel

        SelectObject(hdc, oldBrush);
        DeleteObject(baneBrush);

        HBRUSH baneSvartBrush = CreateSolidBrush(RGB(112, 84, 62));
        oldBrush = (HBRUSH)SelectObject(hdc, baneSvartBrush);

        Rectangle(hdc, 1025, 200, 1050, 501);//høyre indre kant
        Rectangle(hdc, 250, 200, 275, 501); //venstre indre kant
        Rectangle(hdc, 270, 200, 1050, 225); //øvre indre kant
        Rectangle(hdc, 270, 475, 1050, 501); //nedre indre kant

        Rectangle(hdc, 1200, 75, 1225, 625); // høyre rektangel ytre
        Rectangle(hdc, 75, 75, 100, 625); // venstre rektangel ytre
        Rectangle(hdc, 99, 75, 1200, 100); // øvre rektangel ytre
        Rectangle(hdc, 99, 600, 1200, 625); // nedre rektangel ytre


        SelectObject(hdc, oldBrush);
        DeleteObject(baneSvartBrush);


        // ---- Start/Mål strek ----
        HPEN startPen = CreatePen(PS_SOLID, 20, RGB(0, 0, 0));
        HPEN oldStartPen = (HPEN)SelectObject(hdc, startPen);

        MoveToEx(hdc, 500, 100, NULL);
        LineTo(hdc, 500, 200);

        SelectObject(hdc, oldStartPen);
        DeleteObject(startPen);

        //Skrift midt på banen
        WCHAR lapText[100];
        swprintf_s(lapText, L"Red car: %d  Blue car: %d", car1Laps, car2Laps);

        // farge og font
        SetTextColor(hdc, RGB(255, 255, 255));  // hvit tekst
        SetBkMode(hdc, TRANSPARENT);            // gjennomsiktig bakgrunn

        // Font
        HFONT hFont = CreateFont(
            50, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
        HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

        //Teksten midt på banen
        TextOut(hdc, 400, 400, lapText, wcslen(lapText));

        //Rydd opp
        SelectObject(hdc, oldFont);
        DeleteObject(hFont);

        WCHAR headerText[] = L"Racing";
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);

        HFONT hHeaderFont = CreateFont(
            100, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
        HFONT oldOverskriftFont = (HFONT)SelectObject(hdc, hHeaderFont);

        TextOut(hdc, 350, 240, headerText, wcslen(headerText));

        SelectObject(hdc, oldOverskriftFont);
        DeleteObject(hHeaderFont);

        // ---- Hindringer ----
        HBRUSH obsBrush = CreateSolidBrush(RGB(200, 50, 67));
        HBRUSH oldObsBrush = (HBRUSH)SelectObject(hdc, obsBrush);

        for (int i = 0; i < numObstacles; i++)
        {
            Rectangle(hdc,
                obstacles[i].x,
                obstacles[i].y,
                obstacles[i].x + obstacles[i].width,
                obstacles[i].y + obstacles[i].height);
        }

        SelectObject(hdc, oldObsBrush);
        DeleteObject(obsBrush);

        // ---- Bil 1 ----
        HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
        oldBrush = (HBRUSH)SelectObject(hdc, redBrush);
        Rectangle(hdc, car1X, car1Y, car1X + 25, car1Y + 25);
        SelectObject(hdc, oldBrush);
        DeleteObject(redBrush);

        // ---- Bil 2 ----
        HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 255));
        oldBrush = (HBRUSH)SelectObject(hdc, blueBrush);
        Rectangle(hdc, car2X, car2Y, car2X + 25, car2Y + 25);
        SelectObject(hdc, oldBrush);
        DeleteObject(blueBrush);


        // Sett tilbake pennen HELT TIL SLUTT
        SelectObject(hdc, oldPen);

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_KEYDOWN:
    {
        if (wParam == 'W') wDown = true;
        if (wParam == 'S') sDown = true;
        if (wParam == 'A') aDown = true;
        if (wParam == 'D') dDown = true;

        if (wParam == 'I') upDown = true;
        if (wParam == 'K') downDown = true;
        if (wParam == 'J') leftDown = true;
        if (wParam == 'L') rightDown = true;
    }
    break;

    case WM_KEYUP:
    {
        if (wParam == 'W') wDown = false;
        if (wParam == 'S') sDown = false;
        if (wParam == 'A') aDown = false;
        if (wParam == 'D') dDown = false;

        if (wParam == 'I') upDown = false;
        if (wParam == 'K') downDown = false;
        if (wParam == 'J') leftDown = false;
        if (wParam == 'L') rightDown = false;
    }
    break;


    case WM_TIMER:
    {
        // -----------------------------
        // 1️⃣ Beregn ny posisjon for bil 1
        // -----------------------------
        int currentSpeed1 = carSpeed;
        if (isOnEdge(car1X, car1Y, 25, 25)) currentSpeed1 = 3;

        int newCar1X = car1X;
        int newCar1Y = car1Y;
        if (wDown) newCar1Y -= currentSpeed1;
        if (sDown) newCar1Y += currentSpeed1;
        if (aDown) newCar1X -= currentSpeed1;
        if (dDown) newCar1X += currentSpeed1;

        RECT nextCar1 = { newCar1X, newCar1Y, newCar1X + 25, newCar1Y + 25 };

        // -----------------------------
        // 2️⃣ Beregn ny posisjon for bil 2
        // -----------------------------
        int currentSpeed2 = carSpeed;
        if (isOnEdge(car2X, car2Y, 25, 25)) currentSpeed2 = 3;

        int newCar2X = car2X;
        int newCar2Y = car2Y;
        if (upDown) newCar2Y -= currentSpeed2;
        if (downDown) newCar2Y += currentSpeed2;
        if (leftDown) newCar2X -= currentSpeed2;
        if (rightDown) newCar2X += currentSpeed2;

        RECT nextCar2 = { newCar2X, newCar2Y, newCar2X + 25, newCar2Y + 25 };

        // -----------------------------
        // 3️⃣ Oppdater hindringer med kollisjon og bouncing
        // -----------------------------
        for (int i = 0; i < numObstacles; i++)
        {
            // Lag neste posisjon basert på original fart
            int newObsX = obstacles[i].x + obstaclesOriginalDx[i];
            int newObsY = obstacles[i].y + obstaclesOriginalDy[i];
            RECT obsNext = { newObsX, newObsY, newObsX + obstacles[i].width, newObsY + obstacles[i].height };

            // Sjekk kollisjon med biler
            bool collisionWithCar = overlaps(obsNext, nextCar1) || overlaps(obsNext, nextCar2);

            if (collisionWithCar)
            {
                // Stopp hindringen midlertidig
                obstacles[i].dx = 0;
                obstacles[i].dy = 0;
            }
            else
            {
                // Hvis hindringen ikke beveger seg, gjenopprett original hastighet
                if (obstacles[i].dx == 0 && obstacles[i].dy == 0)
                {
                    obstacles[i].dx = obstaclesOriginalDx[i];
                    obstacles[i].dy = obstaclesOriginalDy[i];
                }
            }

            // Sjekk bouncing på kanten basert på original fart
            if (!isOnTrack(obstacles[i].x + obstacles[i].dx, obstacles[i].y + obstacles[i].dy,
                obstacles[i].width, obstacles[i].height))
            {
                // Snu retning
                obstaclesOriginalDx[i] = -obstaclesOriginalDx[i];
                obstaclesOriginalDy[i] = -obstaclesOriginalDy[i];

                // Hvis hindringen beveger seg, oppdater dx/dy
                if (obstacles[i].dx != 0 || obstacles[i].dy != 0)
                {
                    obstacles[i].dx = obstaclesOriginalDx[i];
                    obstacles[i].dy = obstaclesOriginalDy[i];
                }
            }

            // Oppdater posisjon
            obstacles[i].x += obstacles[i].dx;
            obstacles[i].y += obstacles[i].dy;
        }

        // -----------------------------
        // 4️ Flytt bil 1 hvis ikke blokkert av hindring
        // -----------------------------
        bool car1Blocked = false;
        for (int i = 0; i < numObstacles; i++)
        {
            RECT obsRect = { obstacles[i].x, obstacles[i].y,
                             obstacles[i].x + obstacles[i].width,
                             obstacles[i].y + obstacles[i].height };
            if (overlaps(nextCar1, obsRect))
            {
                car1Blocked = true;
                break;
            }
        }
        if (!car1Blocked && isOnTrack(newCar1X, newCar1Y, 25, 25))
        {
            car1X = newCar1X;
            car1Y = newCar1Y;
        }

        // -----------------------------
        // 5️⃣ Flytt bil 2 hvis ikke blokkert av hindring
        // -----------------------------
        bool car2Blocked = false;
        for (int i = 0; i < numObstacles; i++)
        {
            RECT obsRect = { obstacles[i].x, obstacles[i].y,
                             obstacles[i].x + obstacles[i].width,
                             obstacles[i].y + obstacles[i].height };
            if (overlaps(nextCar2, obsRect))
            {
                car2Blocked = true;
                break;
            }
        }
        if (!car2Blocked && isOnTrack(newCar2X, newCar2Y, 25, 25))
        {
            car2X = newCar2X;
            car2Y = newCar2Y;
        }

        // -----------------------------
        // 6️⃣ Runde-telling
        // -----------------------------
        if (car1Y > 200) car1CanCountLap = true;
        if (car2Y > 200) car2CanCountLap = true;

        RECT car1Rect = { car1X, car1Y, car1X + 25, car1Y + 25 };
        RECT car2Rect = { car2X, car2Y, car2X + 25, car2Y + 25 };

        if (overlaps(car1Rect, startLine) && car1CanCountLap)
        {
            car1Laps++;
            car1CanCountLap = false;
            if (car1Laps >= lapsToWin)
            {
                KillTimer(hWnd, 1);
                MessageBox(hWnd, L"Bil 1 vinner!", L"FERDIG!", MB_OK);
            }
        }

        if (overlaps(car2Rect, startLine) && car2CanCountLap)
        {
            car2Laps++;
            car2CanCountLap = false;
            if (car2Laps >= lapsToWin)
            {
                KillTimer(hWnd, 1);
                MessageBox(hWnd, L"Bil 2 vinner!", L"FERDIG!", MB_OK);
            }
        }

        // -----------------------------
        // 7️⃣ Oppdater vinduet
        // -----------------------------
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
