/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
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

#include "PeripheralPins.h"

/************RTC***************/
const PinMap PinMap_RTC[] = {
    {NC, OSC32KCLK, 0},
};


/************UART***************/
const PinMap PinMap_UART_TX[] = {
    {PTA10, UART0, 3},
    {PTA3 , UART0, 6},
    {PTB1 , UART0, 2},

    {PTC7,  UART1, 2},
    {PTC9 , UART1, 2},

    {PTD7 , UART2, 2},
    {PTE12, UART2, 3},


    {NC  ,  NC    , 0}
};

const PinMap PinMap_UART_RX[] = {
    {PTA11, UART0, 3},
    {PTA2 , UART0, 6},
    {PTB0 , UART0, 2},

    {PTC6 , UART1, 2},
    {PTC8 , UART1, 2},

    {PTD17, UART2, 3},
    {PTD6 , UART2, 2},

    {NC  ,  NC    , 0}
};

const PinMap PinMap_UART_CTS[] = {
    {PTA0 , UART0, 6},
    {PTC8 , UART0, 6},

    {PTA6 , UART1, 6},
    {PTE2 , UART1, 6},

    {PTD11, UART2, 6},
    {PTE9 , UART2, 3},

    {NC   , NC    , 0}
};

const PinMap PinMap_UART_RTS[] = {
    {PTA1 , UART0, 6},
    {PTC9,  UART0, 6},

    {PTA7 , UART1, 6},
    {PTE6 , UART1, 6},

    {PTD12, UART2, 6},
    {PTE3 , UART2, 3},

    {NC   , NC    , 0}
};


