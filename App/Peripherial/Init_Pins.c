#include "LNDC01_PERIPHERIAL.h"

void Config_pin(const T_IO_pins_configuration pinc);

#include "LNDC01_Pins.h"
/*------------------------------------------------------------------------------

 ------------------------------------------------------------------------------*/
void Config_pin(const T_IO_pins_configuration pinc)
{
  pinc.port->PCR[pinc.pin_num] = LSHIFT(pinc.irqc, 16) |  
                                 LSHIFT(pinc.lock, 15) |
                                 LSHIFT(pinc.mux, 8) |
                                 LSHIFT(pinc.DSE, 6) |
                                 LSHIFT(pinc.ODE, 5) |
                                 LSHIFT(pinc.PFE, 4) |
                                 LSHIFT(pinc.PUPD, 0);

  if (pinc.init == 0) pinc.gpio->PCOR = LSHIFT(1, pinc.pin_num);
  else pinc.gpio->PSOR = LSHIFT(1, pinc.pin_num);
  pinc.gpio->PDDR = (pinc.gpio->PDDR & ~LSHIFT(1, pinc.pin_num)) | LSHIFT(pinc.dir, pinc.pin_num);
}


/*------------------------------------------------------------------------------

 \return int
 ------------------------------------------------------------------------------*/
int Init_pins(void)
{
  int i;

  // Включаем тактирование на всех портах
  PCC->CLKCFG[PCC_PORTA_INDEX] = BIT(30);  
  PCC->CLKCFG[PCC_PORTB_INDEX] = BIT(30);  
  PCC->CLKCFG[PCC_PORTC_INDEX] = BIT(30);  
  PCC->CLKCFG[PCC_PORTD_INDEX] = BIT(30);  
  PCC->CLKCFG[PCC_PORTE_INDEX] = BIT(30);  

  for (i = 0; i < (sizeof(LNDC01_pins_conf) / sizeof(LNDC01_pins_conf[0])); i++)
  {
    Config_pin(LNDC01_pins_conf[i]);
  }

  return 0;
}

#define ESC1_LED_PORT  GPIOD
#define ESC1_LED_PIN   2
#define ESC2_LED_PORT  GPIOD
#define ESC2_LED_PIN   3
#define ESC3_LED_PORT  GPIOE
#define ESC3_LED_PIN   11
#define ESC4_LED_PORT  GPIOE
#define ESC4_LED_PIN   3


void Set_ESC_LED_state(uint32_t num, uint32_t state)
{
  if (state == 0)
  {
    switch (num)
    {
    case 1:
      ESC1_LED_PORT->PCOR = LSHIFT(1, ESC1_LED_PIN); // Устанавливаем бит в Port Clear Output Register
      break;
    case 2:
      ESC2_LED_PORT->PCOR = LSHIFT(1, ESC2_LED_PIN); // Устанавливаем бит в Port Clear Output Register
      break;
    case 3:
      ESC3_LED_PORT->PCOR = LSHIFT(1, ESC3_LED_PIN); // Устанавливаем бит в Port Clear Output Register
      break;
    case 4:
      ESC4_LED_PORT->PCOR = LSHIFT(1, ESC4_LED_PIN); // Устанавливаем бит в Port Clear Output Register
      break;
    }
  }
  else
  {
    switch (num)
    {
    case 1:
      ESC1_LED_PORT->PSOR = LSHIFT(1, ESC1_LED_PIN); // Устанавливаем бит в Port Set Output Register
      break;
    case 2:
      ESC2_LED_PORT->PSOR = LSHIFT(1, ESC2_LED_PIN); // Устанавливаем бит в Port Set Output Register
      break;
    case 3:
      ESC3_LED_PORT->PSOR = LSHIFT(1, ESC3_LED_PIN); // Устанавливаем бит в Port Set Output Register
      break;
    case 4:
      ESC4_LED_PORT->PSOR = LSHIFT(1, ESC4_LED_PIN); // Устанавливаем бит в Port Set Output Register
      break;
    }
  }

}
