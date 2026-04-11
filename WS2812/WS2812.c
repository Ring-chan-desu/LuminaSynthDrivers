#include "WS2812.h"
#include "main.h"
#include "tim.h"

#include <stdint.h>

uint8_t WS2812_TempGRAM[COUNT][3] = {   {0x01	,0		,0		},
                                        {0		,0x01	,0		},	
                                        {0		,0		,0x01	},
                                        {0x01	,0		,0x01	}, };
uint8_t WS2812_TempGRAM2[COUNT][3] = {0};   
char WS2812_TempStrings[COUNT][7] = {"#010000","#000001","#000100","#010100"};

/**
 * @brief 将显存数据转换成一维数组并发送
 * 
 * @param GRAM 二维显存数组
 */
void WS2812_RamUpdate(uint8_t (*GRAM)[3])
{

    uint32_t WS2812_GRAM_Line[64 + COUNT * 3 * 8 + 64] = {0};//声明并定义用于发送的一维数组
    for (uint8_t index = 0 ; index < 64 + COUNT * 3 * 8 + 64 ; index ++)
    {
        WS2812_GRAM_Line[index] = 1;
    }

    /*数组头 64个0码*/
    for (uint8_t HeadIndex = 0 ; HeadIndex < 64 ; HeadIndex ++)
    {

        WS2812_GRAM_Line[HeadIndex] = RESET_CODE;

    }

    /*数组中部 颜色数据*/
    for (uint8_t CountIndex = 0 ; CountIndex < COUNT ; CountIndex ++)//按个数遍历灯珠
    {

        for (uint8_t ColorIndex = 0 ; ColorIndex < 3 ; ColorIndex ++)//遍历颜色
        {

            for (uint8_t BitIndex = 0 ; BitIndex < 8 ; BitIndex ++)//遍历位
            {

                WS2812_GRAM_Line[64 + CountIndex * 24 + ColorIndex * 8 + BitIndex] = ( ( GRAM[CountIndex][ColorIndex] >> ( 7 - BitIndex ) ) & 0x01 ) ? CODE_1 : CODE_0;//先移位,再按位与保留最低位.后根据三目运算符判断0码还是1码

            }
        }
    }

    /*数组尾 64个0码*/
    for (uint8_t TailIndex = 64 + COUNT * 24 ; TailIndex < 64 + COUNT * 24 + 64 ; TailIndex ++)
    {

        WS2812_GRAM_Line[TailIndex] = RESET_CODE;

    }

    HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_1);
    __HAL_TIM_SetCounter(&htim3,0);
    HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, WS2812_GRAM_Line, sizeof(WS2812_GRAM_Line) / sizeof(uint32_t));
}

/**
 * @brief 将代表16进制的单字符转换成10进制数
 * 
 * @param c 要转换的字符
 */
uint8_t WS2812_HexCharToOctValue(char Character)
{

    if(Character >= '0' && Character <= '9') return Character - '0';
    if(Character >= 'A' && Character <= 'F') return Character - 'A' + 10;
    if(Character >= 'a' && Character <= 'f') return Character - 'a' + 10;

    return 0;
}

/**
 * @brief 将16进制颜色代码转换成对应的RGB值并且赋值给显存数组
 * 
 * @param String 存储颜色代码的(二维)数组
 * @param GRam 显存数组
 */
void WS2812_HexColorToOctValue(uint8_t (*GRam)[3], char(*String)[7])
{

    for (uint8_t StringIndex = 0 ; StringIndex < COUNT ; StringIndex ++) // 遍历每个字符串
    {
        
        for (uint8_t ColorIndex = 1 ; ColorIndex <= 3 ; ColorIndex ++) // 遍历每个字符串的字符
        {
                                                
            GRam[StringIndex][ColorIndex - 1] = WS2812_HexCharToOctValue(String[StringIndex][ColorIndex * 2]) + WS2812_HexCharToOctValue(String[StringIndex][ColorIndex * 2 - 1]) * 16; // 赋值
        }
    }
}
//Pic was made by "CodeImg" https://github.com/subframe7536/vscode-codeimg.git