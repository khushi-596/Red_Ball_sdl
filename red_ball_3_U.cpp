#include <graphics.h>
#include <SDL2/SDL.h>
#include <cmath>
#include <algorithm>
#include <vector>
using namespace std;

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define PI 3.14159265f

volatile bool windowClosed = false;

int SDLCALL closeEventWatcher(void* userdata, SDL_Event* event) {
    if (event->type == SDL_QUIT)
        windowClosed = true;

    return 0;
}

void safePutpixel(int x, int y, int color) {
    if (x >= 0 && x < WIN_WIDTH &&
        y >= 0 && y < WIN_HEIGHT) {
        putpixel(x, y, color);
    }
}

void DDA(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;

    int steps = max(abs(dx), abs(dy));

    if (steps == 0) {
        safePutpixel(x1, y1, getcolor());
        return;
    }

    float Xinc = (float)dx / steps;
    float Yinc = (float)dy / steps;

    float x = x1;
    float y = y1;

    for (int i = 0; i <= steps; i++) {
        safePutpixel((int)round(x), (int)round(y), getcolor());
        x += Xinc;
        y += Yinc;
    }
}

void Bresenham(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int err = dx - dy;

    while (true) {
        safePutpixel(x1, y1, getcolor());

        if (x1 == x2 && y1 == y2)
            break;

        int e2 = 2 * err;

        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx)  { err += dx; y1 += sy; }
    }
}

void circleBres(int xc, int yc, int r) {
    int x = 0;
    int y = r;
    int delta = 2 * (1 - r);
    int p;

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
            p = 2 * delta + 2 * y + 1;
            if (p <= 0) { x++; delta += 2 * x + 1; }
            else        { x++; y--; delta += 2 * x - 2 * y + 2; }
        }
        else {
            p = 2 * delta - 2 * x - 1;
            if (p <= 0) { x++; y--; delta += 2 * x - 2 * y + 2; }
            else        { y--; delta += 1 - 2 * y; }
        }
    }
}

void drawSun(int xc, int yc, int r, float rotationDeg) {
    setcolor(YELLOW);
    circleBres(xc, yc, r);

    for (int i = 0; i < 360; i += 45) {
        float rad = (i + rotationDeg) * PI / 180.0f;

        int x1 = xc + (int)((r + 8) * cos(rad));
        int y1 = yc + (int)((r + 8) * sin(rad));
        int x2 = xc + (int)((r + 25) * cos(rad));
        int y2 = yc + (int)((r + 25) * sin(rad));

        Bresenham(x1, y1, x2, y2);
    }
}

void drawGround() {
    setcolor(GREEN);
    DDA(20, 420, 780, 420);
}

void drawPlatform() {
    int x1 = 165, y1 = 325, x2 = 380;

    setcolor(GREEN);
    DDA(x1, y1, x2, y1);
    DDA(x1, y1, x1, 350);
    DDA(x1, 350, x2, 350);
    DDA(x2, y1, x2, 420);
}

void drawStairs() {
    setcolor(WHITE);

    DDA(470, 420, 510, 420);
    DDA(510, 420, 510, 405);

    DDA(510, 405, 550, 405);
    DDA(550, 405, 550, 390);

    DDA(550, 390, 590, 390);
    DDA(590, 390, 590, 375);

    DDA(590, 375, 630, 375);
    DDA(630, 375, 630, 420);
}

void drawWoodenBox(int x, int y) {
    int w = 75, h = 75;

    setcolor(BROWN);
    DDA(x, y, x + w, y);
    DDA(x + w, y, x + w, y + h);
    DDA(x + w, y + h, x, y + h);
    DDA(x, y + h, x, y);

    Bresenham(x + 8, y + 8, x + w - 8, y + h - 8);
    Bresenham(x + w - 8, y + 8, x + 8, y + h - 8);
}

void drawEnemy(int x, int y) {
    int w = 65, h = 55;

    setcolor(DARKGRAY);
    DDA(x, y, x + w, y);
    DDA(x + w, y, x + w, y + h);
    DDA(x + w, y + h, x, y + h);
    DDA(x, y + h, x, y);

    setcolor(WHITE);
    circleBres(x + 20, y + 22, 6);
    circleBres(x + 45, y + 22, 6);

    setcolor(RED);
    circleBres(x + 20, y + 22, 2);
    circleBres(x + 45, y + 22, 2);

    setcolor(WHITE);
    DDA(x + 20, y + 39, x + 45, y + 39);
    DDA(x + 20, y + 39, x + 24, y + 43);
    DDA(x + 45, y + 39, x + 41, y + 43);
}

void drawRedBall(int xc, int yc, int r, float angle) {
    setcolor(RED);
    circleBres(xc, yc, r);

    setcolor(WHITE);
    circleBres(xc - 9, yc - 5, 5);
    circleBres(xc + 9, yc - 5, 5);

    setcolor(BLACK);
    circleBres(xc - 9, yc - 4, 3);
    circleBres(xc + 9, yc - 4, 3);

    setcolor(WHITE);
    Bresenham(xc - 13, yc + 7, xc - 8, yc + 13);
    Bresenham(xc - 8, yc + 13, xc, yc + 16);
    Bresenham(xc, yc + 16, xc + 8, yc + 13);
    Bresenham(xc + 8, yc + 13, xc + 13, yc + 7);
}

void drawStar(int xc, int yc) {
    setcolor(YELLOW);

    int points[10][2];

    for (int i = 0; i < 10; i++) {
        float angle = -PI / 2 + i * PI / 5;
        int radius = (i % 2 == 0) ? 15 : 7;

        points[i][0] = xc + (int)(radius * cos(angle));
        points[i][1] = yc + (int)(radius * sin(angle));
    }

    for (int i = 0; i < 10; i++) {
        int next = (i + 1) % 10;
        Bresenham(points[i][0], points[i][1], points[next][0], points[next][1]);
    }
}

void drawFlag(int x, int y) {
    setcolor(WHITE);
    DDA(x, y, x, y + 70);
    DDA(x, y, x + 35, y + 12);
    DDA(x + 35, y + 12, x, y + 25);
    DDA(x, y + 25, x, y);
}

// All level solids are rects; the ball is a circle resolved against them.
struct Rect {
    float x, y, w, h;
};

static inline float clampf(float v, float lo, float hi) {
    return max(lo, min(v, hi));
}

// Detects circle-vs-rect overlap; fills push-out normal + penetration depth.
bool circleRectOverlap(float cx, float cy, float radius, const Rect& r,
                        float& outNx, float& outNy, float& outPen) {
    float closestX = clampf(cx, r.x, r.x + r.w);
    float closestY = clampf(cy, r.y, r.y + r.h);

    float dx = cx - closestX;
    float dy = cy - closestY;
    float distSq = dx * dx + dy * dy;

    if (distSq >= radius * radius)
        return false;

    float dist = sqrtf(distSq);

    if (dist > 0.0001f) {
        outNx = dx / dist;
        outNy = dy / dist;
        outPen = radius - dist;
    }
    else {
        // Center is inside the rect: push out via the nearest edge.
        float overlapLeft   = cx - r.x;
        float overlapRight  = (r.x + r.w) - cx;
        float overlapTop    = cy - r.y;
        float overlapBottom = (r.y + r.h) - cy;

        float m = min(min(overlapLeft, overlapRight), min(overlapTop, overlapBottom));

        if (m == overlapTop)         { outNx = 0; outNy = -1; outPen = radius + overlapTop; }
        else if (m == overlapBottom) { outNx = 0; outNy = 1;  outPen = radius + overlapBottom; }
        else if (m == overlapLeft)   { outNx = -1; outNy = 0; outPen = radius + overlapLeft; }
        else                         { outNx = 1;  outNy = 0; outPen = radius + overlapRight; }
    }

    return true;
}

// Pushes the ball out of the rect and clamps velocity along the hit axis.
bool resolveCircleRect(float& ballX, float& ballY, float radius,
                        float& vx, float& vy, bool& grounded, const Rect& r) {
    float nx, ny, pen;

    if (!circleRectOverlap(ballX, ballY, radius, r, nx, ny, pen))
        return false;

    ballX += nx * pen;
    ballY += ny * pen;

    if (fabs(ny) >= fabs(nx)) {
        if (ny < 0) {
            if (vy > 0) vy = 0;
            grounded = true;
        }
        else {
            if (vy < 0) vy = 0;
        }
    }
    else {
        vx = 0;
    }

    return true;
}

// Sub-steps movement so fast motion can't tunnel through thin solids.
void moveBallWithCollision(float& ballX, float& ballY, float radius,
                            float& vx, float& vy, bool& grounded,
                            const vector<Rect>& solids) {
    grounded = false;

    float remainingX = vx;
    float remainingY = vy;

    float maxStep = radius * 0.5f;
    float dist = max(fabs(remainingX), fabs(remainingY));
    int steps = max(1, (int)ceil(dist / maxStep));

    float stepX = remainingX / steps;
    float stepY = remainingY / steps;

    for (int s = 0; s < steps; s++) {
        ballX += stepX;
        ballY += stepY;

        for (const Rect& r : solids)
            resolveCircleRect(ballX, ballY, radius, vx, vy, grounded, r);
    }
}

int main() {
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
    float ballAngle = 0.0f;

    float enemyX = 660.0f;
    float enemyY = 365.0f;
    const float enemyW = 65.0f;
    const float enemyH = 55.0f;
    float enemySpeed = 2.0f;
    int enemyDirection = 1;
    bool enemyAlive = true;

    const int boxX = 70, boxY = 345, boxW = 75, boxH = 75;

    const int sunX = 650, sunY = 100, sunRadius = 35;
    float sunRotation = 0.0f;
    const float sunRotationSpeed = 0.15f; // degrees per frame — slow spin

    bool star1 = true;
    bool star2 = true;
    const float star1X = 250, star1Y = 275;
    const float star2X = 530, star2Y = 335;
    const float starPickupRadius = 22.0f;

    // Static level solids, built once.
    vector<Rect> solids;

    // Ground slab, extended past the window edges.
    solids.push_back({ -200.0f, 420.0f, (float)WIN_WIDTH + 400.0f, 200.0f });

    // Platform block, top down to the ground.
    solids.push_back({ 165.0f, 325.0f, 380.0f - 165.0f, 420.0f - 325.0f });

    // Wooden box block.
    solids.push_back({ (float)boxX, (float)boxY, (float)boxW, (float)boxH });

    // Stair treads (step 1 is flush with the ground, no collider needed).
    solids.push_back({ 510.0f, 405.0f, 550.0f - 510.0f, 420.0f - 405.0f });
    solids.push_back({ 550.0f, 390.0f, 590.0f - 550.0f, 420.0f - 390.0f });
    solids.push_back({ 590.0f, 375.0f, 630.0f - 590.0f, 420.0f - 375.0f });

    bool running = true;
    bool previousJump = false;

    while (running) {
        if (windowClosed) { running = false; break; }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                windowClosed = true;
                running = false;
                break;
            }
        }

        if (!running) break;

        const Uint8* keys = SDL_GetKeyboardState(NULL);

        if (keys[SDL_SCANCODE_ESCAPE] || keys[SDL_SCANCODE_Q]) {
            running = false;
            break;
        }

        if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
            vx = -moveSpeed;

        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
            vx = moveSpeed;

        bool jumpPressed = keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_SPACE];

        if (jumpPressed && !previousJump && grounded) {
            vy = jumpPower;
            grounded = false;
        }

        previousJump = jumpPressed;

        vy += gravity;

        ballAngle += vx / (float)ballRadius;

        moveBallWithCollision(ballX, ballY, (float)ballRadius, vx, vy, grounded, solids);

        vx *= friction;
        if (fabs(vx) < 0.05f) vx = 0.0f;

        if (ballX < ballRadius) { ballX = ballRadius; vx = 0; }
        if (ballX > WIN_WIDTH - ballRadius) { ballX = WIN_WIDTH - ballRadius; vx = 0; }

        if (enemyAlive) {
            enemyX += enemySpeed * enemyDirection;
            if (enemyX > 710) enemyDirection = -1;
            if (enemyX < 650) enemyDirection = 1;
        }

        // Enemy check is detection-only: it dies and bounces the ball.
        if (enemyAlive) {
            Rect enemyRect{ enemyX, enemyY, enemyW, enemyH };
            float nx, ny, pen;

            if (circleRectOverlap(ballX, ballY, (float)ballRadius, enemyRect, nx, ny, pen)) {
                enemyAlive = false;

                vy = jumpPower * 0.65f;

                if (ballX < enemyX + enemyW / 2.0f) {
                    ballX = enemyX - ballRadius - 2;
                    vx = -3.0f;
                }
                else {
                    ballX = enemyX + enemyW + ballRadius + 2;
                    vx = 3.0f;
                }
            }
        }

        if (star1 && hypot(ballX - star1X, ballY - star1Y) < starPickupRadius)
            star1 = false;

        if (star2 && hypot(ballX - star2X, ballY - star2Y) < starPickupRadius)
            star2 = false;

        setbkcolor(BLACK);
        cleardevice();

        sunRotation += sunRotationSpeed;
        if (sunRotation >= 360.0f) sunRotation -= 360.0f;

        drawSun(sunX, sunY, sunRadius, sunRotation);
        drawGround();
        drawPlatform();
        drawStairs();

        if (star1) drawStar((int)star1X, (int)star1Y);
        if (star2) drawStar((int)star2X, (int)star2Y);

        drawWoodenBox(boxX, boxY);

        if (enemyAlive)
            drawEnemy((int)round(enemyX), (int)round(enemyY));

        drawFlag(760, 350);

        drawRedBall((int)round(ballX), (int)round(ballY), ballRadius, ballAngle);

        refresh();
        delay(16);
    }

    SDL_DelEventWatch(closeEventWatcher, NULL);
    closegraph();

    return 0;
}
