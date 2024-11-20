#include "bsp_tim.h"
extern TIM_HandleTypeDef htim2,htim3,htim4,htim8;
Clock_ Clock;
static float CPU_FREQ_Hz_us=1000;
static float CPU_FREQ_Hz_ms=1;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim8)
	{
		Clock.CNT++;
    }
}

// float DWT_GetDeltaT(uint32_t *cnt_last)
// {

// }

void MY_Delay_ms(float Delay)
{
    uint32_t tickstart = Clock.CNT;
    float wait         = Delay;
    while ((float)(Clock.CNT - tickstart) < wait * (float)CPU_FREQ_Hz_ms) ;
}
