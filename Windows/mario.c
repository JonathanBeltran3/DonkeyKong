#include "includes/mario.h"

Mario createMario(float x, float y, int health, float speedX, float speedY, float acceleration, float width, float height){
    Mario mario;
    mario.x = x;
    mario.y = y;
    mario.health = health;
    mario.speedX = speedX;
    mario.speedY = speedY;
    mario.acceleration = acceleration;
    mario.width = width;
    mario.height = height;
    return mario;
}
