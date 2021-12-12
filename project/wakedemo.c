#include <msp430.h>
#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include "buzzer.h"
#include "tune.h"

// WARNING: LCD DISPLAY USES P1.0.  Do not touch!!! 

#define LED BIT6		/* note that bit zero req'd for display */

#define SW1 1
#define SW2 2
#define SW3 4
#define SW4 8

#define SWITCHES 15

static char 
switch_update_interrupt_sense()
{
  char p2val = P2IN;
  /* update switch interrupt to detect changes from current buttons */
  P2IES |= (p2val & SWITCHES);	/* if switch up, sense down */
  P2IES &= (p2val | ~SWITCHES);	/* if switch down, sense up */
  return p2val;
}

void 
switch_init()			/* setup switch */
{  
  P2REN |= SWITCHES;		/* enables resistors for switches */
  P2IE |= SWITCHES;		/* enable interrupts from switches */
  P2OUT |= SWITCHES;		/* pull-ups for switches */
  P2DIR &= ~SWITCHES;		/* set switches' bits for input */
  switch_update_interrupt_sense();
}

int switches = 0;

void
switch_interrupt_handler()
{
  char p2val = switch_update_interrupt_sense();
  switches = ~p2val & SWITCHES;
}


// axis zero for col, axis 1 for row
short drawPos[2] = {10,10}, controlPos[2] = {10,10};
short velocity[2] = {3,8}, limits[2] = {screenWidth-36, screenHeight-8};

short redrawScreen = 1;
u_int controlFontColor = COLOR_GREEN;
int tuneState = 1;
int tuneCounter = 0;

void wdt_c_handler()
{
  static int secCount = 0;

  tuneCounter++;
  if (tuneCounter >= 250) {
    tuneState = (tuneState + 1) % 4;
    tuneCounter = 0;
  }

  secCount ++;
  if (secCount >= 25) {		/* 10/sec */
    secCount = 0;
    redrawScreen = 1;
  }
}
  
void update_shape();

void main()
{
  
  P1DIR |= LED;		/**< Green led on when CPU on */
  P1OUT |= LED;
  configureClocks();
  lcd_init();
  switch_init();
  buzzer_init();
  
  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */
  
  clearScreen(COLOR_BLUE);
  while (1) {			/* forever */
    if (redrawScreen) {
      redrawScreen = 0;
      update_shape();
    }
    P1OUT &= ~LED;	/* led off */
    or_sr(0x10);	/**< CPU OFF */
    P1OUT |= LED;	/* led on */
  }
}

    
    
void
update_shape()
{
  static unsigned char row = screenHeight / 2, col = screenWidth / 2;
  static char blue = 31, green = 0, red = 31;
  static int step = 0;
  static int movingTop = 0;
  
  if (switches & SW4){
    return; // skips the rest if button 4 is pressed
  }

  drawString8x12(20, 20, "demo", COLOR_ORANGE, COLOR_BLUE);
  
  // a color in this BGR encoding is BBBB BGGG GGGR RRRR
  unsigned int color = (blue << 11) | (green << 5) | red;
  blue++; blue &= 31;
  green += 2; green &= 63;

  if (movingTop) {
    step--;
    drawStar(col, row + step, color);

  } else {
    step++;
    drawStar(col, row + step, color);
  }
    
  if (switches & SW3){
    green = (green + 1) % 64;
    playTune(tuneState);
  }
  if (switches & SW2) blue = (blue + 2) % 32;
  if (switches & SW1) red = (red - 3) % 32;

  clearScreen(COLOR_BLUE);

  if (row + step <= 25) movingTop = 0;
  if (row + step >= screenHeight - 25) movingTop = 1;
    
  
}


/* Switch on S2 */
void
__interrupt_vec(PORT2_VECTOR) Port_2(){
  if (P2IFG & SWITCHES) {	      /* did a button cause this interrupt? */
    P2IFG &= ~SWITCHES;		      /* clear pending sw interrupts */
    switch_interrupt_handler();	/* single handler for all switches */
  }
}
