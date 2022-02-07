#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#ifdef main
#undef main
#endif
//########################################################################################################################################################
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

//Macros
#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 640
#define FPS 60
#define radius 100

typedef struct{
    Sint16 x;
    Sint16 y;
    Sint16 r;
    Uint32 color;
}CIRCLE;

int main(){
    
    srand(time(0));
    int no_circle = rand()%3 + 5;
    printf("%d\n\n", no_circle);
    
    CIRCLE circle[15];
    for(int i=0; i<no_circle; i++){
        bool conflict = false;
        srand(100000*(i+1)*time(0));
        circle[i].x=rand()%760+100;
        circle[i].y=rand()%440+100;
        long long multip=9;
        while(conflict == false){
            for(int j=0; j<i; j++){
                if((circle[i].x-circle[j].x)*(circle[i].x-circle[j].x) + (circle[i].y-circle[j].y)*(circle[i].y-circle[j].y) > 4*radius*radius);
                else{
                    conflict = true;
                    break;
                }
            }
            if(conflict == true){
                printf("%d useless ----> x:%d, y:%d, r:100\n", i, circle[i].x, circle[i].y);
                srand(100000000*(i+8)*time(0));
                circle[i].x = abs(circle[i].x * circle[i].x * rand() * multip *multip+ rand()) %760 + 100;
                circle[i].y = abs(circle[i].y * circle[i].y * rand() * multip *multip+ rand()) %440 + 100;
                conflict = false;
                multip *= 9;
            }
            else
                conflict = true;
        }
        printf("%d ok ----> x:%d, y:%d, r:75\n", i, circle[i].x, circle[i].y);
    }
    
    
    
    SDL_Window *sdlWindow = SDL_CreateWindow("State.io", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                             SCREEN_WIDTH,SCREEN_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_MOUSE_FOCUS);
    
    SDL_Renderer *sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    
    SDL_bool shall_exit = SDL_FALSE;
    while (shall_exit == SDL_FALSE) {
        SDL_SetRenderDrawColor(sdlRenderer, 0xbd, 0xf5, 0xf5, 0xff);
        SDL_RenderClear(sdlRenderer);
    

        
        //draw circle using gfx_primitives
        Uint32 color2 = 0xff0000ff;
//        Sint16 x = 30, y = 75, r = 75;
//        filledCircleColor(sdlRenderer, x, y, r, color2);
        for(int i=0; i<no_circle; i++)
            filledCircleColor(sdlRenderer, circle[i].x, circle[i].y, 100, color2);


        
        SDL_RenderPresent(sdlRenderer);
        
        SDL_Event sdlEvent;
        while (SDL_PollEvent(&sdlEvent)) {
            switch (sdlEvent.type) {
                case SDL_QUIT:
                    shall_exit = SDL_TRUE;
                    break;
            }
        }
    }
    SDL_DestroyWindow(sdlWindow);
    SDL_QUIT;
    return 0;
}

//void DrawCircle(SDL_Renderer * renderer, int32_t centreX, int32_t centreY, int32_t radius)
//{
//    const int32_t diameter = (radius * 2);
//
//    int32_t x = (radius - 1);
//    int32_t y = 0;
//    int32_t tx = 1;
//    int32_t ty = 1;
//    int32_t error = (tx - diameter);
//
//    while (x >= y)
//    {
//        //  Each of the following renders an octant of the circle
//        SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
//        SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
//        SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
//        SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
//        SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
//        SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
//        SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
//        SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);
//
//        if (error <= 0)
//        {
//            ++y;
//            error += ty;
//            ty += 2;
//        }
//
//        if (error > 0)
//        {
//            --x;
//            tx += 2;
//            error += (tx - diameter);
//        }
//    }
//}