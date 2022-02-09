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
//circle colors:
#define white 0xffffffff
#define not_conquered 0xcc262626
#define red_norm 0x660000ff
#define red_bold 0x900000ff
#define blue_norm 0xbbffd480
#define blue_bold 0xffffaa00
#define yellow_norm 0xcc80ffff
#define yellow_bold 0xff00ffff
#define green_norm 0xdd80ffaa
#define green_bold 0xff00e64c
#define not_available 0xbb4d4d4d
//menu:
#define button_radius 20
#define button_color 0xffff3333

//Structs & Unions########################################################################################################################################
typedef struct{
    Sint16 x;
    Sint16 y;
    Uint32 color;
    Uint32 center_color;
    bool is_available;
}CIRCLE; //circle: barrack
typedef struct{
    int x;
    int y;
}menu_select_button;
typedef struct{
    int x;
    int y;
}MOUSE;
typedef enum{Map1=1, Map2, Map_test, Map_random}MAP;

//Function Prototypes#####################################################################################################################################
void define_random(int *no_circle, CIRCLE circle[], int *no_is_available); //defining number and position of circles in random maps
void define_test(int *no_circle, CIRCLE circle[], int *no_is_available); //defining position of the circles in memory-leak test map
void define_map1(int *no_circle, CIRCLE circle[], int *no_is_available); //defining position of the circles in map1
void define_map2(int *no_circle, CIRCLE circle[], int *no_is_available); //defining position of the circles in map2
void draw_circle(SDL_Renderer *renderer, CIRCLE circle[], Uint32 color, Uint32 color2, int i); //i: number of the circle
void init_color(SDL_Renderer *renderer, int no_circle, int no_is_available, CIRCLE circle[]); //coloring random mamp circles
SDL_Texture* Texture(SDL_Renderer* renderer, char* address); //address: path of the image which is supposed to get loaded
SDL_Rect Rect(SDL_Texture *tex, int x, int y, int w_decrement); //defining a rect for texture rendering
void menu_generator(SDL_Renderer *renderer, SDL_Texture *background, SDL_Texture *title,SDL_Texture *map1,SDL_Texture *map2,
                    SDL_Texture *map_test, SDL_Texture *map_random, bool *exit, int *map_no, MAP enum_map);
void random_map_generator(SDL_Renderer *renderer, SDL_Texture *background, int no_circle, int no_is_available, CIRCLE circle[], bool *exit);
void nonrandom_map_generator(SDL_Renderer *renderer, SDL_Texture *background, CIRCLE circle[], bool *exit, int no_circle);

//The main CODE###########################################################################################################################################
int main(){
    int no_circle; //number of circles
    int no_is_available; //number of available circles
    int map_no; MAP enum_map;  //which map is supposed to get generated
    CIRCLE circle[MAX_CIRCLE];
    srand(time(NULL));
    
    //SDL initialization & creating window and renderer
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER);
    SDL_Window *window = SDL_CreateWindow("State.io", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH,SCREEN_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_MOUSE_FOCUS);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    
    //Loading background
    SDL_Texture *background = Texture(renderer, "..\\Resources\\Background.bmp");
    //Loading images for menu generating
    SDL_Texture *title = Texture(renderer, "..\\Resources\\Title.bmp");
    SDL_Texture *map1 = Texture(renderer, "..\\Resources\\Map1.bmp");
    SDL_Texture *map2 = Texture(renderer, "..\\Resources\\Map2.bmp");
    SDL_Texture *map_test = Texture(renderer, "..\\Resources\\Map Test.bmp");
    SDL_Texture *map_random = Texture(renderer, "..\\Resources\\Map Random.bmp");
    
    //generating menu
    bool exit = false;
    while (!exit)
        menu_generator(renderer, background, title, map1, map2, map_test, map_random, &exit, &map_no, enum_map);
    
    //freeing menu-related textures
    SDL_DestroyTexture(title);
    SDL_DestroyTexture(map1);
    SDL_DestroyTexture(map2);
    SDL_DestroyTexture(map_test);
    SDL_DestroyTexture(map_random);
    
    exit = false;
    //generating the selected map
    if(map_no==Map1){
        define_map1(&no_circle, circle, &no_is_available);
        while(!exit)
            nonrandom_map_generator(renderer, background, circle, &exit, no_circle);
    }else if(map_no==Map2){
        define_map2(&no_circle, circle, &no_is_available);
        while(!exit)
            nonrandom_map_generator(renderer, background, circle, &exit, no_circle);
    }else if(map_no==Map_test){
        define_test(&no_circle, circle, &no_is_available);
        while(!exit)
            nonrandom_map_generator(renderer, background, circle, &exit, no_circle);
    }else if(map_no==Map_random){
        define_random(&no_circle, circle, &no_is_available);
        while(!exit)
            random_map_generator(renderer, background, no_circle, no_is_available, circle, &exit);
    }
    
    //freeing remaining things
    SDL_DestroyTexture(background);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void define_random(int *no_circle, CIRCLE circle[], int *no_is_available){
    //number of circles
    *no_circle = rand()%4 + 9;
    //position of circles
    for(int i=0; i<*no_circle; i++){
        bool conflict = false;
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
    //state(available or unavailable) of circles
    int no_is_unavailable=0;
    no_is_unavailable = rand()%4 +1;
    *no_is_available = *no_circle - no_is_unavailable;
    for(int i=0; i<no_is_unavailable; i++)
        circle[i].is_available = false;
}

void define_test(int *no_circle, CIRCLE circle[], int *no_is_available){
    *no_circle = *no_is_available = 13;
    for(int i=0; i<5; i++){
        circle[i].x = (i)*(2*radius+41) + 23 + radius;
        circle[i].y = 32 + radius;
        circle[i].is_available = true;
        circle[i].color = red_norm;
        circle[i].center_color = red_bold;
    }
    for(int i=5; i<8; i++){
        circle[i].x = (i-5)*(2*radius+170) + 85 + radius;
        circle[i].y = 3*radius + 63 + 32;
        circle[i].is_available = true;
        circle[i].color = red_norm;
        circle[i].center_color = red_bold;
    }
    //enemy color
    circle[6].color = blue_norm;
    circle[6].center_color = blue_bold;
    for(int i=8; i<13; i++){
        circle[i].x = (i-8)*(2*radius+41) + 23 + radius;
        circle[i].y = 2*(2*radius+63) + 32 + radius;
        circle[i].is_available = true;
        circle[i].color = red_norm;
        circle[i].center_color = red_bold;
    }
}

void define_map1(int *no_circle, CIRCLE circle[], int *no_is_available){
    *no_circle = 9;
    *no_is_available = 8;
    for(int j=0; j<3; j++) {
        for (int i = 0; i < 3; i++) {
            circle[3 * j + i].x = (3 * j + i) % 3 * (2 * radius + 170) + 85 + radius;
            circle[3 * j + i].y = ((3 * j + i)/3) * (2 * radius + 63) + 32 + radius;
            circle[3 * j + i].is_available = true;
            circle[3 * j + i].color = white;
            circle[3 * j + i].center_color = not_conquered;
        }
    }
    //setting color
    circle[1].color = red_norm; circle[1].center_color = red_bold;
    circle[4].color = not_available; circle[4].center_color = NULL; circle[4].is_available = false;
    circle[6].color = blue_norm; circle[6].center_color = blue_bold;
    circle[8].color = yellow_norm; circle[8].center_color = yellow_bold;
}

void define_map2(int *no_circle, CIRCLE circle[], int *no_is_available){
    *no_circle = *no_is_available = 7;
    for(int i=0; i<2; i++){
        circle[i].x = (i)*(2*radius+490) + 85 + radius;
        circle[i].y = 32 + radius;
        circle[i].is_available = true;
        circle[i].color = white;
        circle[i].center_color = not_conquered;
    }
    for(int i=2; i<5; i++){
        circle[i].x = (i-2)*(2*radius+170) + 85 + radius;
        circle[i].y = 3*radius + 63 + 32;
        circle[i].is_available = true;
        circle[i].color = white;
        circle[i].center_color = not_conquered;
    }
    for(int i=5; i<7; i++){
        circle[i].x = (i-5)*(2*radius+490) + 85 + radius;
        circle[i].y = 5*radius + 2*63 + 32;
        circle[i].is_available = true;
        circle[i].color = white;
        circle[i].center_color = not_conquered;
    }
    //setting color
    circle[1].color = blue_norm;   circle[1].center_color = blue_bold;
    circle[3].color = red_norm;    circle[3].center_color = red_bold;
    circle[5].color = yellow_norm; circle[5].center_color = yellow_bold;
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
        circle[no_is_unavailable].center_color = blue_bold;
    }else if(no_is_available==6 || no_is_available==7 || no_is_available==8){
        no_is_unavailable++;
        draw_circle(renderer, circle, blue_norm, blue_bold, no_is_unavailable);
        circle[no_is_unavailable].color = blue_norm;
        circle[no_is_unavailable].center_color = blue_bold;
        no_is_unavailable++;
        draw_circle(renderer, circle, yellow_norm, yellow_bold, no_is_unavailable);
        circle[no_is_unavailable].color = yellow_norm;
        circle[no_is_unavailable].center_color = yellow_bold;
    }else{
        no_is_unavailable++;
        draw_circle(renderer, circle, blue_norm,blue_bold, no_is_unavailable);
        circle[no_is_unavailable].color = blue_norm;
        circle[no_is_unavailable].center_color = blue_bold;
        no_is_unavailable++;
        draw_circle(renderer, circle, yellow_norm, yellow_bold, no_is_unavailable);
        circle[no_is_unavailable].color = yellow_norm;
        circle[no_is_unavailable].center_color = yellow_bold;
        no_is_unavailable++;
        draw_circle(renderer, circle, green_norm, green_bold, no_is_unavailable);
        circle[no_is_unavailable].color = green_norm;
        circle[no_is_unavailable].center_color = green_bold;
    }
    //circles which are not conquered yet
    for(int i=no_is_unavailable+1; i<no_circle; i++) {
        draw_circle(renderer, circle, white, not_conquered, i);
        circle[i].color = white;
        circle[i].center_color = not_conquered;
    }
}

SDL_Texture* Texture(SDL_Renderer *renderer, char *address){
    SDL_Surface *surface = SDL_LoadBMP(address);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_free(surface);
    return tex;
}

SDL_Rect Rect(SDL_Texture *tex, int x, int y, int w_decrement){
    SDL_Rect rect;
    SDL_QueryTexture(tex, NULL, NULL, &rect.w, &rect.h);
    rect.x = x;
    rect.w -= w_decrement;
    rect.y = y;
    return rect;
}

void menu_generator(SDL_Renderer *renderer, SDL_Texture *background, SDL_Texture *title,SDL_Texture *map1,
                    SDL_Texture *map2, SDL_Texture *map_test, SDL_Texture *map_random, bool *exit, int *map_no, MAP enum_map){
    
    //rendering background
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background, NULL, NULL);
    
    //calculating textures' dimensions and positions for rendering them
    SDL_Rect dest_title = Rect(title, (SCREEN_WIDTH - dest_title.w)/2, 35, 0);
    SDL_Rect dest_map1 = Rect(map1, (SCREEN_WIDTH - dest_map1.w)/2 - 130, dest_title.y -20, 100);
    SDL_Rect dest_map2 = Rect(map2, (SCREEN_WIDTH - dest_map2.w)/2 + 190, dest_title.y -20, 100);
    SDL_Rect dest_map_test = Rect(map_test, (SCREEN_WIDTH - dest_map_test.w)/2 + 30, dest_title.y/2 + dest_map1.h/4 -40, 100);
    SDL_Rect dest_map_random = Rect(map1, (SCREEN_WIDTH - dest_map_random.w)/2 + 35, dest_title.y + dest_map2.h/3 + 5, 100);
    
    //defining select buttons' positions
    menu_select_button select_map1 = {dest_map1.w/2-170, dest_map1.h/2};
    menu_select_button select_map2 = {dest_map2.w/2+140, dest_map1.h/2};
    menu_select_button select_map_test = {dest_map_test.w/2-90, dest_map_test.h/2+110};
    menu_select_button select_map_random = {dest_map_random.w/2-135, dest_map_random.h/2+220};
    
    //generating the map
    SDL_RenderCopy(renderer, title, NULL, &dest_title);
    SDL_RenderCopy(renderer, map1, NULL, &dest_map1);
    filledCircleColor(renderer, select_map1.x, select_map1.y, button_radius, button_color);
    SDL_RenderCopy(renderer, map2, NULL, &dest_map2);
    filledCircleColor(renderer, select_map2.x, select_map2.y, button_radius, button_color);
    SDL_RenderCopy(renderer, map_test, NULL, &dest_map_test);
    filledCircleColor(renderer, select_map_test.x, select_map_test.y, button_radius, button_color);
    SDL_RenderCopy(renderer, map_random, NULL, &dest_map_random);
    filledCircleColor(renderer, select_map_random.x, select_map_random.y, button_radius, button_color);
    SDL_RenderPresent(renderer);
    
    //getting cursor position
    MOUSE mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    
    //calculating select buttons' distances from cursor
    int distance1 = (mouse.x-select_map1.x)*(mouse.x-select_map1.x) + (mouse.y-select_map1.y)*(mouse.y-select_map1.y);
    int distance2 = (mouse.x-select_map2.x)*(mouse.x-select_map2.x) + (mouse.y-select_map2.y)*(mouse.y-select_map2.y);
    int distance3 = (mouse.x-select_map_test.x)*(mouse.x-select_map_test.x) + (mouse.y-select_map_test.y)*(mouse.y-select_map_test.y);
    int distance4 = (mouse.x-select_map_random.x)*(mouse.x-select_map_random.x) + (mouse.y-select_map_random.y)*(mouse.y-select_map_random.y);
    
    //checking if the exit button is pushed or one of the maps are selected
    SDL_Event event;
    while(SDL_PollEvent((&event))){
        switch(event.type){
            case SDL_MOUSEBUTTONDOWN:
                if(distance1 <= 400) {
                    *map_no = Map1;
                    *exit = true;
                }else if(distance2 <= 400) {
                    *map_no = Map2;
                    *exit = true;
                }else if(distance3 <= 400) {
                    *map_no = Map_test;
                    *exit = true;
                }else if(distance4 <= 400) {
                    *map_no = Map_random;
                    *exit = true;
                }
                break;
            case SDL_QUIT:
                *exit = true;
                break;
        }
    }
}

void random_map_generator(SDL_Renderer *renderer, SDL_Texture *background, int no_circle, int no_is_available, CIRCLE circle[], bool *exit){
    
    //rendering background
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background, NULL, NULL);
    
    //generating random map
    init_color(renderer, no_circle, no_is_available, circle);
    SDL_RenderPresent(renderer);
    
    //checking if the exit button is pushed
    SDL_Event event;
    while(SDL_PollEvent((&event))){
        switch(event.type)
            case SDL_QUIT:
                *exit = true;
        break;
    }
}

void nonrandom_map_generator(SDL_Renderer *renderer, SDL_Texture *background, CIRCLE circle[], bool *exit, int no_circle){
    //rendering background
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background, NULL, NULL);
    
    //rendering circles
    for(int i=0; i<no_circle; i++)
        draw_circle(renderer, circle, circle[i].color, circle[i].center_color, i);
    SDL_RenderPresent(renderer);
    
    //checking if the exit button is pushed
    SDL_Event event;
    while(SDL_PollEvent((&event))){
        switch(event.type)
            case SDL_QUIT:
                *exit = true;
        break;
    }
}