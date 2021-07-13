#include "SPI.h"
#include "Screen_HX8353E.h"
#include "labyrinth.h"


#define WALL 0
#define HOLE 0xf800
#define END 0x7e0
#define ANCHOR 10 //position to sum to array coordinates to draw the labirynth on the screen.

typedef struct{
  uint8_t x;
  uint8_t y;
}Acc;

typedef Acc AccWin[2];

typedef struct{
  uint8_t x;
  uint8_t y;
}Pos;

typedef Pos PosWin[2];

typedef struct {
  bool visited[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint8_t x[10] = {18,  109, 82, 22, 109, 92, 22, 48, 72, 80};
  uint8_t y[10] = {108, 103, 89, 76, 76,  33, 66, 32, 41, 15};
}CheckPoints;

AccWin accwin;
CheckPoints checkpoints;
PosWin poswin;

Screen_HX8353E screen;
uint16_t* imgPtr = (uint16_t*) imgArr;  //pointer to the labirynth array

/*
   screen initialization.
*/
void _initScreen() {
  screen.begin();
}

/*
   draw on the screen the a message indicating the player's victory.
*/
void drawWinGame(uint8_t &scores) {
  screen.clear(blackColour);
  screen.gText(18, 20, "YOU WIN!", whiteColour, blackColour, 2, 2);
  screen.gText(35, 60, "" + String(scores) + " points");
  screen.gText(7, 100, "Press S1 to restart");
}

/*
   draw on the screen the first page of the game.
*/
void drawFirstPageGame() {
  screen.clear(blackColour);
  screen.gText(10, 20, "LABYRINTH", whiteColour, blackColour, 2, 2);
  screen.gText(35, 40, "GAME!", whiteColour, blackColour, 2, 2);
  screen.gText(10, 100, "Press S1 to start!");
}

/*
   draw on the sceen the basic visual of the game.
   params:
   - scores : player's scores.
*/
void drawStartGame(uint8_t &scores) {
  screen.rectangle(120, 62, 125, 63, whiteColour);
  screen.rectangle(62, 120, 63, 125, whiteColour);
  screen.circle(4, 80, 4, whiteColour);
  screen.circle(4, 95, 4, whiteColour);
  screen.circle(4, 110, 4, whiteColour);
  screen.gText(11, 2, "score : ");
  screen.gText(55, 2, String(scores));
}

/*
   draw on the screen a message indicating the player takes a break from the game.
*/
void drawPause() {
  screen.clear(blackColour);
  screen.gText(30, 20, "PAUSE!", whiteColour, blackColour, 2, 2);
  screen.gText(5, 100, "Press S1 to continue game");
}

/*
   draw on the screen a message indicating the player failed the game.
*/
void drawLooseGame() {
  screen.clear(blackColour);
  screen.gText(10, 20, "YOU LOOSE!", whiteColour, blackColour, 2, 2);
  screen.gText(5, 100, "Press S1 to restart");
}

/*
   draw the player's scores on the screen.
   params:
   - scores : scores of the player.
*/
void drawScores(uint8_t &scores) {
  screen.gText(55, 2, String(scores));
}

/*
   draw bars on the screen representing the values obtained from the acceleration sensor.
   params:
   accwin : window of 2 elements of data from acceleration sensor ( only x and y axes )
*/
void drawAccelBar(AccWin &accwin) {

  if (accwin[1].x != accwin[0].x) {                                       //only if new data is different from the past data
    if (accwin[0].x < 50) {                                               //deleting left rectangle
      screen.rectangle(11 + accwin[0].x, 120, 61, 125, blackColour);
    } else { //deleting right rectangle
      screen.rectangle(64, 120, 64 + accwin[0].x - 50, 125, blackColour);
    }

    if (accwin[1].x < 50) {                                               //creating new left rectangle
      screen.rectangle(11 + accwin[1].x, 120, 61, 125, whiteColour);
    } else {                                                              //creating new right rectangle
      screen.rectangle(64, 120, 64 + accwin[1].x - 50, 125, whiteColour);
    }
  }
  if (accwin[1].y != accwin[0].y ) {                                               //do it only if data changed
    if (accwin[0].y < 50) {                                               //deleting old rectangle bottom
      screen.rectangle(120, 64, 125, 64 + map(accwin[0].y, 49, 0, 0, 59), blackColour);
    } else {                                                              //deleting old rectangle upper
      screen.rectangle(120, map(accwin[0].y, 50, 100, 61, 11), 125, 61, blackColour);
    }

    if (accwin[1].y < 50) {                                               //creating new rectangle bottom
      screen.rectangle(120, 64, 125, 64 + map(accwin[1].y, 49, 0, 0, 59), whiteColour);
    } else {                                                              //creating new rectangle upper
      screen.rectangle(120, map(accwin[1].y, 50, 100, 61, 11), 125, 61, whiteColour);
    }
  }
}

/*
   draw the entire labirynth on the screen.
*/
void drawLabyrinth() {
  uint16_t* img = (uint16_t*) imgArr;
  for (volatile uint8_t y = 0; y < IMAGE_H; y++) {
    for (volatile uint8_t x = 0; x < IMAGE_W; x++) {
      if (*(img) == WALL) {
        screen.point(x + ANCHOR, y + ANCHOR, whiteColour); // labirynth
        *(img++);
      } else if ( *(img) == HOLE ) {
        screen.point(x + ANCHOR, y + ANCHOR, redColour); //holes
        *(img++);
      } else if ( *(img) == END ) {
        screen.point(x + ANCHOR, y + ANCHOR, greenColour); //end hole
        *(img++);
      } else {
        *(img++);
      }
    }
  }
}

/*
   draw a yellow marble on the screen. Used to initiate the initial position of the player's marble.
   params:
   - poswin : cooirdinates of the marble
*/
void setBall(PosWin &poswin) {
  screen.circle(poswin[1].x, poswin[1].y, 1, yellowColour);
  screen.circle(poswin[1].x, poswin[1].y, 2, yellowColour);
}

/*
   draw a black marble on the screen. Used to "cancel" the yellow marble
   - poswin : cooirdinates of the marble  
*/
void drawBlackBall(PosWin &poswin) {
  screen.circle(poswin[0].x, poswin[0].y, 2, blackColour);
  screen.circle(poswin[0].x, poswin[0].y, 1, blackColour);
}

/*
   manage the graphics visualization of life
   params:
   - life : player's life
*/
void drawlife(uint8_t &life) {
  if (life == 3) {
    screen.circle(4, 80, 4, whiteColour);
    screen.circle(4, 95, 4, whiteColour);
    screen.circle(4, 110, 4, whiteColour);
  } else if (life == 2) {
    screen.circle(4, 80, 4, blackColour);
  } else if (life == 1) {
    screen.circle(4, 95, 4, blackColour);
  } else {
    screen.circle(4, 110, 4, blackColour);
  }
}

/*
   re-draw labirynth data on screen.
   params:
   - x : x coord based on marble's position
   - y : y coord based on marble's position
   copy a 5x5 rectangle with center coordinate <x,y> from the labirynth array to the screen.
*/
void drawLabirynthArea(uint8_t &x, uint8_t &y) {
  uint8_t i, j, limitX, limitY;
  uint16_t pos;

  i = x - 2 - ANCHOR;
  j = y - 2 - ANCHOR;
  limitX = i + 4;
  limitY = j + 4;

  for (volatile uint8_t yy = j; yy < limitY + 1; yy++) {
    for (volatile uint8_t xx = i; xx < limitX + 1; xx++) {
      pos = *(imgPtr + ( IMAGE_H * yy) + xx);
      if (pos == 0xffff)
        screen.point(xx + ANCHOR, yy + ANCHOR, blackColour);
      else if (pos == HOLE)
        screen.point(xx + ANCHOR, yy + ANCHOR, redColour);
    }
  }
}
/*
   restore the background around the ball.
   params :
   - x : marble's x coordinate;
   - y : marble's y coordinate;
   - moveX : how much the marble moved on the x axis;
   - moveY : how much the marble moved on the y axis;
   - dirX : direction of the marble on the x axis;
   - dirY : direction of the marble on the y axis.
*/
void restoreBackground(uint8_t &x, uint8_t &y, uint8_t &moveX, uint8_t &moveY, bool &dirX, bool &dirY) {
  uint8_t i, j;
  uint16_t pos;

  if (moveX > 0 && moveY == 0 && dirX == 1) {          //marble go to the right, draw background on its left
    //Serial.print("right\n");
    for (i = x - 2 - moveX; i < x - 2; i++) {
      for (j = y - 2; j <= y + 2; j++) {
        pos = *(imgPtr + ( IMAGE_H * (j - ANCHOR)) + i - ANCHOR);
        if (pos == HOLE)
          screen.point(i, j, redColour);
        else if (pos == END)
          screen.point(i, j, greenColour);
      }
    }
  } else if (moveX > 0 && moveY == 0 && dirX == 0) {     //marble go to the left, draw background on its right
    //Serial.print("left\n");
    for (i = x + 2; i < x + 2 + moveX; i++) {
      for (j = y - 2; j <= y + 2; j++) {
        pos = *(imgPtr + ( IMAGE_H * (j - ANCHOR)) + i - ANCHOR);
        if (pos == HOLE)
          screen.point(i, j, redColour);
        else if (pos == END)
          screen.point(i, j, greenColour);
      }
    }


  } else if (moveX == 0 && moveY > 0 && dirY == 1) {     //marble go up, draw background on its bottom
    //Serial.print("up\n");
    for (i = x - 2; i <= x + 2; i++) {
      for (j = y + 2; j <= y + 2 + moveY; j++) {
        pos = *(imgPtr + ( IMAGE_H * (j - ANCHOR)) + i - ANCHOR);
        if (pos == HOLE)
          screen.point(i, j, redColour);
        else if (pos == END)
          screen.point(i, j, greenColour);
      }
    }

  } else if (moveX == 0 && moveY > 0 && dirY == 0) {      //marble go down, draw background on its head
    //Serial.print("down\n");
    for (i = x - 2; i <= x + 2; i++) {
      for (j = y - 2 - moveY; j <= y - 2; j++) {
        pos = *(imgPtr + ( IMAGE_H * (j - ANCHOR)) + i - ANCHOR);
        if (pos == HOLE)
          screen.point(i, j, redColour);
        else if (pos == END)
          screen.point(i, j, greenColour);
      }
    }
  } else if (moveX > 0 && moveY > 0) {

    if (dirX == 1 && dirY == 1) {                             //right-up
      //Serial.print("right-up\n");

      //delete left part
      for (i = x - 2 - moveX; i < x - 2; i++) {
        for (j = y - 2; j <= y + 2 + moveY; j++) {
          pos = *(imgPtr + ( IMAGE_H * (j - ANCHOR)) + i - ANCHOR);
          if (pos == HOLE)
            screen.point(i, j, redColour);
          else if (pos == END)
            screen.point(i, j, greenColour);
        }
      }

      //delete bottom part
      for (i = x - 2; i <= x + 2; i++) {
        for (j = y + 2; j <= y + 2 + moveY; j++) {
          pos = *(imgPtr + ( IMAGE_H * (j - ANCHOR)) + i - ANCHOR);
          if (pos == HOLE)
            screen.point(i, j, redColour);
          else if (pos == END)
            screen.point(i, j, greenColour);
        }
      }

    } else if (dirX == 1 && dirY == 0) {                       //right-down
      //Serial.print("right-down\n");

      //delete left part
      for (i = x - 2 - moveX; i < x - 2; i++) {
        for (j = y - 2 - moveY; j <= y + 2; j++) {
          pos = *(imgPtr + ( IMAGE_H * (j - ANCHOR)) + i - ANCHOR);
          if (pos == HOLE)
            screen.point(i, j, redColour);
          else if (pos == END)
            screen.point(i, j, greenColour);
        }
      }

      //delete upper part
      for (i = x - 2; i <= x + 2; i++) {
        for (j = y - 2 - moveY; j <= y - 2; j++) {
          pos = *(imgPtr + ( IMAGE_H * (j - ANCHOR)) + i - ANCHOR);
          if (pos == HOLE)
            screen.point(i, j, redColour);
          else if (pos == END)
            screen.point(i, j, greenColour);
        }
      }

    } else if (dirX == 0 && dirY == 1) {                       //left-up
      //Serial.print("left-up\n");

      //delete right part
      for (i = x + 2; i < x + 2 + moveX; i++) {
        for (j = y - 2; j <= y + 2 + moveY; j++) {
          pos = *(imgPtr + ( IMAGE_H * (j - ANCHOR)) + i - ANCHOR);
          if (pos == HOLE)
            screen.point(i, j, redColour);
          else if (pos == END)
            screen.point(i, j, greenColour);
        }
      }

      //delete bottom part
      for (i = x - 2; i <= x + 2; i++) {
        for (j = y + 2; j <= y + 2 + moveY; j++) {
          pos = *(imgPtr + ( IMAGE_H * (j - ANCHOR)) + i - ANCHOR);
          if (pos == HOLE)
            screen.point(i, j, redColour);
          else if (pos == END)
            screen.point(i, j, greenColour);
        }
      }


    } else if (dirX == 0 && dirY == 0) {                        //left-down
      //Serial.print("left-down\n");

      //delete right part
      for (i = x + 2; i < x + 2 + moveX; i++) {
        for (j = y - 2 - moveY; j <= y + 2; j++) {
          pos = *(imgPtr + ( IMAGE_H * (j - ANCHOR)) + i - ANCHOR);
          if (pos == HOLE)
            screen.point(i, j, redColour);
          else if (pos == END)
            screen.point(i, j, greenColour);
        }
      }

      //delete upper part
      for (i = x - 2; i <= x + 2; i++) {
        for (j = y - 2 - moveY; j <= y - 2; j++) {
          pos = *(imgPtr + ( IMAGE_H * (j - ANCHOR)) + i - ANCHOR);
          if (pos == HOLE)
            screen.point(i, j, redColour);
          else if (pos == END)
            screen.point(i, j, greenColour);
        }
      }
    }
  }
}
