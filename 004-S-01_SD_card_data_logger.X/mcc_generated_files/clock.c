/**
  @Generated PIC24 / dsPIC33 / PIC32MM MCUs Source File

  @Company:
    Microchip Technology Inc.

  @File Name:
    clock.c

  @Summary:
    This is the clock.c file generated using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.95-b-SNAPSHOT
        Device            :  dsPIC33EP128GS804
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.36
        MPLAB             :  MPLAB X v5.10
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include <stdint.h>
#include "xc.h"
#include "clock.h"

void CLOCK_Initialize(void)
{
    // FRCDIV FRC/2; PLLPRE 2; DOZE 1:8; PLLPOST 1:2; DOZEN disabled; ROI disabled; 
    CLKDIV = 0x3100;
    // TUN Center frequency; 
    OSCTUN = 0x00;
    // ROON disabled; ROSEL FOSC; RODIV 0; ROSSLP disabled; 
    REFOCON = 0x00;
    // PLLDIV 28; 
    PLLFBD = 0x1C;
    // ENAPLL disabled; APSTSCLR 1:1; FRCSEL ASRCSEL determines input clock source; SELACLK Auxiliary Oscillators; ASRCSEL No clock input; 
    ACLKCON = 0x2700;
    // LFSR 0; 
    LFSR = 0x00;
    // ADCMD disabled; PWMMD disabled; T3MD disabled; T4MD disabled; T1MD enabled; U2MD enabled; T2MD disabled; U1MD enabled; SPI2MD disabled; SPI1MD enabled; C2MD disabled; C1MD enabled; T5MD disabled; I2C1MD disabled; 
    PMD1 = 0xF295;
    // IC4MD disabled; IC3MD disabled; OC1MD disabled; IC2MD disabled; OC2MD disabled; IC1MD disabled; OC3MD disabled; OC4MD disabled; 
    PMD2 = 0xF0F;
    // CMPMD disabled; I2C2MD disabled; 
    PMD3 = 0x402;
    // PWM2MD disabled; PWM1MD disabled; PWM4MD disabled; SPI3MD disabled; PWM3MD disabled; PWM6MD disabled; PWM5MD disabled; PWM8MD disabled; PWM7MD disabled; 
    PMD6 = 0xFF01;
    // CMP3MD disabled; CMP4MD disabled; PGA1MD disabled; PTGMD disabled; CMP1MD disabled; CMP2MD disabled; DMAMD enabled; 
    PMD7 = 0xF0A;
    // CLC3MD disabled; CLC4MD disabled; CCSMD disabled; PGA2MD disabled; CLC1MD disabled; CLC2MD disabled; ABGMD disabled; 
    PMD8 = 0x63E;
    // CF no clock failure; NOSC PRIPLL; CLKLOCK unlocked; OSWEN Switch is Complete; 
    __builtin_write_OSCCONH((uint8_t) (0x03));
    __builtin_write_OSCCONL((uint8_t) (0x01));
    // Wait for Clock switch to occur
    while (OSCCONbits.OSWEN != 0);
    while (OSCCONbits.LOCK != 1);
}
