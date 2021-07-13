#define ACC_X 23
#define ACC_Y 24
#define MINMAP_ACC 0
#define MAXMAP_ACC 100

/*
 * get acceleration data and save it in accX and accY.
 */
void getAccelData(){
  accwin[0].x = accwin[1].x;
  accwin[0].y = accwin[1].y;
  accwin[1].x = map(analogRead(ACC_X), 1250,2883,MINMAP_ACC,MAXMAP_ACC);
  accwin[1].y = map(analogRead(ACC_Y), 1250,2883,MINMAP_ACC,MAXMAP_ACC);   
}

/*
 * accelerometer initialization.
 */
void _accelInit(AccWin &accwin){
  accwin[0].x = 50;
  accwin[0].y = 50;
  accwin[1].x = 50;
  accwin[1].y = 50;
  
  analogRead(ACC_X);
  analogRead(ACC_Y);  
  analogRead(ACC_X);
  analogRead(ACC_Y);
}
