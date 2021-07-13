#define BUTTON_S1 33

volatile uint8_t mode = 0;  //0 = initialPage , 1 = game , 2 = pause
long debouncing_time = 15;
volatile unsigned long last_micros;

/*
 * called by the button's interrupt to control the game pages.
 */
void go() {
  
  if ((long)(micros() - last_micros) >= debouncing_time * 1000) {
    
    pushButtonSound();
    
    if (loose == 0 && win == 0) {
      
      if (mode == 0) { //initial page
        mode = 1; //gamepage
        screen.clear(blackColour);
        drawLabyrinth();
        drawStartGame(scores);
        setBall(poswin);

      } else if (mode == 1) { //gamepage
        
        mode = 2; //pause
        drawPause();

      } else if (mode == 2) { //pause
        
        mode = 1;             //gamepage
        screen.clear(blackColour);
        drawLabyrinth();
        drawStartGame(scores);
        setBall(poswin);
      }

    } else {  //loose or win
      
      resetGame();
      mode = 1; //gamepage
      screen.clear(blackColour);
      drawLabyrinth();
      drawStartGame(scores);
      setBall(poswin);
      
    }    
    last_micros = micros();
  }
}

/*
 * initialize the button S1
 */
void _initButtonS1() {
  pinMode(BUTTON_S1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_S1), go, LOW);
}
