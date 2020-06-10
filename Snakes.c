// Snakes: The C game implemented to get familiar with ASCII encodings in C
// Date Completed: 10th June, 2020
// Author: Bhavya Saraf

/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Enjoy!!!
*/

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdbool.h>
#include<fcntl.h>
#include<termios.h>
#include<time.h>

#define update() printf("\033[H\033[J")
#define gotoxy(x, y) printf("\033[%d;%dH", x, y)
#define SCREEN_WIDTH 50
#define SCREEN_HEIGHT 30

char HORIZONTAL_BORDER[1+SCREEN_WIDTH];

/*InitHorizontalBorder Initializes HorizontalBorder string*/
void InitHorizontalBorder(){
    for(int i=0 ; i<SCREEN_WIDTH ; i++){HORIZONTAL_BORDER[i] = '*';}
    HORIZONTAL_BORDER[SCREEN_WIDTH] = '\0';
}

/*Position Struct Can be moulded into doubly linked lists*/
typedef struct Position{
    int row;
    int col;
    struct Position* next;
    struct Position* prev;
}Position;

/*Snake*/
typedef struct Snake{
    Position* snakeHead;
    Position* snakeTail;
    int snakeLength;
    char snakeDirection;
    int snakeSpeed;
}Snake;

/*Food*/
typedef struct Food{
    Position* foodPosition;
}Food;

/*Global Food Variable*/
Food* food;

/*Global Game win/lose state*/
bool lost = false;

/*MakeFrame draws frame of **/
void MakeFrame(){
    puts(HORIZONTAL_BORDER);
    for(int i=2 ; i<=SCREEN_HEIGHT-1 ; i++){
        gotoxy(i,1);  puts("*");
        gotoxy(i,SCREEN_WIDTH+1); puts("*");
    }
    puts(HORIZONTAL_BORDER);
}

/*DrawSnake draws snake*/
void DrawSnake(Snake* snake){
    if(snake == NULL){return;}
    for(Position* curPos = snake->snakeHead ; curPos != NULL ; curPos = curPos->next){
        gotoxy(curPos->row, curPos->col);
        puts("o");
    }
}

/*DrawFood draws global food*/
void DrawFood(){
    if(food == NULL){return;}
    gotoxy(food->foodPosition->row, food->foodPosition->col);
    puts("x");
}

/*NewFood gives new food*/
Food* NewFood(int foodPosRow, int foodPosCol){
    Food* newFood = (Food*)malloc(sizeof(Food));
    newFood->foodPosition = (Position*)malloc(sizeof(Position));
    newFood->foodPosition->row = foodPosRow;
    newFood->foodPosition->col = foodPosCol;
    newFood->foodPosition->next = NULL;
    newFood->foodPosition->prev = NULL;
    return newFood;
}

/*DeleteFood deletes global food*/
void DeleteFood(){
    if(food == NULL){return;}
    free(food->foodPosition);
    free(food);
}

/*NewSnake makes a new snake with the given stats of size 1*/
Snake* NewSnake(int snakeStartRow, int snakeStartCol, int snakeSpeed, char snakeStartDirection){
    Snake* newSnake = (Snake*)malloc(sizeof(Snake));
    newSnake->snakeHead = (Position*)malloc(sizeof(Position));
    newSnake->snakeHead->row = snakeStartRow;
    newSnake->snakeHead->col = snakeStartCol;
    newSnake->snakeHead->prev = NULL;
    newSnake->snakeHead->next = NULL;
    newSnake->snakeTail = newSnake->snakeHead;
    newSnake->snakeLength = 1;
    newSnake->snakeDirection = snakeStartDirection;
    newSnake->snakeSpeed = snakeSpeed;
    return newSnake;
}

/*Clear the snake stats from the memory*/
void DeleteSnake(Snake* snake){
    if(snake == NULL){return;}
    Position* curPos = snake->snakeHead;
    while(curPos){
        Position* tmpPos = curPos->next;
        free(curPos);
        curPos = tmpPos;
    }
    free(snake);
}

/*SnakeWallCollision determines if snake has collided with any of the four walls*/
bool SnakeWallCollision(Snake* snake){
    if(snake == NULL){return true;}
    if(snake->snakeHead->row <= 1 || snake->snakeHead->row >= SCREEN_HEIGHT){return true;}
    if(snake->snakeHead->col <= 1 || snake->snakeHead->col >= SCREEN_WIDTH){return true;}
    return false;
}

/*SnakeSnakeCollision determines if the snake has collided with itself*/
bool SnakeSnakeCollision(Snake* snake){
    if(snake == NULL){return true;}
    for(Position* curPos=snake->snakeHead->next ; curPos != NULL ; curPos = curPos->next){
        if(curPos->row == snake->snakeHead->row && curPos->col == snake->snakeHead->col){return true;}
    }
    return false;
}

/*SnakeEatFood determines if snake has eaten food*/
bool SnakeEatFood(Snake* snake){
    if(snake == NULL || food == NULL){return false;}
    if(snake->snakeHead->row == food->foodPosition->row && snake->snakeHead->col == food->foodPosition->col){return true;}
    return false;
}

/*Move Snake moves snake based on direction*/
void MoveSnake(Snake* snake){
    if(snake == NULL){return;}
    if(snake->snakeDirection == 'U'){
        snake->snakeTail->row = snake->snakeHead->row - 1;
        snake->snakeTail->col = snake->snakeHead->col;
    }else if(snake->snakeDirection == 'D'){
        snake->snakeTail->row = snake->snakeHead->row + 1;
        snake->snakeTail->col = snake->snakeHead->col;
    }else if(snake->snakeDirection == 'L'){
        snake->snakeTail->row = snake->snakeHead->row;
        snake->snakeTail->col = snake->snakeHead->col - 1;
    }else{
        snake->snakeTail->row = snake->snakeHead->row;
        snake->snakeTail->col = snake->snakeHead->col + 1;
    }

    if(SnakeWallCollision(snake)){
        update();
        puts("GAME OVER\n");
        lost = true;
    }

    if(SnakeSnakeCollision(snake)){
        update();
        puts("GAME OVER\n");
        lost = true;
    }

    if(SnakeEatFood(snake)){
        snake->snakeLength++;
        Position* newTail = (Position*)malloc(sizeof(Position));
        newTail->row = snake->snakeTail->row;
        newTail->col = snake->snakeTail->col;
        newTail->next = NULL;
        newTail->prev = snake->snakeTail;
        snake->snakeTail->next = newTail;
        snake->snakeTail = newTail;
        DeleteFood();
        food = NULL;
    }

    if(snake->snakeLength == 1){
        return;    
    }

    Position* newTail = snake->snakeTail->prev;
    snake->snakeTail->next = snake->snakeHead;
    snake->snakeTail->prev = NULL;
    snake->snakeHead->prev = snake->snakeTail;
    snake->snakeHead = snake->snakeTail;
    newTail->next = NULL;
    snake->snakeTail = newTail;
}

/*Wait for keyboard hit*/
int kbhit(void){
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if(ch != EOF){
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

/*Wait for keypress*/
char Getch(){
    char c;
    system("stty raw");
    c= getchar();
    system("stty sane");
    return(c);
}

/*This function checks if a key has pressed, then checks if its any of the arrow keys/ p/esc key. It changes direction acording to the key pressed.*/
int GetKeysPressed(){
    int pressed = -1;
    if(kbhit()){
        pressed=Getch();
    }
    return pressed;
}

/*Detects key press and updates snake direction accordingly*/
void UpdateSnakeDirection(Snake* snake){
    int press = GetKeysPressed();
    if(press != -1){
        if(press == 'w' && snake->snakeDirection != 'D'){
            snake->snakeDirection = 'U';
        }else if(press == 'a' && snake->snakeDirection != 'R'){
            snake->snakeDirection = 'L';
        }else if(press == 's' && snake->snakeDirection != 'U'){
            snake->snakeDirection = 'D';
        }else if(press == 'd' && snake->snakeDirection != 'L'){
            snake->snakeDirection = 'R';
        }
    }    
}

/*Add a delay each frame*/
void Delay(int ms) { 
    clock_t start_time = clock();  
    while (clock() < start_time + ms); 
}

/*GameLoop has is the main game loop*/
void GameLoop(){
    srand(time(NULL));
    InitHorizontalBorder();
    Snake* newSnake = NewSnake(20, 10, 1, 'U');
    food = NewFood(20, 15);
    int ind = 1;
    while(1){
            update();
            UpdateSnakeDirection(newSnake);
            MoveSnake(newSnake);

            MakeFrame(); 
            DrawFood();
            DrawSnake(newSnake);

            if(lost){break;}

            //sleep(1.1);
            if(ind%100 == 0 || food == NULL){
                DeleteFood();
                int foodRow = 2 + (SCREEN_HEIGHT + rand())%SCREEN_HEIGHT;
                int foodCol = 1 + (SCREEN_WIDTH + rand())%SCREEN_WIDTH;
                food = NewFood(foodRow, foodCol);
            }
            ind++;
            Delay(200000);
    }
    DeleteFood();
    DeleteSnake(newSnake);
}

int main(){     
    GameLoop();
    return 0;
}
