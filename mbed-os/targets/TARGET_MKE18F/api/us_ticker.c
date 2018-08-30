/* mbed Microcontroller Library
 * Copyright (c) 2006-2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stddef.h>
#include "us_ticker_api.h"
#include "PeripheralNames.h"
#include "fsl_lpit.h"
#include "fsl_clock_config.h"

/*-----------------------------------------------------------------------------------------------------
  Пролучаем структуру с частотой следования тиков таймера и его разрядностью 
  
  \param   
  
  \return const ticker_info_t* 
-----------------------------------------------------------------------------------------------------*/
const ticker_info_t* us_ticker_get_info(void)
{
  static const ticker_info_t info = {
    1000000,    // 1 MHz
    32          // 32 bit counter
  };
  return &info;
}

static bool us_ticker_inited = false;

/*-----------------------------------------------------------------------------------------------------
  
  
  \param void  
-----------------------------------------------------------------------------------------------------*/
static void pit_isr(void)
{
  LPIT_ClearStatusFlags(LPIT0, LPIT_MSR_TIF3_MASK);
  LPIT_ClearStatusFlags(LPIT0, LPIT_MSR_TIF2_MASK);
  LPIT_StopTimer(LPIT0, kLPIT_Chnl_2);
  LPIT_StopTimer(LPIT0, kLPIT_Chnl_3);

  us_ticker_irq_handler();
}

/*-----------------------------------------------------------------------------------------------------
  
  
  \param   
-----------------------------------------------------------------------------------------------------*/
void us_ticker_init(void)
{
  uint32_t           busClock;
  lpit_config_t      pitConfig;
  lpit_chnl_params_t chnlSetup;

  LPIT_GetDefaultConfig(&pitConfig);
  LPIT_Init(LPIT0,&pitConfig);

  busClock = CLOCK_GetFreq(kCLOCK_BusClk);

  // Повторно не инициализируем таймер, он должен считать без перебоев с момента старта системы 
  if (!us_ticker_inited)
  {

    LPIT_SetTimerPeriod(LPIT0, kLPIT_Chnl_0, busClock / 1000000 - 1);
    LPIT_SetTimerPeriod(LPIT0, kLPIT_Chnl_1, 0xFFFFFFFF);

    //LPIT_SetTimerChainMode(LPIT0, kLPIT_Chnl_1, true);
    chnlSetup.chainChannel          = true;
    chnlSetup.timerMode             = kLPIT_PeriodicCounter;
    chnlSetup.triggerSelect         = kLPIT_Trigger_TimerChn0;
    chnlSetup.triggerSource         = kLPIT_TriggerSource_Internal;
    chnlSetup.enableReloadOnTrigger = false;
    chnlSetup.enableStopOnTimeout   = false;
    chnlSetup.enableStartOnTrigger  = false;
    LPIT_SetupChannel(LPIT0, kLPIT_Chnl_1,&chnlSetup);

    LPIT_StartTimer(LPIT0, kLPIT_Chnl_0);
    LPIT_StartTimer(LPIT0, kLPIT_Chnl_1);
  }

  /* Configure interrupt generation counters and disable ticker interrupts. */
  LPIT_StopTimer(LPIT0, kLPIT_Chnl_3);
  LPIT_StopTimer(LPIT0, kLPIT_Chnl_2);
  LPIT_SetTimerPeriod(LPIT0, kLPIT_Chnl_2, busClock / 1000000 - 1);

  //LPIT_SetTimerChainMode(LPIT0, kLPIT_Chnl_3, true);
  chnlSetup.chainChannel          = true;
  chnlSetup.timerMode             = kLPIT_PeriodicCounter;
  chnlSetup.triggerSelect         = kLPIT_Trigger_TimerChn0;
  chnlSetup.triggerSource         = kLPIT_TriggerSource_Internal;
  chnlSetup.enableReloadOnTrigger = false;
  chnlSetup.enableStopOnTimeout   = false;
  chnlSetup.enableStartOnTrigger  = false;
  LPIT_SetupChannel(LPIT0, kLPIT_Chnl_3,&chnlSetup);

  NVIC_SetVector(LPIT0_Ch3_IRQn, (uint32_t) pit_isr);
  NVIC_EnableIRQ(LPIT0_Ch3_IRQn);
  LPIT_DisableInterrupts(LPIT0, kLPIT_Channel3TimerInterruptEnable);

  us_ticker_inited = true;
}

/*-----------------------------------------------------------------------------------------------------
  
  
  \param void  
  
  \return uint32_t 
-----------------------------------------------------------------------------------------------------*/
uint32_t us_ticker_read(void)
{
  return ~(LPIT_GetCurrentTimerCount(LPIT0, kLPIT_Chnl_1));
}

/*-----------------------------------------------------------------------------------------------------
  
  
  \param void  
-----------------------------------------------------------------------------------------------------*/
void us_ticker_disable_interrupt(void)
{
  LPIT_DisableInterrupts(LPIT0, kLPIT_Channel3TimerInterruptEnable);
}

/*-----------------------------------------------------------------------------------------------------
  
  
  \param void  
-----------------------------------------------------------------------------------------------------*/
void us_ticker_clear_interrupt(void)
{
  LPIT_ClearStatusFlags(LPIT0, LPIT_MSR_TIF3_MASK);
}

/*-----------------------------------------------------------------------------------------------------
  Устанавливает время в микросекундах когда должно возникнуть прерывание
  
 
  \param timestamp  - абсолютное время системы в микросекундах (в общем случае - тиках)
-----------------------------------------------------------------------------------------------------*/
void us_ticker_set_interrupt(timestamp_t timestamp)
{
  const uint32_t now_ticks = us_ticker_read(); // Читаем текущее время 
  uint32_t       delta_ticks;

  // Вычисляем сколько тиков надо отсчитать чтобы достигнуть заданного в переменной timestamp времени 
  if (timestamp >= now_ticks)
  {
    delta_ticks = timestamp - now_ticks;
  }
  else
  {
    delta_ticks = (uint32_t)((uint64_t) timestamp + 0xFFFFFFFF - now_ticks);
  }

  if (delta_ticks == 0) delta_ticks = 1; /* The requested delay is less than the minimum resolution of this counter. */
  
  LPIT_StopTimer(LPIT0, kLPIT_Chnl_3);
  LPIT_StopTimer(LPIT0, kLPIT_Chnl_2);
  LPIT_SetTimerPeriod(LPIT0, kLPIT_Chnl_3, delta_ticks);
  LPIT_EnableInterrupts(LPIT0, kLPIT_Channel3TimerInterruptEnable);
  LPIT_StartTimer(LPIT0, kLPIT_Chnl_3);
  LPIT_StartTimer(LPIT0, kLPIT_Chnl_2);
}

/*-----------------------------------------------------------------------------------------------------
  
  
  \param void  
-----------------------------------------------------------------------------------------------------*/
void us_ticker_fire_interrupt(void)
{
  NVIC_SetPendingIRQ(LPIT0_Ch3_IRQn);
}
