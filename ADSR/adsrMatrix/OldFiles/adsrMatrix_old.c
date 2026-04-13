#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "ILI9341V.h"
#include "ADSR.h"
#include "adsrMatrix.h"
#include "RTOSBridge.h"

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

// 起始点XY
// 缓冲区
// 线数组下标
// 绘制FLAG
// 统一高宽
// TargetX

// 信号量?不用信号量,刷新时钟统一,指示点不统一即可.
//现阶段指示点统一

// DMA轮询

// 当前阶段共用ADSR

/* ----矩阵缓冲区---- */
uint8_t MatrixBuffer[MATRIX_HEIGHT * 2] = {0};

/* ----横坐标---- */
uint16_t Matrix_LineIndex = 0;

/**
 * @brief 矩阵选区
 * 
 * @param x 起始点横坐标
 * @param y 起始点纵坐标
 */
void ADSR_Matrix_Init(uint8_t x, uint8_t y)
{
	// 写入起始点和终点 然后直接开启绘制...似乎适应性很差
	// 之后可以在DMA轮询模块,每次发送之前做一下下面的流程,所以这个函数是选择函数而不是初始化函数
    point Start = {x, y};
    point End   = {x + MATRIX_WIDTH - 1, y + MATRIX_HEIGHT - 1};
    ILI9341V_SetRange(Start, End);
    ILI9341V_WriteCommand(0x2C);
    HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
}

/**
 * @brief 实时生成ADSR包络曲线
 */
void ADSR_Matrix_CurvePaint()
{
	/* ----绘制初始化---- */ // ##对象化之后属性待修改

	// ## 防止果冻效应,这里应该加上快照,也就是参数保存之类的.

	/*test*/
	uint8_t *MatrixBufferPoint = MatrixBuffer;
	uint16_t *Matrix_LineIndexPoint = &Matrix_LineIndex;
	/*testend*/

	uint16_t ConstTargetX = TargetX; // 获取TargetX横坐标用于绘图
	memset(MatrixBufferPoint , 0, MATRIX_HEIGHT * 2); // 擦除半段
	if((*Matrix_LineIndexPoint) >= MATRIX_WIDTH) (*Matrix_LineIndexPoint) = 0; // 横坐标检测,防止越界访问

	/* ----数对结构体,函数值赋值---- */ // ##目前共用ADSR,暂不修改
	#pragma region 
	entry Entry;
	if((*Matrix_LineIndexPoint) != 0) // 下标检测,防止越界访问 上一函数值赋值
	{
		if(ADSR_FunctionValueCalculate(p2, p3, p4, (*Matrix_LineIndexPoint) - 1) == 0)
		{
			Entry.LastNumber.Number = ADSR_FunctionValueCalculate(p2, p3, p4, (*Matrix_LineIndexPoint) - 1);
		}
		else
		{
			Entry.LastNumber.Number = ADSR_FunctionValueCalculate(p2, p3, p4, (*Matrix_LineIndexPoint) - 1) - 1;
		}
	}
	else
	{
		Entry.LastNumber.Number = 0;
	}
	
	if(ADSR_FunctionValueCalculate(p2, p3, p4, (*Matrix_LineIndexPoint)) == 0)
	{
		Entry.CurrentNumber.Number	= ADSR_FunctionValueCalculate(p2, p3, p4, (*Matrix_LineIndexPoint)); // 当前函数值
	}
	else
	{
		Entry.CurrentNumber.Number	= ADSR_FunctionValueCalculate(p2, p3, p4, (*Matrix_LineIndexPoint)) - 1; // 当前函数值
	}

	/* ----数对结构体,缓冲区下标赋值---- */
	Entry.CurrentNumber.Index 	= Entry.CurrentNumber.Number	* 2;
	Entry.LastNumber.Index		= Entry.LastNumber.Number		* 2; // 数对 参数补齐
	#pragma endregion

	// /* 正常绘制 */
	// MatrixBufferPoint[Entry.CurrentNumber.Index ]		= 0xFF;
	// MatrixBufferPoint[Entry.CurrentNumber.Index + 1]		= 0xFF;

	/* ----中间补齐逻辑---- */
	#pragma region 
    if(Entry.CurrentNumber.Number == Entry.LastNumber.Number)
    {
        /* 正常绘制一个点 */
		if(Entry.CurrentNumber.Number != 0)
		{
			MatrixBufferPoint[Entry.CurrentNumber.Index]		= 0xFF; // ##数组缓冲区待修改
			MatrixBufferPoint[Entry.CurrentNumber.Index + 1]	= 0xFF;
		}
    }
    else if(Entry.CurrentNumber.Number > Entry.LastNumber.Number)
    {
		/* 向上补齐 */
        uint16_t Start = Entry.LastNumber.Index ;
		uint16_t Length = Entry.CurrentNumber.Index - Entry.LastNumber.Index + 2;
		memset(MatrixBufferPoint + Start , 0xFF, Length); // ##数组缓冲区待修改
    }
    else if(Entry.CurrentNumber.Number < Entry.LastNumber.Number)
    {
		/* 向下补齐 */
        uint16_t Start = Entry.CurrentNumber.Index ;
		uint16_t Length = Entry.LastNumber.Index - Entry.CurrentNumber.Index + 2;
		memset(MatrixBufferPoint + Start, 0xFF, Length); // ##数组缓冲区待修改
    }
	#pragma endregion

	/* 指示点绘制 */
	if(TracerRunFlag)
	{
		uint8_t MatchArray[6] = {ConstTargetX - 2, ConstTargetX - 1, ConstTargetX, ConstTargetX + 1, ConstTargetX + 2, ConstTargetX + 3};

		for (uint8_t i = 0; i < 6; i++) // 横坐标
		{
			if((*Matrix_LineIndexPoint) == MatchArray[i]) // ##线数组下标待修改
			{
				for (int8_t Offset = -2; Offset < 4; Offset++)
				{
					MatrixBufferPoint[Entry.CurrentNumber.Index + Offset*2]		= 0xF8; // ##数组缓冲区待修改
					MatrixBufferPoint[Entry.CurrentNumber.Index + Offset*2 + 1]	= 0x00;
				}
			}		
		}
	}
	
	if((*Matrix_LineIndexPoint) == MATRIX_WIDTH - 1) // 溢出检测 检测到溢出说明周期结束
	{
		osSemaphoreRelease(adsrMatrix_DisplayPeriod_SemHandle); // ADSR周期信号量释放
	}
	(*Matrix_LineIndexPoint) ++;
}
/* Pic was made by "CodeImg" https://github.com/subframe7536/vscode-codeimg.git */
/* おやすみのキスして? */

/* ----回调部分---- */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1)
    {
		osSemaphoreRelease(adsrMatrix_DMAComplete_SemHandle); // 发送完成信号量释放
    }
}

/* ----RTOS---- */

/* ---- adsrMatrix DisplayPeriod Semaphore ---- */
osSemaphoreId_t adsrMatrix_DisplayPeriod_SemHandle;

void adsrMatrix_DisplayPeriod_SemInit(void)
{
	const osSemaphoreAttr_t adsrMatrix_DisplayPeriod_SemAttr = {
		.name = "adsrMatrix_DisplayPeriod_Sem"
	};
	// 创建二进制信号量：初始值0，最大值1
	adsrMatrix_DisplayPeriod_SemHandle = osSemaphoreNew(1, 0, &adsrMatrix_DisplayPeriod_SemAttr);
}

/* ---- adsrMatrix DMAComplete Semaphore ---- */
osSemaphoreId_t adsrMatrix_DMAComplete_SemHandle;

void adsrMatrix_DMAComplete_SemInit(void)
{
	const osSemaphoreAttr_t adsrMatrix_DMAComplete_SemAttr = {
		.name = "adsrMatrix_DMAComplete_Sem"
	};
	// 创建二进制信号量：初始值0，最大值1
	adsrMatrix_DMAComplete_SemHandle = osSemaphoreNew(1, 1, &adsrMatrix_DMAComplete_SemAttr);
}

/* ---- adsrMatrix DMAEnable Task ---- */
osThreadId_t adsrMatrix_DMAEnable_TaskHandle;

void adsrMatrix_DMAEnable_Task(void *argument)
{
	for(;;)
	{
		osSemaphoreAcquire(adsrMatrix_DMAComplete_SemHandle, osWaitForever);
		ADSR_Matrix_CurvePaint();
		
		uint16_t y = 20;
		uint16_t x = 20;
		point Start = {x + Matrix_LineIndex, y};
		point End   = {x + Matrix_LineIndex, y + MATRIX_HEIGHT};
		ILI9341V_SetRange(Start, End);
		ILI9341V_WriteCommand(0x2C);
		HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);

		HAL_SPI_Transmit_DMA(&hspi1, MatrixBuffer, MATRIX_HEIGHT * 2);
	}
}

void adsrMatrix_DMAEnable_TaskInit(void)
{
	const osThreadAttr_t adsrMatrix_DMAEnable_TaskAttr = {
		.name = "adsrMatrix_DMAEnable_Task",
		.stack_size = 128 * 4,
		.priority = (osPriority_t) osPriorityNormal,
	};
	adsrMatrix_DMAEnable_TaskHandle = osThreadNew(adsrMatrix_DMAEnable_Task, NULL, &adsrMatrix_DMAEnable_TaskAttr);
}
