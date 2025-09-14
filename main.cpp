#include <cmath>
#include <iostream>
#include <windows.h>
#include <mmsystem.h>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// ===================================================================
//
//                      MASTER CONTROL & STATE
//
// ===================================================================

// Global state for scene management
// 1: Urban, 2: Padma, 3: Village
int currentScene = 1;

// Forward declarations for init, keyboard, and mouse functions
void init_urban();
void init_padma();
void init_village();

void keyboard_urban(unsigned char key, int x, int y);
void keyboard_padma(unsigned char key, int x, int y);
void keyboard_village(unsigned char key, int x, int y);

void mouse_urban(int button, int state, int x, int y);
void mouse_village(int button, int state, int x, int y);


// ===================================================================
//
//                      SCENE 1: URBAN VIEW
//
// ===================================================================

// --------------------- Urban Globals ---------------------
float carX = 0.0f;
float carV = 2.0f;

float trainX = 10.0f;
float trainV = 2.0f;

float sunX_urban = 420.0f;
float sunY_urban = 440.0f;

float carX2 = 0, carV2 = 1.5f;
float busX = 0, busV = 0.8f;

int trafficLightState = 0; // 0: Green, 1: Yellow, 2: Red
float manY = 0.0f;
float manV = 1.0f;
bool isManOnCrossing = false;

const float NORMAL_CAR_V = 2.0f;
const float NORMAL_CAR_V2 = 1.5f;
const float NORMAL_BUS_V = 0.8f;

int lastClickTime = 0;
const int DOUBLE_CLICK_INTERVAL = 300;


// --------------------- Urban Utils -----------------------
void drawCircle_urban(float cx, float cy, float r, int segments = 48) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; ++i) {
        float a = (2.0f * 3.1415926535f * i) / segments;
        glVertex2f(cx + r * std::cos(a), cy + r * std::sin(a));
    }
    glEnd();
}

void circle_urban(GLfloat rx, GLfloat ry, GLfloat cx, GLfloat cy) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 360; i++) {
        float angle = 3.1416f * i / 180;
        float x = rx * cosf(angle);
        float y = ry * sinf(angle);
        glVertex2f((x + cx), (y + cy));
    }
    glEnd();
}

void filledRect(float x1, float y1, float x2, float y2) {
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

void roundedRect(float x1, float y1, float x2, float y2, float r, int segments = 12) {
    float cx_tl = x1 + r; float cy_tl = y2 - r;
    float cx_tr = x2 - r; float cy_tr = y2 - r;
    float cx_bl = x1 + r; float cy_bl = y1 + r;
    float cx_br = x2 - r; float cy_br = y1 + r;

    filledRect(x1, y1 + r, x2, y2 - r);
    filledRect(x1 + r, y1, x2 - r, y2);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx_tl, cy_tl);
    for (int i = segments; i <= 2 * segments; ++i) {
        float a = (2.0f * 3.1415926535f * i) / (4 * segments);
        glVertex2f(cx_tl + r * std::cos(a), cy_tl + r * std::sin(a));
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx_tr, cy_tr);
    for (int i = 0; i <= segments; ++i) {
        float a = (2.0f * 3.1415926535f * i) / (4 * segments);
        glVertex2f(cx_tr + r * std::cos(a), cy_tr + r * std::sin(a));
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx_bl, cy_bl);
    for (int i = 2 * segments; i <= 3 * segments; ++i) {
        float a = (2.0f * 3.1415926535f * i) / (4 * segments);
        glVertex2f(cx_bl + r * std::cos(a), cy_bl + r * std::sin(a));
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx_br, cy_br);
    for (int i = 3 * segments; i <= 4 * segments; ++i) {
        float a = (2.0f * 3.1415926535f * i) / (4 * segments);
        glVertex2f(cx_br + r * std::cos(a), cy_br + r * std::sin(a));
    }
    glEnd();
}

// ---------------- Urban Scene pieces ---------------------
void Field1() {
    glColor3f(0.369f, 0.51f, 0.412f);
    glBegin(GL_QUADS);
    glVertex2f(0, 190);
    glVertex2f(500, 190);
    glVertex2f(500, 240);
    glVertex2f(0, 240);
    glEnd();
}

void Road1() {
    glColor3f(0.153f, 0.153f, 0.153f);
    glBegin(GL_QUADS);
    glVertex2f(0, 110);
    glVertex2f(500, 110);
    glVertex2f(500, 190);
    glVertex2f(0, 190);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    const int dashW = 40, dashH = 10, y1 = 150;
    int xs[] = {0, 60, 120, 250, 310, 370, 430};
    for (int x: xs) {
        glBegin(GL_QUADS);
        glVertex2f(x, y1);
        glVertex2f(x + dashW, y1);
        glVertex2f(x + dashW, y1 + dashH);
        glVertex2f(x, y1 + dashH);
        glEnd();
    }
}

void RoadSide1() {
    glColor3f(0.369f, 0.51f, 0.412f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(700, 0);
    glVertex2f(700, 110);
    glVertex2f(0, 110);
    glEnd();
}

void drawPassengerShade() {
    float sLeft = 300.0f;
    float sRight = 420.0f;
    float sBottom = 195.0f;
    float sTop = 260.0f;

    glColor3f(0.28f, 0.28f, 0.32f);
    roundedRect(sLeft, sTop - 6.0f, sRight, sTop + 6.0f, 6.0f);
    glColor3f(0.2f, 0.2f, 0.22f);
    filledRect(sLeft + 4.0f, sBottom, sLeft + 8.0f, sTop);
    filledRect(sRight - 8.0f, sBottom, sRight - 4.0f, sTop);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.86f, 0.95f, 1.0f, 0.28f);
    roundedRect(sLeft + 8.0f, sBottom, sRight - 8.0f, sTop - 4.0f, 4.0f);

    glColor3f(0.36f, 0.19f, 0.07f);
    filledRect(sLeft + 12.0f, sBottom + 12.0f, sRight - 12.0f, sBottom + 20.0f);
    glDisable(GL_BLEND);
}

void Crossing10() {
    glColor3f(0.9f, 0.9f, 0.9f);
    int startY = 110;
    int stripeHeight = 8;
    int numStripes = 7;
    float startX1 = 170.0f;
    float endX1 = 250.0f;
    float scaleStep = 0.04f;

    for (int i = 0; i < numStripes; ++i) {
        float factor = 1.0f - scaleStep * i;
        float centerX = (startX1 + endX1) / 2.0f;
        float halfWidth = (endX1 - startX1) / 2.0f * factor;
        float x1 = centerX - halfWidth;
        float x2 = centerX + halfWidth;
        int y = startY + i * (stripeHeight + 4);

        glBegin(GL_QUADS);
        glVertex2f(x1, y);
        glVertex2f(x2, y);
        glVertex2f(x2, y + stripeHeight);
        glVertex2f(x1, y + stripeHeight);
        glEnd();
    }
}

void trainline10() {
    glColor3f(0.25f, 0.25f, 0.25f);
    glBegin(GL_QUADS);
    glVertex2f(0, 230);
    glVertex2f(500, 230);
    glVertex2f(500, 235);
    glVertex2f(0, 235);
    glEnd();

    glColor3f(0.55f, 0.35f, 0.2f);
    for (int x = 0; x <= 500; x += 18) {
        glBegin(GL_QUADS);
        glVertex2f(x, 225);
        glVertex2f(x + 10, 225);
        glVertex2f(x + 10, 240);
        glVertex2f(x, 240);
        glEnd();
    }
}

void drawCar1() {
    glPushMatrix();
    glTranslatef(0.0f, 22.0f, 0.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(150, 150);
    glVertex2f(225, 150);
    glVertex2f(225, 165);
    glVertex2f(150, 165);
    glEnd();
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(150, 155);
    glVertex2f(155, 155);
    glVertex2f(155, 160);
    glVertex2f(150, 160);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(220, 155);
    glVertex2f(225, 155);
    glVertex2f(225, 160);
    glVertex2f(220, 160);
    glEnd();
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(155, 165);
    glVertex2f(175, 185);
    glVertex2f(205, 185);
    glVertex2f(220, 165);
    glEnd();
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(170, 170);
    glVertex2f(185, 170);
    glVertex2f(185, 180);
    glVertex2f(175, 180);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(190, 170);
    glVertex2f(210, 170);
    glVertex2f(200, 180);
    glVertex2f(190, 180);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle_urban(165.0f, 150.0f, 6.0f);
    drawCircle_urban(210.0f, 150.0f, 6.0f);
    glPopMatrix();
}

void drawCar3() {
    glPushMatrix();
    glTranslatef(0.0f, -22.0f, 0.0f);
    glColor3f(0.0f, 0.7f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(150, 150);
    glVertex2f(210, 150);
    glVertex2f(210, 175);
    glVertex2f(150, 175);
    glEnd();
    glColor3f(1.0f, 0.8f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(150, 160);
    glVertex2f(155, 160);
    glVertex2f(155, 165);
    glVertex2f(150, 165);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(205, 160);
    glVertex2f(210, 160);
    glVertex2f(210, 165);
    glVertex2f(205, 165);
    glEnd();
    glColor3f(0.0f, 0.5f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(160, 175);
    glVertex2f(175, 195);
    glVertex2f(195, 195);
    glVertex2f(205, 175);
    glEnd();
    glColor3f(0.8f, 0.9f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(165, 180);
    glVertex2f(180, 180);
    glVertex2f(180, 190);
    glVertex2f(170, 190);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(185, 180);
    glVertex2f(200, 180);
    glVertex2f(195, 190);
    glVertex2f(185, 190);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle_urban(165.0f, 150.0f, 7.0f);
    drawCircle_urban(195.0f, 150.0f, 7.0f);
    glPopMatrix();
}

void drawBus1() {
    int roadBase = 150;
    int busOffset = roadBase - 20;
    glPushMatrix();
    glColor3ub(255, 81, 76);
    glBegin(GL_QUADS);
    glVertex2f(90, busOffset + 98);
    glVertex2f(120, busOffset + 98);
    glVertex2f(120, busOffset + 100);
    glVertex2f(90, busOffset + 100);
    glEnd();
    glColor3ub(26, 26, 0);
    glBegin(GL_QUADS);
    glVertex2f(115, busOffset + 89);
    glVertex2f(120, busOffset + 89);
    glVertex2f(120, busOffset + 100);
    glVertex2f(115, busOffset + 100);
    glEnd();
    glColor3ub(255, 81, 76);
    glBegin(GL_QUADS);
    glVertex2f(10, busOffset + 80);
    glVertex2f(120, busOffset + 80);
    glVertex2f(120, busOffset + 105);
    glVertex2f(10, busOffset + 105);
    glEnd();
    glColor3ub(255, 81, 76);
    glBegin(GL_QUADS);
    glVertex2f(10, busOffset + 55);
    glVertex2f(122, busOffset + 55);
    glVertex2f(122, busOffset + 80);
    glVertex2f(10, busOffset + 80);
    glEnd();
    glColor3ub(0, 51, 0);
    glBegin(GL_QUADS);
    glVertex2f(11, busOffset + 81);
    glVertex2f(118, busOffset + 81);
    glVertex2f(118, busOffset + 102);
    glVertex2f(11, busOffset + 102);
    glEnd();
    glColor3ub(230, 255, 255);
    for (int i = 0; i < 4; i++) {
        int x = 12 + i * 25;
        glBegin(GL_QUADS);
        glVertex2f(x, busOffset + 85);
        glVertex2f(x + 18, busOffset + 85);
        glVertex2f(x + 18, busOffset + 100);
        glVertex2f(x, busOffset + 100);
        glEnd();
    }
    glColor3ub(255, 255, 204);
    drawCircle_urban(55, busOffset + 65, 8.0f);
    drawCircle_urban(65, busOffset + 75, 4.0f);
    drawCircle_urban(25, busOffset + 75, 6.0f);
    drawCircle_urban(45, busOffset + 65, 4.0f);
    drawCircle_urban(95, busOffset + 75, 4.0f);
    glColor3ub(0, 0, 0);
    drawCircle_urban(30, busOffset + 55, 10.0f);
    glColor3ub(255, 255, 255);
    drawCircle_urban(30, busOffset + 55, 6.0f);
    glColor3ub(0, 0, 0);
    drawCircle_urban(95, busOffset + 55, 10.0f);
    glColor3ub(255, 255, 255);
    drawCircle_urban(95, busOffset + 55, 6.0f);
    glPopMatrix();
}

void Train1() {
    glPushMatrix();
    glTranslatef(trainX, 170.0f, 1.0f);
    glScalef(0.7f, 0.7f, 1.0f);

    glPushMatrix();
    glTranslatef(350.0f, 100.0f, 0.0f);
    glScalef(0.9f, 0.9f, 1.0f);

    glColor3f(0.1f, 0.4f, 0.1f);
    glBegin(GL_POLYGON);
    glVertex2f(0, 0);
    glVertex2f(165, 0);
    glVertex2f(165, 20);
    glVertex2f(150, 50);
    glVertex2f(110, 50);
    glVertex2f(110, 70);
    glVertex2f(0, 70);
    glVertex2f(0, 50);
    glEnd();

    glColor3f(1.0f, 1.0f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(5, 55);
    glVertex2f(25, 55);
    glVertex2f(25, 65);
    glVertex2f(5, 65);
    glEnd();

    glColor3f(0.9f, 0.8f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(0, 20);
    glVertex2f(155, 20);
    glVertex2f(150, 30);
    glVertex2f(0, 30);
    glEnd();

    glColor3f(1.0f, 1.0f, 0.0f);
    drawCircle_urban(158, 12, 5, 20);

    glColor3f(0.1f, 0.1f, 0.1f);
    drawCircle_urban(30, 0, 12, 30);
    drawCircle_urban(60, 0, 12, 30);
    drawCircle_urban(90, 0, 12, 30);
    drawCircle_urban(120, 0, 12, 30);

    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(-15, 5);
    glVertex2f(0, 5);
    glVertex2f(0, 15);
    glVertex2f(-15, 15);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(155.0f, 100.0f, 0.0f);
    glColor3f(0.1f, 0.45f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(180, 0);
    glVertex2f(180, 60);
    glVertex2f(0, 60);
    glEnd();
    glColor3f(0.9f, 0.8f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(0, 15);
    glVertex2f(180, 15);
    glVertex2f(180, 45);
    glVertex2f(0, 45);
    glEnd();
    glColor3f(0.2f, 0.2f, 0.2f);
    for (int i = 0; i < 4; ++i) {
        glBegin(GL_QUADS);
        glVertex2f(20 + (i * 40), 25);
        glVertex2f(40 + (i * 40), 25);
        glVertex2f(40 + (i * 40), 35);
        glVertex2f(20 + (i * 40), 35);
        glEnd();
    }
    glColor3f(0.1f, 0.1f, 0.1f);
    drawCircle_urban(40, 0, 10, 30);
    drawCircle_urban(140, 0, 10, 30);
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(-15, 5);
    glVertex2f(0, 5);
    glVertex2f(0, 15);
    glVertex2f(-15, 15);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(180, 5);
    glVertex2f(195, 5);
    glVertex2f(195, 15);
    glVertex2f(180, 15);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-40.0f, 100.0f, 0.0f);
    glColor3f(0.1f, 0.45f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(180, 0);
    glVertex2f(180, 60);
    glVertex2f(0, 60);
    glEnd();
    glColor3f(0.9f, 0.8f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(0, 15);
    glVertex2f(180, 15);
    glVertex2f(180, 45);
    glVertex2f(0, 45);
    glEnd();
    glColor3f(0.2f, 0.2f, 0.2f);
    for (int i = 0; i < 4; ++i) {
        glBegin(GL_QUADS);
        glVertex2f(20 + (i * 40), 25);
        glVertex2f(40 + (i * 40), 25);
        glVertex2f(40 + (i * 40), 35);
        glVertex2f(20 + (i * 40), 35);
        glEnd();
    }
    glColor3f(0.1f, 0.1f, 0.1f);
    drawCircle_urban(40, 0, 10, 30);
    drawCircle_urban(140, 0, 10, 30);
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(-15, 5);
    glVertex2f(0, 5);
    glVertex2f(0, 15);
    glVertex2f(-15, 15);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(180, 5);
    glVertex2f(195, 5);
    glVertex2f(195, 15);
    glVertex2f(180, 15);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-235.0f, 100.0f, 0.0f);
    glColor3f(0.1f, 0.4f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(180, 0);
    glVertex2f(180, 60);
    glVertex2f(0, 60);
    glEnd();
    glColor3f(0.9f, 0.8f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(0, 15);
    glVertex2f(180, 15);
    glVertex2f(180, 45);
    glVertex2f(0, 45);
    glEnd();
    glColor3f(0.1f, 0.35f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(70, 10);
    glVertex2f(110, 10);
    glVertex2f(110, 50);
    glVertex2f(70, 50);
    glEnd();
    glColor3f(0.1f, 0.1f, 0.1f);
    drawCircle_urban(40, 0, 10, 30);
    drawCircle_urban(140, 0, 10, 30);
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(180, 5);
    glVertex2f(195, 5);
    glVertex2f(195, 15);
    glVertex2f(180, 15);
    glEnd();
    glPopMatrix();
    glPopMatrix();
}

void House11() {
    glColor3f(0.165f, 0.059f, 0.302f);
    glBegin(GL_QUADS);
    glVertex2f(0, 210);
    glVertex2f(70, 210);
    glVertex2f(70, 370);
    glVertex2f(0, 370);
    glEnd();
    glColor3f(1, 1, 0);
    float w = 15, h = 20, sx = 7, sy = 250, gapX = 20, gapY = 20;
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 2; ++c) {
            float x = sx + c * (w + gapX);
            float y = sy + r * (h + gapY);
            glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x + w, y);
            glVertex2f(x + w, y + h);
            glVertex2f(x, y + h);
            glEnd();
        }
}

void House22() {
    glColor3f(0.227f, 0.404f, 0.478f);
    glBegin(GL_QUADS);
    glVertex2f(90, 220);
    glVertex2f(170, 220);
    glVertex2f(170, 410);
    glVertex2f(90, 410);
    glEnd();
    glColor3f(1, 1, 0);
    float w = 20, h = 20, sx = 100, sy = 250, gapX = 22, gapY = 20;
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 2; ++c) {
            float x = sx + c * (w + gapX);
            float y = sy + r * (h + gapY);
            glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x + w, y);
            glVertex2f(x + w, y + h);
            glVertex2f(x, y + h);
            glEnd();
        }
    glColor3f(0.5f, 0.35f, 0.05f);
    glBegin(GL_QUADS);
    glVertex2f(125, 220);
    glVertex2f(140, 220);
    glVertex2f(140, 270);
    glVertex2f(125, 270);
    glEnd();
}

void House33() {
    glColor3f(0.031f, 0.212f, 0.388f);
    glBegin(GL_QUADS);
    glVertex2f(430, 210);
    glVertex2f(500, 210);
    glVertex2f(500, 380);
    glVertex2f(430, 380);
    glEnd();
    glColor3f(1, 1, 0);
    float w = 15, h = 20, sx = 437, sy = 260, gapX = 20, gapY = 20;
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 2; ++c) {
            float x = sx + c * (w + gapX), y = sy + r * (h + gapY);
            glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x + w, y);
            glVertex2f(x + w, y + h);
            glVertex2f(x, y + h);
            glEnd();
        }
}

void House44() {
    glColor3f(0.221f, 0.092f, 0.076f);
    glBegin(GL_QUADS);
    glVertex2f(330, 220);
    glVertex2f(400, 220);
    glVertex2f(400, 400);
    glVertex2f(330, 400);
    glEnd();
    glColor3f(1, 1, 0);
    float w = 15, h = 20, sx = 337, sy = 250, gapX = 20, gapY = 20;
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 2; ++c) {
            float x = sx + c * (w + gapX), y = sy + r * (h + gapY);
            glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x + w, y);
            glVertex2f(x + w, y + h);
            glVertex2f(x, y + h);
            glEnd();
        }
}

void Man11() {
    glPushMatrix();
    glTranslatef(185.0f, 80.0f, 0.0f);
    glScalef(0.6f, 0.6f, 1.0f);
    glTranslatef(-185.0f, -10.0f, 0.0f);
    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(170, 70);
    glVertex2f(200, 70);
    glVertex2f(200, 130);
    glVertex2f(170, 130);
    glEnd();
    glColor3f(0.949f, 0.941f, 0.369f);
    glBegin(GL_QUADS);
    glVertex2f(180, 130);
    glVertex2f(190, 130);
    glVertex2f(190, 140);
    glVertex2f(180, 140);
    glEnd();
    glColor3f(0, 0, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(170, 100);
    glVertex2f(160, 100);
    glVertex2f(170, 130);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(200, 100);
    glVertex2f(210, 100);
    glVertex2f(200, 130);
    glEnd();
    glColor3f(0.949f, 0.941f, 0.369f);
    glBegin(GL_QUADS);
    glVertex2f(160, 60);
    glVertex2f(170, 60);
    glVertex2f(170, 100);
    glVertex2f(160, 100);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(200, 60);
    glVertex2f(210, 60);
    glVertex2f(210, 100);
    glVertex2f(200, 100);
    glEnd();
    glColor3f(0, 0, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(170, 10);
    glVertex2f(180, 10);
    glVertex2f(185, 70);
    glVertex2f(170, 70);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(190, 10);
    glVertex2f(200, 10);
    glVertex2f(200, 70);
    glVertex2f(185, 70);
    glEnd();
    glColor3f(0, 0, 0);
    drawCircle_urban(185.0f, 153.0f, 15.0f);
    glPopMatrix();
}

void Man22() {
    glPushMatrix();
    glTranslatef(255.0f, 35.0f, 0.0f);
    glScalef(0.7f, 0.7f, 1.0f);
    glTranslatef(-255.0f, -10.0f, 0.0f);
    glColor3f(0.0f, 0.078f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(240, 70);
    glVertex2f(270, 70);
    glVertex2f(270, 130);
    glVertex2f(240, 130);
    glEnd();
    glColor3f(0.949f, 0.941f, 0.369f);
    glBegin(GL_QUADS);
    glVertex2f(250, 130);
    glVertex2f(260, 130);
    glVertex2f(260, 140);
    glVertex2f(250, 140);
    glEnd();
    glColor3f(0.0f, 0.078f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(240, 100);
    glVertex2f(230, 100);
    glVertex2f(240, 130);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(270, 100);
    glVertex2f(280, 100);
    glVertex2f(270, 130);
    glEnd();
    glColor3f(0.949f, 0.941f, 0.369f);
    glBegin(GL_QUADS);
    glVertex2f(230, 60);
    glVertex2f(240, 60);
    glVertex2f(240, 100);
    glVertex2f(230, 100);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(270, 60);
    glVertex2f(280, 60);
    glVertex2f(280, 100);
    glVertex2f(270, 100);
    glEnd();
    glColor3f(0, 0, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(240, 10);
    glVertex2f(250, 10);
    glVertex2f(255, 70);
    glVertex2f(240, 70);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(260, 10);
    glVertex2f(270, 10);
    glVertex2f(270, 70);
    glVertex2f(255, 70);
    glEnd();
    glColor3f(0, 0, 0);
    drawCircle_urban(255.0f, 153.0f, 15.0f);
    glPopMatrix();
}

void tree11() {
    glPushMatrix();
    glTranslatef(15.0f, 10.0f, 0.0f);
    glScalef(0.8f, 0.8f, 1.0f);

    glColor3ub(139, 146, 22);
    circle_urban(8, 22, 0, 150);
    glColor3ub(139, 146, 22);
    circle_urban(8, 22, 10, 170);
    glColor3ub(139, 146, 22);
    circle_urban(8, 22, 13, 140);
    glColor3ub(139, 146, 22);
    circle_urban(7, 25, 22, 150);
    glColor3ub(139, 146, 22);
    circle_urban(8, 22, 30, 150);
    glColor3ub(139, 146, 22);
    circle_urban(10, 40, 0, 250);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 30, 295);
    glColor3ub(139, 146, 22);
    circle_urban(8, 15, 30, 293);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 45, 285);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 45, 280);
    glColor3ub(139, 146, 22);
    circle_urban(8, 15, 45, 275);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 55, 235);
    glColor3ub(139, 146, 22);
    circle_urban(9, 32, 50, 255);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 59, 225);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 56, 255);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 63, 195);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 50, 180);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 58, 165);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 49, 150);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 38, 140);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 55, 190);
    glColor3ub(139, 146, 22);
    circle_urban(9.5, 19, 55, 190);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 55, 205);
    glColor3ub(139, 146, 22);
    circle_urban(9.5, 20, 55, 205);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 50, 218);
    glColor3ub(139, 146, 22);
    circle_urban(9.5, 20, 50, 218);
    glColor3ub(139, 146, 22);
    circle_urban(10, 20, 27, 130);
    glColor3ub(139, 146, 22);
    circle_urban(35, 70, 20, 200);
    glColor3ub(139, 146, 22);
    circle_urban(15, 30, 30, 255);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 42, 225);
    glColor3ub(139, 146, 22);
    circle_urban(9.5, 20, 42, 224);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 30, 225);
    glColor3ub(139, 146, 22);
    circle_urban(10, 20, 30, 224);
    glColor3ub(0, 77, 26);
    circle_urban(10, 18, 42, 263);
    glColor3ub(139, 146, 22);
    circle_urban(10, 18, 42, 262);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 30, 180);
    glColor3ub(139, 146, 22);
    circle_urban(10, 20, 30, 179);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 20, 180);
    glColor3ub(139, 146, 22);
    circle_urban(10, 20, 20, 179);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 40, 155);
    glColor3ub(139, 146, 22);
    circle_urban(10, 20, 40, 156);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 20, 280);
    glColor3ub(0, 77, 26);
    circle_urban(9, 21, 20, 270);
    glColor3ub(139, 146, 22);
    circle_urban(9, 21, 20, 269);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 10, 255);
    glColor3ub(0, 77, 26);
    circle_urban(9, 20, 10, 245);
    glColor3ub(139, 146, 22);
    circle_urban(9, 20, 10.5, 244);

    glBegin(GL_TRIANGLE_FAN);
    glColor3ub(75, 35, 5);
    glVertex2f(15, 90);
    glVertex2f(22, 90);
    glVertex2f(21, 100);
    glVertex2f(20, 110);
    glVertex2f(18, 120);
    glVertex2f(16, 130);
    glVertex2f(17, 140);
    glVertex2f(18, 145);
    glVertex2f(20, 150);
    glVertex2f(22, 150);
    glVertex2f(21, 147);
    glVertex2f(20, 145);
    glVertex2f(18, 140);
    glVertex2f(16, 130);
    glVertex2f(13, 120);
    glVertex2f(16, 130);
    glVertex2f(18, 140);
    glVertex2f(20, 145);
    glVertex2f(22, 150);
    glVertex2f(22, 160);
    glVertex2f(18, 150);
    glEnd();
    glPopMatrix();
}

void tree22() {
    glPushMatrix();
    glTranslatef(460.0f, 10.0f, 0.0f);
    glScalef(0.8f, 0.8f, 1.0f);

    glColor3ub(139, 146, 22);
    circle_urban(8, 22, 0, 150);
    glColor3ub(139, 146, 22);
    circle_urban(8, 22, 10, 170);
    glColor3ub(139, 146, 22);
    circle_urban(8, 22, 13, 140);
    glColor3ub(139, 146, 22);
    circle_urban(7, 25, 22, 150);
    glColor3ub(139, 146, 22);
    circle_urban(8, 22, 30, 150);
    glColor3ub(139, 146, 22);
    circle_urban(10, 40, 0, 250);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 30, 295);
    glColor3ub(139, 146, 22);
    circle_urban(8, 15, 30, 293);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 45, 285);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 45, 280);
    glColor3ub(139, 146, 22);
    circle_urban(8, 15, 45, 275);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 55, 235);
    glColor3ub(139, 146, 22);
    circle_urban(9, 32, 50, 255);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 59, 225);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 56, 255);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 63, 195);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 50, 180);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 58, 165);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 49, 150);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 38, 140);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 55, 190);
    glColor3ub(139, 146, 22);
    circle_urban(9.5, 19, 55, 190);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 55, 205);
    glColor3ub(139, 146, 22);
    circle_urban(9.5, 20, 55, 205);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 50, 218);
    glColor3ub(139, 146, 22);
    circle_urban(9.5, 20, 50, 218);
    glColor3ub(139, 146, 22);
    circle_urban(10, 20, 27, 130);
    glColor3ub(139, 146, 22);
    circle_urban(35, 70, 20, 200);
    glColor3ub(139, 146, 22);
    circle_urban(15, 30, 30, 255);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 42, 225);
    glColor3ub(139, 146, 22);
    circle_urban(9.5, 20, 42, 224);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 30, 225);
    glColor3ub(139, 146, 22);
    circle_urban(10, 20, 30, 224);
    glColor3ub(0, 77, 26);
    circle_urban(10, 18, 42, 263);
    glColor3ub(139, 146, 22);
    circle_urban(10, 18, 42, 262);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 30, 180);
    glColor3ub(139, 146, 22);
    circle_urban(10, 20, 30, 179);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 20, 180);
    glColor3ub(139, 146, 22);
    circle_urban(10, 20, 20, 179);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 40, 155);
    glColor3ub(139, 146, 22);
    circle_urban(10, 20, 40, 156);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 20, 280);
    glColor3ub(0, 77, 26);
    circle_urban(9, 21, 20, 270);
    glColor3ub(139, 146, 22);
    circle_urban(9, 21, 20, 269);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 10, 255);
    glColor3ub(0, 77, 26);
    circle_urban(9, 20, 10, 245);
    glColor3ub(139, 146, 22);
    circle_urban(9, 20, 10.5, 244);

    glBegin(GL_TRIANGLE_FAN);
    glColor3ub(75, 35, 5);
    glVertex2f(15, 90);
    glVertex2f(22, 90);
    glVertex2f(21, 100);
    glVertex2f(20, 110);
    glVertex2f(18, 120);
    glVertex2f(16, 130);
    glVertex2f(17, 140);
    glVertex2f(18, 145);
    glVertex2f(20, 150);
    glVertex2f(22, 150);
    glVertex2f(21, 147);
    glVertex2f(20, 145);
    glVertex2f(18, 140);
    glVertex2f(16, 130);
    glVertex2f(13, 120);
    glVertex2f(16, 130);
    glVertex2f(18, 140);
    glVertex2f(20, 145);
    glVertex2f(22, 150);
    glVertex2f(22, 160);
    glVertex2f(18, 150);
    glEnd();
    glPopMatrix();
}

void tree33() {
    glPushMatrix();
    glTranslatef(270.0f, 150.0f, 0.0f);
    glScalef(0.5f, 0.5f, 1.0f);

    glColor3ub(139, 146, 22);
    circle_urban(8, 22, 0, 150);
    glColor3ub(139, 146, 22);
    circle_urban(8, 22, 10, 170);
    glColor3ub(139, 146, 22);
    circle_urban(8, 22, 13, 140);
    glColor3ub(139, 146, 22);
    circle_urban(7, 25, 22, 150);
    glColor3ub(139, 146, 22);
    circle_urban(8, 22, 30, 150);
    glColor3ub(139, 146, 22);
    circle_urban(10, 40, 0, 250);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 30, 295);
    glColor3ub(139, 146, 22);
    circle_urban(8, 15, 30, 293);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 45, 285);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 45, 280);
    glColor3ub(139, 146, 22);
    circle_urban(8, 15, 45, 275);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 55, 235);
    glColor3ub(139, 146, 22);
    circle_urban(9, 32, 50, 255);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 59, 225);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 56, 255);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 63, 195);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 50, 180);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 58, 165);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 49, 150);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 38, 140);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 55, 190);
    glColor3ub(139, 146, 22);
    circle_urban(9.5, 19, 55, 190);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 55, 205);
    glColor3ub(139, 146, 22);
    circle_urban(9.5, 20, 55, 205);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 50, 218);
    glColor3ub(139, 146, 22);
    circle_urban(9.5, 20, 50, 218);
    glColor3ub(139, 146, 22);
    circle_urban(10, 20, 27, 130);
    glColor3ub(139, 146, 22);
    circle_urban(35, 70, 20, 200);
    glColor3ub(139, 146, 22);
    circle_urban(15, 30, 30, 255);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 42, 225);
    glColor3ub(139, 146, 22);
    circle_urban(9.5, 20, 42, 224);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 30, 225);
    glColor3ub(139, 146, 22);
    circle_urban(10, 20, 30, 224);
    glColor3ub(0, 77, 26);
    circle_urban(10, 18, 42, 263);
    glColor3ub(139, 146, 22);
    circle_urban(10, 18, 42, 262);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 30, 180);
    glColor3ub(139, 146, 22);
    circle_urban(10, 20, 30, 179);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 20, 180);
    glColor3ub(139, 146, 22);
    circle_urban(10, 20, 20, 179);
    glColor3ub(0, 77, 26);
    circle_urban(10, 20, 40, 155);
    glColor3ub(139, 146, 22);
    circle_urban(10, 20, 40, 156);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 20, 280);
    glColor3ub(0, 77, 26);
    circle_urban(9, 21, 20, 270);
    glColor3ub(139, 146, 22);
    circle_urban(9, 21, 20, 269);
    glColor3ub(139, 146, 22);
    circle_urban(9, 22, 10, 255);
    glColor3ub(0, 77, 26);
    circle_urban(9, 20, 10, 245);
    glColor3ub(139, 146, 22);
    circle_urban(9, 20, 10.5, 244);

    glBegin(GL_TRIANGLE_FAN);
    glColor3ub(75, 35, 5);
    glVertex2f(15, 90);
    glVertex2f(22, 90);
    glVertex2f(21, 100);
    glVertex2f(20, 110);
    glVertex2f(18, 120);
    glVertex2f(16, 130);
    glVertex2f(17, 140);
    glVertex2f(18, 145);
    glVertex2f(20, 150);
    glVertex2f(22, 150);
    glVertex2f(21, 147);
    glVertex2f(20, 145);
    glVertex2f(18, 140);
    glVertex2f(16, 130);
    glVertex2f(13, 120);
    glVertex2f(16, 130);
    glVertex2f(18, 140);
    glVertex2f(20, 145);
    glVertex2f(22, 150);
    glVertex2f(22, 160);
    glVertex2f(18, 150);
    glEnd();
    glPopMatrix();
}

void Lamppost(float x) {
    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(x, 110);
    glVertex2f(x + 5, 110);
    glVertex2f(x + 5, 200);
    glVertex2f(x, 200);
    glEnd();
    glColor3f(1.0f, 1.0f, 0.6f);
    drawCircle_urban(x + 2.5f, 210.0f, 8.0f);
}
void Lamppost110() { Lamppost(100); }
void Lamppost220() { Lamppost(440); }

void Signal() {
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(205, 190);
    glVertex2f(215, 190);
    glVertex2f(215, 260);
    glVertex2f(205, 260);
    glEnd();

    if (trafficLightState == 2) glColor3f(1.0f, 0.0f, 0.0f);
    else glColor3f(0.4f, 0.1f, 0.1f);
    drawCircle_urban(210, 250, 6);

    if (trafficLightState == 1) glColor3f(1.0f, 1.0f, 0.0f);
    else glColor3f(0.4f, 0.4f, 0.1f);
    drawCircle_urban(210, 235, 6);

    if (trafficLightState == 0) glColor3f(0.0f, 1.0f, 0.0f);
    else glColor3f(0.1f, 0.4f, 0.1f);
    drawCircle_urban(210, 220, 6);
}

void cloud_urban(float cx, float cy, float s = 1.0f) {
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle_urban(cx, cy, 12 * s);
    drawCircle_urban(cx + 12 * s, cy + 4 * s, 14 * s);
    drawCircle_urban(cx - 12 * s, cy + 4 * s, 14 * s);
    drawCircle_urban(cx + 24 * s, cy, 10 * s);
}
void cloud110() { cloud_urban(120, 420, 1.2f); }
void cloud220() { cloud_urban(340, 440, 1.0f); }

void suncircle1() {
    glColor3f(1.0f, 0.9f, 0.0f);
    drawCircle_urban(sunX_urban, sunY_urban, 15.0f, 60);
}

// ---------------- Urban Animation ------------------------
void update_carA(int) {
    if (currentScene != 1) { // Only run for Urban scene
        glutTimerFunc(16, update_carA, 0);
        return;
    }
    bool shouldStop = false;
    float carFrontX = carX + 225.0f;
    float stopLine = 165.0f;
    bool isApproachingStopLine = (carFrontX >= stopLine && carFrontX < stopLine + carV);

    if ((trafficLightState == 2 || isManOnCrossing) && isApproachingStopLine) {
        shouldStop = true;
    }

    if (!shouldStop) {
        carX += carV;
    }

    if (carX > 500.0f) carX = -250.0f;
    glutPostRedisplay();
    glutTimerFunc(16, update_carA, 0);
}

void update_train(int) {
    if (currentScene != 1) {
        glutTimerFunc(16, update_train, 0);
        return;
    }
    trainX += trainV;
    if (trainX > 520.0f) trainX = -520.0f;
    glutPostRedisplay();
    glutTimerFunc(16, update_train, 0);
}

void update_sun(int) {
    if (currentScene != 1) {
        glutTimerFunc(33, update_sun, 0);
        return;
    }
    sunY_urban += 0.15f;
    if (sunY_urban > 510.0f) sunY_urban = 330.0f;
    glutPostRedisplay();
    glutTimerFunc(33, update_sun, 0);
}

void update_carB(int) {
    if (currentScene != 1) {
        glutTimerFunc(16, update_carB, 0);
        return;
    }
    bool shouldStop = false;
    float carFrontX = carX2 + 150.0f;
    float stopLine = 255.0f;
    bool isApproachingStopLine = (carFrontX <= stopLine && carFrontX > stopLine - carV2);

    if ((trafficLightState == 2 || isManOnCrossing) && isApproachingStopLine) {
        shouldStop = true;
    }

    if (!shouldStop) {
        carX2 -= carV2;
    }

    if (carX2 < -250.0f) carX2 = 500.0f;
    glutPostRedisplay();
    glutTimerFunc(16, update_carB, 0);
}

void update_bus(int) {
    if (currentScene != 1) {
        glutTimerFunc(16, update_bus, 0);
        return;
    }
    bool shouldStop = false;
    float busFrontX = busX + 122.0f;
    float stopLine = 165.0f;
    bool isApproachingStopLine = (busFrontX >= stopLine && busFrontX < stopLine + busV);

    if ((trafficLightState == 2 || isManOnCrossing) && isApproachingStopLine) {
        shouldStop = true;
    }

    if (!shouldStop) {
        busX += busV;
    }
    if (busX > 500.0f) busX = -130.0f;
    glutPostRedisplay();
    glutTimerFunc(16, update_bus, 0);
}

void update_man(int) {
    if (currentScene != 1) {
        glutTimerFunc(16, update_man, 0);
        return;
    }
    float crossingEnd = 130.0f;
    float crossingStart = 0.0f;

    if (trafficLightState == 2) {
        if (manY < crossingEnd) {
            manY += manV;
        }
    } else if (trafficLightState == 0) {
        if (manY > crossingStart) {
            manY -= manV;
        }
    }
    isManOnCrossing = (manY > 0.0f);

    glutPostRedisplay();
    glutTimerFunc(16, update_man, 0);
}

// ---------------- Urban Input Callbacks ------------------
void keyboard_urban(unsigned char key, int x, int y) {
    switch (key) {
        case 'r': case 'R': trafficLightState = 2; break;
        case 'y': case 'Y': trafficLightState = 1; break;
        case 'g': case 'G': trafficLightState = 0; break;
    }
}

void mouse_urban(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int currentTime = glutGet(GLUT_ELAPSED_TIME);
        int timeSinceLastClick = currentTime - lastClickTime;

        if (timeSinceLastClick <= DOUBLE_CLICK_INTERVAL) {
            std::cout << "Double click: Urban speed reset to normal." << std::endl;
            carV = NORMAL_CAR_V;
            carV2 = NORMAL_CAR_V2;
            busV = NORMAL_BUS_V;
        } else {
            std::cout << "Single click: Urban speed increased." << std::endl;
            carV *= 1.5f;
            carV2 *= 1.5f;
            busV *= 1.5f;
        }
        lastClickTime = currentTime;
    }
}

// ---------------- Urban Display & Init ---------------------
void display_urban() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glBegin(GL_QUADS);
    glColor3f(0.529f, 0.808f, 0.922f);
    glVertex2f(0, 500);
    glVertex2f(500, 500);
    glColor3f(0.678f, 0.847f, 0.902f);
    glVertex2f(500, 220);
    glVertex2f(0, 220);
    glEnd();

    suncircle1();
    cloud110();
    cloud220();
    Field1();
    trainline10();
    Train1();
    House11();
    House22();
    House33();
    House44();
    tree33();
    Road1();
    Crossing10();
    Signal();
    RoadSide1();
    drawPassengerShade();

    glPushMatrix();
    glTranslatef(busX, 0.0f, 0.0f);
    drawBus1();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(carX, 0.0f, 0.0f);
    drawCar1();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(carX2, 0.0f, 0.0f);
    drawCar3();
    glPopMatrix();

    Lamppost110();
    Lamppost220();
    tree11();
    tree22();

    float crossing_progress = manY / 150.0f;
    if (crossing_progress < 0.0f) crossing_progress = 0.0f;
    if (crossing_progress > 1.0f) crossing_progress = 1.0f;
    float scale = 1.0f - 0.3f * crossing_progress;

    glPushMatrix();
    glTranslatef(0.0f, manY, 0.0f);
    glTranslatef(185.0f, 45.0f, 0.0f);
    glScalef(scale, scale, 1.0f);
    glTranslatef(-185.0f, -45.0f, 0.0f);
    Man11();
    glPopMatrix();

    Man22();

    glutSwapBuffers();
}

void init_urban() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 500, 0, 500);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.03f, 0.04f, 0.06f, 1.0f);
}


// ===================================================================
//
//                      SCENE 2: PADMA VIEW
//
// ===================================================================

// --------------------- Padma Globals ---------------------
float padma_moveX = -20.0f;
float padma_speed = 0.05f;
bool padma_isMoving = false;

// --------------------- Padma Utils -----------------------
void drawCircle_padma(float cx, float cy, float r, int segments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * 3.1416f * i / segments;
        glVertex2f(cx + r * cos(theta), cy + r * sin(theta));
    }
    glEnd();
}

// ---------------- Padma Scene pieces ---------------------
void drawWater() {
    for (int y = -20; y < 0; y++) {
        float factor = (float)(y + 20) / 20.0;
        float r = 0.0 + 0.2 * factor;
        float g = 0.1 + 0.2 * factor;
        float b = 0.3 + 0.4 * factor;

        glColor3f(r, g, b);

        glBegin(GL_QUADS);
        glVertex2f(-20, y);
        glVertex2f(20, y);
        glVertex2f(20, y + 1);
        glVertex2f(-20, y + 1);
        glEnd();
    }
}

void drawBoat(float offsetX, float offsetY, float scale) {
    glPushMatrix();
    glTranslatef(offsetX, offsetY, 0);
    glScalef(scale, scale, 1);

    glColor3f(0.1, 0.1, 0.1);
    glBegin(GL_POLYGON);
    glVertex2f(-8, -5);
    glVertex2f(-8, -3);
    glVertex2f(6, -3);
    glVertex2f(7, -4);
    glVertex2f(6.5, -5);
    glEnd();

    glColor3f(0.9, 0.6, 0.2);
    glBegin(GL_POLYGON);
    glVertex2f(-6, -3);
    glVertex2f(5.5, -3);
    glVertex2f(5.5, 2);
    glVertex2f(-6, 2);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(-2, 2);
    glVertex2f(3, 2);
    glVertex2f(3, 5);
    glVertex2f(-2, 5);
    glEnd();

    glColor3f(0.4, 0.8, 1.0);
    glBegin(GL_POLYGON);
    glVertex2f(-5, -1);
    glVertex2f(-2, -1);
    glVertex2f(-2, 1);
    glVertex2f(-5, 1);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(-1, -1);
    glVertex2f(2, -1);
    glVertex2f(2, 1);
    glVertex2f(-1, 1);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(-1, 3);
    glVertex2f(2, 3);
    glVertex2f(2, 4.5);
    glVertex2f(-1, 4.5);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(-1.8, 3);
    glVertex2f(-0.8, 3);
    glVertex2f(-0.8, 4);
    glVertex2f(-1.8, 4);
    glEnd();

    drawCircle_padma(4.5, 0, 0.6, 40);

    glColor3f(0.3, 0.6, 0.9);
    glBegin(GL_POLYGON);
    glVertex2f(2.5, -3);
    glVertex2f(4, -3);
    glVertex2f(4, 1);
    glVertex2f(2.5, 1);
    glEnd();

    glColor3f(0.9, 0.2, 0.2);
    glBegin(GL_POLYGON);
    glVertex2f(4.5, -3);
    glVertex2f(5.5, -3);
    glVertex2f(5.5, 1);
    glVertex2f(4.5, 1);
    glEnd();

    glPopMatrix();
}

// ---------------- Padma Display & Init ---------------------
void display_padma() {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_QUADS);
    glColor3f(0.9, 0.5, 0.2);
    glVertex2i(-20, 0);
    glVertex2i(20, 0);
    glColor3f(0.2, 0.4, 0.6);
    glVertex2i(20, 20);
    glVertex2i(-20, 20);
    glEnd();

    drawWater();

    glColor3f(1.0, 0.8, 0.0);
    drawCircle_padma(0, 6, 3, 100);

    glPushMatrix();
    glTranslatef(padma_moveX, 0.0f, 0.0f);

    glColor3f(0.1, 0.4, 0.8);
    glBegin(GL_QUADS);
    glVertex2f(5, 2.8);
    glVertex2f(11, 2.8);
    glVertex2f(11, 6);
    glVertex2f(5, 6);
    glEnd();

    glColor3f(0.8, 0.8, 0.2);
    glBegin(GL_QUADS);
    glVertex2f(11, 2.8);
    glVertex2f(13.2, 2.8);
    glVertex2f(12.7, 5.5);
    glVertex2f(11, 5.5);
    glEnd();

    glColor3f(0.6, 0.9, 1.0);
    glBegin(GL_POLYGON);
    glVertex2f(11.5, 3.6);
    glVertex2f(12.9, 3.6);
    glVertex2f(12.6, 5.0);
    glVertex2f(11.5, 5.0);
    glEnd();

    glColor3f(0.0, 0.0, 0.0);
    drawCircle_padma(6, 2.5, 0.6, 40);
    drawCircle_padma(10, 2.5, 0.6, 40);
    drawCircle_padma(12, 2.5, 0.6, 40);
    glPopMatrix();

    glColor3f(0.7, 0.7, 0.7);
    glBegin(GL_QUADS);
    glVertex2i(-20, 2);
    glVertex2i(20, 2);
    glVertex2i(20, 1);
    glVertex2i(-20, 1);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2i(-20, -1);
    glVertex2i(20, -1);
    glVertex2i(20, -2);
    glVertex2i(-20, -2);
    glEnd();

    glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_LINES);
    glVertex2i(-20, -1.3);
    glVertex2i(20, -1.3);
    glVertex2i(-20, -1.7);
    glVertex2i(20, -1.7);
    glEnd();

    for (int x = -20; x < 20; x += 2) {
        glBegin(GL_LINES);
        glVertex2f(x, -1);
        glVertex2f(x + 2, 1);
        glEnd();
        glBegin(GL_LINES);
        glVertex2f(x + 2, -1);
        glVertex2f(x, 1);
        glEnd();
    }

    for (int x = -15; x <= 15; x += 10) {
        glColor3f(0.7, 0.7, 0.7);
        glBegin(GL_QUADS);
        glVertex2i(x - 1, -2);
        glVertex2i(x + 1, -2);
        glVertex2i(x + 1, -10);
        glVertex2i(x - 1, -10);
        glEnd();
        glBegin(GL_QUADS);
        glVertex2i(x - 2, -10);
        glVertex2i(x + 2, -10);
        glVertex2i(x + 2, -11);
        glVertex2i(x - 2, -11);
        glEnd();
    }

    for (int x = -16; x <= 16; x += 8) {
        glColor3f(0.8, 0.8, 0.8);
        glBegin(GL_LINES);
        glVertex2i(x, 2);
        glVertex2i(x, 6);
        glEnd();
        glColor3f(1.0, 1.0, 0.7);
        drawCircle_padma(x, 6, 0.4, 20);
    }

    glPushMatrix();
    glTranslatef(padma_moveX, 0.0f, 0.0f);
    drawBoat(-2, -8, 0.6);
    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

void init_padma() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-20, 20, -20, 20);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ---------------- Padma Animation ------------------------
void update_padma(int value) {
    if (currentScene != 2) { // Only run for Padma scene
        glutTimerFunc(16, update_padma, 0);
        return;
    }

    if (padma_isMoving) {
        padma_moveX += padma_speed;
        if (padma_moveX > 25) padma_moveX = -25;
    }
    glutPostRedisplay();
    glutTimerFunc(16, update_padma, 0);
}

// ---------------- Padma Input Callbacks ------------------
void keyboard_padma(unsigned char key, int x, int y) {
    if (key == 's') padma_isMoving = true;
    if (key == 'p') padma_isMoving = false;
}


// ===================================================================
//
//                      SCENE 3: VILLAGE VIEW
//
// ===================================================================

// --------------------- Village Globals ---------------------
bool village_isNight = false;
float village_sunY = 480.0f;
float village_sunSpeed = 0.2f;
bool village_sunGoingDown = true;
float village_boat2_initial = 200.0f;
float village_rickshawPosition_night = 50.0f;
float village_rickshaw_speed = 0.5f; // start with a visible speed

// --------------------- Village Utils -----------------------
void drawCircle_village(float radius, float X, float Y) {
    glBegin(GL_POLYGON);
    float pi = 3.1416;
    for (int i = 0; i < 360; i++) {
        float A = (i * 2 * pi) / 180;
        float x = radius * cos(A) + X;
        float y = radius * sin(A) + Y;
        glVertex2f(x, y);
    }
    glEnd();
}

// ---------------- Village Scene pieces ---------------------
void hill_sky() {
    if (village_isNight) {
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(0.0f, 330.0f);
        glVertex2f(500.0f, 330.0f);
        glVertex2f(500.0f, 500.0f);
        glVertex2f(0.0f, 500.0f);
        glEnd();

        glPushMatrix();
        glTranslatef(420, 450, 0);
        glColor3f(1.0, 1.0, 0.9);
        drawCircle_village(20, 0, 0);
        glColor3f(0.0, 0.0, 0.0);
        drawCircle_village(15, 5, 0);
        glPopMatrix();

        glColor3f(1.0, 1.0, 1.0);
        drawCircle_village(2, 100, 480);
        drawCircle_village(2, 200, 460);
        drawCircle_village(2, 300, 490);
        drawCircle_village(2, 400, 470);
        drawCircle_village(2, 250, 450);
        drawCircle_village(2, 150, 440);
        drawCircle_village(2, 350, 460);

    } else {
        glColor3f(0.3f, 0.6f, 1.0f);
        glBegin(GL_QUADS);
        glVertex2f(0.0f, 330.0f);
        glVertex2f(500.0f, 330.0f);
        glVertex2f(500.0f, 500.0f);
        glVertex2f(0.0f, 500.0f);
        glEnd();

        glPushMatrix();
        glTranslatef(420, village_sunY, 0);
        glColor3f(1.0, 1.0, 0.4);
        drawCircle_village(20, 0, 0);
        glPopMatrix();
    }

    glBegin(GL_TRIANGLES);
    glColor3f(village_isNight ? 0.0f : 0.0f, village_isNight ? 0.2f : 0.3f, 0.0f);
    glVertex2f(0.0f, 330.0f);
    glVertex2f(40.0f, 390.0f);
    glVertex2f(40.0f, 330.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(village_isNight ? 0.1f : 0.2f, village_isNight ? 0.3f : 0.6f, village_isNight ? 0.1f : 0.2f);
    glVertex2f(40.0f, 390.0f);
    glVertex2f(80.0f, 330.0f);
    glVertex2f(40.0f, 330.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(village_isNight ? 0.0f : 0.0f, village_isNight ? 0.25f : 0.35f, 0.1f);
    glVertex2f(50.0f, 330.0f);
    glVertex2f(95.0f, 370.0f);
    glVertex2f(95.0f, 330.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(village_isNight ? 0.1f : 0.2f, village_isNight ? 0.4f : 0.55f, 0.2f);
    glVertex2f(95.0f, 370.0f);
    glVertex2f(140.0f, 330.0f);
    glVertex2f(95.0f, 330.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(village_isNight ? 0.0f : 0.0f, village_isNight ? 0.3f : 0.4f, 0.1f);
    glVertex2f(110.0f, 330.0f);
    glVertex2f(150.0f, 380.0f);
    glVertex2f(150.0f, 330.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(village_isNight ? 0.1f : 0.25f, village_isNight ? 0.45f : 0.65f, 0.25f);
    glVertex2f(150.0f, 380.0f);
    glVertex2f(190.0f, 330.0f);
    glVertex2f(150.0f, 330.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(village_isNight ? 0.0f : 0.0f, village_isNight ? 0.35f : 0.45f, 0.15f);
    glVertex2f(170.0f, 330.0f);
    glVertex2f(195.0f, 370.0f);
    glVertex2f(195.0f, 330.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(village_isNight ? 0.1f : 0.2f, village_isNight ? 0.5f : 0.7f, 0.2f);
    glVertex2f(195.0f, 370.0f);
    glVertex2f(230.0f, 330.0f);
    glVertex2f(195.0f, 330.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(village_isNight ? 0.0f : 0.05f, village_isNight ? 0.25f : 0.4f, 0.1f);
    glVertex2f(250.0f, 330.0f);
    glVertex2f(270.0f, 360.0f);
    glVertex2f(270.0f, 330.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(village_isNight ? 0.1f : 0.25f, village_isNight ? 0.4f : 0.7f, 0.25f);
    glVertex2f(270.0f, 360.0f);
    glVertex2f(320.0f, 330.0f);
    glVertex2f(270.0f, 330.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(village_isNight ? 0.0f : 0.0f, village_isNight ? 0.35f : 0.5f, 0.1f);
    glVertex2f(280.0f, 330.0f);
    glVertex2f(295.0f, 355.0f);
    glVertex2f(295.0f, 330.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(village_isNight ? 0.1f : 0.3f, village_isNight ? 0.5f : 0.8f, 0.3f);
    glVertex2f(295.0f, 355.0f);
    glVertex2f(350.0f, 330.0f);
    glVertex2f(295.0f, 330.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(village_isNight ? 0.0f : 0.05f, village_isNight ? 0.35f : 0.5f, 0.1f);
    glVertex2f(410.0f, 330.0f);
    glVertex2f(430.0f, 365.0f);
    glVertex2f(430.0f, 330.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(village_isNight ? 0.1f : 0.3f, village_isNight ? 0.55f : 0.9f, 0.3f);
    glVertex2f(430.0f, 365.0f);
    glVertex2f(490.0f, 330.0f);
    glVertex2f(430.0f, 330.0f);
    glEnd();

    glColor3f(0.3f, 0.1f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(370.0f, 330.0f);
    glVertex2f(373.0f, 330.0f);
    glVertex2f(373.0f, 360.0f);
    glVertex2f(370.0f, 360.0f);
    glEnd();
    glColor3f(0.3f, 0.3f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(355.0f, 360.0f);
    glVertex2f(385.0f, 360.0f);
    glVertex2f(370.0f, 375.0f);
    glEnd();
    glColor3f(0.3f, 0.4f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(357.0f, 365.0f);
    glVertex2f(383.0f, 365.0f);
    glVertex2f(370.0f, 380.0f);
    glEnd();

    glColor3f(0.3f, 0.1f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(390.0f, 330.0f);
    glVertex2f(393.0f, 330.0f);
    glVertex2f(393.0f, 380.0f);
    glVertex2f(390.0f, 380.0f);
    glEnd();
    glColor3f(0.3f, 0.3f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(375.0f, 380.0f);
    glVertex2f(410.0f, 380.0f);
    glVertex2f(392.0f, 400.0f);
    glEnd();
    glColor3f(0.3f, 0.4f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(377.0f, 390.0f);
    glVertex2f(407.0f, 390.0f);
    glVertex2f(392.0f, 410.0f);
    glEnd();

    int t = 130;
    glColor3f(0.3f, 0.1f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(370.0f - t, 330.0f);
    glVertex2f(373.0f - t, 330.0f);
    glVertex2f(373.0f - t, 360.0f);
    glVertex2f(370.0f - t, 360.0f);
    glEnd();
    glColor3f(0.3f, 0.3f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(355.0f - t, 360.0f);
    glVertex2f(385.0f - t, 360.0f);
    glVertex2f(370.0f - t, 375.0f);
    glEnd();
    glColor3f(0.3f, 0.4f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(357.0f - t, 365.0f);
    glVertex2f(383.0f - t, 365.0f);
    glVertex2f(370.0f - t, 380.0f);
    glEnd();
}

void field() {
    if (village_isNight) {
        glColor3f(0.3f, 0.5f, 0.0f);
    } else {
        glColor3f(0.7f, 0.9f, 0.0f);
    }
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 320.0f);
    glVertex2f(500.0f, 290.0f);
    glVertex2f(500.0f, 330.0f);
    glVertex2f(0.0f, 330.0f);
    glEnd();

    if (village_isNight) {
        glColor3f(0.25f, 0.4f, 0.0f);
    } else {
        glColor3f(0.6f, 0.8f, 0.0f);
    }
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 310.0f);
    glVertex2f(500.0f, 280.0f);
    glVertex2f(500.0f, 305.0f);
    glVertex2f(0.0f, 320.0f);
    glEnd();
}

void road() {
    if (village_isNight) {
        glColor3f(0.4f, 0.35f, 0.15f);
    } else {
        glColor3f(0.9f, 0.8f, 0.3f);
    }
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 310.0f);
    glVertex2f(0.0f, 280.0f);
    glVertex2f(500.0f, 266.0f);
    glVertex2f(500.0f, 280.0f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(250.0f, 280.0f);
    glVertex2f(450.0f, 280.0f);
    glVertex2f(350.0f, 260.0f);
    glVertex2f(500.0f, 210.0f);
    glVertex2f(500.0f, 190.0f);
    glVertex2f(460.0f, 180.0f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(500.0f, 230.0f);
    glVertex2f(500.0f, 190.0f);
    glVertex2f(0.0f, 100.0f);
    glVertex2f(0.0f, 130.0f);
    glEnd();
}

void drawCloud_village() {
    glColor3f(1.0, 1.0, 1.0);
    drawCircle_village(20, 100, 450);
    drawCircle_village(20, 70, 450);
    drawCircle_village(20, 130, 450);
    drawCircle_village(25, 100, 475);
}

void rickshaw() {
    glPushMatrix();
    glTranslatef(village_rickshawPosition_night, 0.0f, 0.0f);

    glColor3f(0.0, 0.0, 0.0);
    drawCircle_village(8, 250, 290);

    if (village_isNight) {
        glColor3f(0.4f, 0.35f, 0.15f);
    } else {
        glColor3f(0.9f, 0.8f, 0.3f);
    }
    drawCircle_village(7, 250, 290);

    glColor3f(0.0, 0.0, 0.0);
    drawCircle_village(8, 280, 290);

    if (village_isNight) {
        glColor3f(0.4f, 0.35f, 0.15f);
    } else {
        glColor3f(0.9f, 0.8f, 0.3f);
    }
    drawCircle_village(7, 280, 290);

    glColor3f(0.0, 0.0, 0.0);
    drawCircle_village(3, 265, 290);
    drawCircle_village(4, 270, 330);

    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(265.0f, 290.0f); glVertex2f(280.0f, 290.0f);
    glVertex2f(265.0f, 290.0f); glVertex2f(263.0f, 283.0f);
    glVertex2f(265.0f, 290.0f); glVertex2f(252.0f, 305.0f);
    glVertex2f(252.0f, 305.0f); glVertex2f(265.0f, 305.0f);
    glVertex2f(253.0f, 312.0f); glVertex2f(275.0f, 307.0f);
    glVertex2f(288.0f, 325.0f); glVertex2f(288.0f, 320.0f);
    glVertex2f(286.0f, 325.0f); glVertex2f(286.0f, 320.0f);
    glVertex2f(283.0f, 325.0f); glVertex2f(283.0f, 320.0f);
    glVertex2f(280.0f, 325.0f); glVertex2f(281.0f, 320.0f);
    glEnd();

    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(250.0f, 290.0f); glVertex2f(255.0f, 315.0f);
    glVertex2f(252.0f, 305.0f); glVertex2f(275.0f, 305.0f);
    glVertex2f(280.0f, 290.0f); glVertex2f(282.0f, 310.0f);
    glColor3f(0.9, 0.5, 0.1);
    glVertex2f(264.0f, 318.0f); glVertex2f(260.0f, 318.0f);
    glEnd();

    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex2f(252.0f, 315.0f); glVertex2f(260.0f, 315.0f);
    glVertex2f(260.0f, 320.0f); glVertex2f(252.0f, 317.0f);
    glVertex2f(273.0f, 305.0f); glVertex2f(288.0f, 305.0f);
    glVertex2f(288.0f, 315.0f); glVertex2f(273.0f, 312.0f);
    glVertex2f(288.0f, 315.0f); glVertex2f(288.0f, 320.0f);
    glVertex2f(280.0f, 320.0f); glVertex2f(277.0f, 310.0f);
    glEnd();

    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex2f(288.0f, 325.0f);
    glVertex2f(290.0f, 350.0f);
    glVertex2f(277.0f, 350.0f);
    glVertex2f(279.0f, 325.0f);
    glEnd();

    glColor3f(0.7, 0.0, 0.4);
    glBegin(GL_QUADS);
    glVertex2f(267.0f, 326.0f);
    glVertex2f(274.0f, 326.0f);
    glVertex2f(273.0f, 310.0f);
    glVertex2f(265.0f, 310.0f);
    glVertex2f(267.0f, 326.0f);
    glVertex2f(267.0f, 320.0f);
    glVertex2f(264.0f, 315.0f);
    glVertex2f(263.0f, 320.0f);
    glEnd();

    glColor3f(0.7, 0.3, 0.2);
    glBegin(GL_QUADS);
    glVertex2f(273.0f, 310.0f);
    glVertex2f(265.0f, 310.0f);
    glVertex2f(262.0f, 292.0f);
    glVertex2f(266.0f, 292.0f);
    glEnd();

    if (village_isNight) {
        float lampX = 250.0f;
        float lampY = 300.0f;

        glPushMatrix();
        glTranslatef(lampX, lampY, 0.0f);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 1.0f, 0.3f, 0.3f);

        glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(-160.0f, 40.0f);
        glVertex2f(-160.0f, -40.0f);
        glEnd();

        glDisable(GL_BLEND);
        glPopMatrix();

        glColor3f(1.0f, 1.0f, 0.0f);
        drawCircle_village(3, lampX, lampY);
    }
    glPopMatrix();
}

static void drawHouse(float x, float y, float scaleX, float scaleY) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scaleX, scaleY, 1.0f);

    glColor3f(0.88f, 0.52f, 0.40f);
    glBegin(GL_QUADS);
    glVertex2f(80.0f, 80.0f);
    glVertex2f(380.0f, 80.0f);
    glVertex2f(380.0f, 300.0f);
    glVertex2f(80.0f, 300.0f);
    glEnd();

    glColor3f(0.88f, 0.84f, 0.76f);
    glBegin(GL_QUADS);
    glVertex2f(380.0f, 80.0f);
    glVertex2f(560.0f, 80.0f);
    glVertex2f(560.0f, 380.0f);
    glVertex2f(380.0f, 380.0f);
    glEnd();

    glColor3f(0.88f, 0.52f, 0.40f);
    glBegin(GL_QUADS);
    glVertex2f(560.0f, 80.0f);
    glVertex2f(860.0f, 80.0f);
    glVertex2f(860.0f, 300.0f);
    glVertex2f(560.0f, 300.0f);
    glEnd();

    glColor3f(0.44f, 0.28f, 0.19f);
    glBegin(GL_QUADS);
    glVertex2f(440.0f, 90.0f);
    glVertex2f(500.0f, 90.0f);
    glVertex2f(500.0f, 240.0f);
    glVertex2f(440.0f, 240.0f);
    glEnd();

    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(470.0f, 90.0f);
    glVertex2f(470.0f, 240.0f);
    glEnd();

    glColor3f(0.8f, 0.8f, 0.81f);
    glBegin(GL_QUADS);
    glVertex2f(420.0f, 70.0f);
    glVertex2f(520.0f, 70.0f);
    glVertex2f(520.0f, 90.0f);
    glVertex2f(420.0f, 90.0f);
    glEnd();

    glColor3f(0.77f, 0.77f, 0.78f);
    glBegin(GL_QUADS);
    glVertex2f(430.0f, 55.0f);
    glVertex2f(510.0f, 55.0f);
    glVertex2f(510.0f, 70.0f);
    glVertex2f(430.0f, 70.0f);
    glEnd();

    for (int i = 0; i < 3; i++) {
        float y1 = 220.0f + i * 55.0f;
        float y2 = y1 + 40.0f;
        glColor3f(0.94f, 0.94f, 0.94f);
        glBegin(GL_QUADS);
        glVertex2f(420.0f, y1);
        glVertex2f(520.0f, y1);
        glVertex2f(520.0f, y2);
        glVertex2f(420.0f, y2);
        glEnd();
        if (village_isNight) glColor3f(1.0f, 1.0f, 0.3f);
        else glColor3f(0.6f, 0.85f, 1.0f);
        glBegin(GL_QUADS);
        glVertex2f(426.0f, y1 + 6.0f);
        glVertex2f(514.0f, y1 + 6.0f);
        glVertex2f(514.0f, y2 - 6.0f);
        glVertex2f(426.0f, y2 - 6.0f);
        glEnd();
        glColor3f(0.2f, 0.2f, 0.25f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(470.0f, y1 + 4.0f); glVertex2f(470.0f, y2 - 4.0f);
        glVertex2f(426.0f, (y1 + y2) / 2.0f); glVertex2f(514.0f, (y1 + y2) / 2.0f);
        glEnd();
    }
    glColor3f(0.94f, 0.94f, 0.94f);
    glBegin(GL_QUADS);
    glVertex2f(140.0f, 200.0f);
    glVertex2f(260.0f, 200.0f);
    glVertex2f(260.0f, 260.0f);
    glVertex2f(140.0f, 260.0f);
    glEnd();
    if (village_isNight) glColor3f(1.0f, 1.0f, 0.3f);
    else glColor3f(0.6f, 0.85f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(145.0f, 205.0f);
    glVertex2f(255.0f, 205.0f);
    glVertex2f(255.0f, 255.0f);
    glVertex2f(145.0f, 255.0f);
    glEnd();
    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(135.0f, 260.0f);
    glVertex2f(265.0f, 260.0f);
    glVertex2f(260.0f, 270.0f);
    glVertex2f(140.0f, 270.0f);
    glEnd();
    glColor3f(0.94f, 0.94f, 0.94f);
    glBegin(GL_QUADS);
    glVertex2f(140.0f, 120.0f);
    glVertex2f(260.0f, 120.0f);
    glVertex2f(260.0f, 180.0f);
    glVertex2f(140.0f, 180.0f);
    glEnd();
    if (village_isNight) glColor3f(1.0f, 1.0f, 0.3f);
    else glColor3f(0.6f, 0.85f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(145.0f, 125.0f);
    glVertex2f(255.0f, 125.0f);
    glVertex2f(255.0f, 175.0f);
    glVertex2f(145.0f, 175.0f);
    glEnd();
    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(135.0f, 180.0f);
    glVertex2f(265.0f, 180.0f);
    glVertex2f(260.0f, 190.0f);
    glVertex2f(140.0f, 190.0f);
    glEnd();
    glColor3f(0.94f, 0.94f, 0.94f);
    glBegin(GL_QUADS);
    glVertex2f(680.0f, 200.0f);
    glVertex2f(800.0f, 200.0f);
    glVertex2f(800.0f, 260.0f);
    glVertex2f(680.0f, 260.0f);
    glEnd();
    if (village_isNight) glColor3f(1.0f, 1.0f, 0.3f);
    else glColor3f(0.6f, 0.85f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(685.0f, 205.0f);
    glVertex2f(795.0f, 205.0f);
    glVertex2f(795.0f, 255.0f);
    glVertex2f(685.0f, 255.0f);
    glEnd();
    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(675.0f, 260.0f);
    glVertex2f(805.0f, 260.0f);
    glVertex2f(800.0f, 270.0f);
    glVertex2f(680.0f, 270.0f);
    glEnd();
    glColor3f(0.94f, 0.94f, 0.94f);
    glBegin(GL_QUADS);
    glVertex2f(680.0f, 120.0f);
    glVertex2f(800.0f, 120.0f);
    glVertex2f(800.0f, 180.0f);
    glVertex2f(680.0f, 180.0f);
    glEnd();
    if (village_isNight) glColor3f(1.0f, 1.0f, 0.3f);
    else glColor3f(0.6f, 0.85f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(685.0f, 125.0f);
    glVertex2f(795.0f, 125.0f);
    glVertex2f(795.0f, 175.0f);
    glVertex2f(685.0f, 175.0f);
    glEnd();
    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(675.0f, 180.0f);
    glVertex2f(805.0f, 180.0f);
    glVertex2f(800.0f, 190.0f);
    glVertex2f(680.0f, 190.0f);
    glEnd();
    glColor3f(0.38f, 0.26f, 0.17f);
    glBegin(GL_QUADS);
    glVertex2f(90.0f, 310.0f);
    glVertex2f(370.0f, 310.0f);
    glVertex2f(370.0f, 330.0f);
    glVertex2f(90.0f, 330.0f);
    glEnd();
    for (int i = 0; i < 9; i++) {
        float px = 110.0f + i * 28.0f;
        glBegin(GL_QUADS);
        glVertex2f(px, 300.0f);
        glVertex2f(px + 4.0f, 300.0f);
        glVertex2f(px + 4.0f, 330.0f);
        glVertex2f(px, 330.0f);
        glEnd();
    }
    glColor3f(0.38f, 0.26f, 0.17f);
    glBegin(GL_QUADS);
    glVertex2f(570.0f, 310.0f);
    glVertex2f(850.0f, 310.0f);
    glVertex2f(850.0f, 330.0f);
    glVertex2f(570.0f, 330.0f);
    glEnd();
    for (int i = 0; i < 9; i++) {
        float px = 590.0f + i * 28.0f;
        glBegin(GL_QUADS);
        glVertex2f(px, 300.0f);
        glVertex2f(px + 4.0f, 300.0f);
        glVertex2f(px + 4.0f, 330.0f);
        glVertex2f(px, 330.0f);
        glEnd();
    }
    glPopMatrix();
}

void object_house_tree() {
    glColor3f(0.8f, 0.0f, 0.3f);
    drawCircle_village(4, 284, 210);
    drawCircle_village(4, 276, 210);
    drawCircle_village(4, 280, 214);
    drawCircle_village(4, 310, 220);
    drawCircle_village(4, 300, 220);
    drawCircle_village(4, 305, 225);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle_village(2, 280, 210);
    drawCircle_village(2, 305, 220);
    drawCircle_village(2, 290, 210);
    int gh = 30;
    glColor3f(0.3f, 0.8f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(300.0f, 160.0f + gh);
    glVertex2f(310.0f, 160.0f + gh);
    glVertex2f(280.0f, 180.0f + gh);
    glVertex2f(300.0f, 160.0f + gh);
    glVertex2f(310.0f, 160.0f + gh);
    glVertex2f(290.0f, 180.0f + gh);
    glVertex2f(305.0f, 160.0f + gh);
    glVertex2f(315.0f, 160.0f + gh);
    glVertex2f(305.0f, 190.0f + gh);
    glVertex2f(305.0f, 160.0f + gh);
    glVertex2f(315.0f, 160.0f + gh);
    glVertex2f(325.0f, 190.0f + gh);
    glVertex2f(305.0f, 160.0f + gh);
    glVertex2f(315.0f, 160.0f + gh);
    glVertex2f(340.0f, 180.0f + gh);
    glEnd();
    glLineWidth(5.0f);
    int l = 10;
    for (int i = 0; i < 4; i++) {
        glBegin(GL_LINES);
        glColor3f(0.6f, 0.3f, 0.0f);
        glVertex2f(0.0f, 230.0f + l * i);
        glVertex2f(180.0f, 230.0f + l * i);
        glEnd();
    }
    int l1 = 10;
    for (int i = 0; i < 18; i++) {
        glBegin(GL_LINES);
        glColor3f(0.6f, 0.3f, 0.0f);
        glVertex2f(0.0f + l1 * i, 230.0f);
        glVertex2f(0.0f + l1 * i, 270.0f);
        glEnd();
    }
    glBegin(GL_QUADS);
    glColor3f(0.8f, 0.4f, 0.0f);
    glVertex2f(190.0f, 235.0f);
    glVertex2f(220.0f, 235.0f);
    glVertex2f(215.0f, 220.0f);
    glVertex2f(195.0f, 220.0f);
    glEnd();
    glColor3f(0.5f, 0.2f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(190.0f, 240.0f); glVertex2f(195.0f, 220.0f);
    glVertex2f(220.0f, 240.0f); glVertex2f(215.0f, 220.0f);
    glVertex2f(205.0f, 240.0f); glVertex2f(205.0f, 220.0f);
    glEnd();
    glColor3f(0.6f, 0.2f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(75.0f, 220.0f);
    glVertex2f(82.0f, 220.0f);
    glVertex2f(82.0f, 300.0f);
    glVertex2f(75.0f, 300.0f);
    glEnd();
    glColor3f(0.0f, 0.4f, 0.0f);
    drawCircle_village(15, 78, 310);
    drawCircle_village(15, 70, 310);
    drawCircle_village(15, 90, 330);
    drawCircle_village(15, 65, 330);
    drawCircle_village(15, 95, 315);
    drawCircle_village(15, 74, 350);
    drawCircle_village(15, 84, 350);
    drawCircle_village(13, 80, 365);
    int y = 60;
    glColor3f(0.6f, 0.2f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(75.0f - y, 220.0f);
    glVertex2f(82.0f - y, 220.0f);
    glVertex2f(82.0f - y, 300.0f);
    glVertex2f(75.0f - y, 300.0f);
    glEnd();
    glColor3f(0.0f, 0.5f, 0.0f);
    drawCircle_village(15, 78 - y, 310);
    drawCircle_village(15, 70 - y, 310);
    drawCircle_village(15, 90 - y, 330);
    drawCircle_village(15, 65 - y, 330);
    drawCircle_village(15, 95 - y, 315);
    drawCircle_village(15, 74 - y, 350);
    drawCircle_village(15, 84 - y, 350);
    drawCircle_village(13, 80 - y, 365);
    drawHouse(8.0f, 188.0f, 0.32f, 0.38f);
    glColor3f(0.7f, 0.4f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(450.0f, 260.0f);
    glVertex2f(460.0f, 260.0f);
    glVertex2f(460.0f, 360.0f);
    glVertex2f(450.0f, 360.0f);
    glEnd();
    glColor3f(0.2f, 0.6f, 0.3f);
    drawCircle_village(16, 460, 365);
    drawCircle_village(16, 477, 365);
    drawCircle_village(16, 440, 367);
    drawCircle_village(16, 452, 388);
    drawCircle_village(16, 466, 388);
    drawCircle_village(16, 459, 405);
    glColor3f(0.9f, 0.4f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(450.0f, 300.0f);
    glVertex2f(500.0f, 300.0f);
    glVertex2f(500.0f, 340.0f);
    glVertex2f(465.0f, 340.0f);
    glEnd();
    glColor3f(0.9f, 0.7f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(465.0f, 300.0f);
    glVertex2f(465.0f, 260.0f);
    glVertex2f(500.0f, 260.0f);
    glVertex2f(500.0f, 300.0f);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.3f, 0.0f, 0.0f);
    glVertex2f(485.0f, 260.0f);
    glVertex2f(495.0f, 260.0f);
    glVertex2f(495.0f, 280.0f);
    glVertex2f(485.0f, 280.0f);
    glEnd();
    glBegin(GL_QUADS);
    if (village_isNight) glColor3f(1.0f, 1.0f, 0.3f);
    else glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(470.0f, 285.0f);
    glVertex2f(470.0f, 275.0f);
    glVertex2f(480.0f, 275.0f);
    glVertex2f(480.0f, 285.0f);
    glEnd();
    glColor3f(0.6f, 0.2f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(465.0f, 260.0f);
    glVertex2f(460.0f, 250.0f);
    glVertex2f(500.0f, 250.0f);
    glVertex2f(500.0f, 260.0f);
    glEnd();
}

void river() {
    glColor3f(0.6f, 0.7f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 100.0f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(500.0f, 0.0f);
    glVertex2f(500.0f, 190.0f);
    glEnd();

    glColor3f(0.6f, 0.4f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 100.0f);
    glVertex2f(0.0f, 90.0f);
    glVertex2f(500.0f, 180.0f);
    glVertex2f(500.0f, 190.0f);
    glEnd();
}

void bambooPole(float x, float y) {
    glColor3f(0.75f, 0.55f, 0.25f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + 6, y);
    glVertex2f(x + 10, y + 120);
    glVertex2f(x + 4, y + 120);
    glEnd();
    glColor3f(0.5f, 0.3f, 0.1f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(x, y + 30); glVertex2f(x + 10, y + 30);
    glVertex2f(x, y + 60); glVertex2f(x + 10, y + 60);
    glVertex2f(x, y + 90); glVertex2f(x + 10, y + 90);
    glEnd();
}

void boat1() {
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(100.0f, 15.0f);
    glVertex2f(130.0f, 15.0f);
    glVertex2f(130.0f, 30.0f);
    glVertex2f(100.0f, 30.0f);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(100.0f, 15.0f);
    glVertex2f(100.0f, 30.0f);
    glVertex2f(80.0f, 40.0f);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(130.0f, 15.0f);
    glVertex2f(130.0f, 30.0f);
    glVertex2f(150.0f, 40.0f);
    glEnd();
    glColor3f(0.7f, 0.4f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(100.0f, 30.0f);
    glVertex2f(130.0f, 30.0f);
    glVertex2f(130.0f, 55.0f);
    glVertex2f(100.0f, 55.0f);
    glEnd();
    glBegin(GL_LINES);
    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(115.0f, 55.0f);
    glVertex2f(115.0f, 120.0f);
    glEnd();
    glColor3f(0.6f, 0.0f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(113.0f, 65.0f);
    glVertex2f(113.0f, 110.0f);
    glVertex2f(100.0f, 110.0f);
    glVertex2f(100.0f, 65.0f);
    glEnd();
    glColor3f(0.6f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(113.0f + 17.0f, 65.0f);
    glVertex2f(113.0f + 17.0f, 110.0f);
    glVertex2f(100.0f + 17.0f, 110.0f);
    glVertex2f(100.0f + 17.0f, 65.0f);
    glEnd();
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(130.0f, 110.0f);
    glVertex2f(150.0f, 40.0f);
    glVertex2f(113.0f, 110.0f);
    glVertex2f(150.0f, 40.0f);
    glEnd();
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(95.0f, 110.0f);
    glVertex2f(113.0f + 20.0f, 110.0f);
    glEnd();
}

void boat2() {
    glPushMatrix();
    glTranslatef(village_boat2_initial, 0.0f, 0.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(100.0f, 15.0f);
    glVertex2f(130.0f, 15.0f);
    glVertex2f(130.0f, 30.0f);
    glVertex2f(100.0f, 30.0f);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(100.0f, 15.0f);
    glVertex2f(100.0f, 30.0f);
    glVertex2f(80.0f, 40.0f);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(130.0f, 15.0f);
    glVertex2f(130.0f, 30.0f);
    glVertex2f(150.0f, 40.0f);
    glEnd();
    glColor3f(0.7f, 0.4f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(100.0f, 30.0f);
    glVertex2f(130.0f, 30.0f);
    glVertex2f(130.0f, 55.0f);
    glVertex2f(100.0f, 55.0f);
    glEnd();
    glBegin(GL_LINES);
    glLineWidth(3.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(115.0f, 55.0f);
    glVertex2f(115.0f, 120.0f);
    glEnd();
    glColor3f(0.9f, 0.0f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(113.0f, 65.0f);
    glVertex2f(113.0f, 110.0f);
    glVertex2f(100.0f, 110.0f);
    glVertex2f(100.0f, 65.0f);
    glEnd();
    glColor3f(0.9f, 0.1f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(113.0f + 17.0f, 65.0f);
    glVertex2f(113.0f + 17.0f, 110.0f);
    glVertex2f(100.0f + 17.0f, 110.0f);
    glVertex2f(100.0f + 17.0f, 65.0f);
    glEnd();
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(130.0f, 110.0f);
    glVertex2f(150.0f, 40.0f);
    glVertex2f(113.0f, 110.0f);
    glVertex2f(150.0f, 40.0f);
    glEnd();
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(95.0f, 110.0f);
    glVertex2f(113.0f + 20.0f, 110.0f);
    glEnd();
    glPopMatrix();
}

// ---------------- Village Display & Init ---------------------
void display_village() {
    glClear(GL_COLOR_BUFFER_BIT);

    hill_sky();
    drawCloud_village();
    field();
    road();

    rickshaw();
    object_house_tree();

    river();

    glPushMatrix();
    glTranslatef(300.0f, 100.0f, 0.0f);
    boat1();
    glPopMatrix();

    bambooPole(450, 150);

    boat2();

    glFlush();
    glutSwapBuffers();
}

void init_village() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 500.0, 0.0, 500.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.3f, 0.7f, 0.3f, 1.0f);
}

// ---------------- Village Animation ------------------------
void update_village(int value) {
    if (currentScene != 3) { // Only run for Village scene
        glutTimerFunc(16, update_village, 0);
        return;
    }
    village_rickshawPosition_night -= village_rickshaw_speed;
    if (village_rickshawPosition_night < -300.0f) {
        village_rickshawPosition_night = 600.0f;
    }

    if (village_sunGoingDown) {
        village_sunY -= village_sunSpeed;
        if (village_sunY <= 330.0f) {
            village_sunGoingDown = false;
        }
    } else {
        village_sunY += village_sunSpeed;
        if (village_sunY >= 480.0f) {
            village_sunGoingDown = true;
        }
    }

    village_boat2_initial -= 0.3f;
    if (village_boat2_initial < -250.0f) {
        village_boat2_initial = 600.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update_village, 0);
}

// ---------------- Village Input Callbacks ------------------
void mouse_village(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        village_isNight = !village_isNight;
    }
}

void keyboard_village(unsigned char key, int x, int y) {
    if (key == 'F' || key == 'f') {
        village_rickshaw_speed += 0.5f;
        if (village_rickshaw_speed > 5.0f) village_rickshaw_speed = 5.0f;
    } else if (key == 'S' || key == 's') {
        village_rickshaw_speed -= 0.5f;
        if (village_rickshaw_speed < 0.5f) village_rickshaw_speed = 0.5f;
    }
}


// ===================================================================
//
//                 MASTER DISPLAY, INPUT, AND MAIN
//
// ===================================================================

void master_display() {
    switch (currentScene) {
        case 1:
            display_urban();
            break;
        case 2:
            display_padma();
            break;
        case 3:
            display_village();
            break;
    }
}

void master_keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case '1':
            if (currentScene != 1) {
                currentScene = 1;
                init_urban(); // Set up projection and colors
                sndPlaySound("urban.wav", SND_ASYNC | SND_LOOP);
                std::cout << "Switched to Urban View (1)" << std::endl;
            }
            break;
        case '2':
            if (currentScene != 2) {
                currentScene = 2;
                init_padma(); // Set up projection and colors
                //sndPlaySound(NULL, 0, 0); // Stop any playing sound
                std::cout << "Switched to Padma View (2)" << std::endl;
            }
            break;
        case '3':
            if (currentScene != 3) {
                currentScene = 3;
                init_village(); // Set up projection and colors
                sndPlaySound("village.wav", SND_ASYNC | SND_LOOP);
                std::cout << "Switched to Village View (3)" << std::endl;
            }
            break;
        default:
            // Delegate other keys to the current scene's handler
            switch (currentScene) {
                case 1: keyboard_urban(key, x, y); break;
                case 2: keyboard_padma(key, x, y); break;
                case 3: keyboard_village(key, x, y); break;
            }
            break;
    }
    glutPostRedisplay();
}

void master_mouse(int button, int state, int x, int y) {
    switch (currentScene) {
        case 1: mouse_urban(button, state, x, y); break;
        case 2: /* Padma scene has no mouse function */ break;
        case 3: mouse_village(button, state, x, y); break;
    }
}


int main(int argc, char ** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Group Project: Multi-Scene View | Press 1, 2, 3 to switch");

    // Set initial scene
    currentScene = 1;
    init_urban();

    // Register master callbacks
    glutDisplayFunc(master_display);
    glutKeyboardFunc(master_keyboard);
    glutMouseFunc(master_mouse);

    // Register ALL animation timers from ALL scenes
    // Urban timers
    glutTimerFunc(16, update_carA, 0);
    glutTimerFunc(16, update_train, 0);
    glutTimerFunc(33, update_sun, 0);
    glutTimerFunc(16, update_carB, 0);
    glutTimerFunc(16, update_bus, 0);
    glutTimerFunc(16, update_man, 0);
    // Padma timer
    glutTimerFunc(16, update_padma, 0);
    // Village timer
    glutTimerFunc(16, update_village, 0);

    // Play sound for the initial scene
    sndPlaySound("urban.wav", SND_ASYNC | SND_LOOP);

    std::cout << "Project loaded. Press keys to interact with scenes." << std::endl;
    std::cout << "  1: Urban View" << std::endl;
    std::cout << "  2: Padma View" << std::endl;
    std::cout << "  3: Village View" << std::endl;


    glutMainLoop();
    return 0;
}
