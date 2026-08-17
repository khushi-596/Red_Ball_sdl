#include <graphics.h>
#include <iostream>
#include <cmath>

using namespace std;

// DDA Algorithm
void DDA(int x1,int y1,int x2,int y2)
{
    int dx=x2-x1;
    int dy=y2-y1;

    int steps=max(abs(dx),abs(dy));

    float Xinc=(float)dx/steps;
    float Yinc=(float)dy/steps;

    float x=x1;
    float y=y1;

    for(int i=0;i<=steps;i++)
    {
        putpixel(round(x),round(y),getcolor());
        x+=Xinc;
        y+=Yinc;
    }
}

// Bresenham Line Algorithm
void Bresenham(int x1,int y1,int x2,int y2)
{
    int dx=abs(x2-x1);
    int dy=abs(y2-y1);

    int sx=(x1<x2)?1:-1;
    int sy=(y1<y2)?1:-1;

    int err=dx-dy;

    while(true)
    {
        putpixel(x1,y1,getcolor());

        if(x1==x2 && y1==y2)
            break;

        int e2=2*err;

        if(e2>-dy)
        {
            err-=dy;
            x1+=sx;
        }

        if(e2<dx)
        {
            err+=dx;
            y1+=sy;
        }
    }
}

// Bresenham Circle Algorithm
void circleBres(int xc,int yc,int r)
{
    int x=0;
    int y=r;
    int delta=2*(1-r);
    int p;

    while(y>=x)
    {
        putpixel(xc+x,yc+y,getcolor());
        putpixel(xc-x,yc+y,getcolor());
        putpixel(xc+x,yc-y,getcolor());
        putpixel(xc-x,yc-y,getcolor());

        putpixel(xc+y,yc+x,getcolor());
        putpixel(xc-y,yc+x,getcolor());
        putpixel(xc+y,yc-x,getcolor());
        putpixel(xc-y,yc-x,getcolor());

        if(delta<0)
        {
            p=2*delta+2*y+1;

            if(p<=0)
            {
                x++;
                delta+=2*x+1;
            }
            else
            {
                x++;
                y--;
                delta+=2*x-2*y+2;
            }
        }
        else
        {
            p=2*delta-2*x-1;

            if(p<=0)
            {
                x++;
                y--;
                delta+=2*x-2*y+2;
            }
            else
            {
                y--;
                delta+=1-2*y;
            }
        }
    }
}

void drawGround()
{
    setcolor(WHITE);

    // Extended ground
    DDA(40,420,780,420);
}

void drawPlatform()
{
    setcolor(WHITE);

    DDA(220,300,450,300);

    DDA(450,300,450,420);
}

void drawWoodenBox()
{
    setcolor(BROWN);
    // square
    Bresenham(180,420,180,340);
    Bresenham(180,340,260,340);
    Bresenham(260,340,260,420);
    Bresenham(260,420,180,420);

    // cross
    Bresenham(180,340,260,420);
    Bresenham(260,340,180,420);

    // top and bottom strips
    DDA(180,355,260,355);
    DDA(180,405,260,405);

    setcolor(WHITE);
}

void drawWhiteBox()
{
    setcolor(WHITE);

    // Left side
    Bresenham(520,420,520,360);

    // Top
    Bresenham(520,360,580,360);

    // Right side
    Bresenham(580,360,580,420);

    // Bottom
    Bresenham(580,420,520,420);
}
void drawStairs()
{
    setcolor(WHITE);

    // First step
    Bresenham(620,420,620,390);
    Bresenham(620,390,680,390);

    // Second step
    Bresenham(680,390,680,360);
    Bresenham(680,360,740,360);
}

// Red Ball
void drawBall()
{
    setcolor(RED);
    circleBres(330,275,25);
}

void drawSun()
{
    setcolor(YELLOW);
    circleBres(540,90,35);
}

int main()
{
    int gd=DETECT,gm;
    initgraph(&gd,&gm,NULL);
    setbkcolor(BLACK);
    cleardevice();
    drawGround();
    drawPlatform();
    drawWoodenBox();
    drawWhiteBox();
    drawStairs();
    drawBall();
    drawSun();
    getch();
    closegraph();
    return 0;
}
