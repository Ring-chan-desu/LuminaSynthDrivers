#include "ILI9341V.h"

#include "main.h"
#include "dma.h"
#include "spi.h"
#include "gpio.h"

#include <stdint.h>

// uint8_t ILI9341V_GRAM[320][240] = {0};

/* 分辨率 240*320 */

/**
 * @brief 指令发送
 * 
 * @param Data 要发送的指令
 */
void ILI9341V_WriteCommand(uint8_t Command)
{
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);// DC拉低,进入指令模式
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);// 片选拉低,选择
    HAL_SPI_Transmit(&hspi1, &Command, 1, HAL_MAX_DELAY);//
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);// 片选拉高,取消选择
}

/**
 * @brief 数据发送
 * 
 * @param Data 要发送的数据
 */
void ILI9341V_WriteData(uint8_t Data)
{
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &Data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

/**
 * @brief 初始化函数
 */
void ILI9341V_Init()
{
    ILI9341V_Reset();            // 通过引脚硬复位

    ILI9341V_WriteCommand(0x01); // Software Reset 软复位
    HAL_Delay(10);

    ILI9341V_WriteCommand(0x11); // Sleep Out 退出睡眠模式
    HAL_Delay(120);             // 必须延时120ms,初始化这边使用个阻塞延时感觉也没太大所谓()

    ILI9341V_WriteCommand(0x3A); // Pixel Format
    ILI9341V_WriteData(0x55);    // 16-bit

    ILI9341V_WriteCommand(0x29); // Display ON
}

/**
 * @brief 硬件复位函数
 * 
 * @note 有别于发送0x01复位,这玩意是通过拉低硬件引脚实现的复位(＃°Д°)
 */
void ILI9341V_Reset()
{
    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET); // RST拉低
    HAL_Delay(10);
    HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
    HAL_Delay(120);
}

/**
 * @brief 清屏函数
 */
void ILI9341V_Clear()
{
    point TempStartpoint = {0,0};
    point TempEndpoint = {320,240};

    ILI9341V_SetRange(TempStartpoint, TempEndpoint);

    ILI9341V_WriteCommand(0x2C); // 写数据指令
    
    for (uint16_t width = 1 ; width <= 320 ; width ++)
    {
        
        for (uint8_t height = 1 ; height <= 240 ; height ++)
        {

            ILI9341V_WriteData(0x00);
            ILI9341V_WriteData(0x00);
        }
    }
}

/**
 * @brief 数字处理函数
 * 
 * @param Number 要处理的数字
 * 
 * @return 一个包含数字高四位,低四位的结构体
 */
number ILI9341V_SingleNumberDevider(uint16_t Number)
{
    number TempNumber;
    TempNumber.HighBits    =   Number  >>   8;
    TempNumber.LowBits     =   Number  &    0xFF;

    return TempNumber;
}

/**
 * @brief 列地址设置
 */
void ILI9341V_ColumnAddressSet(uint16_t StartColumnNumber, uint16_t EndColumnNumber)
{

    number Start = ILI9341V_SingleNumberDevider(StartColumnNumber);
    number End = ILI9341V_SingleNumberDevider(EndColumnNumber);

    ILI9341V_WriteCommand(0x2A); // 告诉屏幕:现在咱要开始写行地址啦喵~

    ILI9341V_WriteData(Start.HighBits);
    ILI9341V_WriteData(Start.LowBits);

    ILI9341V_WriteData(End.HighBits);
    ILI9341V_WriteData(End.LowBits);
    
}

/**
 * @brief 行地址设置
 */
void ILI9341V_RowAddressSet(uint16_t StartRowNumber, uint16_t EndRowNumber)
{

    number Start = ILI9341V_SingleNumberDevider(StartRowNumber);
    number End = ILI9341V_SingleNumberDevider(EndRowNumber);

    ILI9341V_WriteCommand(0x2B); // 告诉屏幕:现在咱要开始写列地址啦喵~

    ILI9341V_WriteData(Start.HighBits);
    ILI9341V_WriteData(Start.LowBits);

    ILI9341V_WriteData(End.HighBits);
    ILI9341V_WriteData(End.LowBits);
    
}

/**
 * @brief 根据给定点设置填充范围
 * 
 * @param StartPoint 起始点
 * @param Endpoint   终止点
 */
void ILI9341V_SetRange(point StartPoint, point Endpoint)
{

    ILI9341V_ColumnAddressSet(StartPoint.y, Endpoint.y);
    ILI9341V_RowAddressSet(StartPoint.x, Endpoint.x);
}

/**
 * @brief 十六进制颜色字符转十进制数字
 * 
 * @param Character 字符
 */
uint8_t ILI9341V_HexCharToOctValue(char Character)
{

    if(Character >= '0' && Character <= '9') return Character - '0';
    if(Character >= 'A' && Character <= 'F') return Character - 'A' + 10;
    if(Character >= 'a' && Character <= 'f') return Character - 'a' + 10;

    return 0;
}

/**
 * @brief 十六进制颜色代码转RGB565
 * 
 * @param String 十六进制颜色代码
 */
color ILI9341V_HexColorToOctValue(char* String)
{
    color ResultColor;

    uint32_t FinalColor = 0; // 最终颜色(RGB565)

    uint8_t Red     = ILI9341V_HexCharToOctValue(String[1]) * 16 + ILI9341V_HexCharToOctValue(String[2]);
    uint8_t Green   = ILI9341V_HexCharToOctValue(String[3]) * 16 + ILI9341V_HexCharToOctValue(String[4]);
    uint8_t Blue    = ILI9341V_HexCharToOctValue(String[5]) * 16 + ILI9341V_HexCharToOctValue(String[6]);

        Red = Red >> 3;
        Green = Green >> 2;
        Blue = Blue >> 3;

    FinalColor = Red << 11 | Green << 5 | Blue;

    ResultColor.HighBits = FinalColor >> 8;
    ResultColor.LowBits = FinalColor & 0xFF;

    return ResultColor;
}

/**
 * @brief 绘制100*100单色矩形测试函数
 * * @param StartX 左下角起始横坐标
 * @param StartY 左下角起始纵坐标
 * @param HexColor 十六进制颜色字符串 (如 "#FF0000")
 */
void ILI9341V_DrawTestRect(uint16_t StartX, uint16_t StartY, char* HexColor)
{
    // 定义矩形范围（100*100）
    point StartPoint = {StartX, StartY};
    point EndPoint = {StartX + 99, StartY + 99};

    // 转换颜色为RGB565格式
    color RectColor = ILI9341V_HexColorToOctValue(HexColor);

    // 设置屏幕填充区域
    ILI9341V_SetRange(StartPoint, EndPoint);

    // 发送开始写显存指令
    ILI9341V_WriteCommand(0x2C);

    // 循环填充像素数据
    for (uint32_t i = 0; i < 100 * 100; i++)
    {
        ILI9341V_WriteData(RectColor.HighBits);
        ILI9341V_WriteData(RectColor.LowBits);
    }
}