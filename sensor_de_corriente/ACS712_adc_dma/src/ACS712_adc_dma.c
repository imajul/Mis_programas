/*=============================================================================
 * Program: ACS712_adc_dma
 * Date: 2021/02/21
 *===========================================================================*/

/*
 * @brief    ADC DMA example
 * @author   TDII - UTN FRH
 * @version  1.0
 * @date     June 2016
 * @details  Trigger an interrupt after completing N_SAMPLES A/D conversions
 *           on ADC0_CH1 analog pin using the DMA controller
 */

#include "sapi.h"
#include "board.h"
//#include "qei_18xx_43xx.h"
//#include "adc_18xx_43xx.h"

#define N_SAMPLES   100

static uint8_t dma_ch_adc;  /* There are 8 DMA channels available */
static uint8_t dma_tc_adc;  /* TC: terminal count                 */

static LPC_QEI_T encoder;

static uint32_t dma_buffer[N_SAMPLES];
volatile static uint16_t adc_buffer[N_SAMPLES];


/* Hardware initialization */
static void initHardware(void)
{
    ADC_CLOCK_SETUP_T adc_setup;

    Board_Init();

    /* ADC configuration */
    // Setup ADC0: 10-bit, 100kHz
    Chip_ADC_Init(LPC_ADC0, &adc_setup);
    Chip_ADC_SetSampleRate(LPC_ADC0, &adc_setup, 1);

    // Enable ADC0 CH1 and its interrupt
    Chip_ADC_EnableChannel(LPC_ADC0, ADC_CH1, ENABLE);
    Chip_ADC_Int_SetChannelCmd(LPC_ADC0, ADC_CH1, ENABLE);

    // Enable ADC0 burst mode
    Chip_ADC_SetBurstCmd(LPC_ADC0, ENABLE);

    /* DMA controller configuration */
    Chip_GPDMA_Init(LPC_GPDMA);

    // Get a free channel for a ADC->Memory DMA transfer
    dma_ch_adc = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, GPDMA_CONN_ADC_0);

    // Enable the interrupt in the NVIC
    NVIC_ClearPendingIRQ(DMA_IRQn);
    NVIC_EnableIRQ(DMA_IRQn);
}

/* In this example there is only one ADC->Memory DMA transfer */
void startDMATransfer(void)
{
    dma_tc_adc = 0;

    Chip_GPDMA_Transfer(LPC_GPDMA, dma_ch_adc,
            GPDMA_CONN_ADC_0, (uint32_t)&dma_buffer,
            GPDMA_TRANSFERTYPE_P2M_CONTROLLER_DMA, N_SAMPLES);
}

/* DMA interrupt triggered after N_SAMPLES conversions of ADC0_1 */
void DMA_IRQHandler(void)
{
    if (Chip_GPDMA_Interrupt(LPC_GPDMA, dma_ch_adc) == SUCCESS) {
        dma_tc_adc = 1;
    }
}


/* Main function */
int main(void)
{
    volatile uint32_t counter_debug = 0;
    uint8_t i = 0;

    initHardware();

    startDMATransfer();

    while (1) {
        if (dma_tc_adc)
        {
            for (i = 0; i < N_SAMPLES; i++)
            {
                adc_buffer[i] = ADC_DR_RESULT(dma_buffer[i]);
            }
            /* Put a breakpoint here and check the different values
             * of adc_buffer[] changing the voltage on ADC0 CH1 pin */

            startDMATransfer();

            counter_debug = 0;
        }

        counter_debug++;

        /* ... */
    }
    return 0;
}
