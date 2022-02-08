#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#ifdef main
#undef main
#endif
//Libraries###############################################################################################################################################
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
//colors:
#define white 0xffffffff
#define red_norm 0x660000ff
#define red_bold 0x900000ff
#define blue_norm 0xbbffd480
#define blue_bold 0xffffaa00
#define yellow_norm 0xcc80ffff
#define yellow_bold 0xff00ffff
#define green_norm 0xdd80ffaa
#define green_bold 0xff00e64c
#define not_available 0xbb4d4d4d

//Structs & Unions########################################################################################################################################
typedef struct{
    Sint16 x;
    Sint16 y;
    Sint16 r;
    Uint32 color;
    bool is_available;
}CIRCLE;

//Function Prototypes#####################################################################################################################################
void define_circle(int *no_circle, CIRCLE circle[], int *no_is_available);
void draw_circle(SDL_Renderer *renderer, CIRCLE circle[], Uint32 color, Uint32 color2, int i); //i: number of the circle
void init_color(SDL_Renderer *renderer, int no_circle, int no_is_available, CIRCLE circle[]);

//The main CODE###########################################################################################################################################
int main(){
    int no_circle;
    CIRCLE circle[MAX_CIRCLE];
    int no_is_available;
    define_circle(&no_circle, circle, &no_is_available);
    
    //SDL initialization & creating window and renderer
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER);
    SDL_Window *window = SDL_CreateWindow("State.io", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH,SCREEN_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_MOUSE_FOCUS);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    //Loading background
    SDL_Surface *surface = SDL_LoadBMP("..\\Resources\\Background.bmp");
    SDL_Texture *back = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_free(surface);
    surface = SDL_LoadBMP("..\\Resources\\Title.bmp");
    SDL_Texture *title = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_free(surface);
    surface = SDL_LoadBMP("..\\Resources\\Map1.bmp");
    SDL_Texture *map1 = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_free(surface);
    surface = SDL_LoadBMP("..\\Resources\\Map2.bmp");
    SDL_Texture *map2 = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_free(surface);
    surface = SDL_LoadBMP("..\\Resources\\Map Test.bmp");
    SDL_Texture *map_test = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_free(surface);
    surface = SDL_LoadBMP("..\\Resources\\Map Random.bmp");
    SDL_Texture *map_random = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_free(surface);
    
    SDL_bool shall_exit = SDL_FALSE;
    while (shall_exit == SDL_FALSE){
        SDL_RenderClear(renderer);
        SDL_Rect dest_title;
        SDL_QueryTexture(title, NULL, NULL, &dest_title.w, &dest_title.h);
        dest_title.x = (SCREEN_WIDTH - dest_title.w)/2;
        dest_title.y = 35;
        
        SDL_Rect dest_map1;
        SDL_QueryTexture(map1, NULL, NULL, &dest_map1.w, &dest_map1.h);
        dest_map1.x = -170;
        dest_map1.x = (SCREEN_WIDTH - dest_map1.w)/2 - 180;
        dest_map1.y = dest_title.y -20;
        
        SDL_Rect dest_map2;
        SDL_QueryTexture(map2, NULL, NULL, &dest_map2.w, &dest_map2.h);
        dest_map2.x = (SCREEN_WIDTH - dest_map2.w)/2 + 180;
        dest_map2.y = dest_title.y -20;
    
        SDL_Rect dest_map_test;
        SDL_QueryTexture(map_test, NULL, NULL, &dest_map_test.w, &dest_map_test.h);
        dest_map_test.x = (SCREEN_WIDTH - dest_map_test.w)/2;
        dest_map_test.y = dest_title.y/2 + dest_map1.h/4 -20;
    
        SDL_Rect dest_map_random;
        SDL_QueryTexture(map_random, NULL, NULL, &dest_map_random.w, &dest_map_random.h);
        dest_map_random.x = (SCREEN_WIDTH - dest_map_random.w)/2;
        dest_map_random.y = dest_title.y + dest_map2.h/3 + 35;
        
        //generating the map
        SDL_RenderCopy(renderer, back, NULL, NULL);
        SDL_RenderCopy(renderer, title, NULL, &dest_title);
        SDL_RenderCopy(renderer, map1, NULL, &dest_map1);
        SDL_RenderCopy(renderer, map2, NULL, &dest_map2);
        SDL_RenderCopy(renderer, map_test, NULL, &dest_map_test);
        SDL_RenderCopy(renderer, map_random, NULL, &dest_map_random);
//        init_color(renderer, no_circle, no_is_available, circle);
        SDL_RenderPresent(renderer);
        
        //checking if the exit button is pushed
        SDL_Event sdlEvent;
        while (SDL_PollEvent(&sdlEvent)) {
            switch (sdlEvent.type) {
                case SDL_QUIT:
                    shall_exit = SDL_TRUE;
                    break;
            }
        }
    }
    SDL_DestroyTexture(back);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void define_circle(int *no_circle, CIRCLE circle[], int *no_is_available){
    srand(time(0));
    *no_circle = rand()%4 + 9;
    int no_is_unavailable=0;
    for(int i=0; i<*no_circle; i++){
        bool conflict = false;
        srand(100000*(i+1)*time(0));
        circle[i].x=rand()%760+100;
        circle[i].y=rand()%440+100;
        long long multip=9;
        while(conflict == false){
            for(int j=0; j<i; j++){
                if((circle[i].x - circle[j].x)*(circle[i].x - circle[j].x)+(circle[i].y - circle[j].y)*(circle[i].y - circle[j].y) <= 4*radius*radius+1600){
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
        circle[i].is_available = true;
    }
    srand(800000000*time(0));
    no_is_unavailable = rand()%4 +1;
    *no_is_available = *no_circle - no_is_unavailable;
    for(int i=0; i<no_is_unavailable; i++)
        circle[i].is_available = false;
}

void draw_circle(SDL_Renderer *renderer, CIRCLE circle[], Uint32 color, Uint32 color2, int i){
    filledCircleColor(renderer, circle[i].x, circle[i].y, radius, white);
    filledCircleColor(renderer, circle[i].x, circle[i].y, radius, color);
    boxColor(renderer, circle[i].x-15, circle[i].y-15,circle[i].x+15, circle[i].y+15, color2);
}

void init_color(SDL_Renderer *renderer, int no_circle, int no_is_available, CIRCLE circle[]){
    int no_is_unavailable = no_circle - no_is_available;
    //not available circles
    for(int i=0; i<no_is_unavailable; i++){
        filledCircleColor(renderer, circle[i].x, circle[i].y, radius, white);
        filledCircleColor(renderer, circle[i].x, circle[i].y, radius, not_available);
        circle[i].color = not_available;
    }
    //own circle
    draw_circle(renderer, circle, red_norm, red_bold, no_is_unavailable);
    circle[no_is_unavailable].color = red_norm;
    //enemy circle(s)
    if(no_is_available == 5){
        no_is_unavailable++;
        draw_circle(renderer, circle, blue_norm, blue_bold, no_is_unavailable);
        circle[no_is_unavailable].color = blue_norm;
    }
    else if(no_is_available==6 || no_is_available==7 || no_is_available==8){
        no_is_unavailable++;
        draw_circle(renderer, circle, blue_norm, blue_bold, no_is_unavailable);
        circle[no_is_unavailable].color = blue_norm;
        no_is_unavailable++;
        draw_circle(renderer, circle, yellow_norm, yellow_bold, no_is_unavailable);
        circle[no_is_unavailable].color = yellow_norm;
    }
    else{
        no_is_unavailable++;
        draw_circle(renderer, circle, blue_norm,blue_bold, no_is_unavailable);
        circle[no_is_unavailable].color = blue_norm;
        no_is_unavailable++;
        draw_circle(renderer, circle, yellow_norm, yellow_bold, no_is_unavailable);
        circle[no_is_unavailable].color = yellow_norm;
        no_is_unavailable++;
        draw_circle(renderer, circle, green_norm, green_bold, no_is_unavailable);
        circle[no_is_unavailable].color = green_norm;
    }
    //circles which are not conquered yet
    for(int i=no_is_unavailable+1; i<no_circle; i++) {
        draw_circle(renderer, circle, white, 0xcc262626, i);
        circle[i].color = white;
    }
}