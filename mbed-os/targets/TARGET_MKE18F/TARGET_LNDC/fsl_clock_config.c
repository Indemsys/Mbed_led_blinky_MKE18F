/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/
/*
 * How to setup clock using clock driver functions:
 *
 * 1. Call CLOCK_InitXXX() to configure corresponding SCG clock source.
 *    Note: The clock could not be set when it is being used as system clock.
 *    In default out of reset, the CPU is clocked from FIRC(IRC48M),
 *    so before setting FIRC, change to use another avaliable clock source.
 *
 * 2. Call CLOCK_SetXtal0Freq() to set XTAL0 frequency based on board settings.
 *
 * 3. Call CLOCK_SetXxxModeSysClkConfig() to set SCG mode for Xxx run mode.
 *    Wait until the system clock source is changed to target source.
 *
 * 4. If power mode change is needed, call SMC_SetPowerModeProtection() to allow
 *    corresponding power mode and SMC_SetPowerModeXxx() to change to Xxx mode.
 *    Supported run mode and clock restrictions could be found in Reference Manual.
 */

/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Clocks v4.1
processor: MKE18F512xxx16
package_id: MKE18F512VLL16
mcu_data: ksdk2_0
processor_version: 4.0.0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

#include "fsl_smc.h"
#include "fsl_clock_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SCG_CLKOUTCNFG_SPLL                               6U  /*!< SCG CLKOUT clock select: System PLL */
#define SCG_SOSC_DISABLE                                  0U  /*!< System OSC disabled */
#define SIM_CLKOUT_DIV_BY8                                3U  /*!< CLKOUT divider ratio: Divided by 8 */
#define SIM_CLKOUT_SEL_SCGCLKOUT                          1U  /*!< CLKOUT pin clock select: SCGCLKOUT, see SCG_CLKOUTCNFG register */

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* System clock frequency. */
extern uint32_t SystemCoreClock;

/*******************************************************************************
 * Code
 ******************************************************************************/
/*FUNCTION**********************************************************************
 *
 * Function Name : CLOCK_CONFIG_SetScgOutSel
 * Description   : Set the SCG clock out select (CLKOUTSEL).
 * Param setting : The selected clock source.
 *
 *END**************************************************************************/
static void CLOCK_CONFIG_SetScgOutSel(uint8_t setting)
{
     SCG->CLKOUTCNFG = SCG_CLKOUTCNFG_CLKOUTSEL(setting);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : CLOCK_CONFIG_SetClkOutClock
 * Description   : Selects the clock to output on the CLKOUT pin.
 * Param src     : The selected clock source.
 * Param div     : CLKOUT divider ratio.
 *
 *END**************************************************************************/
static void CLOCK_CONFIG_SetClkOutClock(uint8_t src, uint8_t div)
{
     uint32_t temp;
     temp = SIM->CHIPCTL;
     /* Selects the clock to output on the CLKOUT pin. */
     temp &= ~SIM_CHIPCTL_CLKOUTSEL_MASK;
     temp |= SIM_CHIPCTL_CLKOUTSEL(src);
     /* Set CLKOUT divider ratio. */
     temp &= ~SIM_CHIPCTL_CLKOUTDIV_MASK;
     temp |= SIM_CHIPCTL_CLKOUTDIV(div);
     SIM->CHIPCTL = temp;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : CLOCK_CONFIG_FircSafeConfig
 * Description   : This function is used to safely configure FIRC clock.
 *                 In default out of reset, the CPU is clocked from FIRC(IRC48M).
 *                 Before setting FIRC, change to use SIRC as system clock,
 *                 then configure FIRC. After FIRC is set, change back to use FIRC
 *                 in case SIRC need to be configured.
 * Param fircConfig  : FIRC configuration.
 *
 *END**************************************************************************/
static void CLOCK_CONFIG_FircSafeConfig(const scg_firc_config_t *fircConfig)
{
    scg_sys_clk_config_t curConfig;
    const scg_sirc_config_t scgSircConfig = {.enableMode = kSCG_SircEnable,
                                             .div1 = kSCG_AsyncClkDisable,
                                             .div2 = kSCG_AsyncClkDivBy2,
                                             .range = kSCG_SircRangeHigh};
    scg_sys_clk_config_t sysClkSafeConfigSource = {
         .divSlow = kSCG_SysClkDivBy4, /* Slow clock divider */
         .divBus = kSCG_SysClkDivBy1,  /* Bus clock divider */
         .divCore = kSCG_SysClkDivBy1, /* Core clock divider */
         .src = kSCG_SysClkSrcSirc     /* System clock source */
    };
    /* Init Sirc. */
    CLOCK_InitSirc(&scgSircConfig);
    /* Change to use SIRC as system clock source to prepare to change FIRCCFG register. */
    CLOCK_SetRunModeSysClkConfig(&sysClkSafeConfigSource);
    /* Wait for clock source switch finished. */
    do
    {
         CLOCK_GetCurSysClkConfig(&curConfig);
    } while (curConfig.src != sysClkSafeConfigSource.src);

    /* Init Firc. */
    CLOCK_InitFirc(fircConfig);
    /* Change back to use FIRC as system clock source in order to configure SIRC if needed. */
    sysClkSafeConfigSource.src = kSCG_SysClkSrcFirc;
    CLOCK_SetRunModeSysClkConfig(&sysClkSafeConfigSource);
    /* Wait for clock source switch finished. */
    do
    {
         CLOCK_GetCurSysClkConfig(&curConfig);
    } while (curConfig.src != sysClkSafeConfigSource.src);
}

/*******************************************************************************
 ************************ BOARD_InitBootClocks function ************************
 ******************************************************************************/
void BOARD_InitBootClocks(void)
{
    BOARD_BootClockHSRUN();
}

/*******************************************************************************
 ********************* Configuration BOARD_BootClockVLPR ***********************
 ******************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!Configuration
name: BOARD_BootClockVLPR
outputs:
- {id: Bus_clock.outFreq, value: 2 MHz}
- {id: Core_clock.outFreq, value: 4 MHz}
- {id: FIRCDIV1_CLK.outFreq, value: 48 MHz}
- {id: FIRCDIV2_CLK.outFreq, value: 48 MHz}
- {id: Flash_clock.outFreq, value: 500 kHz}
- {id: LPO1KCLK.outFreq, value: 1 kHz}
- {id: LPO_clock.outFreq, value: 128 kHz}
- {id: SIRCDIV1_CLK.outFreq, value: 8 MHz}
- {id: SIRCDIV2_CLK.outFreq, value: 4 MHz}
- {id: SIRC_CLK.outFreq, value: 8 MHz}
- {id: System_clock.outFreq, value: 4 MHz}
settings:
- {id: SCGMode, value: SIRC}
- {id: powerMode, value: VLPR}
- {id: SCG.DIVBUS.scale, value: '2', locked: true}
- {id: SCG.DIVCORE.scale, value: '2', locked: true}
- {id: SCG.DIVSLOW.scale, value: '8', locked: true}
- {id: SCG.FIRCDIV1.scale, value: '1', locked: true}
- {id: SCG.FIRCDIV2.scale, value: '1', locked: true}
- {id: SCG.SCSSEL.sel, value: SCG.SIRC}
- {id: SCG.SIRCDIV1.scale, value: '1', locked: true}
- {id: SCG.SIRCDIV2.scale, value: '2', locked: true}
- {id: SCG.SPLLDIV1.scale, value: '1', locked: true}
- {id: SCG.SPLLDIV2.scale, value: '1', locked: true}
- {id: SCG.SPLLSRCSEL.sel, value: SCG.FIRC}
- {id: 'SCG::RCCR[DIVBUS].bitField', value: BitFieldValue}
- {id: 'SCG::RCCR[DIVCORE].bitField', value: BitFieldValue}
- {id: 'SCG::RCCR[DIVSLOW].bitField', value: BitFieldValue}
- {id: 'SCG::RCCR[SCS].bitField', value: BitFieldValue}
- {id: SCG_FIRCCSR_FIRCLPEN_CFG, value: Enabled}
- {id: SCG_SPLLCSR_SPLLEN_CFG, value: Enabled}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/*******************************************************************************
 * Variables for BOARD_BootClockVLPR configuration
 ******************************************************************************/
const scg_sys_clk_config_t g_sysClkConfig_BOARD_BootClockVLPR =
    {
        .divSlow = kSCG_SysClkDivBy8,             /* Slow Clock Divider: divided by 8 */
        .divBus = kSCG_SysClkDivBy2,              /* Bus Clock Divider: divided by 2 */
        .divCore = kSCG_SysClkDivBy2,             /* Core Clock Divider: divided by 2 */
        .src = kSCG_SysClkSrcSirc,                /* Slow IRC is selected as System Clock Source */
    };
const scg_sosc_config_t g_scgSysOscConfig_BOARD_BootClockVLPR =
    {
        .freq = 0U,                               /* System Oscillator frequency: 0Hz */
        .enableMode = SCG_SOSC_DISABLE,           /* System OSC disabled */
        .monitorMode = kSCG_SysOscMonitorDisable, /* Monitor disabled */
        .div1 = kSCG_AsyncClkDisable,             /* System OSC Clock Divider 1: Clock output is disabled */
        .div2 = kSCG_AsyncClkDisable,             /* System OSC Clock Divider 2: Clock output is disabled */
        .workMode = kSCG_SysOscModeExt,           /* Use external clock */
    };
const scg_sirc_config_t g_scgSircConfig_BOARD_BootClockVLPR =
    {
        .enableMode = kSCG_SircEnable | kSCG_SircEnableInLowPower,/* Enable SIRC clock, Enable SIRC in low power mode */
        .div1 = kSCG_AsyncClkDivBy1,              /* Slow IRC Clock Divider 1: divided by 1 */
        .div2 = kSCG_AsyncClkDivBy2,              /* Slow IRC Clock Divider 2: divided by 2 */
        .range = kSCG_SircRangeHigh,              /* Slow IRC high range clock (8 MHz) */
    };
const scg_firc_config_t g_scgFircConfig_BOARD_BootClockVLPR =
    {
        .enableMode = kSCG_FircEnable | kSCG_FircEnableInLowPower,/* Enable FIRC clock, Enable FIRC in low power mode */
        .div1 = kSCG_AsyncClkDivBy1,              /* Fast IRC Clock Divider 1: divided by 1 */
        .div2 = kSCG_AsyncClkDivBy1,              /* Fast IRC Clock Divider 2: divided by 1 */
        .range = kSCG_FircRange48M,               /* Fast IRC is trimmed to 48MHz */
        .trimConfig = NULL,                       /* Fast IRC Trim disabled */
    };
const scg_spll_config_t g_scgSysPllConfig_BOARD_BootClockVLPR =
    {
        .enableMode = kSCG_SysPllEnable,          /* Enable SPLL clock */
        .monitorMode = kSCG_SysPllMonitorDisable, /* Monitor disabled */
        .div1 = kSCG_AsyncClkDivBy1,              /* System PLL Clock Divider 1: divided by 1 */
        .div2 = kSCG_AsyncClkDivBy1,              /* System PLL Clock Divider 2: divided by 1 */
        .src = kSCG_SysPllSrcFirc,                /* System PLL clock source is Fast IRC */
        .prediv = 0,                              /* Divided by 1 */
        .mult = 0,                                /* Multiply Factor is 16 */
    };
/*******************************************************************************
 * Code for BOARD_BootClockVLPR configuration
 ******************************************************************************/
void BOARD_BootClockVLPR(void)
{
    /* Init FIRC. */
    CLOCK_CONFIG_FircSafeConfig(&g_scgFircConfig_BOARD_BootClockVLPR);
    /* Init SIRC. */
    CLOCK_InitSirc(&g_scgSircConfig_BOARD_BootClockVLPR);
    /* Set SCG to SIRC mode. */
    CLOCK_SetVlprModeSysClkConfig(&g_sysClkConfig_BOARD_BootClockVLPR);
    /* Allow SMC all power modes. */
    SMC_SetPowerModeProtection(SMC, kSMC_AllowPowerModeAll);
    /* Set VLPR power mode. */
    SMC_SetPowerModeVlpr(SMC);
    while (SMC_GetPowerModeState(SMC) != kSMC_PowerStateVlpr)
    {
    }
    /* Set SystemCoreClock variable. */
    SystemCoreClock = BOARD_BOOTCLOCKVLPR_CORE_CLOCK;
}

/*******************************************************************************
 ********************** Configuration BOARD_BootClockRUN ***********************
 ******************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!Configuration
name: BOARD_BootClockRUN
outputs:
- {id: Bus_clock.outFreq, value: 60 MHz}
- {id: Core_clock.outFreq, value: 120 MHz}
- {id: FIRCDIV1_CLK.outFreq, value: 48 MHz}
- {id: FIRCDIV2_CLK.outFreq, value: 48 MHz}
- {id: Flash_clock.outFreq, value: 24 MHz}
- {id: LPO1KCLK.outFreq, value: 1 kHz}
- {id: LPO_clock.outFreq, value: 128 kHz}
- {id: PLLDIV1_CLK.outFreq, value: 120 MHz}
- {id: PLLDIV2_CLK.outFreq, value: 60 MHz}
- {id: SIRCDIV1_CLK.outFreq, value: 8 MHz}
- {id: SIRCDIV2_CLK.outFreq, value: 4 MHz}
- {id: SIRC_CLK.outFreq, value: 8 MHz}
- {id: SOSC_CLK.outFreq, value: 16 MHz}
- {id: System_clock.outFreq, value: 120 MHz}
settings:
- {id: SCGMode, value: SPLL}
- {id: powerMode, value: HSRUN}
- {id: PCC.PCC_LPUART2_SEL.sel, value: SCG.SOSCDIV2_CLK}
- {id: SCG.DIVBUS.scale, value: '2', locked: true}
- {id: SCG.DIVCORE.scale, value: '1', locked: true}
- {id: SCG.DIVSLOW.scale, value: '5'}
- {id: SCG.FIRCDIV1.scale, value: '1', locked: true}
- {id: SCG.FIRCDIV2.scale, value: '1', locked: true}
- {id: SCG.PREDIV.scale, value: '4', locked: true}
- {id: SCG.SCSSEL.sel, value: SCG.SPLL_DIV2_CLK}
- {id: SCG.SIRCDIV1.scale, value: '1', locked: true}
- {id: SCG.SIRCDIV2.scale, value: '2', locked: true}
- {id: SCG.SPLLDIV1.scale, value: '1', locked: true}
- {id: SCG.SPLLDIV2.scale, value: '2', locked: true}
- {id: SCG.SPLLSRCSEL.sel, value: SCG.FIRC}
- {id: SCG.SPLL_mul.scale, value: '20', locked: true}
- {id: 'SCG::RCCR[DIVBUS].bitField', value: BitFieldValue}
- {id: 'SCG::RCCR[DIVCORE].bitField', value: BitFieldValue}
- {id: 'SCG::RCCR[DIVSLOW].bitField', value: BitFieldValue}
- {id: 'SCG::RCCR[SCS].bitField', value: BitFieldValue}
- {id: SCG_SIRCCSR_SIRCLPEN_CFG, value: Disabled}
- {id: SCG_SOSCCSR_SOSCEN_CFG, value: Enabled}
- {id: SCG_SOSCCSR_SOSCLPEN_CFG, value: Enabled}
- {id: SCG_SPLLCSR_SPLLEN_CFG, value: Enabled}
sources:
- {id: SCG.SOSC.outFreq, value: 16 MHz, enabled: true}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/*******************************************************************************
 * Variables for BOARD_BootClockRUN configuration
 ******************************************************************************/
const scg_sys_clk_config_t g_sysClkConfig_BOARD_BootClockRUN =
    {
        .divSlow = kSCG_SysClkDivBy5,             /* Slow Clock Divider: divided by 5 */
        .divBus = kSCG_SysClkDivBy2,              /* Bus Clock Divider: divided by 2 */
        .divCore = kSCG_SysClkDivBy1,             /* Core Clock Divider: divided by 1 */
        .src = kSCG_SysClkSrcSysPll,              /* System PLL is selected as System Clock Source */
    };
const scg_sosc_config_t g_scgSysOscConfig_BOARD_BootClockRUN =
    {
        .freq = 16000000U,                        /* System Oscillator frequency: 16000000Hz */
        .enableMode = kSCG_SysOscEnable | kSCG_SysOscEnableInLowPower,/* Enable System OSC clock, Enable System OSC in low power mode */
        .monitorMode = kSCG_SysOscMonitorDisable, /* Monitor disabled */
        .div1 = kSCG_AsyncClkDisable,             /* System OSC Clock Divider 1: Clock output is disabled */
        .div2 = kSCG_AsyncClkDisable,             /* System OSC Clock Divider 2: Clock output is disabled */
        .workMode = kSCG_SysOscModeExt,           /* Use external clock */
    };
const scg_sirc_config_t g_scgSircConfig_BOARD_BootClockRUN =
    {
        .enableMode = kSCG_SircEnable,            /* Enable SIRC clock */
        .div1 = kSCG_AsyncClkDivBy1,              /* Slow IRC Clock Divider 1: divided by 1 */
        .div2 = kSCG_AsyncClkDivBy2,              /* Slow IRC Clock Divider 2: divided by 2 */
        .range = kSCG_SircRangeHigh,              /* Slow IRC high range clock (8 MHz) */
    };
const scg_firc_config_t g_scgFircConfig_BOARD_BootClockRUN =
    {
        .enableMode = kSCG_FircEnable,            /* Enable FIRC clock */
        .div1 = kSCG_AsyncClkDivBy1,              /* Fast IRC Clock Divider 1: divided by 1 */
        .div2 = kSCG_AsyncClkDivBy1,              /* Fast IRC Clock Divider 2: divided by 1 */
        .range = kSCG_FircRange48M,               /* Fast IRC is trimmed to 48MHz */
        .trimConfig = NULL,                       /* Fast IRC Trim disabled */
    };
const scg_spll_config_t g_scgSysPllConfig_BOARD_BootClockRUN =
    {
        .enableMode = kSCG_SysPllEnable,          /* Enable SPLL clock */
        .monitorMode = kSCG_SysPllMonitorDisable, /* Monitor disabled */
        .div1 = kSCG_AsyncClkDivBy1,              /* System PLL Clock Divider 1: divided by 1 */
        .div2 = kSCG_AsyncClkDivBy2,              /* System PLL Clock Divider 2: divided by 2 */
        .src = kSCG_SysPllSrcFirc,                /* System PLL clock source is Fast IRC */
        .prediv = 3,                              /* Divided by 4 */
        .mult = 4,                                /* Multiply Factor is 20 */
    };
/*******************************************************************************
 * Code for BOARD_BootClockRUN configuration
 ******************************************************************************/
void BOARD_BootClockRUN(void)
{
    scg_sys_clk_config_t curConfig;

    /* Init SOSC according to board configuration. */
    CLOCK_InitSysOsc(&g_scgSysOscConfig_BOARD_BootClockRUN);
    /* Set the XTAL0 frequency based on board settings. */
    CLOCK_SetXtal0Freq(g_scgSysOscConfig_BOARD_BootClockRUN.freq);
    /* Init FIRC. */
    CLOCK_CONFIG_FircSafeConfig(&g_scgFircConfig_BOARD_BootClockRUN);
    /* Set HSRUN power mode. */
    SMC_SetPowerModeProtection(SMC, kSMC_AllowPowerModeAll);
    SMC_SetPowerModeHsrun(SMC);
    while (SMC_GetPowerModeState(SMC) != kSMC_PowerStateHsrun)
    {
    }

    /* Init SIRC. */
    CLOCK_InitSirc(&g_scgSircConfig_BOARD_BootClockRUN);
    /* Init SysPll. */
    CLOCK_InitSysPll(&g_scgSysPllConfig_BOARD_BootClockRUN);
    /* Set SCG to SPLL mode. */
    CLOCK_SetHsrunModeSysClkConfig(&g_sysClkConfig_BOARD_BootClockRUN);
    /* Wait for clock source switch finished. */
    do
    {
         CLOCK_GetCurSysClkConfig(&curConfig);
    } while (curConfig.src != g_sysClkConfig_BOARD_BootClockRUN.src);
    /* Set SystemCoreClock variable. */
    SystemCoreClock = BOARD_BOOTCLOCKRUN_CORE_CLOCK;
}

/*******************************************************************************
 ********************* Configuration BOARD_BootClockHSRUN **********************
 ******************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!Configuration
name: BOARD_BootClockHSRUN
called_from_default_init: true
outputs:
- {id: Bus_clock.outFreq, value: 84 MHz}
- {id: CLKOUT.outFreq, value: 21 MHz}
- {id: Core_clock.outFreq, value: 168 MHz}
- {id: FIRCDIV1_CLK.outFreq, value: 48 MHz}
- {id: FIRCDIV2_CLK.outFreq, value: 48 MHz}
- {id: Flash_clock.outFreq, value: 24 MHz}
- {id: LPO1KCLK.outFreq, value: 1 kHz}
- {id: LPO_clock.outFreq, value: 128 kHz}
- {id: PCC.PCC_ADC0_CLK.outFreq, value: 84 MHz}
- {id: PCC.PCC_ADC1_CLK.outFreq, value: 84 MHz}
- {id: PCC.PCC_ADC2_CLK.outFreq, value: 84 MHz}
- {id: PCC.PCC_FLEXIO_CLK.outFreq, value: 84 MHz}
- {id: PCC.PCC_FTM0_CLK.outFreq, value: 168 MHz}
- {id: PCC.PCC_FTM1_CLK.outFreq, value: 168 MHz}
- {id: PCC.PCC_FTM2_CLK.outFreq, value: 168 MHz}
- {id: PCC.PCC_FTM3_CLK.outFreq, value: 168 MHz}
- {id: PCC.PCC_LPI2C0_CLK.outFreq, value: 84 MHz}
- {id: PCC.PCC_LPI2C1_CLK.outFreq, value: 84 MHz}
- {id: PCC.PCC_LPIT0_CLK.outFreq, value: 84 MHz}
- {id: PCC.PCC_LPSPI0_CLK.outFreq, value: 84 MHz}
- {id: PCC.PCC_LPSPI1_CLK.outFreq, value: 84 MHz}
- {id: PCC.PCC_LPTMR0_CLK.outFreq, value: 84 MHz}
- {id: PCC.PCC_LPUART0_CLK.outFreq, value: 84 MHz}
- {id: PCC.PCC_LPUART1_CLK.outFreq, value: 84 MHz}
- {id: PCC.PCC_LPUART2_CLK.outFreq, value: 84 MHz}
- {id: PLLDIV1_CLK.outFreq, value: 168 MHz}
- {id: PLLDIV2_CLK.outFreq, value: 84 MHz}
- {id: SIRCDIV1_CLK.outFreq, value: 8 MHz}
- {id: SIRCDIV2_CLK.outFreq, value: 4 MHz}
- {id: SIRC_CLK.outFreq, value: 8 MHz}
- {id: SOSC_CLK.outFreq, value: 16 MHz}
- {id: System_clock.outFreq, value: 168 MHz}
settings:
- {id: SCGMode, value: SPLL}
- {id: powerMode, value: HSRUN}
- {id: CLKOUTConfig, value: 'yes'}
- {id: PCC.PCC_ADC0_SEL.sel, value: SCG.PLLDIV2_CLK}
- {id: PCC.PCC_ADC1_SEL.sel, value: SCG.PLLDIV2_CLK}
- {id: PCC.PCC_ADC2_SEL.sel, value: SCG.PLLDIV2_CLK}
- {id: PCC.PCC_FLEXIO_SEL.sel, value: SCG.PLLDIV2_CLK}
- {id: PCC.PCC_FTM0_SEL.sel, value: SCG.PLLDIV1_CLK}
- {id: PCC.PCC_FTM1_SEL.sel, value: SCG.PLLDIV1_CLK}
- {id: PCC.PCC_FTM2_SEL.sel, value: SCG.PLLDIV1_CLK}
- {id: PCC.PCC_FTM3_SEL.sel, value: SCG.PLLDIV1_CLK}
- {id: PCC.PCC_LPI2C0_SEL.sel, value: SCG.PLLDIV2_CLK}
- {id: PCC.PCC_LPI2C1_SEL.sel, value: SCG.PLLDIV2_CLK}
- {id: PCC.PCC_LPIT0_SEL.sel, value: SCG.PLLDIV2_CLK}
- {id: PCC.PCC_LPSPI0_SEL.sel, value: SCG.PLLDIV2_CLK}
- {id: PCC.PCC_LPSPI1_SEL.sel, value: SCG.PLLDIV2_CLK}
- {id: PCC.PCC_LPTMR0_SEL.sel, value: SCG.PLLDIV2_CLK}
- {id: PCC.PCC_LPUART0_SEL.sel, value: SCG.PLLDIV2_CLK}
- {id: PCC.PCC_LPUART1_SEL.sel, value: SCG.PLLDIV2_CLK}
- {id: PCC.PCC_LPUART2_SEL.sel, value: SCG.PLLDIV2_CLK}
- {id: SCG.CLKOUTSEL.sel, value: SCG.SPLL_DIV2_CLK}
- {id: SCG.DIVBUS.scale, value: '2', locked: true}
- {id: SCG.DIVCORE.scale, value: '1', locked: true}
- {id: SCG.DIVSLOW.scale, value: '7'}
- {id: SCG.FIRCDIV1.scale, value: '1', locked: true}
- {id: SCG.FIRCDIV2.scale, value: '1', locked: true}
- {id: SCG.SCSSEL.sel, value: SCG.SPLL_DIV2_CLK}
- {id: SCG.SIRCDIV1.scale, value: '1', locked: true}
- {id: SCG.SIRCDIV2.scale, value: '2', locked: true}
- {id: SCG.SPLLDIV1.scale, value: '1', locked: true}
- {id: SCG.SPLLDIV2.scale, value: '2', locked: true}
- {id: SCG.SPLL_mul.scale, value: '21'}
- {id: 'SCG::RCCR[DIVBUS].bitField', value: BitFieldValue}
- {id: 'SCG::RCCR[DIVCORE].bitField', value: BitFieldValue}
- {id: 'SCG::RCCR[DIVSLOW].bitField', value: BitFieldValue}
- {id: 'SCG::RCCR[SCS].bitField', value: BitFieldValue}
- {id: SCG_SOSCCFG_OSC_MODE_CFG, value: ModeOscLowPower}
- {id: SCG_SOSCCFG_RANGE_CFG, value: High}
- {id: SCG_SOSCCSR_SOSCEN_CFG, value: Enabled}
- {id: SCG_SPLLCSR_SPLLEN_CFG, value: Enabled}
- {id: SIM.CLKOUTDIV.scale, value: '8', locked: true}
- {id: SIM.CLKOUTSEL.sel, value: SCG.SCGCLKOUT}
sources:
- {id: OSC32.OSC32kHz.outFreq, value: 32.768 kHz, enabled: true}
- {id: SCG.SOSC.outFreq, value: 16 MHz, enabled: true}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/*******************************************************************************
 * Variables for BOARD_BootClockHSRUN configuration
 ******************************************************************************/
const scg_sys_clk_config_t g_sysClkConfig_BOARD_BootClockHSRUN =
    {
        .divSlow = kSCG_SysClkDivBy7,             /* Slow Clock Divider: divided by 7 */
        .divBus = kSCG_SysClkDivBy2,              /* Bus Clock Divider: divided by 2 */
        .divCore = kSCG_SysClkDivBy1,             /* Core Clock Divider: divided by 1 */
        .src = kSCG_SysClkSrcSysPll,              /* System PLL is selected as System Clock Source */
    };
const scg_sosc_config_t g_scgSysOscConfig_BOARD_BootClockHSRUN =
    {
        .freq = 16000000U,                        /* System Oscillator frequency: 16000000Hz */
        .enableMode = kSCG_SysOscEnable,          /* Enable System OSC clock */
        .monitorMode = kSCG_SysOscMonitorDisable, /* Monitor disabled */
        .div1 = kSCG_AsyncClkDisable,             /* System OSC Clock Divider 1: Clock output is disabled */
        .div2 = kSCG_AsyncClkDisable,             /* System OSC Clock Divider 2: Clock output is disabled */
        .workMode = kSCG_SysOscModeOscLowPower,   /* Oscillator low power */
    };
const scg_sirc_config_t g_scgSircConfig_BOARD_BootClockHSRUN =
    {
        .enableMode = kSCG_SircEnable | kSCG_SircEnableInLowPower,/* Enable SIRC clock, Enable SIRC in low power mode */
        .div1 = kSCG_AsyncClkDivBy1,              /* Slow IRC Clock Divider 1: divided by 1 */
        .div2 = kSCG_AsyncClkDivBy2,              /* Slow IRC Clock Divider 2: divided by 2 */
        .range = kSCG_SircRangeHigh,              /* Slow IRC high range clock (8 MHz) */
    };
const scg_firc_config_t g_scgFircConfig_BOARD_BootClockHSRUN =
    {
        .enableMode = kSCG_FircEnable,            /* Enable FIRC clock */
        .div1 = kSCG_AsyncClkDivBy1,              /* Fast IRC Clock Divider 1: divided by 1 */
        .div2 = kSCG_AsyncClkDivBy1,              /* Fast IRC Clock Divider 2: divided by 1 */
        .range = kSCG_FircRange48M,               /* Fast IRC is trimmed to 48MHz */
        .trimConfig = NULL,                       /* Fast IRC Trim disabled */
    };
const scg_spll_config_t g_scgSysPllConfig_BOARD_BootClockHSRUN =
    {
        .enableMode = kSCG_SysPllEnable,          /* Enable SPLL clock */
        .monitorMode = kSCG_SysPllMonitorDisable, /* Monitor disabled */
        .div1 = kSCG_AsyncClkDivBy1,              /* System PLL Clock Divider 1: divided by 1 */
        .div2 = kSCG_AsyncClkDivBy2,              /* System PLL Clock Divider 2: divided by 2 */
        .src = kSCG_SysPllSrcSysOsc,              /* System PLL clock source is System OSC */
        .prediv = 0,                              /* Divided by 1 */
        .mult = 5,                                /* Multiply Factor is 21 */
    };
/*******************************************************************************
 * Code for BOARD_BootClockHSRUN configuration
 ******************************************************************************/
void BOARD_BootClockHSRUN(void)
{
    scg_sys_clk_config_t curConfig;

    /* Init SOSC according to board configuration. */
    CLOCK_InitSysOsc(&g_scgSysOscConfig_BOARD_BootClockHSRUN);
    /* Set the XTAL0 frequency based on board settings. */
    CLOCK_SetXtal0Freq(g_scgSysOscConfig_BOARD_BootClockHSRUN.freq);
    /* Init FIRC. */
    CLOCK_CONFIG_FircSafeConfig(&g_scgFircConfig_BOARD_BootClockHSRUN);
    /* Set HSRUN power mode. */
    SMC_SetPowerModeProtection(SMC, kSMC_AllowPowerModeAll);
    SMC_SetPowerModeHsrun(SMC);
    while (SMC_GetPowerModeState(SMC) != kSMC_PowerStateHsrun)
    {
    }

    /* Init SIRC. */
    CLOCK_InitSirc(&g_scgSircConfig_BOARD_BootClockHSRUN);
    /* Init SysPll. */
    CLOCK_InitSysPll(&g_scgSysPllConfig_BOARD_BootClockHSRUN);
    /* Set SCG to SPLL mode. */
    CLOCK_SetHsrunModeSysClkConfig(&g_sysClkConfig_BOARD_BootClockHSRUN);
    /* Wait for clock source switch finished. */
    do
    {
         CLOCK_GetCurSysClkConfig(&curConfig);
    } while (curConfig.src != g_sysClkConfig_BOARD_BootClockHSRUN.src);
    /* Set SystemCoreClock variable. */
    SystemCoreClock = BOARD_BOOTCLOCKHSRUN_CORE_CLOCK;
    /* Set SCG CLKOUT selection. */
    CLOCK_CONFIG_SetScgOutSel(SCG_CLKOUTCNFG_SPLL);
    /* Set the clock output on the CLKOUT pin. */
    CLOCK_CONFIG_SetClkOutClock(SIM_CLKOUT_SEL_SCGCLKOUT, SIM_CLKOUT_DIV_BY8);
    /* Set PCC ADC0 selection */
    CLOCK_SetIpSrc(kCLOCK_Adc0, kCLOCK_IpSrcSysPllAsync);
    /* Set PCC ADC1 selection */
    CLOCK_SetIpSrc(kCLOCK_Adc1, kCLOCK_IpSrcSysPllAsync);
    /* Set PCC ADC2 selection */
    CLOCK_SetIpSrc(kCLOCK_Adc2, kCLOCK_IpSrcSysPllAsync);
    /* Set PCC LPSPI0 selection */
    CLOCK_SetIpSrc(kCLOCK_Lpspi0, kCLOCK_IpSrcSysPllAsync);
    /* Set PCC LPSPI1 selection */
    CLOCK_SetIpSrc(kCLOCK_Lpspi1, kCLOCK_IpSrcSysPllAsync);
    /* Set PCC LPI2C0 selection */
    CLOCK_SetIpSrc(kCLOCK_Lpi2c0, kCLOCK_IpSrcSysPllAsync);
    /* Set PCC LPI2C1 selection */
    CLOCK_SetIpSrc(kCLOCK_Lpi2c1, kCLOCK_IpSrcSysPllAsync);
    /* Set PCC LPUART0 selection */
    CLOCK_SetIpSrc(kCLOCK_Lpuart0, kCLOCK_IpSrcSysPllAsync);
    /* Set PCC LPUART1 selection */
    CLOCK_SetIpSrc(kCLOCK_Lpuart1, kCLOCK_IpSrcSysPllAsync);
    /* Set PCC LPUART2 selection */
    CLOCK_SetIpSrc(kCLOCK_Lpuart2, kCLOCK_IpSrcSysPllAsync);
    /* Set PCC FLEXIO selection */
    CLOCK_SetIpSrc(kCLOCK_Flexio0, kCLOCK_IpSrcSysPllAsync);
    /* Set PCC LPTMR0 selection */
    CLOCK_SetIpSrc(kCLOCK_Lptmr0, kCLOCK_IpSrcSysPllAsync);
    /* Set PCC LPIT0 selection */
    CLOCK_SetIpSrc(kCLOCK_Lpit0, kCLOCK_IpSrcSysPllAsync);
    /* Set PCC FTM0 selection */
    CLOCK_SetIpSrc(kCLOCK_Ftm0, kCLOCK_IpSrcSysPllAsync);
    /* Set PCC FTM1 selection */
    CLOCK_SetIpSrc(kCLOCK_Ftm1, kCLOCK_IpSrcSysPllAsync);
    /* Set PCC FTM2 selection */
    CLOCK_SetIpSrc(kCLOCK_Ftm2, kCLOCK_IpSrcSysPllAsync);
    /* Set PCC FTM3 selection */
    CLOCK_SetIpSrc(kCLOCK_Ftm3, kCLOCK_IpSrcSysPllAsync);
}

