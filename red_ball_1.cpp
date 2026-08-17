#include <graphics.h>
#include <iostream>
#include <cmath>

using namespace std;

//------------------------------
// DDA Algorithm
//------------------------------
void DDA(int x1, int y1, int x2, int y2)
{
    int x = x2 - x1;
    int y = y2 - y1;

    int l;

    if(abs(x) < abs(y))
        l = abs(y);
    else
        l = abs(x);

    float dx = (float)x / l;
    float dy = (float)y / l;

    int i = 0;

    while(i <= l)
    {
        putpixel(round(x1), round(y1), WHITE);

        x1 = x1 + dx;
        y1 = y1 + dy;

        i++;
    }
}

//------------------------------
// Bresenham Algorithm
// (For 0 <= slope <= 1)
//------------------------------
void Bresenham(int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    // Case 1 : 0 <= m <= 1
    if(dx > 0 && dy >= 0 && abs(dx) >= abs(dy))
    {
        int p = 2*dy - dx;

        while(x1 <= x2)
        {
            putpixel(x1,y1,WHITE);

            if(p < 0)
                p += 2*dy;
            else
            {
                y1++;
                p += 2*dy - 2*dx;
            }
            x1++;
        }
    }

    // Case 2 : m > 1
    else if(dx > 0 && dy > 0 && abs(dy) > abs(dx))
    {
        int p = 2*dx - dy;

        while(y1 <= y2)
        {
            putpixel(x1,y1,WHITE);

            if(p < 0)
                p += 2*dx;
            else
            {
                x1++;
                p += 2*dx - 2*dy;
            }
            y1++;
        }
    }

    // Case 3 : -1 <= m < 0
    else if(dx > 0 && dy < 0 && abs(dx) >= abs(dy))
    {
        dy = abs(dy);

        int p = 2*dy - dx;

        while(x1 <= x2)
        {
            putpixel(x1,y1,WHITE);

            if(p < 0)
                p += 2*dy;
            else
            {
                y1--;
                p += 2*dy - 2*dx;
            }
            x1++;
        }
    }

    // Case 4 : m < -1
    else if(dx > 0 && dy < 0 && abs(dy) > abs(dx))
    {
        dy = abs(dy);

        int p = 2*dx - dy;

        while(y1 >= y2)
        {
            putpixel(x1,y1,WHITE);

            if(p < 0)
                p += 2*dx;
            else
            {
                x1++;
                p += 2*dx - 2*dy;
            }
            y1--;
        }
    }

    // Case 5 : Right to Left (0 <= m <= 1)
    else if(dx < 0 && dy <= 0 && abs(dx) >= abs(dy))
    {
        dx = abs(dx);
        dy = abs(dy);

        int p = 2*dy - dx;

        while(x1 >= x2)
        {
            putpixel(x1,y1,WHITE);

            if(p < 0)
                p += 2*dy;
            else
            {
                y1--;
                p += 2*dy - 2*dx;
            }
            x1--;
        }
    }

    // Case 6 : Right to Left (m > 1)
    else if(dx < 0 && dy < 0 && abs(dy) > abs(dx))
    {
        dx = abs(dx);
        dy = abs(dy);

        int p = 2*dx - dy;

        while(y1 >= y2)
        {
            putpixel(x1,y1,WHITE);

            if(p < 0)
                p += 2*dx;
            else
            {
                x1--;
                p += 2*dx - 2*dy;
            }
            y1--;
        }
    }

    // Case 7 : Right to Left (-1 <= m < 0)
    else if(dx < 0 && dy > 0 && abs(dx) >= abs(dy))
    {
        dx = abs(dx);

        int p = 2*dy - dx;

        while(x1 >= x2)
        {
            putpixel(x1,y1,WHITE);

            if(p < 0)
                p += 2*dy;
            else
            {
                y1++;
                p += 2*dy - 2*dx;
            }
            x1--;
        }
    }

    // Case 8 : Right to Left (m < -1)
    else if(dx < 0 && dy > 0 && abs(dy) > abs(dx))
    {
        dx = abs(dx);

        int p = 2*dx - dy;

        while(y1 <= y2)
        {
            putpixel(x1,y1,WHITE);

            if(p < 0)
                p += 2*dx;
            else
            {
                x1--;
                p += 2*dx - 2*dy;
            }
            y1++;
        }
    }
}

//------------------------------
// Draw Ground
//------------------------------
void drawGround()
{
    DDA(50,420,590,420);
}

//------------------------------
// Floating Platform
//------------------------------
void drawPlatform()
{
    DDA(220,300,420,300);
}

//------------------------------
// Wall
//------------------------------
void drawWall()
{
    DDA(420,420,420,300);
}

//------------------------------
// Bridge
//------------------------------
void drawBridge()
{
    DDA(70,180,170,180);

    DDA(70,180,70,140);
    DDA(170,180,170,140);
}

//------------------------------
// Flag
//------------------------------
void drawFlag()
{
    DDA(540,420,540,180);

    DDA(540,180,580,190);
    DDA(540,200,580,210);
}

//------------------------------
// Spikes
//------------------------------
void drawSpikes()
{
    for(int x=100;x<=300;x+=40)
    {
        Bresenham(x,420,x+20,390);
        Bresenham(x+20,390,x+40,420);
    }
}

//------------------------------
int main()
{
    int gd = DETECT, gm;

    initgraph(&gd, &gm, NULL);

    setbkcolor(BLACK);
    cleardevice();

    setcolor(WHITE);

    drawGround();
    drawPlatform();
    drawWall();
    drawBridge();
    drawFlag();
    drawSpikes();

    getch();
    closegraph();

    return 0;
}
