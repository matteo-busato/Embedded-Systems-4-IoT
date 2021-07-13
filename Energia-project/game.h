#define MINMOV 0
#define MAXMOV 5
#define MAXMAPHIGH 70
#define MINMAPHIGH 50
#define MAXMAPLOW 49
#define MINMAPLOW 29
uint8_t scores;
volatile bool loose = 0;
volatile bool win = 0;
uint8_t life = 3;


/*
   initialize the initial position of the marble.
*/
void initPositions() {
  poswin[1].x = 18;
  poswin[1].y = 108;
  poswin[0].x = 0;
  poswin[0].y = 0;
}

/*
   initialize the game.
*/
void initGame() {
  scores = 0;
  drawFirstPageGame();
  initPositions();
}

/*
   reset the game settings.
*/
void resetGame() {
  uint8_t i;
  for (i = 0; i < 9; i++) {
    checkpoints.visited[i] = 0;
  }
  scores = 0;
  life = 3;
  loose = 0;
  win = 0;
  initPositions();
}

/*
   set the win boolean and draw the win page.
*/
void winGame() {
  win = 1;
  drawWinGame(scores);
}

/*
   set the loose boolean and draw the loose page.
*/
void looseGame() {
  loose = 1;
  drawLooseGame();
}

/*
   check the marble's position and assign the scores based on the checkpoint.
   Set the checkpoint position.
   Check if the player wins.
   params:
   - pos : value in the labirynth array based on the marble's position.
*/
bool checkScores(uint16_t &pos) {
  bool state = 0;

  if (pos == 10 && checkpoints.visited[0] == 0) {
    checkpoints.visited[0] = 1;
    state = 1;
    checkPointSound();
  } else if (pos == 20 && checkpoints.visited[1] == 0) {
    checkpoints.visited[1] = 1;
    if (checkpoints.visited[0] == 0)
      checkpoints.visited[0] = 1;
    state = 1;
    checkPointSound();
  } else if (pos == 30 && checkpoints.visited[2] == 0) {
    checkpoints.visited[2] = 1;
    state = 1;
    checkPointSound();
  } else if (pos == 40 && checkpoints.visited[3] == 0) {
    checkpoints.visited[3] = 1;
    state = 1;
    checkPointSound();
  } else if (pos == 50 && checkpoints.visited[4] == 0) {
    checkpoints.visited[4] = 1;
    state = 1;
    checkPointSound();
  } else if (pos == 60 && checkpoints.visited[5] == 0) {
    checkpoints.visited[5] = 1;
    state = 1;
    checkPointSound();
  } else if (pos == 70 && checkpoints.visited[6] == 0) {
    checkpoints.visited[6] = 1;
    state = 1;
    checkPointSound();
  } else if (pos == 80 && checkpoints.visited[7] == 0) {
    checkpoints.visited[7] = 1;
    state = 1;
    checkPointSound();
  } else if (pos == 90 && checkpoints.visited[8] == 0) {
    checkpoints.visited[8] = 1;
    state = 1;
    checkPointSound();
  } else if (pos == END) { //end of game
    scores += 10;
    winGame();
    winSound();
    return 1;
  }
  if (state) {
    scores += 10;
    drawScores(scores);
  }
  return 0;
}

/*
   Control if the marble can move left. The marble can't move if there's walls.
   params:
   - x : x new position;

   return : how much the marble can move left.
*/
uint8_t checkLeft(uint8_t &x) {
  int8_t i, j, X, Y;
  uint16_t pos;

  for (i = 1; i <= x; i++) {                    //control how much can I move to the left    
    X = poswin[1].x - ANCHOR - i - 3;
    for (j = -2; j < 3; j++) {                 //control if I find walls upper and lower the marble's center.
      Y = poswin[1].y - ANCHOR + j;
      pos = *(imgPtr + ( IMAGE_H * Y) + X );
      if (pos == WALL) {
        return i - 1;           //return the possible movement.
      }
      if (checkScores(pos))     //if the player wins, I need to stop this procedure.
        return 99;              // 99 is only a number to check the winning situation from the caller scope.
    }
  }
  return x;                     //I can move without restrictions.
}

/*
   Control if the marble can move right. The marble can't move if there's walls.
   params:
   - x : x new position;

   return : how much the marble can move right.
*/
uint8_t checkRight(uint8_t &x) {
  int8_t i, j, X, Y;
  uint16_t pos;

  for (i = 1; i <= x; i++) {                                    //control how much can I move to the right
    X = poswin[1].x - ANCHOR + i + 3;
    for (j = -2; j < 3; j++) {                                  //control if I find walls upper and lower the marble's center.
      Y = poswin[1].y - ANCHOR + j;
      pos = *(imgPtr + ( IMAGE_H * Y) + X );
      if (pos == WALL) {
        return i - 1;         //return the possible movement.
      }
      if (checkScores(pos))   //if the player wins, I need to stop this procedure.
        return 99;            // 99 is only a number to check the winning situation from the caller scope.
    }
  }
  return x;                   //I can move without restrictions.
}

/*
   Control if the marble can move up. The marble can't move if there's walls.
   params:
   - y : y new position;

   return : how much the marble can move up.
*/
uint8_t checkUp(uint8_t &y) {
  int8_t i, j, X, Y;
  uint16_t pos;

  for (i = 1; i <= y; i++) {                                //control how much can I move up
    // Y = ballY - ANCHOR - i - 3;
    Y = poswin[1].y - ANCHOR - i - 3;
    for (j = -2; j < 3; j++) {                            //control if I find walls upper and lower the marble's center.
      X = poswin[1].x - ANCHOR + j;
      if (X < 0 || X > 105 || Y < 0 || Y > 105) {
        return 0;
      } else {
        pos = *(imgPtr + ( IMAGE_H * Y) + X );
        if (pos == WALL) {
          return i - 1;           //return the possible movement.
        }
        if (checkScores(pos))     //if the player wins, I need to stop this procedure.
          return 99;
      }
    }
  }
  return y;                       //I can move without restrictions.
}

/*
   Control if the marble can move down. The marble can't move if there's walls.
   params:
   - y : y new position;

   return : how much the marble can move down.
*/
uint8_t checkDown(uint8_t &y) {
  int8_t i, j, X, Y;
  uint16_t pos;

  for (i = 1; i <= y; i++) {                      //control how much can I move to the bottom
    Y = poswin[1].y - ANCHOR + i + 3;
    for (j = -2; j < 3; j++) {                    //control if I find walls upper and lower the marble's center.
      X = poswin[1].x - ANCHOR + j;
      if (X < 0 || X > 105 || Y < 0 || Y > 105) {
        return 0;
      } else {
        pos = *(imgPtr + ( IMAGE_H * Y ) + X );
        if (pos == WALL) {
          return i - 1;           //return the possible movement.
        }
        if (checkScores(pos))     //if the player wins, I need to stop this procedure.
          return 99;              // 99 is only a number to check the winning situation from the caller scope.
      }
    }
  }
  return y;                       //I can move without restrictions.
}

/*
   Set the marble's position to the last checkPoint;
   Delete the marble on its old position on the screen;
   Draw the marble on its new position on the screen.
*/
void backToCheckout() {
  uint8_t i = 0;
  while (checkpoints.visited[i] == 1 && i <= 9) {
    i++;
  }  
  drawLabirynthArea(poswin[1].x, poswin[1].y);

  poswin[0].x = poswin[1].x;
  poswin[0].y = poswin[1].y;
  poswin[1].x = checkpoints.x[i];
  poswin[1].y = checkpoints.y[i];
  setBall(poswin);
}

/*
   Check if the marble's position correspond to a hole.

*/
void checkHoles() {
  uint16_t pos;
  pos = *(imgPtr + ( IMAGE_H * (poswin[1].y - 10)) + poswin[1].x - 10 );

  if (pos == HOLE) {          //the marble's center position touches the hole
    life--;
    drawlife(life);
    if (life == 0) {         //you loose
      looseGame();
      looseSound();
    } else {                  //back to the last checkPoint
      holeSound();
      backToCheckout();
    }
  }
}

/*
   manage marble's movement.
   params :
   - accwin : struct cointaining the accelerometer data
*/
void manageMovement(AccWin &accwin) {

  bool dirX, dirY; //indicate the direction I'm looking for.
  uint8_t x, y, moveX, moveY, pos;

  poswin[0].x = poswin[1].x;
  poswin[0].y = poswin[1].y;
  moveX = 0;
  moveY = 0;

  //control and map the movement directions and how much I'm moving on x and y axis

  if (accwin[1].x < 50) {             //left movement
    x = constrain(accwin[1].x, MINMAPLOW, MAXMAPLOW);
    x = map(x, MINMAPLOW, MAXMAPLOW, MAXMOV, MINMOV);
    dirX = 0;
    if (poswin[1].x - x < 13)
      poswin[1].x = 13;
    else {
      if (x >= 1) {
        moveX = checkLeft(x);
        if (moveX == 99)
          return;
        poswin[1].x = poswin[1].x - moveX;
      }
    }
  } else {                           //right movement
    x = constrain(accwin[1].x, MINMAPHIGH, MAXMAPHIGH);
    x = map(x, MINMAPHIGH, MAXMAPHIGH, MINMOV, MAXMOV);
    dirX = 1;

    if (poswin[1].x + x > 112)
      poswin[1].x = 112;
    else {
      if (x >= 1) {
        moveX = checkRight(x);
        if (moveX == 99)
          return;
        poswin[1].x = poswin[1].x + moveX;
      }
    }
  }


  if (accwin[1].y < 50) {              //down movement
    y = constrain(accwin[1].y, MINMAPLOW, MAXMAPLOW);
    y = map(y, MAXMAPLOW, MINMAPLOW, MINMOV, MAXMOV);
    dirY = 0;
    if (poswin[1].y + y > 111)
      poswin[1].y = 111;
    else {
      if (y >= 1) {
        moveY = checkDown(y);
        if (moveY == 99)
          return;
        poswin[1].y = poswin[1].y + moveY;
      }
    }
  } else {                       //up movement
    y = constrain(accwin[1].y, MINMAPHIGH, MAXMAPHIGH);
    y = map(y, MINMAPHIGH, MAXMAPHIGH, MINMOV, MAXMOV);
    dirY = 1;
    if (poswin[1].y - y < 13)
      poswin[1].y = 13;
    else {
      if (y >= 1) {
        moveY = checkUp(y);
        if (moveY == 99)
          return;
        poswin[1].y = poswin[1].y - moveY;
      }
    }
  }


  //if I'm moving
  if (poswin[1].x != poswin[0].x || poswin[1].y != poswin[0].y) {

    drawBlackBall(poswin);      //delete the old ball on the screen

    restoreBackground(poswin[1].x, poswin[1].y , moveX, moveY, dirX, dirY); //refresh the background around the new ball

    setBall(poswin);    //draw the new ball

    checkHoles();     //check for holes
  }
}
