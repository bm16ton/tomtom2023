/*
 *  linux/include/asm-arm/arch-s3c2410/uncompress.h
 *
 *  Copyright (C) 2002 SAMSUNG ELECTORNICS 
 *                         SW.LEE (hitchcar@sec.samsung.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


/*** !!!!! VERY IMPORTANT  SEE asm/arch/s3c2410.h   !!!!! */

// I/O PORT 
#define rGPACON    (*(volatile unsigned *)0x56000000) //Port A control
#define rGPADAT    (*(volatile unsigned *)0x56000004) //Port A data
                        
#define rGPBCON    (*(volatile unsigned *)0x56000010) //Port B control
#define rGPBDAT    (*(volatile unsigned *)0x56000014) //Port B data
#define rGPBUP     (*(volatile unsigned *)0x56000018) //Pull-up control B
                        
#define rGPCCON    (*(volatile unsigned *)0x56000020) //Port C control
#define rGPCDAT    (*(volatile unsigned *)0x56000024) //Port C data
#define rGPCUP     (*(volatile unsigned *)0x56000028) //Pull-up control C
                        
#define rGPDCON    (*(volatile unsigned *)0x56000030) //Port D control
#define rGPDDAT    (*(volatile unsigned *)0x56000034) //Port D data
#define rGPDUP     (*(volatile unsigned *)0x56000038) //Pull-up control D
                        
#define rGPECON    (*(volatile unsigned *)0x56000040) //Port E control
#define rGPEDAT    (*(volatile unsigned *)0x56000044) //Port E data
#define rGPEUP     (*(volatile unsigned *)0x56000048) //Pull-up control E
                        
#define rGPFCON    (*(volatile unsigned *)0x56000050) //Port F control
#define rGPFDAT    (*(volatile unsigned *)0x56000054) //Port F data
#define rGPFUP     (*(volatile unsigned *)0x56000058) //Pull-up control F
                        
#define rGPGCON    (*(volatile unsigned *)0x56000060) //Port G control
#define rGPGDAT    (*(volatile unsigned *)0x56000064) //Port G data
#define rGPGUP     (*(volatile unsigned *)0x56000068) //Pull-up control G
                        
#define rGPHCON    (*(volatile unsigned *)0x56000070) //Port H control
#define rGPHDAT    (*(volatile unsigned *)0x56000074) //Port H data
#define rGPHUP     (*(volatile unsigned *)0x56000078) //Pull-up control H

/* Level trigger and edge trigger */
#define rEXTINT0   (*(volatile unsigned *)0x56000088) //External interrupt control register 0
#define rEXTINT1   (*(volatile unsigned *)0x5600008c) //External interrupt control register 1
#define rEXTINT2   (*(volatile unsigned *)0x56000090) //External interrupt control register 2

#define rGSTATUS2  (*(volatile unsigned *)(0x560000b4)) //Reset type

#define WrUTXH0(ch) (*(volatile unsigned char *)0x50000020)=(unsigned char)(ch)
#define rUTRSTAT0   (*(volatile unsigned *)0x50000010) //UART 0 Tx/Rx status

/*  UART */
#define PA_UART_BASE          0x50000000
#define rULCON0     (*(volatile unsigned *)(PA_UART_BASE+0x00)) //UART 0 Line control
#define rUCON0      (*(volatile unsigned *)(PA_UART_BASE+0x04)) //UART 0 Control
#define rUFCON0     (*(volatile unsigned *)(PA_UART_BASE+0x08)) //UART 0 FIFO control
#define rUMCON0     (*(volatile unsigned *)(PA_UART_BASE+0x0c)) //UART 0 Modem control
#define rUBRDIV0    (*(volatile unsigned *)(PA_UART_BASE+0x28)) //UART 0 Baud rate divisor

/* keyword : SER_BAUD */
#define SER_BAUD_9600    9600 
#define SER_BAUD_14400   11400
#define SER_BAUD_19200   19200
#define SER_BAUD_38400   38400
#define SER_BAUD_57600   57600
#define SER_BAUD_115200  115200
#define CURRENT_BAUD_RATE  SER_BAUD_115200

/***************  Clock Parameter  **********/
#define  FCLK 202800000
#define  HCLK (FCLK/2)
#define  PCLK (HCLK/2)
#define  UCLK  48000000
#define  BUSWIDTH (32)
/********************************************/



/* Data  : 8 4 2 1
 */
static void Led_Display(int data)
{
  rGPFDAT = (0xf<<4);
  rGPFDAT = ((~data & 0xf)<<4);
}

static void Uart_SendByte(int data) 
{ 
        if(data=='\n') 
        { 
            while(!(rUTRSTAT0 & 0x2)); 
	    /*            Delay(10);*/
	    /*because the slow response of hyper_terminal  */
            WrUTXH0('\r'); 
        } 
        while(!(rUTRSTAT0 & 0x2));   /*Wait until THR is empty. */
	/*        Delay(10); */
        WrUTXH0(data); 
}                


static void puts(const char *s)
{
  int index =0;
    while(*(s+index)) 
        Uart_SendByte(*(s+ index++)); 
}


static void Uart_Init(int pclk,int baud) 
{ 
    int i; 
    rUFCON0 = 0x0;   //UART channel 0 FIFO control register, FIFO disable 
    rUMCON0 = 0x0;   //UART chaneel 0 MODEM control register, AFC disable 
    rULCON0 = 0x3;
// [10]       [9]     [8]        [7]        [6]      [5]
//         [4]           [3:2]        [1:0] 
// Clock Sel, Tx Int, Rx Int, Rx Time Out, Rx err, Loop-back, 
//      Send break,  Transmit Mode, Receive Mode
//  0          1       0    ,     0          1        0  
//         0     ,       01          01 
//   PCLK       Level    Pulse    Disable    Generate  Normal 
//      Normal       Interrupt or Polling 
    rUCON0  = 0x245;                        // Control register 
    rUBRDIV0=( (int)(pclk/16/baud) -1 );   //Baud rate divisior register 0 
    for(i=0;i<100;i++);

// added for UART 1 

// 	rUFCON1 = 0x0;   //UART channel 1 FIFO control register, FIFO disable
//    rUMCON1 = 0x0;   //UART chaneel 1 MODEM control register, AFC disable
//    rULCON1 = 0x3;
//    rUCON1  = 0x245; // Control register
//	int Uart2baud =4800;
//    rUBRDIV1=( (int)(pclk/16/Uart2baud) -1 );   //Baud rate divisior register 0
 //   for(i=0;i<100;i++);

// UART2

//	rUFCON2 = 0x0;   //UART channel 0 FIFO control register, FIFO disable
  //  rUMCON2 = 0x0;   //UART chaneel 0 MODEM control register, AFC disable
    //rULCON2 = 0x3;
    //rUCON2  = 0x245;                        // Control register
    //rUBRDIV2=( (int)(pclk/16/baud) -1 );   //Baud rate divisior register 0
    //for(i=0;i<100;i++);


 
} 

/* If you want to change clock , you must check head-s3c2410.S 
 *
 * FCLK 203Mhze in this case
 * And uart 0 
 */
static void Port_Init(void);
static void arch_decomp_setup()
{
   int i ;
   Port_Init();
   //   Led_Display(4);
   Uart_Init(PCLK,CURRENT_BAUD_RATE);
}

#define arch_decomp_wdog()

/***********   [ PORTS ]     ******************************
 * CHECK asm/arch-s3c2410/irq.c
 *
 */

#define INPUT    0
#define OUTPUT   1
#define FUNC     2
#define EINT     2
#define FUNC2    3

#define OUTPUTA  0
#define FUNCA    1

#define LOW  0
#define HIGH 1
#define NA   0

static unsigned int determinePullup(unsigned int invalue)
{
    char bit;
	unsigned int outvalue = 0;

    for (bit = 0;bit < 16;bit++)
	{
        outvalue >>= 1;
        switch (invalue & 3)
        {
            case INPUT:
            break;
            
            case OUTPUT:
            case FUNC:
            case FUNC2:
                outvalue |= 0x8000; 
            break;
        }
        invalue >>= 2;
	}
    return outvalue;
}

static unsigned int determinePullupInt(unsigned int invalue)
{
    char bit;
	unsigned int outvalue = 0;

    for (bit = 0;bit < 16;bit++)
	{
        outvalue >>= 1;
        switch (invalue & 3)
        {
            case INPUT:
            case EINT:
            break;
            
            case OUTPUT:
            case FUNC2:
                outvalue |= 0x8000; 
            break;
        }
        invalue >>= 2;
	}
	return outvalue;
}

static void Port_Init(void)
{
    unsigned int config;
	unsigned int output;

    rGSTATUS2 = 2;  

    config = 0;
	output = 0;
#ifdef EVALUATION_BOARD
    config = FUNCA   | (config<<1); // DAC_PWR_ON (nFCE)
    output = 1       | (output<<1);
    config = FUNCA   | (config<<1); // nRSTOUT
    output = NA      | (output<<1);
    config = FUNCA   | (config<<1); // RF_PWR_ON (nFRE)
    output = LOW     | (output<<1);
    config = FUNCA   | (config<<1); // MUTE_EXT (nFWE)
    output = LOW     | (output<<1);
    config = FUNCA   | (config<<1); // AIN4_PWR (ALE)
    output = LOW     | (output<<1);
    config = FUNCA   | (config<<1); // SD_PWR_ON (CLE)
    output = LOW     | (output<<1);
#else
    config = OUTPUTA | (config<<1); // DAC_PWR_ON (nFCE)
    output = 1       | (output<<1);
    config = FUNCA   | (config<<1); // nRSTOUT
    output = NA      | (output<<1);
    config = OUTPUTA | (config<<1); // RF_PWR_ON (nFRE)
    output = LOW     | (output<<1);
    config = OUTPUTA | (config<<1); // MUTE_EXT (nFWE)
    output = LOW     | (output<<1);
    config = OUTPUTA | (config<<1); // AIN4_PWR (ALE)
    output = LOW     | (output<<1);
    config = OUTPUTA | (config<<1); // SD_PWR_ON (CLE)
    output = LOW     | (output<<1);
#endif
    config = FUNCA   | (config<<1); // N/C (nGCS5)
    output = NA      | (output<<1);
    config = FUNCA   | (config<<1); // N/C (nGCS4)
    output = NA      | (output<<1);
    config = FUNCA   | (config<<1); // N/C (nGCS3)
    output = NA      | (output<<1);
    config = FUNCA   | (config<<1); // N/C (nGCS2)
    output = NA      | (output<<1);
    config = FUNCA   | (config<<1); // N/C (nGCS1)
    output = NA      | (output<<1);
    config = FUNCA   | (config<<1); // LADDR26 (N/C)
    output = NA      | (output<<1);
    config = FUNCA   | (config<<1); // LADDR25 (RAM BA1)
    output = NA      | (output<<1);
    config = FUNCA   | (config<<1); // LADDR24 (RAM BA0)
    output = NA      | (output<<1);
    config = FUNCA   | (config<<1); // LADDR23 (N/C)
    output = NA      | (output<<1);
    config = FUNCA   | (config<<1); // LADDR22 (N/C)
    output = NA      | (output<<1);
    config = FUNCA   | (config<<1); // LADDR21 (N/C)
    output = NA      | (output<<1);
    config = FUNCA   | (config<<1); // LADDR20
    output = NA      | (output<<1);
    config = FUNCA   | (config<<1); // LADDR19
    output = NA      | (output<<1);
    config = FUNCA   | (config<<1); // LADDR18
    output = NA      | (output<<1);
    config = FUNCA   | (config<<1); // LADDR17
    output = NA      | (output<<1);
    config = FUNCA   | (config<<1); // LADDR16
    output = NA      | (output<<1);
    config = FUNCA   | (config<<1); // LADDR0 (N/C)
    output = NA      | (output<<1);

    rGPADAT = output;
    rGPACON = config;

    config = 0;
	output = 0;
    config = INPUT  | (config<<2); // N/C
    output = NA     | (output<<1);
    config = INPUT  | (config<<2); // N/C
    output = NA     | (output<<1);
    config = INPUT  | (config<<2); // N/C
    output = NA     | (output<<1);
    config = INPUT  | (config<<2); // N/C
    output = NA     | (output<<1);
    config = INPUT  | (config<<2); // N/C
    output = NA     | (output<<1);
    config = INPUT  | (config<<2); // N/C
    output = NA     | (output<<1);
    config = OUTPUT | (config<<2); // L3CLOCK
    output = 0      | (output<<1);
    config = OUTPUT | (config<<2); // L3DATA
    output = 0      | (output<<1);
    config = OUTPUT | (config<<2); // L3MODE
    output = 0      | (output<<1);
    config = OUTPUT | (config<<2); // BACKLIGHT_DISABLE
    output = 0      | (output<<1);
	config = OUTPUT | (config<<2); // BACKLIGHT_PWM
    output = 1      | (output<<1);

    rGPBDAT = output;
    rGPBCON = config;
	rGPBUP = determinePullup(config);

    config = 0;
	output = 0;
#ifdef EVALUATION_BOARD
    config = FUNC   | (config<<2); // VD7
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD6
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD5
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD4
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD3
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD2
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD1 (N/C)
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD0 (N/C)
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // LCD_BIAS_PRWEN
    output = 1      | (output<<1);
    config = FUNC   | (config<<2); // LCD_OEN
    output = 1      | (output<<1);
    config = FUNC   | (config<<2); // N/C
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VDEN
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // N/C
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // N/C
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VCLK
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // N/C
    output = NA     | (output<<1);
#else
    config = FUNC   | (config<<2); // VD7
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD6
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD5
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD4
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD3
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD2
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD1 (N/C)
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD0 (N/C)
    output = NA     | (output<<1);
    config = OUTPUT | (config<<2); // LCD_BIAS_PRWEN
    output = 1      | (output<<1);
    config = OUTPUT | (config<<2); // LCD_OEN
    output = 1      | (output<<1);
    config = INPUT  | (config<<2); // N/C
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VDEN
    output = NA     | (output<<1);
    config = INPUT  | (config<<2); // N/C
    output = NA     | (output<<1);
    config = INPUT  | (config<<2); // N/C
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VCLK
    output = NA     | (output<<1);
    config = INPUT  | (config<<2); // N/C
    output = NA     | (output<<1);
#endif

    rGPCDAT = output;
    rGPCCON = config;
	rGPCUP = determinePullup(config);

    config = 0;
	output = 0;
    config = FUNC   | (config<<2); // VD23
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD22
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD21
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD20
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD19
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD18
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD17
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD16
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD15
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD14
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD13
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD12
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD11
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD10
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // VD9
    output = NA     | (output<<1);
	config = FUNC   | (config<<2); // VD8
    output = NA     | (output<<1);

    rGPDDAT = output;
    rGPDCON = config;
	rGPDUP = determinePullup(config);

    config = 0;
	output = 0;
    config = INPUT  | (config<<2); // N/C
    output = NA     | (output<<1);
    config = INPUT  | (config<<2); // N/C
    output = NA     | (output<<1);
    config = OUTPUT | (config<<2); // USB signal
    output = 1      | (output<<1);
    config = OUTPUT | (config<<2); // CHARGE_SHDN#
    output = HIGH   | (output<<1);
    config = OUTPUT | (config<<2); // ACC_PWR_ON#
    output = HIGH   | (output<<1);
    config = FUNC   | (config<<2); // SDDATA3
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // SDDATA2
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // SDDATA1
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // SDDATA0
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // SDCMD
    output = NA     | (output<<1);
    config = OUTPUT | (config<<2); // SDCLK
    output = 0      | (output<<1);
    config = FUNC   | (config<<2); // I2SSDO (DACDAT)
    output = NA     | (output<<1);
    config = INPUT  | (config<<2); // CHARGEFAULT#
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // CDCLK
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // I2SSCLK (BCLK)
    output = NA     | (output<<1);
    config = FUNC   | (config<<2); // I2SLRCK (DACLRC)
    output = NA     | (output<<1);

    rGPEDAT = output;
    rGPECON = config;
	rGPEUP = determinePullup(config);

    config = 0;
	output = 0;
    config = INPUT  | (config<<2); // nIGNITION
    output = NA     | (output<<1);
    config = OUTPUT | (config<<2); // MUTE_INT
    output = 0      | (output<<1);
    config = INPUT  | (config<<2); // DOUT_Y
    output = NA     | (output<<1);
    config = INPUT  | (config<<2); // DOUT_X
    output = NA     | (output<<1);
    config = OUTPUT | (config<<2); // AMP_ON
    output = 0      | (output<<1);
    config = EINT   | (config<<2); // nCD_SD
    output = NA     | (output<<1);
    config = INPUT  | (config<<2); // USB_HOST_DETECT
    output = NA     | (output<<1);
	config = EINT   | (config<<2); // ON/OFF
    output = NA     | (output<<1);

    rGPFDAT = output;
    rGPFCON = config;
	rGPFUP = determinePullupInt(config);

    config = 0;
	output = 0;
    config = FUNC2  | (config<<2); // nYPON
    output = NA     | (output<<1);
    config = FUNC2  | (config<<2); // YMON
    output = NA     | (output<<1);
    config = FUNC2  | (config<<2); // nXPON
    output = NA     | (output<<1);
    config = FUNC2  | (config<<2); // XMON
    output = NA     | (output<<1);
    config = INPUT  | (config<<2); // N/C
    output = NA     | (output<<1);
    config = INPUT  | (config<<2); // nCD_SD
    output = NA     | (output<<1);
    config = OUTPUT | (config<<2); // GPS_SLEEP/nRUN
    output = 0      | (output<<1);
    config = OUTPUT | (config<<2); // GPS_nRESET
    output = 0      | (output<<1);
    config = INPUT  | (config<<2); // ACPWR#
    output = NA     | (output<<1);
    config = INPUT  | (config<<2); // DOCKSNS1
    output = NA     | (output<<1);
    config = INPUT  | (config<<2); // DOCKSNS0
    output = NA     | (output<<1);
    config = OUTPUT | (config<<2); // LCD_VCC_PWREN
    output = 1      | (output<<1);
    config = INPUT  | (config<<2); // CHARGING#
    output = NA     | (output<<1);
    config = INPUT  | (config<<2); // CHARGE_OUT
    output = NA     | (output<<1);
    config = OUTPUT | (config<<2); // GPS_REPRO
    output = 0      | (output<<1);
	config = EINT   | (config<<2); // GPS_1PPS
    output = NA     | (output<<1);

    rGPGDAT = output;
    rGPGCON = config;
	rGPGUP = determinePullupInt(config) | 1; // (1 PPS shouldn't have a pullup)

    config = 0;
	output = 0;
    config = OUTPUT | (config<<2); // GPS_SLEEP/nRUN
    output = 0      | (output<<1);
    config = OUTPUT | (config<<2); // GPS_nRESET
    output = 0      | (output<<1);
    config = INPUT  | (config<<2); // WP_SD
    output = NA     | (output<<1);
    config = OUTPUT | (config<<2); // RXD2
    output = 1      | (output<<1);
    config = OUTPUT | (config<<2); // TXD2
    output = 1      | (output<<1);
    config = FUNC   | (config<<2); // RXD1
    output = 1      | (output<<1);
    config = FUNC   | (config<<2); // TXD1
    output = 1      | (output<<1);
    config = FUNC   | (config<<2); // RXD0
    output = 1      | (output<<1);
    config = FUNC   | (config<<2); // TXD0
    output = 1      | (output<<1);
    config = INPUT  | (config<<2); // nRTS0
    output = NA     | (output<<1);
	config = INPUT  | (config<<2); // nCTS0
    output = NA     | (output<<1);

    rGPHDAT = output;
    rGPHCON = config;
	rGPHUP = determinePullup(config);

    //External interrupt will be falling edge triggered.
    // EINT9 (cs8900A) has the rising edge triggered  
    rEXTINT0 = 0x22222222;    // EINT[7:0]
    rEXTINT1 = 0x22222242;    // EINT[15:8]
    rEXTINT2 = 0x22222222;    // EINT[23:16]
}

