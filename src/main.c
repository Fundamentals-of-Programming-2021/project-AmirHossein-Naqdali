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

//Macros##################################################################################################################################################
#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 640
#define FPS 60
#define radius 75
#define MAX_CIRCLE 15
typedef struct{
    Sint16 x;
    Sint16 y;
    Sint16 r;
    bool is_available;
}CIRCLE;

//Function Prototypes#####################################################################################################################################
void define_circle(int *no_circle, CIRCLE circle[]);

//########################################################################################################################################################
int main(){
    int no_circle;
    CIRCLE circle[MAX_CIRCLE];
    define_circle(&no_circle, circle);
    
    SDL_Window *sdlWindow = SDL_CreateWindow("State.io", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                             SCREEN_WIDTH,SCREEN_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_MOUSE_FOCUS);
    
    SDL_Renderer *sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    
    SDL_bool shall_exit = SDL_FALSE;
    while (shall_exit == SDL_FALSE) {
        SDL_SetRenderDrawColor(sdlRenderer, 0xa6, 0xa6, 0xa6, 0x55);
        SDL_RenderClear(sdlRenderer);
        
        //draw circle using gfx_primitives
        Uint32 color2 = 0xff0000ff;
        Uint32 color = 0xbb4d4d4d;
        for(int i=0; i<no_circle; i++)
            if(circle[i].is_available == true)
                filledCircleColor(sdlRenderer, circle[i].x, circle[i].y, radius, color2);
            else{
                filledCircleColor(sdlRenderer, circle[i].x, circle[i].y, radius, 0xffffffff);
                filledCircleColor(sdlRenderer, circle[i].x, circle[i].y, radius, color);
            }
            


        
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

void define_circle(int *no_circle, CIRCLE circle[]){
    srand(time(0));
    *no_circle = rand()%4 + 9;
    for(int i=0; i<*no_circle; i++){
        bool conflict = false;
        srand(100000*(i+1)*time(0));
        circle[i].x=rand()%760+100;
        circle[i].y=rand()%440+100;
        long long multip=9;
        while(conflict == false){
            for(int j=0; j<i; j++){
                if((circle[i].x - circle[j].x)*(circle[i].x - circle[j].x)+(circle[i].y - circle[j].y)*(circle[i].y - circle[j].y) <= 4*radius*radius+900){
                    conflict = true;
                    break;
                }
            }
            if(conflict == true){
                srand(100000000*(i+8)*time(0));
                circle[i].x = abs(circle[i].x * circle[i].x * rand() * multip * multip+ rand()) %760 + 100;
                circle[i].y = abs(circle[i].y * circle[i].y * rand() * multip * multip+ rand()) %440 + 100;
                conflict = false;
                multip *= 9;
            }
            else
                conflict = true;
        }
        circle[i].is_available = rand()%2;
    }
}