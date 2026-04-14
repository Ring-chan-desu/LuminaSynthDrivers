#include "ADSR_Matrix.h"
#include "../ADSR.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include <stdint.h>
#include <string.h>

extern ADSR ADSR1;

void ADSR_Matrix::ADSR_Matrix_Init(uint8_t x, uint8_t y)
{
    this->StartEntry.x = x;
    this->StartEntry.y = y;
}

// 矩阵数组长度减半,需要做出调整
void ADSR_Matrix::ADSR_Matrix_CurvePaint()
{
	if((this->Matrix_LineIndex) != 0) // 下标检测,防止越界访问 上一函数值赋值
	{
		if(master->ADSR_FunctionValueCalculate(master->p2, master->p3, master->p4, (this->Matrix_LineIndex) - 1) == 0)
		{
			this->Entry.LastNumber.Number = master->ADSR_FunctionValueCalculate(master->p2, master->p3, master->p4, (this->Matrix_LineIndex) - 1);
		}
		else
		{
			this->Entry.LastNumber.Number = master->ADSR_FunctionValueCalculate(master->p2, master->p3, master->p4, (this->Matrix_LineIndex) - 1) - 1;
		}
	}
	else
	{
		this->Entry.LastNumber.Number = 0;
	}
	
	if(master->ADSR_FunctionValueCalculate(master->p2, master->p3, master->p4, (this->Matrix_LineIndex)) == 0)
	{
		this->Entry.CurrentNumber.Number	= master->ADSR_FunctionValueCalculate(master->p2, master->p3, master->p4, (this->Matrix_LineIndex)); // 当前函数值
	}
	else
	{
		this->Entry.CurrentNumber.Number	= master->ADSR_FunctionValueCalculate(master->p2, master->p3, master->p4, (this->Matrix_LineIndex)) - 1; // 当前函数值
	}

	/* ----数对结构体,缓冲区下标赋值---- */
	this->Entry.CurrentNumber.Index 	= this->Entry.CurrentNumber.Number	* 2;
	this->Entry.LastNumber.Index		= this->Entry.LastNumber.Number		* 2; // 数对 参数补齐
}

ADSR_Matrix ADSRMatrix1(&ADSR1);
void test(){
    ADSRMatrix1.ADSR_Matrix_Init(0, 0);
    ADSRMatrix1.ADSR_Matrix_CurvePaint();
}