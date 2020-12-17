//###########################################################################
//
// FILE:   Example_2833xEPwmDeadBand.c
//
// TITLE:  ePWM Deadband Generation Example
//
//! \addtogroup f2833x_example_list
//! <h1> ePWM Deadband Generation (epwm_deadband)</h1>
//!
//! This example configures ePWM1, ePWM2 and ePWM3 for:
//!   - Count up/down
//!   - Deadband
//! 3 Examples are included:
//!   - ePWM1: Active low PWMs
//!   - ePWM2: Active low complementary PWMs
//!   - ePWM3: Active high complementary PWMs
//!
//! Each ePWM is configured to interrupt on the 3rd zero event
//! when this happens the deadband is modified such that
//! 0 <= DB <= DB_MAX.  That is, the deadband will move up and
//! down between 0 and the maximum value.
//!
//! \b External \b Connections \n
//!  - EPWM1A is on GPIO0
//!  - EPWM1B is on GPIO1
//!  - EPWM2A is on GPIO2
//!  - EPWM2B is on GPIO3
//!  - EPWM3A is on GPIO4
//!  - EPWM3B is on GPIO5
//
//
//###########################################################################
// $TI Release: F2833x Support Library v2.01.00.00 $
// $Release Date: Sun Oct  4 16:07:01 IST 2020 $
// $Copyright:
// Copyright (C) 2009-2020 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//###########################################################################

//
// Included Files
//
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

//
// Function Prototypes//函数声明
//
void InitEPwm1Example(void);
void InitEPwm2Example(void);
void InitEPwm3Example(void);
__interrupt void epwm1_isr(void);
__interrupt void epwm2_isr(void);
__interrupt void epwm3_isr(void);

//
// Globals//定义全局变量
//
Uint32  EPwm1TimerIntCount;
Uint32  EPwm2TimerIntCount;
Uint32  EPwm3TimerIntCount;
Uint16  EPwm1_DB_Direction;
Uint16  EPwm2_DB_Direction;
Uint16  EPwm3_DB_Direction;

//
// Defines for the Maximum Dead Band values//定义最大死区值
//
#define EPWM1_MAX_DB   0x03FF
#define EPWM2_MAX_DB   0x03FF
#define EPWM3_MAX_DB   0x03FF

#define EPWM1_MIN_DB   0
#define EPWM2_MIN_DB   0
#define EPWM3_MIN_DB   0

//
// Defines to keep track of which way the Dead Band is moving   定义跟踪死区带的移动方向
//
#define DB_UP   1
#define DB_DOWN 0

//
// Main  主函数
//
void main(void)
{
    //
    // Step 1. Initialize System Control:初始化系统控制
    // PLL, WatchDog, enable Peripheral Clocks
    // This example function is found in the DSP2833x_SysCtrl.c file.
    //
    InitSysCtrl();

    //
    // Step 2. Initialize GPIO:初始化GPIO口
    // This example function is found in the DSP2833x_Gpio.c file and
    // illustrates how to set the GPIO to it's default state.
    //
    // InitGpio();  // Skipped for this example

    //
    // For this case just init GPIO pins for ePWM1, ePWM2, ePWM3
    // These functions are in the DSP2833x_EPwm.c file
    //
    InitEPwm1Gpio();
    InitEPwm2Gpio();
    InitEPwm3Gpio();

    //
    // Step 3. Clear all interrupts and initialize PIE vector table:
    // Disable CPU interrupts
    //
    DINT;//禁止CPU中断

    //
    // Initialize the PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags
    // are cleared.
    // This function is found in the DSP2833x_PieCtrl.c file.
    //
    InitPieCtrl();//初始化PIE控制寄存器的默认状态  默认状态是所有的中断和中断标志被清除

    //
    // Disable CPU interrupts and clear all CPU interrupt flags
    //
    IER = 0x0000;//禁止CPU中断
    IFR = 0x0000;//清除所有CPU中断标志位

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt
    // is not used in this example.  This is useful for debug purposes.
    // The shell ISR routines are found in DSP2833x_DefaultIsr.c.
    // This function is found in DSP2833x_PieVect.c.
    //
    InitPieVectTable();//初始化PIE矢量表

    //
    // Interrupts that are used in this example are re-mapped to
    // ISR functions found within this file.
    //
    EALLOW;    // This is needed to write to EALLOW protected registers设置状态寄存器1的C6位，此时处于禁写状态的寄存器解禁
    PieVectTable.EPWM1_INT = &epwm1_isr;//设置EPWM1_INT中断服务程序的入口地址为epwm1_isr
    PieVectTable.EPWM2_INT = &epwm2_isr;
    PieVectTable.EPWM3_INT = &epwm3_isr;
    EDIS;      // This is needed to disable write to EALLOW protected registers清除状态寄存器1的C6位，寄存器重新恢复禁止写入状态

    //
    // Step 4. Initialize all the Device Peripherals:
    // This function is found in DSP2833x_InitPeripherals.c
    //
    // InitPeripherals();  // Not required for this example
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;//关闭EPWM时钟
    EDIS;

    InitEPwm1Example();
    InitEPwm2Example();
    InitEPwm3Example();

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;//打开EPWM时钟
    EDIS;

    //
    // Step 5. User specific code, enable interrupts
    //
    
    //
    // Initialize counters://初始化计数器
    //
    EPwm1TimerIntCount = 0;
    EPwm2TimerIntCount = 0;
    EPwm3TimerIntCount = 0;

    //
    // Enable CPU INT3 which is connected to EPWM1-3 INT
    //
    IER |= M_INT3;//开启cpu IER中断标志位

    //
    // Enable EPWM INTn in the PIE: Group 3 interrupt 1-3
    //
    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;////PIE中断使能寄存器使能，即PIEIER3.1=1
    PieCtrlRegs.PIEIER3.bit.INTx2 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx3 = 1;

    //
    // Enable global Interrupts and higher priority real-time debug events使能全局中断和更高优先级的实时调试事件
    //
    EINT;       // Enable Global interrupt INTM使能全局中断INTM
    ERTM;       // Enable Global realtime interrupt DBGM使能全局实时中断DBGM

    //
    // Step 6. IDLE loop. Just sit and loop forever (optional)空循环
    //
    for(;;)
    {
        __asm("          NOP");
    }
}

//
// epwm1_isr - 
//
__interrupt void 
epwm1_isr(void)
{
    if(EPwm1_DB_Direction == DB_UP)
    {
        if(EPwm1Regs.DBFED < EPWM1_MAX_DB)
        {
            EPwm1Regs.DBFED++;//死区下降沿延时
            EPwm1Regs.DBRED++;//死区上升沿延时
        }      
        else
        {   
            EPwm1_DB_Direction = DB_DOWN;
            EPwm1Regs.DBFED--;
            EPwm1Regs.DBRED--;
        }
    }
    else
    {
        if(EPwm1Regs.DBFED == EPWM1_MIN_DB)
        {
            EPwm1_DB_Direction = DB_UP;
            EPwm1Regs.DBFED++;
            EPwm1Regs.DBRED++;
        }
        else
        {
            EPwm1Regs.DBFED--;
            EPwm1Regs.DBRED--;
        }
    }
    EPwm1TimerIntCount++;//定时器中断次数

    //
    // Clear INT flag for this timer
    //
    EPwm1Regs.ETCLR.bit.INT = 1;//中断标志清除

    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;////中断应答位
}

//
// epwm2_isr -
//
__interrupt void 
epwm2_isr(void)
{
    if(EPwm2_DB_Direction == DB_UP)
    {
        if(EPwm2Regs.DBFED < EPWM2_MAX_DB)
        {
            EPwm2Regs.DBFED++;
            EPwm2Regs.DBRED++;
        }
        else
        {
            EPwm2_DB_Direction = DB_DOWN;
            EPwm2Regs.DBFED--;
            EPwm2Regs.DBRED--;
        }
    }
    else
    {
        if(EPwm2Regs.DBFED == EPWM2_MIN_DB)
        {
            EPwm2_DB_Direction = DB_UP;
            EPwm2Regs.DBFED++;
            EPwm2Regs.DBRED++;
        }
        else
        {
            EPwm2Regs.DBFED--;
            EPwm2Regs.DBRED--;
        }
    }

    EPwm2TimerIntCount++;

    //
    // Clear INT flag for this timer
    //
    EPwm2Regs.ETCLR.bit.INT = 1;

    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

//
// epwm3_isr - 
//
__interrupt void epwm3_isr(void)
{
    if(EPwm3_DB_Direction == DB_UP)
    {
        if(EPwm3Regs.DBFED < EPWM3_MAX_DB)
        {
            EPwm3Regs.DBFED++;
            EPwm3Regs.DBRED++;
        }
        else
        {
            EPwm3_DB_Direction = DB_DOWN;
            EPwm3Regs.DBFED--;
            EPwm3Regs.DBRED--;
        }
    }
    else
    {
        if(EPwm3Regs.DBFED == EPWM3_MIN_DB)
        {
            EPwm3_DB_Direction = DB_UP;
            EPwm3Regs.DBFED++;
            EPwm3Regs.DBRED++;
        }
        else
        {
            EPwm3Regs.DBFED--;
            EPwm3Regs.DBRED--;
        }
    }

    EPwm3TimerIntCount++;

    //
    // Clear INT flag for this timer
    //
    EPwm3Regs.ETCLR.bit.INT = 1;

    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

//
// InitEPwm1Example - 
//
void 
InitEPwm1Example()
{
    EPwm1Regs.TBPRD = 6000;                        // Set timer period设置周期
    EPwm1Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0时基相位寄存器的值赋值0，初相0
    EPwm1Regs.TBCTR = 0x0000;                      // Clear counter时基计数器清零

    //
    // Setup TBCLK
    //
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up增减计数模式
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading禁止相位加载0

    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV4;       // Clock ratio to SYSCLKOUT V1--12.5KHZ  V2 3KHz V4--0.78KHZ
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV4;

    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // CMPA影子裝載模式Load registers every ZERO设置比较寄存器的阴影寄存器加载条件：时基计数到0或PRD
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;  //CTR=0 影子暫存器A裝載
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

    //
    // Setup compare设置比较寄存器的值
    //
    EPwm1Regs.CMPA.half.CMPA = 4500;

    //
    // Set actions
    //
 //EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;                 //CTR=0 置1 ePWMA輸出高电平
  //  EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;          //  CTR上升至CMPA值 置0 ePWMA輸出低电平
   EPwm1Regs.AQCTLA.bit.CAU =  AQ_CLEAR;             // Set PWM1A on Zero CTR上升至CMPA值 置0 ePWMA輸出低电平
   EPwm1Regs.AQCTLA.bit.CAD =  AQ_SET;           // CTR下降至CMPA值 置1 ePWMA輸出高电平

  // EPwm1Regs.AQCTLB.bit.ZRO = AQ_CLEAR;            // CTR=0 置0 ePWMB輸出低电平
    EPwm1Regs.AQCTLB.bit.CAU =  AQ_SET;          // Set PWM1A on Zero CTR上升至CMPA值 置0 ePWMB輸出低电平
    EPwm1Regs.AQCTLB.bit.CAD = AQ_CLEAR;        //CTR下降至CMPA值 置0 ePWMA輸出低电平

    //
    // Active Low PWMs - Setup Deadband设置死区
    //
    EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;//这里基本上都是配置两个波形都是有延时才输出的，不会旁路掉延时波形
    EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_LOC;//低电平有效，这里会对电平进行一个反转
    EPwm1Regs.DBCTL.bit.IN_MODE = DBA_ALL;//选择 epwmxA作为信号源作为延时依据
    EPwm1Regs.DBRED = EPWM1_MIN_DB;//设置上升沿的延时
    EPwm1Regs.DBFED = EPWM1_MIN_DB;//设置下降沿的延时
    EPwm1_DB_Direction = DB_UP;

    //
    // Interrupt where we will change the Deadband
    //
    EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event选择EPWMx_INT产生的条件，这里选择 001：TBCTR=0x0000时产生
    EPwm1Regs.ETSEL.bit.INTEN = 1;                // Enable INT使能产生EPWM中断信号
    EPwm1Regs.ETPS.bit.INTPRD = ET_3RD;           // Generate INT on 3rd event中断周期设定，每发生三次事件，产生中断信号EPWMx_INT（每三次定时中断触发一次进入中断函数）
}

//
// InitEPwm2Example - 
//
void 
InitEPwm2Example()
{
    EPwm2Regs.TBPRD = 6000;                        // Set timer period
    EPwm2Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
    EPwm2Regs.TBCTR = 0x0000;                      // Clear counter

    //
    // Setup TBCLK
    //
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
    EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV4;       // Clock ratio to SYSCLKOUT
    EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV4;  // Slow just to observe on the scope

    //
    // Setup compare
    //
    EPwm2Regs.CMPA.half.CMPA = 3000;

    //
    // Set actions
    //

    EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;             // Set PWM2A on Zero   CTR上升至CMPA值 置1 ePWMA輸出高
    EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR;           //CTR下降至CMPA值 置0 ePWMA輸出低



    EPwm2Regs.AQCTLB.bit.CAU = AQ_CLEAR;           // Set PWM2A on Zero   CTR上升至CMPA值 置0 ePWMB輸出低
    EPwm2Regs.AQCTLB.bit.CAD = AQ_SET;             //CTR下降至CMPA值 置1 ePWMB輸出高

    //
    // Active Low complementary PWMs - setup the deadband
    //
    EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_LOC;
    EPwm2Regs.DBCTL.bit.IN_MODE = DBA_ALL;
    EPwm2Regs.DBRED = EPWM2_MIN_DB;
    EPwm2Regs.DBFED = EPWM2_MIN_DB;
    EPwm2_DB_Direction = DB_UP;

    //
    // Interrupt where we will modify the deadband
    //
    EPwm2Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;      // Select INT on Zero event
    EPwm2Regs.ETSEL.bit.INTEN = 1;                 // Enable INT
    EPwm2Regs.ETPS.bit.INTPRD = ET_3RD;            // Generate INT on 3rd event
}

//
// InitEPwm3Example - 
//
void 
InitEPwm3Example()
{
    EPwm3Regs.TBPRD = 6000;                         // Set timer period
    EPwm3Regs.TBPHS.half.TBPHS = 0x0000;            // Phase is 0
    EPwm3Regs.TBCTR = 0x0000;                       // Clear counter

    //
    // Setup TBCLK
    //
    EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
    EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
    EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV4;       // Clock ratio to SYSCLKOUT
    EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV4;// Slow so we can observe on the scope

    //
    // Setup compare
    //
    EPwm3Regs.CMPA.half.CMPA = 3000;

    //
    // Set actions
    //
    EPwm3Regs.AQCTLA.bit.CAU = AQ_SET;          // Set PWM3A on Zero
    EPwm3Regs.AQCTLA.bit.CAD = AQ_CLEAR;

    EPwm3Regs.AQCTLB.bit.CAU = AQ_CLEAR;        // Set PWM3A on Zero
    EPwm3Regs.AQCTLB.bit.CAD = AQ_SET;

    //
    // Active high complementary PWMs - Setup the deadband
    //
    EPwm3Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
    EPwm3Regs.DBCTL.bit.IN_MODE = DBA_ALL;
    EPwm3Regs.DBRED = EPWM3_MIN_DB;
    EPwm3Regs.DBFED = EPWM3_MIN_DB;
    EPwm3_DB_Direction = DB_UP;

    //
    // Interrupt where we will change the deadband
    //
    EPwm3Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;    // Select INT on Zero event
    EPwm3Regs.ETSEL.bit.INTEN = 1;               // Enable INT
    EPwm3Regs.ETPS.bit.INTPRD = ET_3RD;          // Generate INT on 3rd event
}

//
// End of File
//

