//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS)
//configuration::advanced::character set - not set
//linker::input::additional dependensies Msimg32.lib; Winmm.lib

#include "windows.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct sprite {
    float x, y, width, height;
    HBITMAP hBitmap;

    sprite() : x(0), y(0), width(0), height(0), hBitmap(NULL) {}
    sprite(float x, float y, float w, float h)
        : x(x), y(y), width(w), height(h), hBitmap(NULL) {
    }
    ~sprite() { // деструктор

    }
};


struct Platforms : sprite {
    bool Solid;
    float MoveX, MoveY;

    Platforms(float x, float y, float w, float h, bool solid = true)
        : sprite(x, y, w, h), Solid(solid), MoveX(0), MoveY(0) {
    }
};



struct Character : sprite {
    int hp, hp, atackPower, current_loc, maxHp;
    string name;
};

struct Hero : Character {

};

struct Enemy : Character {
    float StartX, StartY;
};







// секция данных игры  
//typedef struct {
//    float x, y, width, height, rad, dx, dy, speed;
//    int hp, atackPower, current_loc, maxHp,num;
//    string name;
//    HBITMAP hBitmap;//хэндл к спрайту
//} sprite;
//
//
//sprite hero;//герой
//sprite enemy[2];
//sprite platform[5];
//
//struct {
//    HWND hWnd;//хэндл окна
//    HDC device_context, context;// два контекста устройства (для буферизации)
//    int width, height;//сюда сохраним размеры окна которое создаст программа
//} window;

int gravity = 18;
int all;
bool jump = false;

HBITMAP hBack;// хэндл для фонового изображения


//cекция кода

void InitGame()
{
    hero.hBitmap = (HBITMAP)LoadImageA(NULL, "racket.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    enemy[0].hBitmap = (HBITMAP)LoadImageA(NULL, "racket_enemy.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    for (int i = 0; i <= 4; i++) {
        platform[i].hBitmap = (HBITMAP)LoadImageA(NULL, "racket_enemy.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    }
    hBack = (HBITMAP)LoadImageA(NULL, "back.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    //------------------------------------------------------

    hero.x = 0;
    hero.height = 180;
    hero.y = window.height - hero.height;
    hero.width = 90;

    hero.speed = 15;
    //------------------------------------------------------
    platform[0].x = 150;
    platform[0].width = 350;
    platform[0].y = 700;
    platform[0].height = 30;

    platform[1].x = window.width / 2 - 350;
    platform[1].width = 700;
    platform[1].y = 450;
    platform[1].height = 30;

    platform[2].width = 350;
    platform[2].x = window.width - platform[2].width;
    platform[2].y = window.height - 200;
    platform[2].height = 30;

    platform[3].width = 350;
    platform[3].x = window.width - platform[3].width;
    platform[3].y = window.height / 2 - 200;
    platform[3].height = 30;

    platform[4].x = 0;
    platform[4].width = 350;
    platform[4].y = 200;
    platform[4].height = 30;
    //------------------------------------------------------
    enemy[0].x = platform[3].x;
    enemy[0].height = 180;
    enemy[0].y = platform[2].y - enemy[0].height;
    enemy[0].width = 90;
    enemy[0].speed = 12;
}

void ProcessSound(const char* name)
{
    PlaySound(TEXT(name), NULL, SND_FILENAME | SND_ASYNC);
        //ProcessSound("bounce.wav");
}

void ProcessInput()
{
    if (GetAsyncKeyState(VK_LEFT)) hero.x -= hero.speed;
    if (GetAsyncKeyState(VK_RIGHT)) hero.x += hero.speed;
}


void GravityAndJump() {

    hero.y += gravity;

    if (GetAsyncKeyState(VK_SPACE))
    {
        hero.y -= 110;
    }
}

void ShowBitmap(HDC hDC, int x, int y, int x1, int y1, HBITMAP hBitmapBall, bool alpha = false)
{
    HBITMAP hbm, hOldbm;
    HDC hMemDC;
    BITMAP bm;

    hMemDC = CreateCompatibleDC(hDC); // Создаем контекст памяти, совместимый с контекстом отображения
    hOldbm = (HBITMAP)SelectObject(hMemDC, hBitmapBall);// Выбираем изображение bitmap в контекст памяти

    if (hOldbm) // Если не было ошибок, продолжаем работу
    {
        GetObject(hBitmapBall, sizeof(BITMAP), (LPSTR)&bm); // Определяем размеры изображения

        if (alpha)
        {
            TransparentBlt(window.context, x, y, x1, y1, hMemDC, 0, 0, x1, y1, RGB(0, 0, 0));//все пиксели черного цвета будут интепретированы как прозрачные
        }
        else
        {
            StretchBlt(hDC, x, y, x1, y1, hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY); // Рисуем изображение bitmap
        }

        SelectObject(hMemDC, hOldbm);// Восстанавливаем контекст памяти
    }

    DeleteDC(hMemDC); // Удаляем контекст памяти
}

void ShowSprites()
{
    ShowBitmap(window.context, 0, 0, window.width, window.height, hBack);//задний фон
    ShowBitmap(window.context, hero.x, hero.y, hero.width, hero.height, hero.hBitmap);
    ShowBitmap(window.context, enemy[0].x, enemy[0].y, enemy[0].width, enemy[0].height, enemy[0].hBitmap);
    ShowBitmap(window.context, platform[0].x, platform[0].y, platform[0].width, platform[0].height, platform[0].hBitmap);
    ShowBitmap(window.context, platform[1].x, platform[1].y, platform[1].width, platform[1].height, platform[1].hBitmap);
    ShowBitmap(window.context, platform[2].x, platform[2].y, platform[2].width, platform[2].height, platform[2].hBitmap);
    ShowBitmap(window.context, platform[3].x, platform[3].y, platform[3].width, platform[3].height, platform[3].hBitmap);
    //ShowBitmap(window.context, platform[4].x, platform[4].y, platform[4].width, platform[4].height, platform[4].hBitmap);
}


void WallsCheck()
{
    hero.x = max(hero.x, 0);//если коодината левого угла ракетки меньше нуля, присвоим ей ноль
    hero.x = min(hero.x, window.width - hero.width);//аналогично для правого угла
}

void RoofAndFloorCheck()
{
    hero.y = max(hero.y, 0);
    hero.y = min(hero.y,window.height - hero.height);
}


bool CheckCollisions(float firstx, float firsty, float firstw, float firsth,
    float secondx, float secondy, float secondw, float secondh)
{
    return (firstx < secondx + secondw &&
        firstx + firstw > secondx &&
        firsty < secondy + secondh &&
        firsty + firsth > secondy);
}

void WorkCollisions(float& firstx, float& firsty, float& firstw, float& firsth, float& secondx, float& secondy, float& secondw, float& secondh)
{
   
    float x1, x2, y1, y2, overx, overy, res;
    if (CheckCollisions(firstx, firsty, firstw, firsth,
        secondx, secondy, secondw, secondh))
    {
        x1 = secondx - firstx + secondw;
        x2 = firstx + firstw - secondx;
        y1 = secondy - firsty + secondh;
        y2 = firsty + firsth - secondy;

        overx = min(x1, x2);
        overy = min(y1, y2);

        res = min(overx, overy);

        if (res == x1) {
            firstx = secondx + secondw;

        }
        else if (res == x2) {
            firstx = secondx - firstw;
        }
        else if (res == y1) {

            firsty = secondy + secondh;
        }
        else if (res == y2) {
            firsty = secondy - firsth;
        }
    } 
    else {
        
    }
}
void ShowScore()
{
    //поиграем шрифтами и цветами
    SetTextColor(window.context, RGB(160, 160, 160));
    SetBkColor(window.context, RGB(0, 0, 0));
    SetBkMode(window.context, TRANSPARENT);
    auto hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
    auto hTmp = (HFONT)SelectObject(window.context, hFont);

    char txt[32];//буфер для текста
    _itoa_s(hero.x, txt, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt
    TextOutA(window.context, 10, 10, "Score", 5);
    TextOutA(window.context, 200, 10, (LPCSTR)txt, strlen(txt));

    _itoa_s(hero.y, txt, 10);
    TextOutA(window.context, 10, 100, "Balls", 5);
    TextOutA(window.context, 200, 100, (LPCSTR)txt, strlen(txt));
}
        //ProcessSound("bounce.wav"); platform[0].y >= hero.y
void ProcessRoom()
{
    RoofAndFloorCheck();
    for (int i = 0; i <= 4; i++) {
        WorkCollisions(hero.x, hero.y, hero.width, hero.height, platform[i].x, platform[i].y, platform[i].width, platform[i].height);
    }

    /*CheckCollisions(hero.x, hero.y, hero.width, hero.height, platform[0].x, platform[0].y, platform[0].width, platform[0].height);
    WorkCollisions(hero.x, hero.y, hero.width, hero.height, platform[0].x, platform[0].y, platform[0].width, platform[0].height);*/
    
}

void InitWindow()
{
    SetProcessDPIAware();
    window.hWnd = CreateWindow("edit", 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0);

    RECT r;
    GetClientRect(window.hWnd, &r);
    window.device_context = GetDC(window.hWnd);//из хэндла окна достаем хэндл контекста устройства для рисования
    window.width = r.right - r.left;//определяем размеры и сохраняем
    window.height = r.bottom - r.top;
    window.context = CreateCompatibleDC(window.device_context);//второй буфер
    SelectObject(window.context, CreateCompatibleBitmap(window.device_context, window.width, window.height));//привязываем окно к контексту
    GetClientRect(window.hWnd, &r);

}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    
    InitWindow();//здесь инициализируем все что нужно для рисования в окне
    InitGame();//здесь инициализируем переменные игры

    ShowCursor(NULL);
    
    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        ShowSprites();//рисуем фон, ракетку и шарик
        ShowScore();
        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//копируем буфер в окно
        Sleep(16);//Bждем 16 милисекунд (1/количество кадров в секунду)
        
        ProcessRoom();//обрабатываем отскоки от стен и каретки, попадание шарика в картетку
        ProcessInput();//опрос клавиатуры
        WallsCheck();//проверяем, чтобы ракетка не убежала за экран
        GravityAndJump();
    }
}
