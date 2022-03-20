#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define MOVE_UP 1
#define MOVE_DOWN 0
#define MOVE_STATIC -1

struct {
    SDL_Rect *shape;
    double velocityY;
    double motionY;
} typedef PingPaddle;

struct {
    SDL_Rect *shape;
    double velocity;
    double velocityX;
    double velocityY;
    double motionX;
    double motionY;
} typedef PingBall;

PingPaddle *createPaddle(int, int, int, int, double);
PingBall *createBall(int, int, int, int, double, double, double);
void moveLeftPaddle(PingPaddle *, PingBall *);
void moveRightPaddle(PingPaddle *, int, double);
void moveBall(PingBall *, double);
bool touchingBorderX(SDL_Rect *);
bool touchingBorderY(SDL_Rect *);
bool checkColission(PingPaddle *, PingBall *);

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

    PingPaddle *leftPaddle = createPaddle(50, (SCREEN_HEIGHT - 80) / 2, 15, 80, 200);
    PingPaddle *rightPaddle = createPaddle(SCREEN_WIDTH - 65, (SCREEN_HEIGHT - 80) / 2, 15, 80, 300);
    PingBall *ball = createBall((SCREEN_WIDTH - 15) / 2, (SCREEN_HEIGHT - 15) / 2, 15, 15, 400, 282, 282);

    Uint32 lastUpdate = SDL_GetTicks();
    SDL_Event event;
    bool running = true;
    int direction = MOVE_STATIC;

    while(running) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_UP:
                            direction = MOVE_UP;
                            break;
                        case SDLK_DOWN:
                            direction = MOVE_DOWN;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch(event.key.keysym.sym) {
                        case SDLK_UP:
                        case SDLK_DOWN:
                            direction = MOVE_STATIC;
                            break;
                    }
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

        moveLeftPaddle(leftPaddle, ball);
        moveRightPaddle(rightPaddle, direction, dT);
        moveBall(ball, dT);
        checkColission(leftPaddle, ball);
        checkColission(rightPaddle, ball);

        lastUpdate = current;

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}

PingPaddle *createPaddle(int x, int y, int w, int h, double vy) {
    PingPaddle *paddle = (PingPaddle *) malloc(sizeof(PingPaddle));
    paddle->shape = (SDL_Rect *) malloc(sizeof(SDL_Rect)); 

    paddle->shape->x = x;
    paddle->shape->y = y;
    paddle->shape->w = w;
    paddle->shape->h = h;
    paddle->velocityY = vy;
    paddle->motionY = 0;

    return paddle;
}

PingBall *createBall(int x, int y, int w, int h, double v, double vx, double vy) {
    PingBall *ball = (PingBall *) malloc(sizeof(PingBall));
    ball->shape = (SDL_Rect *) malloc(sizeof(SDL_Rect));

    ball->shape->x = x;
    ball->shape->y = y;
    ball->shape->w = w;
    ball->shape->h = h;
    ball->velocity = v;
    ball->velocityX = vx;
    ball->velocityY = vy;
    ball->motionX = 0;
    ball->motionY = 0;

    return ball;
}

void moveLeftPaddle(PingPaddle *paddle, PingBall *ball) {	
	int centerY = ball->shape->y + (ball->shape->h / 2);
    int posY = centerY - (paddle->shape->h / 2);

    if(touchingBorderY(paddle->shape)) {
        if(posY < 0) posY = 0; 
        if(posY + paddle->shape->h > SCREEN_HEIGHT) posY = SCREEN_HEIGHT - paddle->shape->h;
    }
    paddle->shape->y = posY;
}

void moveRightPaddle(PingPaddle *paddle, int direction, double time) {	
    if(direction == MOVE_STATIC) {
        paddle->motionY = 0;
        return;
    }

	if(direction == MOVE_UP) paddle->motionY += paddle->velocityY * time * -1;
    if(direction == MOVE_DOWN) paddle->motionY += paddle->velocityY * time;

    if(abs(paddle->motionY) >= 1) {
        paddle->shape->y += (long)paddle->motionY;
        paddle->motionY = paddle->motionY - (long)paddle->motionY;

        if(touchingBorderY(paddle->shape)) {
            if(paddle->shape->y < 0) paddle->shape->y = 0; 
            if(paddle->shape->y + paddle->shape->h > SCREEN_HEIGHT) paddle->shape->y = SCREEN_HEIGHT - paddle->shape->h;
        }
    }
}

void moveBall(PingBall *ball, double time) {
    ball->motionX += ball->velocityX * time;
    ball->motionY += ball->velocityY * time;

    if(abs(ball->motionX) >= 1) {
        ball->shape->x += (long)ball->motionX;
        ball->motionX = ball->motionX - (long)ball->motionX;

        if(touchingBorderX(ball->shape)) {
            ball->velocityX *= -1; 
            ball->motionX *= -1;
        }
    }

    if(abs(ball->motionY) >= 1) {
        ball->shape->y += (long)ball->motionY;
        ball->motionY = ball->motionY - (long)ball->motionY;

        if(touchingBorderY(ball->shape)) {
            ball->velocityY *= -1; 
            ball->motionY *= -1;
        }
    }
}

bool touchingBorderX(SDL_Rect *rect) {
    if(rect->x == 0) return true;
    if(rect->x + rect->w == SCREEN_WIDTH) return true;
    return false;
}

bool touchingBorderY(SDL_Rect *rect) {
    if(rect->y <= 0) return true;
    if(rect->y + rect->h >= SCREEN_HEIGHT) return true;
    return false;
}

bool checkColission(PingPaddle *paddle, PingBall *ball) {
    SDL_Rect intersection;
    if(SDL_IntersectRect(ball->shape, paddle->shape, &intersection)) {
        if(intersection.h >= ball->shape->h / 2) {
            double x = intersection.y + (intersection.h / 2) - paddle->shape->y;
            // double m = M_PI / paddle->shape->h; 0° - 90°
            double m = (5 * M_PI) / (6 * paddle->shape->h); // 0 - 75°
            double a = abs(x * m - (5 * M_PI / 12));
            
            if(ball->velocityX > 0) ball->velocityX = ball->velocity * cos(a) * -1;
            else ball->velocityX = ball->velocity * cos(a);
            if(x < paddle->shape->h / 2) ball->velocityY = ball->velocity * sin(a) * -1;
            else ball->velocityY = ball->velocity * sin(a);

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