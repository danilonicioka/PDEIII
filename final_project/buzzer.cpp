#include "buzzer.h"
#include "global.h"
#include <Arduino.h>

void alarm(int buzzer, int risk){
  int tempo = 600;
  if(risk == INTOXICATION)   tempo = 800;
  else if(risk == EXPLOSION) tempo = 400;            
  for(int i=INIT_FREQ; i<=FINAL_FREQ; i=i+STEP){
    tone(buzzer,i);
    delay(tempo);
  }
  for(int i=FINAL_FREQ; i>=INIT_FREQ; i=i-STEP){
    tone(buzzer,i);
    delay(tempo);
  }
}
