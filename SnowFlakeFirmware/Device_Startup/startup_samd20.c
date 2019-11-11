/**
 * \file
 *
 * \brief gcc starttup file for SAMD20
 *
 * Copyright (c) 2018 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#include "samd20.h"

/* Initialize segments */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

/** \cond DOXYGEN_SHOULD_SKIP_THIS */
int main(void);
/** \endcond */

void __libc_init_array(void);

/* Default empty handler */
void Dummy_Handler(void);

/* Cortex-M0+ core handlers */
void NonMaskableInt_Handler  ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void HardFault_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SVCall_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PendSV_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SysTick_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));

/* Peripherals handlers */
void PM_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SYSCTRL_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void WDT_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void RTC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EIC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void NVMCTRL_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EVSYS_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SERCOM0_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SERCOM1_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SERCOM2_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SERCOM3_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#ifdef ID_SERCOM4
void SERCOM4_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_SERCOM5
void SERCOM5_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif
void TC0_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC1_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC2_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC3_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC4_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC5_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#ifdef ID_TC6
void TC6_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_TC7
void TC7_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_ADC
void ADC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_AC
void AC_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_DAC
void DAC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif
#ifdef ID_PTC
void PTC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif

/* Exception Table */
__attribute__ ((section(".vectors")))
const DeviceVectors exception_table = {

        /* Configure Initial Stack Pointer, using linker-generated symbols */
        .pvStack                = (void*) (&_estack),

        .pfnReset_Handler       = (void*) Reset_Handler,
        .pfnNonMaskableInt_Handler = (void*) NonMaskableInt_Handler,
        .pfnHardFault_Handler   = (void*) HardFault_Handler,
        .pvReservedM12          = (void*) (0UL), /* Reserved */
        .pvReservedM11          = (void*) (0UL), /* Reserved */
        .pvReservedM10          = (void*) (0UL), /* Reserved */
        .pvReservedM9           = (void*) (0UL), /* Reserved */
        .pvReservedM8           = (void*) (0UL), /* Reserved */
        .pvReservedM7           = (void*) (0UL), /* Reserved */
        .pvReservedM6           = (void*) (0UL), /* Reserved */
        .pfnSVCall_Handler      = (void*) SVCall_Handler,
        .pvReservedM4           = (void*) (0UL), /* Reserved */
        .pvReservedM3           = (void*) (0UL), /* Reserved */
        .pfnPendSV_Handler      = (void*) PendSV_Handler,
        .pfnSysTick_Handler     = (void*) SysTick_Handler,

        /* Configurable interrupts */
        .pfnPM_Handler          = (void*) PM_Handler,             /*  0 Power Manager */
        .pfnSYSCTRL_Handler     = (void*) SYSCTRL_Handler,        /*  1 System Control */
        .pfnWDT_Handler         = (void*) WDT_Handler,            /*  2 Watchdog Timer */
        .pfnRTC_Handler         = (void*) RTC_Handler,            /*  3 Real-Time Counter */
        .pfnEIC_Handler         = (void*) EIC_Handler,            /*  4 External Interrupt Controller */
        .pfnNVMCTRL_Handler     = (void*) NVMCTRL_Handler,        /*  5 Non-Volatile Memory Controller */
        .pfnEVSYS_Handler       = (void*) EVSYS_Handler,          /*  6 Event System Interface */
        .pfnSERCOM0_Handler     = (void*) SERCOM0_Handler,        /*  7 Serial Communication Interface 0 */
        .pfnSERCOM1_Handler     = (void*) SERCOM1_Handler,        /*  8 Serial Communication Interface 1 */
        .pfnSERCOM2_Handler     = (void*) SERCOM2_Handler,        /*  9 Serial Communication Interface 2 */
        .pfnSERCOM3_Handler     = (void*) SERCOM3_Handler,        /* 10 Serial Communication Interface 3 */
#ifdef ID_SERCOM4
        .pfnSERCOM4_Handler     = (void*) SERCOM4_Handler,        /* 11 Serial Communication Interface 4 */
#else
        .pvReserved11           = (void*) (0UL),                  /* 11 Reserved */
#endif
#ifdef ID_SERCOM5
        .pfnSERCOM5_Handler     = (void*) SERCOM5_Handler,        /* 12 Serial Communication Interface 5 */
#else
        .pvReserved12           = (void*) (0UL),                  /* 12 Reserved */
#endif
        .pfnTC0_Handler         = (void*) TC0_Handler,            /* 13 Basic Timer Counter 0 */
        .pfnTC1_Handler         = (void*) TC1_Handler,            /* 14 Basic Timer Counter 1 */
        .pfnTC2_Handler         = (void*) TC2_Handler,            /* 15 Basic Timer Counter 2 */
        .pfnTC3_Handler         = (void*) TC3_Handler,            /* 16 Basic Timer Counter 3 */
        .pfnTC4_Handler         = (void*) TC4_Handler,            /* 17 Basic Timer Counter 4 */
        .pfnTC5_Handler         = (void*) TC5_Handler,            /* 18 Basic Timer Counter 5 */
#ifdef ID_TC6
        .pfnTC6_Handler         = (void*) TC6_Handler,            /* 19 Basic Timer Counter 6 */
#else
        .pvReserved19           = (void*) (0UL),                  /* 19 Reserved */
#endif
#ifdef ID_TC7
        .pfnTC7_Handler         = (void*) TC7_Handler,            /* 20 Basic Timer Counter 7 */
#else
        .pvReserved20           = (void*) (0UL),                  /* 20 Reserved */
#endif
#ifdef ID_ADC
        .pfnADC_Handler         = (void*) ADC_Handler,            /* 21 Analog Digital Converter */
#else
        .pvReserved21           = (void*) (0UL),                  /* 21 Reserved */
#endif
#ifdef ID_AC
        .pfnAC_Handler          = (void*) AC_Handler,             /* 22 Analog Comparators */
#else
        .pvReserved22           = (void*) (0UL),                  /* 22 Reserved */
#endif
#ifdef ID_DAC
        .pfnDAC_Handler         = (void*) DAC_Handler,            /* 23 Digital Analog Converter */
#else
        .pvReserved23           = (void*) (0UL),                  /* 23 Reserved */
#endif
#ifdef ID_PTC
        .pfnPTC_Handler         = (void*) PTC_Handler             /* 24 Peripheral Touch Controller */
#else
        .pvReserved24           = (void*) (0UL)                   /* 24 Reserved */
#endif
};

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void Reset_Handler(void)
{
        uint32_t *pSrc, *pDest;

        /* Initialize the relocate segment */
        pSrc = &_etext;
        pDest = &_srelocate;

        if (pSrc != pDest) {
                for (; pDest < &_erelocate;) {
                        *pDest++ = *pSrc++;
                }
        }

        /* Clear the zero segment */
        for (pDest = &_szero; pDest < &_ezero;) {
                *pDest++ = 0;
        }

        /* Set the vector table base address */
        pSrc = (uint32_t *) & _sfixed;
        SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

        /* Overwriting the default value of the NVMCTRL.CTRLB.MANW bit (errata reference 13134) */
        NVMCTRL->CTRLB.bit.MANW = 1;

        /* Initialize the C library */
        __libc_init_array();

        /* Branch to main function */
        main();

        /* Infinite loop */
        while (1);
}

/**
 * \brief Default interrupt handler for unused IRQs.
 */
void Dummy_Handler(void)
{
        while (1) {
        }
}
