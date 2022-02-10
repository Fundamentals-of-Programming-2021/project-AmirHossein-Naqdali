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
//menu:
#define button_radius 20
#define button_color 0xffff3333
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
//soldiers:
#define MAX_SOLDIERS 10
#define PRODUCING_PERIOD 20 //producing 1 soldier in 10

//Structs & Unions########################################################################################################################################
typedef struct{
    Sint16 x;
    Sint16 y;
    Uint32 color;
    Uint32 center_color;
    bool is_available;
    int no_soldiers;
    float produce;
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
SDL_Texture* Texture(SDL_Renderer* renderer, char* address); //address: path of the image which is supposed to get loaded
SDL_Rect Rect(SDL_Texture *tex, int x, int y, int w_decrement); //defining a rect for texture rendering
void menu_generator(SDL_Renderer *renderer, SDL_Texture *background, SDL_Texture *title,SDL_Texture *map1,SDL_Texture *map2,
                    SDL_Texture *map_test, SDL_Texture *map_random, int *map_no, MAP enum_map);
void define_map1(int *no_circle, CIRCLE circle[], int *no_is_available); //defining position of the circles in map1
void define_map2(int *no_circle, CIRCLE circle[], int *no_is_available); //defining position of the circles in map2
void define_test(int *no_circle, CIRCLE circle[], int *no_is_available); //defining position of the circles in memory-leak test map
void define_random(int *no_circle, CIRCLE circle[], int *no_is_available); //defining number and position of circles in random maps
void draw_circle(SDL_Renderer *renderer, CIRCLE circle[], Uint32 color, Uint32 color2, int i); //i: number of the circle
void soldiers_no_rendering(SDL_Renderer *renderer, int no_circle, CIRCLE circle[]); //prints number of soldiers of each circle
void map_generator(SDL_Renderer *renderer, SDL_Texture *background, CIRCLE circle[], int no_circle, int orig, int dest);
int distance(int x1, int y1, int x2, int y2); //calculating distance between d1(x1, y1) and d2(x2, y2)
int which_circle_origin(CIRCLE circle[], int no_circle, MOUSE mouse); //determining which circle is clicked(The ATTACKER)
int which_circle_destination(CIRCLE circle[], int no_circle, MOUSE mouse); //determining which circle is clicked(The DEFENDER)
void soldier_update(int *orig, int *dest, CIRCLE circle[], int condition); //updates the map after soldiers movement

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
    menu_generator(renderer, background, title, map1, map2, map_test, map_random, &map_no, enum_map);
    
    //freeing menu-related textures
    SDL_DestroyTexture(title);
    SDL_DestroyTexture(map1);
    SDL_DestroyTexture(map2);
    SDL_DestroyTexture(map_test);
    SDL_DestroyTexture(map_random);
    
    //variables for checking if both origin and destination circles are valid for soldiers movement
    int orig=-1, dest=-1;
    //generating the selected map
    if(map_no==Map1){
        define_map1(&no_circle, circle, &no_is_available);
        map_generator(renderer, background, circle, no_circle, orig, dest);
    }else if(map_no==Map2){
        define_map2(&no_circle, circle, &no_is_available);
        map_generator(renderer, background, circle, no_circle, orig, dest);
    }else if(map_no==Map_test){
        define_test(&no_circle, circle, &no_is_available);
        map_generator(renderer, background, circle, no_circle, orig, dest);
    }else if(map_no==Map_random){
        define_random(&no_circle, circle, &no_is_available);
        map_generator(renderer, background, circle, no_circle, orig, dest);
    }
    
    //freeing remaining things
    SDL_DestroyTexture(background);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
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

void menu_generator(SDL_Renderer *renderer, SDL_Texture *background, SDL_Texture *title,SDL_Texture *map1,SDL_Texture *map2,
                    SDL_Texture *map_test, SDL_Texture *map_random, int *map_no, MAP enum_map){
    while (true) {
        //rendering background
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, NULL, NULL);
    
        //calculating textures' dimensions and positions for rendering them
        SDL_Rect dest_title = Rect(title, (SCREEN_WIDTH - dest_title.w) / 2, 35, 0);
        SDL_Rect dest_map1 = Rect(map1, (SCREEN_WIDTH - dest_map1.w) / 2 - 130, dest_title.y - 20, 100);
        SDL_Rect dest_map2 = Rect(map2, (SCREEN_WIDTH - dest_map2.w) / 2 + 190, dest_title.y - 20, 100);
        SDL_Rect dest_map_test = Rect(map_test, (SCREEN_WIDTH - dest_map_test.w) / 2 + 30,
                                      dest_title.y / 2 + dest_map1.h / 4 - 40, 100);
        SDL_Rect dest_map_random = Rect(map1, (SCREEN_WIDTH - dest_map_random.w) / 2 + 35,
                                        dest_title.y + dest_map2.h / 3 + 5, 100);
    
        //defining select buttons' positions
        menu_select_button select_map1 = {dest_map1.w / 2 - 170, dest_map1.h / 2};
        menu_select_button select_map2 = {dest_map2.w / 2 + 140, dest_map1.h / 2};
        menu_select_button select_map_test = {dest_map_test.w / 2 - 90, dest_map_test.h / 2 + 110};
        menu_select_button select_map_random = {dest_map_random.w / 2 - 135, dest_map_random.h / 2 + 220};
    
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
        int distance1 = distance(mouse.x, mouse.y, select_map1.x, select_map1.y);
        int distance2 = distance(mouse.x, mouse.y, select_map2.x, select_map2.y);
        int distance3 = distance(mouse.x, mouse.y, select_map_test.x, select_map_test.y);
        int distance4 = distance(mouse.x, mouse.y, select_map_random.x, select_map_random.y);
    
        //checking if the exit button is pushed or one of the maps are selected
        SDL_Event event;
        while (SDL_PollEvent((&event))) {
            switch (event.type) {
                case SDL_MOUSEBUTTONDOWN:
                    if (distance1 <= 400) {
                        *map_no = Map1;
                        return;
                    } else if (distance2 <= 400) {
                        *map_no = Map2;
                        return;
                    } else if (distance3 <= 400) {
                        *map_no = Map_test;
                        return;
                    } else if (distance4 <= 400) {
                        *map_no = Map_random;
                        return;
                    }
                    break;
                case SDL_QUIT:
                    return;
            }
        }
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
            circle[3 * j + i].no_soldiers = 0;
            circle[3 * j + i].produce = 0;
        }
    }
    //setting color
    circle[1].color = red_norm; circle[1].center_color = red_bold; circle[1].no_soldiers = MAX_SOLDIERS;
    circle[4].color = not_available; circle[4].center_color = 0x00ffffff; circle[4].is_available = false;
    circle[6].color = blue_norm; circle[6].center_color = blue_bold; circle[6].no_soldiers = MAX_SOLDIERS;
    circle[8].color = yellow_norm; circle[8].center_color = yellow_bold; circle[8].no_soldiers = MAX_SOLDIERS;
}

void define_map2(int *no_circle, CIRCLE circle[], int *no_is_available){
    *no_circle = *no_is_available = 7;
    for(int i=0; i<2; i++){
        circle[i].x = (i)*(2*radius+490) + 85 + radius;
        circle[i].y = 32 + radius;
        circle[i].is_available = true;
        circle[i].color = white;
        circle[i].center_color = not_conquered;
        circle[i].no_soldiers = 0;
        circle[i].produce = 0;
    }
    for(int i=2; i<5; i++){
        circle[i].x = (i-2)*(2*radius+170) + 85 + radius;
        circle[i].y = 3*radius + 63 + 32;
        circle[i].is_available = true;
        circle[i].color = white;
        circle[i].center_color = not_conquered;
        circle[i].no_soldiers = 0;
        circle[i].produce = 0;
    }
    for(int i=5; i<7; i++){
        circle[i].x = (i-5)*(2*radius+490) + 85 + radius;
        circle[i].y = 5*radius + 2*63 + 32;
        circle[i].is_available = true;
        circle[i].color = white;
        circle[i].center_color = not_conquered;
        circle[i].no_soldiers = 0;
        circle[i].produce = 0;
    }
    //setting color
    circle[1].color = blue_norm;   circle[1].center_color = blue_bold; circle[1].no_soldiers = MAX_SOLDIERS;
    circle[3].color = red_norm;    circle[3].center_color = red_bold; circle[3].no_soldiers = MAX_SOLDIERS;
    circle[5].color = yellow_norm; circle[5].center_color = yellow_bold; circle[5].no_soldiers = MAX_SOLDIERS;
}

void define_test(int *no_circle, CIRCLE circle[], int *no_is_available){
    *no_circle = *no_is_available = 13;
    for(int i=0; i<5; i++){
        circle[i].x = (i)*(2*radius+41) + 23 + radius;
        circle[i].y = 32 + radius;
        circle[i].is_available = true;
        circle[i].color = red_norm;
        circle[i].center_color = red_bold;
        circle[i].no_soldiers = MAX_SOLDIERS;
        circle[i].produce = 0;
    }
    for(int i=5; i<8; i++){
        circle[i].x = (i-5)*(2*radius+170) + 85 + radius;
        circle[i].y = 3*radius + 63 + 32;
        circle[i].is_available = true;
        circle[i].color = red_norm;
        circle[i].center_color = red_bold;
        circle[i].no_soldiers = MAX_SOLDIERS;
        circle[i].produce = 0;
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
        circle[i].no_soldiers = MAX_SOLDIERS;
        circle[i].produce = 0;
    }
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
//defining colors and number of the soldiers:
    //not available circles
    for(int i=0; i<no_is_unavailable; i++){
        circle[i].color = not_available;
        circle[i].center_color = 0x00ffffff;
        circle[i].no_soldiers = 0;
        circle[i].produce = 0;
    }
    //own circle
    circle[no_is_unavailable].color = red_norm;
    circle[no_is_unavailable].center_color = red_bold;
    circle[no_is_unavailable].no_soldiers = MAX_SOLDIERS;
    circle[no_is_unavailable].produce = 0;
    //enemy circle(s)
    if(*no_is_available == 5){
        no_is_unavailable++;
        circle[no_is_unavailable].color = blue_norm;
        circle[no_is_unavailable].center_color = blue_bold;
        circle[no_is_unavailable].no_soldiers = MAX_SOLDIERS;
        circle[no_is_unavailable].produce = 0;
    }else if(*no_is_available==6 || *no_is_available==7 || *no_is_available==8){
        no_is_unavailable++;
        circle[no_is_unavailable].color = blue_norm;
        circle[no_is_unavailable].center_color = blue_bold;
        circle[no_is_unavailable].no_soldiers = MAX_SOLDIERS;
        circle[no_is_unavailable].produce = 0;
        no_is_unavailable++;
        circle[no_is_unavailable].color = yellow_norm;
        circle[no_is_unavailable].center_color = yellow_bold;
        circle[no_is_unavailable].no_soldiers = MAX_SOLDIERS;
        circle[no_is_unavailable].produce = 0;
    }else{
        no_is_unavailable++;
        circle[no_is_unavailable].color = blue_norm;
        circle[no_is_unavailable].center_color = blue_bold;
        circle[no_is_unavailable].no_soldiers = MAX_SOLDIERS;
        circle[no_is_unavailable].produce = 0;
        no_is_unavailable++;
        circle[no_is_unavailable].color = yellow_norm;
        circle[no_is_unavailable].center_color = yellow_bold;
        circle[no_is_unavailable].no_soldiers = MAX_SOLDIERS;
        circle[no_is_unavailable].produce = 0;
        no_is_unavailable++;
        circle[no_is_unavailable].color = green_norm;
        circle[no_is_unavailable].center_color = green_bold;
        circle[no_is_unavailable].no_soldiers = MAX_SOLDIERS;
        circle[no_is_unavailable].produce = 0;
    }
    //circles which are not conquered yet
    for(int i=no_is_unavailable+1; i<*no_circle; i++) {
        circle[i].color = white;
        circle[i].center_color = not_conquered;
        circle[i].no_soldiers = 0;
        circle[i].produce = 0;
    }
}

void draw_circle(SDL_Renderer *renderer, CIRCLE circle[], Uint32 color, Uint32 color2, int i){
    filledCircleColor(renderer, circle[i].x, circle[i].y, radius, white);
    filledCircleColor(renderer, circle[i].x, circle[i].y, radius, color);
    boxColor(renderer, circle[i].x-15, circle[i].y-15,circle[i].x+15, circle[i].y+15, color2);
}

void soldiers_no_rendering(SDL_Renderer *renderer, int no_circle, CIRCLE circle[]){
    char help[5] = {'\0'};
    for(int i=0; i<no_circle; i++){
        sprintf(help, "%d", circle[i].no_soldiers);
        if(circle[i].color == white)
            stringRGBA(renderer, circle[i].x+20, circle[i].y, help, 0x00, 0x00, 0x00, 0xff);
        else if(circle[i].color == red_norm)
            stringRGBA(renderer, circle[i].x+20, circle[i].y, help, 0x00, 0x00, 0x00, 0xff);
        else if(circle[i].color == blue_norm)
            stringRGBA(renderer, circle[i].x+20, circle[i].y, help, 0x00, 0x00, 0x00, 0xff);
        else if(circle[i].color == yellow_norm)
            stringRGBA(renderer, circle[i].x+20, circle[i].y, help, 0x00, 0x00, 0x00, 0xff);
        else if(circle[i].color == green_norm)
            stringRGBA(renderer, circle[i].x+20, circle[i].y, help, 0x00, 0x00, 0x00, 0xff);
    }
}

void map_generator(SDL_Renderer *renderer, SDL_Texture *background, CIRCLE circle[], int no_circle, int orig, int dest){
    int condition = 1; //to check if both origin and dest are taken
    while(true){
        //rendering background
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, NULL, NULL);
        
        //rendering circles
        for(int i=0; i<no_circle; i++)
            draw_circle(renderer, circle, circle[i].color, circle[i].center_color, i);
        soldiers_no_rendering(renderer, no_circle, circle);
        SDL_RenderPresent(renderer);
        
        //getting cursor position
        MOUSE mouse;
        
        //producing soldiers in circles
        for(int i=0; i<no_circle; i++){
            if(circle[i].center_color!=not_conquered && circle[i].no_soldiers<MAX_SOLDIERS) {
                circle[i].produce += 1.f / PRODUCING_PERIOD;
                if(circle[i].produce>=1){
                    circle[i].no_soldiers++;
                    circle[i].produce--;
                }
            }
            else
                circle[i].produce = 0;
        }
        
        //update soldier numbers and circle colors after movement
        soldier_update(&orig, &dest, circle, condition);
        
        //checking if the exit button is pushed or if we want to move soldiers
        SDL_Event event;
        while(SDL_PollEvent((&event))){
            switch(event.type){
                case SDL_MOUSEBUTTONDOWN:
                    SDL_GetMouseState(&mouse.x, &mouse.y);
                    orig = which_circle_origin(circle, no_circle, mouse);
                    condition=0;
                    printf("orig:%d\n", orig);
                    break;
                case SDL_MOUSEBUTTONUP:
                    SDL_GetMouseState(&mouse.x, &mouse.y);
                    dest = which_circle_destination(circle, no_circle, mouse);
                    condition=1;
                    printf("dest:%d\n", dest);
                    break;
                case SDL_QUIT:
                    return;
            }
        }
        SDL_Delay(1000/FPS);
    }
}

int distance(int x1, int y1, int x2, int y2){
   return (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
}

int which_circle_origin(CIRCLE circle[], int no_circle, MOUSE mouse){
    for(int i=0; i<no_circle; i++){
        if(distance(mouse.x, mouse.y, circle[i].x, circle[i].y) <= 450){
            if(circle[i].color==red_norm)
                return i;
        }
    }
    return -1;
}

int which_circle_destination(CIRCLE circle[], int no_circle, MOUSE mouse){
    for(int i=0; i<no_circle; i++){
        if(distance(mouse.x, mouse.y, circle[i].x, circle[i].y) <= 450){
            if(circle[i].color!=not_available)
                return i;
        }
    }
    return -1;
}

void soldier_update(int *orig, int *dest, CIRCLE circle[], int condition){
    if(condition && *orig>=0 && *dest>=0 && *orig!=*dest){
        printf("before: orig:%d, dest:%d\n", circle[*orig].no_soldiers, circle[*dest].no_soldiers);
        if(circle[*orig].color == circle[*dest].color){ //circles of the same color
            circle[*dest].no_soldiers += circle[*orig].no_soldiers;
            circle[*orig].no_soldiers = 0;
        }else{ //circles of different colors
            circle[*dest].no_soldiers -= circle[*orig].no_soldiers;
            circle[*orig].no_soldiers = 0;
            if(circle[*dest].no_soldiers < 0){ //if destination circle is conquered
                circle[*dest].no_soldiers *= (-1);
                circle[*dest].color = circle[*orig].color;
                if(circle[*dest].center_color != not_conquered) {
                    switch (circle[*orig].color) {
                        case red_norm:
                            circle[*dest].center_color = red_bold;
                            break;
                        case blue_norm:
                            circle[*dest].center_color = blue_bold;
                            break;
                        case yellow_norm:
                            circle[*dest].center_color = yellow_bold;
                            break;
                        case green_norm:
                            circle[*dest].center_color = green_bold;
                            break;
                    }
                }
            }
        }
        printf("after: orig:%d, dest:%d\n", circle[*orig].no_soldiers, circle[*dest].no_soldiers);
        //changing orig and dest to their primitive amounts
        *orig=-1, *dest=-1;
    }
}