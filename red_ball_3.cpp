#include <graphics.h>
#include <iostream>
#include <cmath>

using namespace std;


// ================= DDA LINE =================
void DDA(int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    int steps = max(abs(dx), abs(dy));

    float Xinc = (float)dx / steps;
    float Yinc = (float)dy / steps;

    float x = x1;
    float y = y1;

    for (int i = 0; i <= steps; i++)
    {
        putpixel(round(x), round(y), getcolor());

        x += Xinc;
        y += Yinc;
    }
}


// ================= BRESENHAM LINE =================
void Bresenham(int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int err = dx - dy;

    while (true)
    {
        putpixel(x1, y1, getcolor());

        if (x1 == x2 && y1 == y2)
            break;

        int e2 = 2 * err;

        if (e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }

        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}


// ================= BRESENHAM CIRCLE =================
void circleBres(int xc, int yc, int r)
{
    int x = 0;
    int y = r;

    int delta = 2 * (1 - r);
    int p;

    while (y >= x)
    {
        putpixel(xc + x, yc + y, getcolor());
        putpixel(xc - x, yc + y, getcolor());
        putpixel(xc + x, yc - y, getcolor());
        putpixel(xc - x, yc - y, getcolor());

        putpixel(xc + y, yc + x, getcolor());
        putpixel(xc - y, yc + x, getcolor());
        putpixel(xc + y, yc - x, getcolor());
        putpixel(xc - y, yc - x, getcolor());

        if (delta < 0)
        {
            p = 2 * delta + 2 * y + 1;

            if (p <= 0)
            {
                x++;
                delta += 2 * x + 1;
            }
            else
            {
                x++;
                y--;
                delta += 2 * x - 2 * y + 2;
            }
        }
        else
        {
            p = 2 * delta - 2 * x - 1;

            if (p <= 0)
            {
                x++;
                y--;
                delta += 2 * x - 2 * y + 2;
            }
            else
            {
                y--;
                delta += 1 - 2 * y;
            }
        }
    }
}


// ================= GROUND =================
void drawGround()
{
    setcolor(WHITE);
    DDA(40, 420, 780, 420);
}


// ================= PLATFORM =================
void drawPlatform()
{
    setcolor(WHITE);

    DDA(220, 300, 450, 300);
    DDA(450, 300, 450, 420);
}


// ================= WOODEN BOX =================
void drawWoodenBox()
{
    setcolor(BROWN);

    Bresenham(180, 420, 180, 340);
    Bresenham(180, 340, 260, 340);
    Bresenham(260, 340, 260, 420);
    Bresenham(260, 420, 180, 420);

    Bresenham(180, 340, 260, 420);
    Bresenham(260, 340, 180, 420);

    DDA(180, 355, 260, 355);
    DDA(180, 405, 260, 405);

    setcolor(WHITE);
}


// ================= WHITE BOX =================
void drawWhiteBox()
{
    setcolor(WHITE);

    Bresenham(520, 420, 520, 360);
    Bresenham(520, 360, 580, 360);
    Bresenham(580, 360, 580, 420);
    Bresenham(580, 420, 520, 420);
}


// ================= STAIRS =================
void drawStairs()
{
    setcolor(WHITE);

    Bresenham(620, 420, 620, 390);
    Bresenham(620, 390, 680, 390);

    Bresenham(680, 390, 680, 360);
    Bresenham(680, 360, 740, 360);
}


// ================= SUN =================
void drawSun()
{
    setcolor(YELLOW);
    circleBres(540, 90, 35);
}


// ================= DRAW BALL =================
void drawBall(int xc, int yc, int r, float angle)
{
    // Draw ONLY one circle
    setcolor(RED);
    circleBres(xc, yc, r);

    // Rotation indicator
    //int x2 = xc + (int)((r - 4) * cos(angle));
    //int y2 = yc + (int)((r - 4) * sin(angle));

    setcolor(RED);

    //Bresenham(xc, yc, x2, y2);
}


// ================= ERASE OLD BALL =================
void eraseBall(int xc, int yc, int r)
{
    // Completely remove old ball
    setcolor(BLACK);

    setfillstyle(SOLID_FILL, BLACK);
    fillellipse(xc, yc, r + 2, r + 2);

    // Restore platform line
    setcolor(WHITE);

    DDA(xc - r - 2, 300, xc + r + 2, 300);
}


// ================= MAIN =================
int main()
{
    int gd = DETECT;
    int gm;

    initgraph(&gd, &gm, NULL);

    setbkcolor(BLACK);

    // =========================================
    // INITIAL BALL POSITION
    // =========================================

    int ballX = 330;
    int ballY = 275;

    int radius = 25;

    float angle = 0;


    // =========================================
    // DRAW STATIC OBJECTS ONLY ONCE
    // =========================================

    cleardevice();

    drawGround();
    drawPlatform();
    drawWoodenBox();
    drawWhiteBox();
    drawStairs();
    drawSun();


    // Draw first ball
    drawBall(ballX, ballY, radius, angle);


    // =========================================
    // BALL ANIMATION
    // =========================================

    for (int i = 0; i < 55; i++)
    {
        delay(40);

        // Remove previous ball completely
        eraseBall(ballX, ballY, radius);

        // Translation
        ballX += 2;

        // Rotation
        angle += 0.20;

        // Draw ONLY the new ball
        drawBall(ballX, ballY, radius, angle);
    }


    closegraph();

    return 0;
}
