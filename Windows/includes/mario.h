#ifndef MARIO_H_INCLUDED
#define MARIO_H_INCLUDED

typedef struct Mario{
    float x;
    float y;
    int health;
    float speedX;
    float speedY;
    float acceleration;
    float width;
    float height;
} Mario;

Mario createMario(float x, float y, int health, float speedX, float speedY, float acceleration, float width, float height);

#endif // MARIO_H_INCLUDED
