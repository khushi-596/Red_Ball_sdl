#include <graphics.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <SDL2/SDL.h>
#include <vector>
using namespace std;

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define PI 3.14159265f

volatile bool windowClosed = false;

int SDLCALL closeEventWatcher(void*, SDL_Event* event)
{
    if (event->type == SDL_QUIT)
        windowClosed = true;
    return 0;
}

void safePutpixel(int x, int y, int color)
{
    if (x >= 0 && x < WIN_WIDTH && y >= 0 && y < WIN_HEIGHT)
        putpixel(x, y, color);
}

void DDA(int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1, dy = y2 - y1;
    int steps = max(abs(dx), abs(dy));

    if (steps == 0) {
        safePutpixel(x1, y1, getcolor());
        return;
    }

    float xInc = (float)dx / steps;
    float yInc = (float)dy / steps;
    float x = x1, y = y1;

    for (int i = 0; i <= steps; i++) {
        safePutpixel((int)round(x), (int)round(y), getcolor());
        x += xInc;
        y += yInc;
    }
}

void Bresenham(int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (true) {
        safePutpixel(x1, y1, getcolor());

        if (x1 == x2 && y1 == y2)
            break;

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void circleBres(int xc, int yc, int r)
{
    int x = 0, y = r;
    int delta = 2 * (1 - r);

    while (y >= x) {
        safePutpixel(xc + x, yc + y, getcolor());
        safePutpixel(xc - x, yc + y, getcolor());
        safePutpixel(xc + x, yc - y, getcolor());
        safePutpixel(xc - x, yc - y, getcolor());
        safePutpixel(xc + y, yc + x, getcolor());
        safePutpixel(xc - y, yc + x, getcolor());
        safePutpixel(xc + y, yc - x, getcolor());
        safePutpixel(xc - y, yc - x, getcolor());

        if (delta < 0) {
            int p = 2 * delta + 2 * y + 1;

            if (p <= 0) {
                x++;
                delta += 2 * x + 1;
            } else {
                x++;
                y--;
                delta += 2 * x - 2 * y + 2;
            }
        } else {
            int p = 2 * delta - 2 * x - 1;

            if (p <= 0) {
                x++;
                y--;
                delta += 2 * x - 2 * y + 2;
            } else {
                y--;
                delta += 1 - 2 * y;
            }
        }
    }
}

//Scan Filling
struct Point {
    int x, y;
};

void fillPolygon(const vector<Point>& poly, int color)
{
    int n = poly.size();
    if (n < 3)
        return;

    int yMin = poly[0].y;
    int yMax = poly[0].y;

    for (const Point& p : poly) {
        yMin = min(yMin, p.y);
        yMax = max(yMax, p.y);
    }

    yMin = max(yMin, 0);
    yMax = min(yMax, WIN_HEIGHT - 1);

    for (int y = yMin; y <= yMax; y++) {
        vector<float> intersections;

        for (int i = 0; i < n; i++) {
            Point p1 = poly[i];
            Point p2 = poly[(i + 1) % n];

            if (p1.y == p2.y)
                continue;

            int yLow = min(p1.y, p2.y);
            int yHigh = max(p1.y, p2.y);

            if (y >= yLow && y < yHigh) {
                float t = (float)(y - p1.y) / (p2.y - p1.y);
                float x = p1.x + t * (p2.x - p1.x);
                intersections.push_back(x);
            }
        }

        sort(intersections.begin(), intersections.end());

        for (size_t i = 0; i + 1 < intersections.size(); i += 2) {
            int x1 = (int)round(intersections[i]);
            int x2 = (int)round(intersections[i + 1]);

            for (int x = x1; x <= x2; x++)
                safePutpixel(x, y, color);
        }
    }
}

void fillCircleScanline(int xc, int yc, int r, int color)
{
    for (int dy = -r; dy <= r; dy++) {
        int span = (int)round(
            sqrtf((float)(r * r - dy * dy))
        );

        for (int dx = -span; dx <= span; dx++)
            safePutpixel(xc + dx, yc + dy, color);
    }
}

//Transformation
struct Mat3 {
    float m[3][3];
};

Mat3 matIdentity()
{
    Mat3 I = {{
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    }};
    return I;
}

Mat3 matTranslation(float tx, float ty)
{
    Mat3 T = {{
        {1, 0, 0},
        {0, 1, 0},
        {tx, ty, 1}
    }};
    return T;
}

Mat3 matRotation(float theta, int choice)
{
    float rad = theta * PI / 180.0f;
    Mat3 R;

    if (choice == 1) {
        R.m[0][0] = cos(rad);
        R.m[0][1] = sin(rad);
        R.m[1][0] = -sin(rad);
        R.m[1][1] = cos(rad);
    } else {
        R.m[0][0] = cos(rad);
        R.m[0][1] = -sin(rad);
        R.m[1][0] = sin(rad);
        R.m[1][1] = cos(rad);
    }

    R.m[0][2] = 0;
    R.m[1][2] = 0;
    R.m[2][0] = 0;
    R.m[2][1] = 0;
    R.m[2][2] = 1;

    return R;
}

Mat3 matMultiply(const Mat3& A, const Mat3& B)
{
    Mat3 R{};

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++)
                R.m[i][j] += A.m[i][k] * B.m[k][j];

    return R;
}

void matTransformPoint(
    const Mat3& M,
    float x,
    float y,
    float& outX,
    float& outY)
{
    outX = x * M.m[0][0] + y * M.m[1][0] + M.m[2][0];
    outY = x * M.m[0][1] + y * M.m[1][1] + M.m[2][1];
}

void drawSun(int xc, int yc, int r, float rotationDeg)
{
    setcolor(YELLOW);
    fillCircleScanline(xc, yc, r, YELLOW);
    circleBres(xc, yc, r);

    for (int i = 0; i < 360; i += 45) {
        Mat3 R = matRotation(i + rotationDeg, 2);

        float x1, y1, x2, y2;

        matTransformPoint(R, r + 8, 0, x1, y1);
        matTransformPoint(R, r + 25, 0, x2, y2);

        Bresenham(
            xc + (int)round(x1),
            yc + (int)round(y1),
            xc + (int)round(x2),
            yc + (int)round(y2)
        );
    }
}

void drawGround()
{
    vector<Point> p = {
        {20, 420}, {780, 420},
        {780, 434}, {20, 434}
    };

    setcolor(GREEN);
    fillPolygon(p, GREEN);
    DDA(20, 420, 780, 420);
}

void drawPlatform()
{
    int x1 = 165, y1 = 325, x2 = 380;

    vector<Point> p = {
        {x1, y1}, {x2, y1},
        {x2, 350}, {x1, 350}
    };

    setcolor(GREEN);
    fillPolygon(p, GREEN);

    DDA(x1, y1, x2, y1);
    DDA(x1, y1, x1, 350);
    DDA(x1, 350, x2, 350);
    DDA(x2, y1, x2, 420);
}

void drawStairs()
{
    setcolor(WHITE);

    fillPolygon({
        {510, 405}, {550, 405},
        {550, 420}, {510, 420}
    }, WHITE);

    fillPolygon({
        {550, 390}, {590, 390},
        {590, 420}, {550, 420}
    }, WHITE);

    fillPolygon({
        {590, 375}, {630, 375},
        {630, 420}, {590, 420}
    }, WHITE);

    DDA(470, 420, 510, 420);
    DDA(510, 420, 510, 405);
    DDA(510, 405, 550, 405);
    DDA(550, 405, 550, 390);
    DDA(550, 390, 590, 390);
    DDA(590, 390, 590, 375);
    DDA(590, 375, 630, 375);
    DDA(630, 375, 630, 420);
}

void drawWoodenBox(int x, int y)
{
    int w = 75, h = 75;

    setcolor(BROWN);

    fillPolygon({
        {x, y},
        {x + w, y},
        {x + w, y + h},
        {x, y + h}
    }, BROWN);

    DDA(x, y, x + w, y);
    DDA(x + w, y, x + w, y + h);
    DDA(x + w, y + h, x, y + h);
    DDA(x, y + h, x, y);

    Bresenham(x + 8, y + 8, x + w - 8, y + h - 8);
    Bresenham(x + w - 8, y + 8, x + 8, y + h - 8);
}

void drawEnemy(int x, int y)
{
    int w = 65, h = 55;

    setcolor(DARKGRAY);

    fillPolygon({
        {x, y},
        {x + w, y},
        {x + w, y + h},
        {x, y + h}
    }, DARKGRAY);

    DDA(x, y, x + w, y);
    DDA(x + w, y, x + w, y + h);
    DDA(x + w, y + h, x, y + h);
    DDA(x, y + h, x, y);

    setcolor(WHITE);
    fillCircleScanline(x + 20, y + 22, 6, WHITE);
    fillCircleScanline(x + 45, y + 22, 6, WHITE);

    setcolor(RED);
    fillCircleScanline(x + 20, y + 22, 2, RED);
    fillCircleScanline(x + 45, y + 22, 2, RED);

    setcolor(WHITE);
    DDA(x + 20, y + 39, x + 45, y + 39);
    DDA(x + 20, y + 39, x + 24, y + 43);
    DDA(x + 45, y + 39, x + 41, y + 43);
}

void drawRedBall(int xc, int yc, int r, float angle)
{
    setcolor(RED);
    fillCircleScanline(xc, yc, r, RED);
    circleBres(xc, yc, r);

    Mat3 R = matRotation(angle, 1);
    Mat3 T = matTranslation(xc, yc);
    Mat3 M = matMultiply(R, T);

    auto worldPt = [&](float x, float y, int& ox, int& oy) {
        float wx, wy;
        matTransformPoint(M, x, y, wx, wy);
        ox = (int)round(wx);
        oy = (int)round(wy);
    };

    int px, py;

    setcolor(WHITE);

    worldPt(-9, -5, px, py);
    fillCircleScanline(px, py, 7, WHITE);

    worldPt(9, -5, px, py);
    fillCircleScanline(px, py, 7, WHITE);

    setcolor(BLACK);

    worldPt(-9, -4, px, py);
    fillCircleScanline(px, py, 3, BLACK);

    worldPt(9, -4, px, py);
    fillCircleScanline(px, py, 3, BLACK);

    setcolor(WHITE);

    int x1, y1, x2, y2, x3, y3, x4, y4, x5, y5;

    worldPt(-13, 7, x1, y1);
    worldPt(-8, 13, x2, y2);
    worldPt(0, 16, x3, y3);
    worldPt(8, 13, x4, y4);
    worldPt(13, 7, x5, y5);

    Bresenham(x1, y1, x2, y2);
    Bresenham(x2, y2, x3, y3);
    Bresenham(x3, y3, x4, y4);
    Bresenham(x4, y4, x5, y5);
}

void drawStar(int xc, int yc)
{
    int points[10][2];

    for (int i = 0; i < 10; i++) {
        float angle = -PI / 2 + i * PI / 5;
        int radius = (i % 2 == 0) ? 15 : 7;

        points[i][0] = xc + (int)(radius * cos(angle));
        points[i][1] = yc + (int)(radius * sin(angle));
    }

    vector<Point> p;

    for (int i = 0; i < 10; i++)
        p.push_back({points[i][0], points[i][1]});

    setcolor(YELLOW);
    fillPolygon(p, YELLOW);

    for (int i = 0; i < 10; i++) {
        int next = (i + 1) % 10;

        Bresenham(
            points[i][0],
            points[i][1],
            points[next][0],
            points[next][1]
        );
    }
}

void drawFlag(int x, int y)
{
    setcolor(RED);

    fillPolygon({
        {x, y},
        {x + 35, y + 12},
        {x, y + 25}
    }, RED);

    setcolor(WHITE);

    DDA(x, y, x, y + 70);
    DDA(x, y, x + 35, y + 12);
    DDA(x + 35, y + 12, x, y + 25);
    DDA(x, y + 25, x, y);
}

struct Rect {
    float x, y, w, h;
};

float clampf(float v, float lo, float hi)
{
    return max(lo, min(v, hi));
}

bool circleRectOverlap(
    float cx, float cy, float radius,
    const Rect& r,
    float& nx, float& ny, float& pen)
{
    float closestX = clampf(cx, r.x, r.x + r.w);
    float closestY = clampf(cy, r.y, r.y + r.h);

    float dx = cx - closestX;
    float dy = cy - closestY;
    float distSq = dx * dx + dy * dy;

    if (distSq >= radius * radius)
        return false;

    float dist = sqrtf(distSq);

    if (dist > 0.0001f) {
        nx = dx / dist;
        ny = dy / dist;
        pen = radius - dist;
    } else {
        float left = cx - r.x;
        float right = r.x + r.w - cx;
        float top = cy - r.y;
        float bottom = r.y + r.h - cy;

        float m = min(min(left, right), min(top, bottom));

        if (m == top) {
            nx = 0; ny = -1; pen = radius + top;
        } else if (m == bottom) {
            nx = 0; ny = 1; pen = radius + bottom;
        } else if (m == left) {
            nx = -1; ny = 0; pen = radius + left;
        } else {
            nx = 1; ny = 0; pen = radius + right;
        }
    }

    return true;
}

bool resolveCircleRect(
    float& ballX, float& ballY, float radius,
    float& vx, float& vy, bool& grounded,
    const Rect& r)
{
    float nx, ny, pen;

    if (!circleRectOverlap(
            ballX, ballY, radius, r, nx, ny, pen))
        return false;

    ballX += nx * pen;
    ballY += ny * pen;

    if (fabs(ny) >= fabs(nx)) {
        if (ny < 0) {
            if (vy > 0) vy = 0;
            grounded = true;
        } else {
            if (vy < 0) vy = 0;
        }
    } else {
        vx = 0;
    }

    return true;
}

void moveBallWithCollision(
    float& ballX, float& ballY, float radius,
    float& vx, float& vy, bool& grounded,
    const vector<Rect>& solids)
{
    grounded = false;

    float dist = max(fabs(vx), fabs(vy));
    float maxStep = radius * 0.5f;
    int steps = max(1, (int)ceil(dist / maxStep));

    float stepX = vx / steps;
    float stepY = vy / steps;

    for (int s = 0; s < steps; s++) {
        ballX += stepX;
        ballY += stepY;

        for (const Rect& r : solids)
            resolveCircleRect(
                ballX, ballY, radius,
                vx, vy, grounded, r
            );
    }
}

int main()
{
    initwindow(WIN_WIDTH, WIN_HEIGHT);
    SDL_AddEventWatch(closeEventWatcher, NULL);

    setbkcolor(BLACK);
    cleardevice();

    float ballX = 30.0f;
    float ballY = 395.0f;
    int ballRadius = 25;

    float vx = 0.0f;
    float vy = 0.0f;

    const float gravity = 0.55f;
    const float jumpPower = -11.0f;
    const float moveSpeed = 5.5f;
    const float friction = 0.88f;

    bool grounded = true;
    bool previousJump = false;
    float ballAngle = 0.0f;

    float enemyX = 660.0f;
    float enemyY = 365.0f;
    const float enemyW = 65.0f;
    const float enemyH = 55.0f;
    float enemySpeed = 2.0f;
    int enemyDirection = 1;
    bool enemyAlive = true;

    const int boxX = 70;
    const int boxY = 345;
    const int boxW = 75;
    const int boxH = 75;

    const int sunX = 650;
    const int sunY = 100;
    const int sunRadius = 35;

    float sunRotation = 0.0f;
    const float sunRotationSpeed = 0.15f;

    bool star1 = true;
    bool star2 = true;

    const float star1X = 250;
    const float star1Y = 275;
    const float star2X = 530;
    const float star2Y = 335;
    const float starPickupRadius = 22.0f;

    vector<Rect> solids;

    solids.push_back({
        -200.0f, 420.0f,
        (float)WIN_WIDTH + 400.0f, 200.0f
    });

    solids.push_back({
        165.0f, 325.0f,
        380.0f - 165.0f,
        420.0f - 325.0f
    });

    solids.push_back({
        (float)boxX, (float)boxY,
        (float)boxW, (float)boxH
    });

    solids.push_back({
        510.0f, 405.0f,
        40.0f, 15.0f
    });

    solids.push_back({
        550.0f, 390.0f,
        40.0f, 30.0f
    });

    solids.push_back({
        590.0f, 375.0f,
        40.0f, 45.0f
    });

    bool running = true;

    while (running)
    {
        if (windowClosed) {
            running = false;
            break;
        }

        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                windowClosed = true;
                running = false;
                break;
            }
        }

        if (!running)
            break;

        const Uint8* keys =
            SDL_GetKeyboardState(NULL);

        if (keys[SDL_SCANCODE_ESCAPE] ||
            keys[SDL_SCANCODE_Q]) {
            running = false;
            break;
        }

        if (keys[SDL_SCANCODE_LEFT] ||
            keys[SDL_SCANCODE_A])
            vx = -moveSpeed;

        if (keys[SDL_SCANCODE_RIGHT] ||
            keys[SDL_SCANCODE_D])
            vx = moveSpeed;

        bool jumpPressed =
            keys[SDL_SCANCODE_UP] ||
            keys[SDL_SCANCODE_W] ||
            keys[SDL_SCANCODE_SPACE];

        if (jumpPressed &&
            !previousJump &&
            grounded) {
            vy = jumpPower;
            grounded = false;
        }

        previousJump = jumpPressed;

        vy += gravity;

        ballAngle += vx / (float)ballRadius;

        moveBallWithCollision(
            ballX, ballY,
            (float)ballRadius,
            vx, vy,
            grounded,
            solids
        );

        vx *= friction;

        if (fabs(vx) < 0.05f)
            vx = 0.0f;

        if (ballX < ballRadius) {
            ballX = ballRadius;
            vx = 0;
        }

        if (ballX > WIN_WIDTH - ballRadius) {
            ballX = WIN_WIDTH - ballRadius;
            vx = 0;
        }

        if (enemyAlive) {
            enemyX += enemySpeed * enemyDirection;

            if (enemyX > 710)
                enemyDirection = -1;

            if (enemyX < 650)
                enemyDirection = 1;
        }

        if (enemyAlive) {
            Rect enemyRect = {
                enemyX, enemyY,
                enemyW, enemyH
            };

            float nx, ny, pen;

            if (circleRectOverlap(
                    ballX, ballY,
                    (float)ballRadius,
                    enemyRect,
                    nx, ny, pen)) {

                enemyAlive = false;
                vy = jumpPower * 0.65f;

                if (ballX < enemyX + enemyW / 2.0f) {
                    ballX = enemyX - ballRadius - 2;
                    vx = -3.0f;
                } else {
                    ballX = enemyX + enemyW + ballRadius + 2;
                    vx = 3.0f;
                }
            }
        }

        if (star1 &&
            hypot(ballX - star1X, ballY - star1Y)
            < starPickupRadius)
            star1 = false;

        if (star2 &&
            hypot(ballX - star2X, ballY - star2Y)
            < starPickupRadius)
            star2 = false;

        cleardevice();

        sunRotation += sunRotationSpeed;

        if (sunRotation >= 360.0f)
            sunRotation -= 360.0f;

        drawSun(
            sunX, sunY,
            sunRadius,
            sunRotation
        );

        drawGround();
        drawPlatform();
        drawStairs();

        if (star1)
            drawStar(
                (int)star1X,
                (int)star1Y
            );

        if (star2)
            drawStar(
                (int)star2X,
                (int)star2Y
            );

        drawWoodenBox(boxX, boxY);

        if (enemyAlive)
            drawEnemy(
                (int)round(enemyX),
                (int)round(enemyY)
            );

        drawFlag(760, 350);

        drawRedBall(
            (int)round(ballX),
            (int)round(ballY),
            ballRadius,
            ballAngle
        );

        refresh();
        delay(16);
    }

    SDL_DelEventWatch(
        closeEventWatcher,
        NULL
    );

    closegraph();
    return 0;
}
