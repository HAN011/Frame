#ifndef _BSP_TIM_H
#define _BSP_TIM_H

#include "main.h"

void MY_Delay_ms(float Delay);

typedef struct
{
    uint32_t CNT;
} Clock_;

#endif