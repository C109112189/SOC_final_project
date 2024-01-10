/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "xil_printf.h"

//additional head file
#include "xparameters.h"
#include "xscugic.h"		//�]�t���_�޲z�M�B�z���������
#include "xil_exception.h"	//���`�B�z�M���_�B�̪���ƻP�w�q
#include "xgpio.h"			//
#include "xuartps.h"		//�]�tUART���������
#include "sleep.h"			//����ɶ����

#include <stdbool.h>
#include <string.h>

//�w�q�a�Ϫ��e�j�p
#define map_height 4
#define map_weight 4
#define map_arry   5

// Parameter definitions
#define INTC_DEVICE_ID 			XPAR_PS7_SCUGIC_0_DEVICE_ID
#define LEDS_DEVICE_ID			XPAR_AXI_GPIO_1_DEVICE_ID					//led8bits
#define BTNS_DEVICE_ID			XPAR_AXI_GPIO_0_DEVICE_ID					//btn5bits
#define SW_DEVICE_ID			XPAR_AXI_GPIO_0_DEVICE_ID					//sw8bits

#define INTC_GPIO_INTERRUPT_ID 	XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR	//�ڲq�O[PS]�]�w�����_���f
#define BTN_INT 				XGPIO_IR_CH1_MASK
#define SW_INT  				XGPIO_IR_CH1_MASK

//volatile�w�q : �C���X�ݮɱq�O���餤���sŪ�����
volatile static u32 ByteSend = 0 , TotalByteSend = 0;

//static int Sokoban_Game_State = 0;										//������:0   ����:1
static int map[map_weight];
//static int map_arry[map_height][map_weight]= {0};								//�s��a�ϰ}�C��ƪ����
//static int Remaining_Destinations_led;									//��ܳѾl�ت��a�ƶq��led
static int led_data;														//��ܳѾl�ت��a�ƶq��led
static int btn_value,sw_value;
//static u8 TransmitBuffer[map_height * map_weight ] = {0};					//�x�s�ഫ��[map���@���}�C]+[�P�_�C���O�_����]�A�@��:73bits
static u8 TransmitBuffer[map_arry] = {0};					//�x�s�ഫ��[map���@���}�C]+[�P�_�C���O�_����]�A�@��:73bits

int score;																	//�C������
int buttonEvent;															//�C�����A
int user1,user2,user3;													//�C�����a
static int loss=0;


XUartPs_Config *Config_1;													//�ŧi"Config_1"�������ܼơA�èϥΡuXUartPs_Config�v�����c (UART�]�ƪ��]�w)
XUartPs Uart_PS_1;															//�ŧiUart_PS_1��XUartPs�����c (XUartPs�X�ʵ{����Ҹ�Ƶ��c)

XGpio LEDInst, BTNInst, SWInst;												//�ŧiLEDInst,BTNInst��XGpio�����c
XScuGic INTCInst;															//�ŧiINTCInst��XScuGic�����c


//�w�q��l�a��
/*int original_map[map_height][map_weight] = {
		{0, 0, 0, 1},
		{0, 0, 0, 1},
		{1, 0, 0, 0},
		{0, 1, 0, 0}
};*/


/*static : ���w��ܥu��b�Ӥ��ϥΡA����Q��L�{���ե�*/
//1. UART-Send Data-Partial
static int	Uart_Init(void);
static int	Uart_SendData(void *InstancePtr);
//static int	Uart_SendData_test(void *InstancePtr);

//2. Button-interrupt-partial
static int 	InterruptSystemSetup(XScuGic *XScuGicInstancePtr);
static int 	IntcInitFunction_BTN(u16 DeviceId, XGpio *GpioInstancePtr);
static void	Intr_Handler(void *baseaddr_p);

//3. �C������
static void gameInit(void);
static void move(void);

//----------------------------------------------------
// 0. Main Function
//----------------------------------------------------
int main()
{
	init_platform();														//��l��UART�B�P��֨�
	int status;																//�]�Ƴ]�ƪ�l�ƬO�_���\���ܼ�

	//��l�Ʀa��
	//Initial_Map(map, init_map);												//�ۭq�禡(���e�a��, ��l�a��)

    // ��l��LED
    status = XGpio_Initialize(&LEDInst, LEDS_DEVICE_ID);					//��l��XGpio (���V GPIO ���, �]��ID�W��)
    if (status != XST_SUCCESS)												//�P�_ GPIO ��l��
    	return XST_FAILURE;													//���ѴN�����{��

    // ��l��BTN
    status = XGpio_Initialize(&BTNInst, BTNS_DEVICE_ID);					//��l��XGpio (���V GPIO ���, �]��ID�W��)
    if (status != XST_SUCCESS)												//�P�_ GPIO ��l��
    	return XST_FAILURE;													//���ѴN�����{��

    //��l��SW
    status = XGpio_Initialize(&SWInst, SW_DEVICE_ID);						//��l��XGpio (���V GPIO ���, �]��ID�W��)
        if (status != XST_SUCCESS)											//�P�_ GPIO ��l��
        	return XST_FAILURE;												//���ѴN�����{��

    //�]�w GPIO ��J/��X
    XGpio_SetDataDirection(&LEDInst, 1, 0x00);								//�]�w GPIO ��J/��X (���V GPIO ���, GPIO���q�D , ��V�]�w 0:output 1:input)
    XGpio_SetDataDirection(&BTNInst, 1, 0xFF);								//�]�w GPIO ��J/��X (���V GPIO ���, GPIO���q�D , ��V�]�w 0:output 1:input)
    XGpio_SetDataDirection(&SWInst,  2, 0xFF);								//�]�w GPIO ��J/��X (���V GPIO ���, GPIO���q�D , ��V�]�w 0:output 1:input)

    //��l��UART
    if(Uart_Init() == XST_FAILURE){											//�P�_ UART ��l��
    	return XST_FAILURE;													//���ѴN�����{��
	}
    else{
    	//printf("Uart Initialization Successful!\n");
    	Uart_SendData(&Uart_PS_1);										//UART ��l�Ʀ��\��۰ʶǿ�@�� UART ���
	}

    //��l��BTN���
    status = IntcInitFunction_BTN(INTC_DEVICE_ID, &BTNInst);				//�ۭq�禡 (�]��ID ,GPIO ���)
    if (status != XST_SUCCESS)												//�P�_���_�����l��
    	return XST_FAILURE;													//���ѴN�����{��

    //��l��SW���_���
        status = IntcInitFunction_BTN(INTC_DEVICE_ID, &SWInst);				//�ۭq�禡 (�]��ID ,GPIO ���)
        if (status != XST_SUCCESS)												//�P�_���_�����l��
        	return XST_FAILURE;													//���ѴN�����{��

    XGpio LED_XGpio;														//�ŧi LED_XGpio �� XGpio �����c
    XGpio_Initialize(&LED_XGpio, XPAR_AXI_GPIO_1_DEVICE_ID);				//��l�� LED_XGpio �ós���� AXI_GPIO_0 ���]��

    gameInit();
    //Uart_SendData(&Uart_PS_1);
    while(1){
    	/*if(isWinner(map)){													//�ۭq���L�禡�P�_�O�_�q�� (���e�a��)
    		//printf("Winner!\n");
    		//printf("Do you wnat to play again?\n");
    		if(btn_value == 0)
    		    break;
    	}
    	else{
    		//�P�_�C�����A
    		Judge_Game_State(&Sokoban_Game_State, map);								//�ۭq�禡(�C�����A, ���e�a��)
    		//printf("Sokoban_Game_State is [%d]\n", Sokoban_Game_State);

    		//LED��ܳѾl�\��ت��a�ƶq
    		Remaining_Box(&Remaining_Destinations_led, map);						//�ۭq�禡(�Ѿl�ت��a�ƶq, ���e�a��)
    		XGpio_DiscreteWrite(&LED_XGpio, 1, Remaining_Destinations_led);			//�NLED����Ƽg�i���w��GPIO�q�D���Ȧs����	(XGpio�����, GPIO���q�D, �n�g�J�Ȧs�������)
    		//printf("Remaining_Destinations_led = [%d]\n", Remaining_Destinations_led);

    		//�M��H���y��
    		Find_Person_Coordinates(&Person_X, &Person_Y, map);						//�ۭq�禡(X�b, Y�b)
    		//printf("Person Coordinate is X[%d] Y[%d]\n", Person_X, Person_Y);

    		//usleep(500000);
    	}*/
    	//Uart_SendData(&Uart_PS_1);
    	//led_data = led_data + 1;	//�L�a�p��
    	//XGpio_DiscreteWrite(&LEDInst, 1,led_data);
    	//usleep(500);
    	if(buttonEvent==3)
    		break;
    }

    cleanup_platform();															//�����֨�
    return 0;
}

//----------------------------------------------------
// 1. UART-Send Data-Partial
//----------------------------------------------------

//1.1 UART-Setup-initialization
int Uart_Init(void){
	int status = 0;

	/*1.�b�uxparameters.h�v���M��u�O�_���ŦX�v���]��ID*/
	Config_1 = XUartPs_LookupConfig(XPAR_XUARTPS_0_DEVICE_ID);
	if (NULL == Config_1) {
		return XST_FAILURE;
	}

	/*UART��l�ƽT�{�ˬd*/
	status = XUartPs_CfgInitialize(&Uart_PS_1, Config_1, Config_1->BaseAddress);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	/*�]�mUART��BaudRate*/
	status = XUartPs_SetBaudRate(&Uart_PS_1,115200);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	/*�]�mUART���ާ@�Ҧ�*/
	XUartPs_SetOperMode(&Uart_PS_1, XUARTPS_OPER_MODE_NORMAL);

	/*(²��ӻ��N�OUART��FIFO�x�s��h��bits(�����q)�|Ĳ�o�ʧ@�A0x20U����Q�i�32
	 * �ҥHFIFO�x�s��32bits(8 Bytes)����Ʈɷ|Ĳ�o�ʧ@)
	*/
	XUartPs_WriteReg(Uart_PS_1.Config.BaseAddress,XUARTPS_TXWM_OFFSET, 0x20U);

	return XST_SUCCESS;

}

//1.2 UART-Send-Data
int Uart_SendData(void *InstancePtr){
	MapToArray(TransmitBuffer, map, buttonEvent);

	while (TotalByteSend < (sizeof(TransmitBuffer))) {
			ByteSend = XUartPs_Send(&Uart_PS_1,(u8*)&TransmitBuffer[TotalByteSend], 1);
			TotalByteSend += ByteSend;
	}
	TotalByteSend = 0;
	//printf("\n");

	return TotalByteSend;
}
//1.3 UART-Send-Data �Ҷǭ��ഫ���A
void MapToArray(u8 *byteArray, int Map_data[map_weight], int Game_State){

	u16 i;
	u8 asicii_zero  = '0';													//�ƭȤ�K�����ɡA�Ϊ�ASICII�ഫ

	for(i = 0 ; i < 4 ; i ++)
		byteArray[i]=map[i];
	byteArray[4]=Game_State;
	/*for (i = 0; i < map_height; i++) {
        for (j = 0; j <map_weight; j++) {
            byteArray[i * map_weight + j] = Map_data[i][j];
            //byteArray[i * map_weight + j] = Map_data[i][j] + asicii_zero;		//�ƭȤ�K�����ɡA�Ϊ�ASICII�ഫ
        }
    }*/
}

//int Uart_SendData_test(void *InstancePtr){
	//MapToArray(TransmitBuffer, map, Sokoban_Game_State);
	//MapToArray_c(TransmitBuffer, init_map, Sokoban_Game_State);
	//Print_Map_Data(TransmitBuffer);

	//while (TotalByteSend < (sizeof(TransmitBuffer))) {
	//		ByteSend = XUartPs_Send(&Uart_PS_1,(u8*)&TransmitBuffer[TotalByteSend], 1);
	//		TotalByteSend += ByteSend;
	//}
	//TotalByteSend = 0;
	//printf("\n");

	//return TotalByteSend;
//}

//----------------------------------------------------
// 2. Button-Interrupt-Partial
//----------------------------------------------------

//2.1 ����button interrupt����l�Ƴ]�w
int InterruptSystemSetup(XScuGic *XScuGicInstancePtr) {
	// Enable interrupt
	XGpio_InterruptEnable(&BTNInst, BTN_INT);
	XGpio_InterruptGlobalEnable(&BTNInst);

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler) XScuGic_InterruptHandler,
			XScuGicInstancePtr);
	Xil_ExceptionEnable();
	return XST_SUCCESS;

}

//2.2
int IntcInitFunction_BTN(u16 DeviceId, XGpio *GpioInstancePtr) {
	XScuGic_Config *IntcConfig;
	int status;

	// Interrupt controller initialisation
	IntcConfig = XScuGic_LookupConfig(DeviceId);
	status = XScuGic_CfgInitialize(&INTCInst, IntcConfig,IntcConfig->CpuBaseAddress);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	// Call to interrupt setup
	status = InterruptSystemSetup(&INTCInst);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	// Connect GPIO interrupt to handler
	status = XScuGic_Connect(&INTCInst, INTC_GPIO_INTERRUPT_ID,
			(Xil_ExceptionHandler) Intr_Handler, (void *) GpioInstancePtr);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	// Enable GPIO interrupts interrupt
	XGpio_InterruptEnable(GpioInstancePtr, 1);
	XGpio_InterruptGlobalEnable(GpioInstancePtr);

	// Enable GPIO and timer interrupts in the controller
	XScuGic_Enable(&INTCInst, INTC_GPIO_INTERRUPT_ID);

	return XST_SUCCESS;
}

//2.3
int IntcInitFunction_SW(u16 DeviceId, XGpio *GpioInstancePtr) {
	XScuGic_Config *IntcConfig;
	int status;

	// Interrupt controller initialisation
	IntcConfig = XScuGic_LookupConfig(DeviceId);
	status = XScuGic_CfgInitialize(&INTCInst, IntcConfig,IntcConfig->CpuBaseAddress);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	// Call to interrupt setup
	status = InterruptSystemSetup(&INTCInst);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	// Connect GPIO interrupt to handler
	status = XScuGic_Connect(&INTCInst, INTC_GPIO_INTERRUPT_ID,
			(Xil_ExceptionHandler) Intr_Handler, (void *) GpioInstancePtr);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	// Enable GPIO interrupts interrupt
	XGpio_InterruptEnable(GpioInstancePtr, 1);
	XGpio_InterruptGlobalEnable(GpioInstancePtr);

	// Enable GPIO and timer interrupts in the controller
	XScuGic_Enable(&INTCInst, INTC_GPIO_INTERRUPT_ID);

	return XST_SUCCESS;
}

//2.3 Button interrupt�ɷ|�@�����e
void Intr_Handler(void *InstancePtr){

	// Disable GPIO interrupts
	XGpio_InterruptDisable(&BTNInst, BTN_INT);
	XGpio_InterruptDisable(&SWInst,  SW_INT);

	// Ignore additional button presses
	if ((XGpio_InterruptGetStatus(&BTNInst) & BTN_INT) != BTN_INT) {
		return;
	}
	if ((XGpio_InterruptGetStatus(&SWInst) & SW_INT) != SW_INT) {
			return;
		}

	btn_value = XGpio_DiscreteRead(&BTNInst, 1);
	sw_value  = XGpio_DiscreteRead(&SWInst , 2);

	// Increment counter based on button value
	// Reset if centre button pressed
	/*if(btn_value != 0){
		switch(btn_value){

			case 16:{
				move();
				Uart_SendData(&Uart_PS_1);
				usleep(200000);
				/*if(Sokoban_Game_State == 0){
					Move_Up(map, Person_X, Person_Y);
					Judge_Game_State(&Sokoban_Game_State, map);
					Uart_SendData(&Uart_PS_1);
					//printf("\nMove UP : %d\n",btn_value);
					usleep(200000);
				}
				else
					Initial_Map(map, init_map);*/
			/*}break;

			case 2:{
				move();
				Uart_SendData(&Uart_PS_1);
				usleep(200000);
								/*
				if(Sokoban_Game_State == 0){
					Move_Down(map, Person_X, Person_Y);
					Judge_Game_State(&Sokoban_Game_State, map);
					Uart_SendData(&Uart_PS_1);
					//printf("\nMove Down : %d\n",btn_value);
					usleep(200000);
				}
				else
					Initial_Map(map, init_map);*/
			/*}break;

			case 4:{
				move();
				Uart_SendData(&Uart_PS_1);
				usleep(200000);
								/*
				if(Sokoban_Game_State == 0){
					Move_Left(map, Person_X, Person_Y);
					Judge_Game_State(&Sokoban_Game_State, map);
					Uart_SendData(&Uart_PS_1);
					//printf("\nMove Left : %d\n",btn_value);
					usleep(200000);
				}
				else
					Initial_Map(map, init_map);*/
			/*}break;

			case 8:{
				move();
				Uart_SendData(&Uart_PS_1);
				usleep(200000);
								/*
				if(Sokoban_Game_State == 0){
					Move_Right(map, Person_X, Person_Y);
					Judge_Game_State(&Sokoban_Game_State, map);
					Uart_SendData(&Uart_PS_1);
					//printf("\nMove Right : %d\n",btn_value);
					usleep(200000);
				}
				else
					Initial_Map(map, init_map);*/
			/*}break;

			case 1:{
				gameInit();
				Uart_SendData(&Uart_PS_1);
				usleep(200000);
				/*if(Sokoban_Game_State == 0){
					Game_Reset(map, init_map);
					Judge_Game_State(&Sokoban_Game_State, map);
					Uart_SendData(&Uart_PS_1);
					//printf("\nReset Game : %d\n",btn_value);
					usleep(200000);
				}
				else{
				    //printf("\ngame over\n");
					break;
				}*/
			/*}break;

			default:{
				printf("Error : Unknown btn_value\n");
			}break;
		}
	}
	else{
		//led_data = 0;
	}*/
	if(sw_value!=0)
		{
			switch(sw_value)
			{
				case (1):	//�Ĥ@����sĲ�o
					led_data=user1;
				break;
				case (2) :
					led_data=user2;
				break;
				case (4) :
					led_data=user3;
				break;
				default:
					led_data=user1;
				break;
			}
			XGpio_DiscreteWrite(&LEDInst, 1, led_data);
		}
	else{
		if(buttonEvent == 2)                //���������
		{
			switch(btn_value)
			{
				case(1):                         //�C����l���m
				{
					//printf("restart\n");
					buttonEvent = 0;
					loss=0;
					//gameInit();
					for(size_t i=0 ;  i<4  ; i++)
					{
						map[i] = rand() % 4;
					}
					Uart_SendData(&Uart_PS_1);
					usleep(200000);
				}
				break;
				case(16):
				{
					buttonEvent = 3;
					Uart_SendData(&Uart_PS_1);
					usleep(200000);
				}break;
				default:
					buttonEvent = 2;
					Uart_SendData(&Uart_PS_1);
					usleep(200000);
				break;
			}
		}
		else if(buttonEvent == 1 ||buttonEvent == 0)
		{
			switch(btn_value)
			{
				case(16):	//�Ĥ@����sĲ�o
				{
					if (map[3]==0)
					{
						move();
						buttonEvent = 1;
						user1=user1+1;
						Uart_SendData(&Uart_PS_1);
						usleep(200000);
					}
					else
					{
						//printf("game over\n");
						buttonEvent = 2;
						Uart_SendData(&Uart_PS_1);
						user3=user2;
						user2=user1;
						user1=0;
					}
				}break;

				case(8) :
				{
					if (map[3]==1)
					{
						move();
						Uart_SendData(&Uart_PS_1);
						buttonEvent = 1;
						user1=user1+1;
						usleep(200000);
					}
					else
					{
						//printf("game over\n");
						buttonEvent = 2;
						Uart_SendData(&Uart_PS_1);
						user3=user2;
						user2=user1;
						user1=0;
					}
				}break;

				case(2) :
				{
					if (map[3]==2)
					{
						move();
						Uart_SendData(&Uart_PS_1);
						buttonEvent = 1;
						user1=user1+1;
						usleep(200000);
					}
					else
					{
						//printf("game over\n");
						buttonEvent = 2;
						Uart_SendData(&Uart_PS_1);
						user3=user2;
						user2=user1;
						user1=0;
					}
				}break;

				case(4) :
				{
					if (map[3]==3)
					{
						move();
						Uart_SendData(&Uart_PS_1);
						buttonEvent = 1;
						user1=user1+1;
						usleep(200000);
					}
					else
					{
						//printf("game over\n");
						buttonEvent = 2;
						Uart_SendData(&Uart_PS_1);
						user3=user2;
						user2=user1;
						user1=0;
					}
				}break;

				case(1) :
				{
					buttonEvent = 0;
					for(size_t i=0 ;  i<4  ; i++)
					{
						map[i] = rand() % 4;
					}
					user3=user2;
					user2=user1;
					user1=0;
					Uart_SendData(&Uart_PS_1);
					usleep(200000);
				}break;

				default:
				{
					    buttonEvent = 1;
						user3=user3;
						user2=user2;
						user1=user1;
				}break;
			}
		}
			XGpio_DiscreteWrite(&LEDInst, 1, user1);
		}
	//XGpio_DiscreteWrite(&LEDInst, 1, led_data);
	(void) XGpio_InterruptClear(&BTNInst, BTN_INT);
	(void) XGpio_InterruptClear(&SWInst , SW_INT);
	// Enable GPIO interrupts
	XGpio_InterruptEnable(&BTNInst, BTN_INT);
	XGpio_InterruptEnable(&SWInst , SW_INT);
}

//----------------------------------------------------
// 3. �C����l��
//----------------------------------------------------
void gameInit(void)
{
	u16 i;
	//�ץ󪬺A��l��
	buttonEvent = 0;
	//��map�i���H����l��
	for( i=0 ;  i<4  ; i++)
		{
			map[i] = rand() % 4;
		}

	//�ϥΪ̤����k�s
	user1=0;user2=0;user3=0;
	//�����k�s
	score=0;
}

//----------------------------------------------------
// 4. ��沾��
//----------------------------------------------------
void move(void)
{
	for (size_t i = 3; i > 0; i--)
		{
			map[i] = map[i - 1];
		}
		map[0] = rand() % 4;

}