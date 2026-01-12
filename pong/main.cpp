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
        : x(x), y(y), width(w), height(h) {
    }
    ~sprite() { 

    }
};



struct Platforms : sprite {
    bool Solid = true;
    bool firstPos = true;
    float speed = 5;
    float firstX = 0;
    float secondX = 0;
    float firstY = 0;
    float secondY = 0;
    int current_loc = 0;

    Platforms () : sprite(){}

    Platforms(float x, float y, float w, float h,  int cur_loc, bool solid = true )
        : sprite(x, y, w, h), current_loc(cur_loc), Solid(solid) {
    }

    Platforms(float x, float y, float w, float h, int cur_loc, bool solid, float spd, float Fx,
        float Sx, float Fy, float Sy, bool Pos)
        : Platforms(x, y, w, h, cur_loc, solid)
    {
        speed = spd;
        firstX = Fx;
        secondX = Sx;
        firstY = Fy; 
        secondY = Sy;
        firstPos = Pos;
    }

    ~Platforms() {

    }
};

struct Character : sprite {
    int hp = 100; 
    int atackPower = 10;
    int current_loc = 0;
    int maxHp = 200;
    float speed = 15;
    float VelocityX = 0;
    float VelocityY = 0;
    string name = "Character";
    bool OnGround = false;

    Character() : sprite() {}

    Character(float x, float y, float w, float h) 
        : sprite(x, y, w, h){ }

    Character(float x, float y, float w, float h, int hitPoint, int atc, int cur_loc, int MHp, float spd,
        float velX, float velY, string n, bool onGround)
        : Character(x, y, w, h)
    {
        hp = hitPoint;
        atackPower = atc;
        current_loc = cur_loc;
        maxHp = MHp;
        speed = spd;
        VelocityX = velX;
        VelocityY = velY;
        name = n;
        OnGround = onGround;
    }

    ~Character(){
    
    }
};

struct Hero : Character {
     

    Hero() : Character() {

    }

    Hero(float x, float y, float w, float h)
        : Character(x, y, w, h)   {

    }

    ~Hero() {

    }
};

struct Enemy : Character {

    bool PatrolRight = true;

    Enemy() : Character() {

    }

    Enemy(float x, float y, float w, float h)
        : Character(x, y, w, h) {

    }

    Enemy(float x, float y, float w, float h, bool PatR)
        : Character(x, y, w, h) {
        PatrolRight = PatR;
    }

    ~Enemy() {
    
    }
};

HBITMAP hBack;// хэндл для фонового изображения
    
vector<Platforms> platform;
Hero hero(0, 0, 90, 180);
Enemy enemy(0, 0, 90, 190);
const float GRAVITY = 200.0f;

//cекция кода

void InitGame()
{
    hero.hBitmap = (HBITMAP)LoadImageA(NULL, "racket.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    enemy.hBitmap = (HBITMAP)LoadImageA(NULL, "racket_enemy.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    
    platform.push_back(Platforms(0, window.height - 30, window.width, 30, 0));// Пол, индекс 0

    platform.push_back(Platforms(200, window.height - 500, 350, 30, 0));//динамическая платформа индекс 1
    platform[1].firstX = platform[1].x;
    platform[1].firstY = platform[1].y;
    platform[1].secondX = platform[1].x + 300;
    platform[1].secondY = platform[1].y - 300;

    platform.push_back(Platforms(window.width - 400, 500, 400, 30, 0));//индекс 2
    platform.push_back(Platforms(1000, 300, 400, 30, 0));//индекс 3
    platform.push_back(Platforms(800, window.height - 300, 600, 30, 0)); //индекс 4

    for (int i = 0; i < platform.size(); i++) {
        platform[i].hBitmap = (HBITMAP)LoadImageA(NULL, "racket_enemy.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    }
    
    hBack = (HBITMAP)LoadImageA(NULL, "back.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    
    //------------------------------------------------------

    hero.y = window.height - hero.height - platform[0].height;

    enemy.x = window.width - enemy.width;
    enemy.y = window.height - enemy.height - platform[0].height;
    enemy.speed = 10;
}

bool CheckCollisions(float x, float y, float w, float h,
    float otherx, float othery, float otherw, float otherh)
{
    return (x < otherx + otherw &&
        x + w > otherx &&
        y < othery + otherh &&
        y + h > othery);
}

float approach(float current, float target, float speed)
{
    float dt = 1.0f / 60.0f;

    if (current < target) {
        // Увеличиваем current, но не больше чем target
        return min(current + speed * dt, target);
    }
    else {
        // Уменьшаем current, но не меньше чем target
        return max(current - speed * dt, target);
    }
}

void WorkCollisions(float& x, float& y, float w, float h,
    float otherx, float othery, float otherw, float otherh)
{

    float x1, x2, y1, y2, overx, overy, res;
    if (CheckCollisions(x, y, w, h,
        otherx, othery, otherw, otherh))
    {
        x1 = otherx - x + otherw;
        x2 = x + w - otherx;
        y1 = othery - y + otherh;
        y2 = y + h - othery;

        overx = min(x1, x2);
        overy = min(y1, y2);

        res = min(overx, overy);

        if (res == x1) {
            x = otherx + otherw;
            hero.VelocityX *= -0.05;

        }
        else if (res == x2) {
            x = otherx - w;
            hero.VelocityX *= -0.05;
        }
        else if (res == y1) {

            y = othery + otherh;
            hero.VelocityY *= -0.05;
        }
        else if (res == y2) {
            y = othery - h;
            hero.OnGround = true;
            enemy.OnGround = true;
            hero.speed = 22.5f;
            hero.VelocityY *= -0.05;
        }
    }
}

void ProcessSound(const char* name)
{
    PlaySound(TEXT(name), NULL, SND_FILENAME | SND_ASYNC);
        //ProcessSound("bounce.wav");
}

void ProcessInput()
{
    if (GetAsyncKeyState(VK_LEFT)) 
    {
        hero.VelocityX = approach(hero.VelocityX, -hero.speed, 150);
    }
    else if (GetAsyncKeyState(VK_RIGHT)) 
    {
        hero.VelocityX = approach(hero.VelocityX, hero.speed, 150);
    }
    else
    {
        hero.VelocityX = approach(hero.VelocityX, 0, 225);
    }
}

void HeroGravityAndJump() {

    if (!hero.OnGround) {
        // Плавно увеличиваем скорость падения
        hero.VelocityY = approach(hero.VelocityY, 800.0f, GRAVITY);
    }
    else {
        hero.VelocityY = 0;
    }

    if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && hero.OnGround) {
        hero.OnGround = false;
        hero.VelocityY = -50.0f;
        //hero.VelocityY = approach(hero.VelocityY, -500, 3000);
    }
}

void EnemyGravity()
{
    if (!enemy.OnGround) {
        
        enemy.VelocityY = approach(enemy.VelocityY, 1000.0f, GRAVITY);
    }
    else {
        enemy.VelocityY = 0;
    }

    if ((enemy.OnGround && hero.x < enemy.x)) {
        enemy.OnGround = false;
        enemy.VelocityY = -50.0f;
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


void WallsCheck()
{
    hero.x = max(hero.x, 0);//если коодината левого угла ракетки меньше нуля, присвоим ей ноль
    hero.x = min(hero.x, window.width - hero.width);//аналогично для правого угла
}

void MovePlat() {

    Platforms& p = platform[1];
    if (p.firstPos) {

        p.x += p.speed;
        p.y -= p.speed;

        if (p.x >= p.secondX && p.y <= p.secondY) {

            p.firstPos = false; 
            p.x = p.secondX;
            p.y = p.secondY;
        }
    }
    else {
        p.x -= p.speed;
        p.y += p.speed;

        if (p.x <= p.firstX && p.y >= p.firstY) {

            p.firstPos = true;
            p.x = p.firstX;
            p.y = p.firstY;
        }
    }
}

void EnemyMove()
{
    if (enemy.PatrolRight)
    {
        enemy.VelocityX = approach(enemy.VelocityX, enemy.speed, 50);
        if (enemy.x > window.width - enemy.width - 50)
        {
            enemy.PatrolRight = false;
        }
    }
    else
    {
        enemy.VelocityX = approach(enemy.VelocityX, -enemy.speed, 50);
        if (enemy.x <= window.width / 2)
        {
            enemy.PatrolRight = true;
        }
    }

}

void ProcessRoom()
{
    hero.OnGround = false;
    enemy.OnGround = false;

    MovePlat();

    hero.x += hero.VelocityX;
    hero.y += hero.VelocityY;

    enemy.x += enemy.VelocityX;
    enemy.y += enemy.VelocityY;

    for (int i = 0; i < platform.size(); i++) {
        WorkCollisions(hero.x, hero.y, hero.width, hero.height, platform[i].x, platform[i].y, platform[i].width, platform[i].height);
    }

    for (int i = 0; i < platform.size(); i++) {
        WorkCollisions(enemy.x, enemy.y, enemy.width, enemy.height, platform[i].x, platform[i].y, platform[i].width, platform[i].height);
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
    if (hero.OnGround) {

    _itoa_s(hero.x, txt, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt
    TextOutA(window.context, 10, 10, "tttt", 5);
    TextOutA(window.context, 200, 10, (LPCSTR)txt, strlen(txt));
    }
    else {
    _itoa_s(hero.y, txt, 10);
    TextOutA(window.context, 10, 100, "ffff", 5);
    TextOutA(window.context, 200, 100, (LPCSTR)txt, strlen(txt));
    }

    char debug[128];
    sprintf_s(debug, "VelocityY: %.1f", hero.VelocityY);
    TextOutA(window.context, 10, 200, debug, strlen(debug));
    sprintf_s(debug, "VelocityY: %.1f", enemy.VelocityY);
    TextOutA(window.context, 10, 400, debug, strlen(debug));

    sprintf_s(debug, "OnGround: %s", hero.OnGround ? "YES" : "NO");
    TextOutA(window.context, 10, 300, debug, strlen(debug));
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
        Sleep(16);//ждем 16 милисекунд (1/количество кадров в секунду)
        
        ProcessRoom();
        HeroGravityAndJump();
        EnemyGravity();
        ProcessInput();
        WallsCheck();
        EnemyMove();
    }
}
