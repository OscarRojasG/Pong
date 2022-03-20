#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

struct {
    SDL_Rect *shape;
    double velocityX;
    double velocityY;
    double motionX;
    double motionY;
} typedef PingRect;

PingRect *createRect(int, int, int, int, double, double);
void movePaddle(PingRect *, double);
void moveBall(PingRect *, double);
bool touchingBorderX(PingRect *);
bool touchingBorderY(PingRect *);
bool checkColission(PingRect *, PingRect *);

int main(int argc, char** argv){
    if(SDL_Init(SDL_INIT_VIDEO) < 0) return 1;

    SDL_Window *window = SDL_CreateWindow(
        "Pong",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0
    );
    if(!window) return 1;

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer) return 1;

    PingRect *leftPaddle = createRect(50, (SCREEN_HEIGHT - 80) / 2, 15, 80, 0, 200);
    PingRect *rightPaddle = createRect(SCREEN_WIDTH - 65, (SCREEN_HEIGHT - 80) / 2, 15, 80, 0, -200);
    PingRect *ball = createRect((SCREEN_WIDTH - 15) / 2, (SCREEN_HEIGHT - 15) / 2, 15, 15, 400, 200);

    Uint32 lastUpdate = SDL_GetTicks();
    SDL_Event event;
    bool running = true;

    while(running) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        SDL_RenderFillRect(renderer, leftPaddle->shape);
        SDL_RenderFillRect(renderer, rightPaddle->shape);
        SDL_RenderFillRect(renderer, ball->shape);

        Uint32 current = SDL_GetTicks();
        double dT = (current - lastUpdate) / 1000.0f; 

        movePaddle(leftPaddle, dT);
        movePaddle(rightPaddle, dT);
        moveBall(ball, dT);
        checkColission(ball, leftPaddle);
        checkColission(ball, rightPaddle);

        lastUpdate = current;

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}

PingRect *createRect(int x, int y, int w, int h, double vx, double vy) {
    PingRect *rect = (PingRect *) malloc(sizeof(PingRect));
    rect->shape = (SDL_Rect *) malloc(sizeof(SDL_Rect)); 

    rect->shape->x = x;
    rect->shape->y = y;
    rect->shape->w = w;
    rect->shape->h = h;
    rect->velocityX = vx;
    rect->velocityY = vy;
    rect->motionX = 0;
    rect->motionY = 0;

    return rect;
}

void movePaddle(PingRect *rect, double time) {	
	rect->motionY += rect->velocityY * time;

    if(abs(rect->motionY) >= 1) {
        rect->shape->y += (long)rect->motionY;
        rect->motionY = rect->motionY - (long)rect->motionY;
    
        if(touchingBorderY(rect)) {
            rect->velocityY *= -1; 
            rect->motionY *= -1;
        }
    }
}

void moveBall(PingRect *rect, double time) {
    rect->motionX += rect->velocityX * time;
    rect->motionY += rect->velocityY * time;

    if(abs(rect->motionX) >= 1) {
        rect->shape->x += (long)rect->motionX;
        rect->motionX = rect->motionX - (long)rect->motionX;

        if(touchingBorderX(rect)) {
            rect->velocityX *= -1; 
            rect->motionX *= -1;
        }
    }
    if(abs(rect->motionY) >= 1) {
        rect->shape->y += (long)rect->motionY;
        rect->motionY = rect->motionY - (long)rect->motionY;
    
        if(touchingBorderY(rect)) {
            rect->velocityY *= -1; 
            rect->motionY *= -1;
        }
    }
}

bool touchingBorderX(PingRect *rect) {
    if(rect->shape->x == 0) return true;
    if(rect->shape->x + rect->shape->w == SCREEN_WIDTH) return true;
    return false;
}

bool touchingBorderY(PingRect *rect) {
    if(rect->shape->y == 0) return true;
    if(rect->shape->y + rect->shape->h == SCREEN_HEIGHT) return true;
    return false;
}

bool checkColission(PingRect *ball, PingRect *paddle) {
    SDL_Rect intersection;
    if(SDL_IntersectRect(ball->shape, paddle->shape, &intersection)) {
        if(ball->shape->y + ball->shape->h >= paddle->shape->y &&
        ball->shape->y <= paddle->shape->y + paddle->shape->h) {
            ball->velocityX *= -1;
            if(ball->motionX < 0) ball->motionX = intersection.w;
            else ball->motionX = intersection.w * -1;
            return true;
        }
        ball->velocityY *= -1;
        if(ball->motionY < 0) ball->motionY = intersection.h;
        else ball->motionY = intersection.h * -1;
        return true;
    }
    return false;
}