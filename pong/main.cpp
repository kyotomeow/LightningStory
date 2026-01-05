//linker::system::subsystem  - Windows(/ SUBSYSTEM:WINDOWS)
//configuration::advanced::character set - not set
//linker::input::additional dependensies Msimg32.lib; Winmm.lib

#include "windows.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct {
    HWND hWnd;//хэндл окна
    HDC device_context, context;// два контекста устройства (для буферизации)
    int width, height;//сюда сохраним размеры окна которое создаст программа
} window;

struct sprite {
    float x, y, width, height;
    HBITMAP hBitmap;

    sprite() : x(0), y(0), width(0), height(0), hBitmap(NULL) {}
    sprite(float x, float y, float w, float h)
        : x(x), y(y), width(w), height(h), hBitmap(NULL) {
    }
    ~sprite() { 

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
    int hp, atackPower, current_loc, maxHp;
    float speed;
    string name;

    /*Character(float x, float y, float w, float h)
        :sprite(x, y, w, h), velocityX(0), velocityY(0), isOnGround(false) {
    }*/
};

struct Hero : Character {
    float VelocityX, VelocityY;
    bool OnGround;

    /*Hero(float x, float y, float w, float h)
        : Character(x, y, w, h), VelocityX(0), VelocityY(0), OnGround(false) {

    }*/
};

struct Enemy : Character {
    float StartX, StartY;
};

HBITMAP hBack;// хэндл для фонового изображения
    
vector<Platforms> platform;
Hero hero;
Enemy enemy;

//cекция кода

void InitGame()
{
    
    hero.hBitmap = (HBITMAP)LoadImageA(NULL, "racket.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    enemy.hBitmap = (HBITMAP)LoadImageA(NULL, "racket_enemy.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    
    platform.push_back(Platforms(200, window.height - 50, 350, 30));
    platform.push_back(Platforms(window.width - 400, 500, 300, 30));
    
    for (int i = 0; i < platform.size(); i++) {
        platform[i].hBitmap = (HBITMAP)LoadImageA(NULL, "racket_enemy.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    }
    
    hBack = (HBITMAP)LoadImageA(NULL, "back.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    
    //------------------------------------------------------
    
    hero.width = 90;
    hero.height = 180;
    hero.x = 0;
    hero.y = window.height - hero.height;
    hero.speed = 30;

    enemy.width = 90;
    enemy.height = 190;
    enemy.x = window.width - enemy.width;
    enemy.y = window.height - enemy.height;
    enemy.speed = 25;
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


//void GravityAndJump() {
//
//    hero.y += gravity;
//
//    if (GetAsyncKeyState(VK_SPACE))
//    {
//        hero.y -= 110;
//    }
//}

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

    for (int i = 0; i < platform.size(); i++) {
        ShowBitmap(window.context,
            platform[i].x,
            platform[i].y,
            platform[i].width,
            platform[i].height,
            platform[i].hBitmap);
    }
    
    ShowBitmap(window.context, enemy.x, enemy.y, enemy.width, enemy.height, enemy.hBitmap);

    ShowBitmap(window.context, hero.x, hero.y, hero.width, hero.height, hero.hBitmap);
    
}


//void WallsCheck()
//{
//    hero.x = max(hero.x, 0);//если коодината левого угла ракетки меньше нуля, присвоим ей ноль
//    hero.x = min(hero.x, window.width - hero.width);//аналогично для правого угла
//}
//
//void RoofAndFloorCheck()
//{
//    hero.y = max(hero.y, 0);
//    hero.y = min(hero.y,window.height - hero.height);
//}


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
//void ShowScore()
//{
//    //поиграем шрифтами и цветами
//    SetTextColor(window.context, RGB(160, 160, 160));
//    SetBkColor(window.context, RGB(0, 0, 0));
//    SetBkMode(window.context, TRANSPARENT);
//    auto hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
//    auto hTmp = (HFONT)SelectObject(window.context, hFont);
//
//    char txt[32];//буфер для текста
//    _itoa_s(hero.x, txt, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt
//    TextOutA(window.context, 10, 10, "Score", 5);
//    TextOutA(window.context, 200, 10, (LPCSTR)txt, strlen(txt));
//
//    _itoa_s(hero.y, txt, 10);
//    TextOutA(window.context, 10, 100, "Balls", 5);
//    TextOutA(window.context, 200, 100, (LPCSTR)txt, strlen(txt));
//}
        //ProcessSound("bounce.wav"); platform[0].y >= hero.y
//void ProcessRoom()
//{
//    RoofAndFloorCheck();
//    for (int i = 0; i <= 4; i++) {
//        WorkCollisions(hero.x, hero.y, hero.width, hero.height, platform[i].x, platform[i].y, platform[i].width, platform[i].height);
//    }
//
//}

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
        //ShowScore();
        BitBlt(window.device_context, 0, 0, window.width, window.height, window.context, 0, 0, SRCCOPY);//копируем буфер в окно
        Sleep(16);//ждем 16 милисекунд (1/количество кадров в секунду)
        
        //ProcessRoom();//обрабатываем отскоки от стен и каретки, попадание шарика в картетку
        ProcessInput();//опрос клавиатуры
        //WallsCheck();//проверяем, чтобы ракетка не убежала за экран
        //GravityAndJump();
    }
}
