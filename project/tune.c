#include <msp430.h>
#include "buzzer.h"

void playTune(int state) {
  switch(state) {
  case 0:
    buzzer_set_period(500);
    break;
  case 1:
    buzzer_set_period(1000);
    break;
  case 2:
    buzzer_set_period(1500);
    break;
  case 3:
    buzzer_set_period(2000);
    break;
  default:
    break;
  }
}
