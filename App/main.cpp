#include "mbed.h"
#include "LNDC01_PERIPHERIAL.h"


int main(void)
{
  Init_pins();
  for (;;)
  {
    Set_ESC_LED_state(1,1);
    wait(0.1);
    Set_ESC_LED_state(1,0);
    wait(0.1);
  }
}

