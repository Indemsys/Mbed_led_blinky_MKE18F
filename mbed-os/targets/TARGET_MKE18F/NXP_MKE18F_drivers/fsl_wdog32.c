/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_wdog32.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.wdog32"
#endif


/*******************************************************************************
 * Code
 ******************************************************************************/

void WDOG32_ClearStatusFlags(WDOG_Type *base, uint32_t mask)
{
    if (mask & kWDOG32_InterruptFlag)
    {
        base->CS |= WDOG_CS_FLG_MASK;
    }
}

void WDOG32_GetDefaultConfig(wdog32_config_t *config)
{
    assert(config);

    config->enableWdog32 = true;
    config->clockSource = kWDOG32_ClockSource1;
    config->prescaler = kWDOG32_ClockPrescalerDivide1;
    config->workMode.enableWait = true;
    config->workMode.enableStop = false;
    config->workMode.enableDebug = false;
    config->testMode = kWDOG32_TestModeDisabled;
    config->enableUpdate = true;
    config->enableInterrupt = false;
    config->enableWindowMode = false;
    config->windowValue = 0U;
    config->timeoutValue = 0xFFFFU;
}

void WDOG32_Init(WDOG_Type *base, const wdog32_config_t *config)
{
    assert(config);

    register uint32_t value = 0U;
    uint32_t primaskValue = 0U;
    register WDOG_Type *regBase = base;
    register const wdog32_config_t *regConfig = config;

    value = WDOG_CS_EN(regConfig->enableWdog32) | WDOG_CS_CLK(regConfig->clockSource) | WDOG_CS_INT(regConfig->enableInterrupt) |
            WDOG_CS_WIN(regConfig->enableWindowMode) | WDOG_CS_UPDATE(regConfig->enableUpdate) |
            WDOG_CS_DBG(regConfig->workMode.enableDebug) | WDOG_CS_STOP(regConfig->workMode.enableStop) |
            WDOG_CS_WAIT(regConfig->workMode.enableWait) | WDOG_CS_PRES(regConfig->prescaler) | WDOG_CS_CMD32EN(true) |
            WDOG_CS_TST(regConfig->testMode);

    /* Disable the global interrupts. Otherwise, an interrupt could effectively invalidate the unlock sequence
     * and the WCT may expire. After the configuration finishes, re-enable the global interrupts. */
    primaskValue = DisableGlobalIRQ();
    if ((regBase->CS) & WDOG_CS_CMD32EN_MASK)
    {
        regBase->CNT = WDOG_UPDATE_KEY;
    }
    else
    {
        regBase->CNT = WDOG_FIRST_WORD_OF_UNLOCK;
        regBase->CNT = WDOG_SECOND_WORD_OF_UNLOCK;
    }
    regBase->WIN = regConfig->windowValue;
    regBase->TOVAL = regConfig->timeoutValue;
    regBase->CS = value;
    EnableGlobalIRQ(primaskValue);
}

void WDOG32_Deinit(WDOG_Type *base)
{
    uint32_t primaskValue = 0U;

    /* Disable the global interrupts */
    primaskValue = DisableGlobalIRQ();
    WDOG32_Unlock(base);
    WDOG32_Disable(base);
    EnableGlobalIRQ(primaskValue);
}
