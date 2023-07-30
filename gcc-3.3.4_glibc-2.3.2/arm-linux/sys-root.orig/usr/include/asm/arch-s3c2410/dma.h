/*
 * linux/arch/arm/mach-s3c2410/dma.h
 *
 * Copyright (C) 2002 MIZI Research, Inc
 *
 * Author: Janghoon Lyu <nandy@mizi.com>
 * Date  : $Date: 2004/05/19 12:33:38 $
 *
 * $Revision: 1.2 $
 *
 * History:
 *
 * 2002-05-28: Janghoon Lyu <nandy@mizi.com>
 *    - copy from linux/arch/arm/mach-s3c2400
 *
 */
//#include "S3C2410.h"
#include <asm/arch/bitfield.h>
#include <asm/arch/s3c2410.h>
#include <asm/arch/hardware.h>

#define MAX_DMA_ADDRESS         0x20000000
#define MAX_DMA_TRANSFER_SIZE   0x100000 /* Data Unit is half word  */

/* DMA buffer struct */
//typedef unsigned int dmach_t;

typedef void (*dma_callback_t)( void *buf, int size );
extern int s3c2410_request_dma(const char *device_id, dmach_t channel,
                                dma_callback_t write_cb, dma_callback_t read_cb); 
extern int s3c2410_dma_queue_buffer(dmach_t channel, void *buf_id, 
                                        dma_addr_t data, int size, int write);
extern int s3c2410_dma_flush_all(dmach_t channel);
extern void s3c2410_free_dma(dmach_t channel);
extern int s3c2410_dma_get_current(dmach_t channel, void **buf_id, dma_addr_t *addr);
extern int s3c2410_dma_stop(dmach_t channel);
extern int s3c2410_init_dma(void);

#define MAX_S3C2410_DMA_CHANNELS  4
#define DMA_CH1  1
#define DMA_CH2  2
#define DMA_BUF_WR 1
#define DMA_BUF_RD 0 

typedef struct dma_buf_s {
	int size;		/* buffer size */
	dma_addr_t dma_start;	/* starting DMA address */
	int ref;		/* number of DMA references */
	void *id;		/* to identify buffer from outside */
	int write;		/* 1: buf to write , 0: but to read  */
	struct dma_buf_s *next;	/* next buf to process */
} dma_buf_t;

/* DMA control register structure */
typedef struct {
	volatile u_long DISRC;
	volatile u_long DISRCC;
	volatile u_long DIDST;
	volatile u_long DIDSTC;
	volatile u_long DCON;
	volatile u_long DSTAT;
	volatile u_long DCSRC;
	volatile u_long DCDST;
	volatile u_long DMASKTRIG;
} dma_regs_t;

/* DMA device structre */
typedef struct {
	dma_callback_t callback;
	u_long dst;
	u_long src;
	u_long ctl;
	u_long dst_ctl;
	u_long src_ctl;
} dma_device_t;

/* DMA channel structure */
typedef struct {
	dmach_t channel;
	unsigned int in_use;	/* Device is allocated */
	const char *device_id;	/* Device name */
	dma_buf_t *head;	/* where to insert buffers */
	dma_buf_t *tail;	/* where to remove buffers */
	dma_buf_t *curr;	/* buffer currently DMA'ed */
	unsigned long queue_count;	/* number of buffers in the queue */
	int active;		/* 1 if DMA is actually processing data */
	dma_regs_t *regs;	/* points to appropriate DMA registers */
	int irq;		/* IRQ used by the channel */
	dma_device_t write;	/* to write */
	dma_device_t read;	/* to read */
} s3c2410_dma_t;

extern s3c2410_dma_t dma_chan[MAX_S3C2410_DMA_CHANNELS];

/* 각 디바이스의 설정 값들 */
typedef struct {
	const char *name;
	u_long write_src;
	u_long write_dst;
	u_long write_ctl;
	u_long write_src_ctl;
	u_long write_dst_ctl;
	u_long read_src;
	u_long read_dst;
	u_long read_ctl;
	u_long read_src_ctl;
	u_long read_dst_ctl;
} dma_type_t;

#define fDMA_BASE_ADDR          Fld(30, 0)      /* base address of src/dst data */
#define DMA_BASE_ADDR(x)        FInsrt(x, fDMA_BASE_ADDR)
//#define DMA_BASE_ADDR(x)	(x&0x7fffffff)
#define fDCON_DSZ               Fld(2, 20)
#define DSZ(x)                  FInsrt(x, fDCON_DSZ)
#define readDSZ(x)              (x&(3<<20)) //FExtr(x, fDCON_DSZ)
//#define readDSZ(x)		(x&(3<<20))
//#define TX_CNT(x)		(x&0xfffff)
#define fDCON_TC                Fld(20,0)
#define TX_CNT(x)               FInsrt(x, fDCON_TC)

	// FIXME: fill in the rest (s2c2410x users manual, page 8-10)
#define HWSRC_CH0_UART0		(1<<24)
#define HWSRC_CH1_I2SSDI	(2<<24)
#define HWSRC_CH2_I2SSDO	(0<<24)

#define DSZ_BYTE		(0<<20)
#define DSZ_WORD		(2<<20)
#define DSZ_HALFWORD		(1<<20)
#define DSZ_RESERVED		(3<<20)

#define DMA_UNDEF		0xffffffff	/* not available */
#define BUF_ON_MEM		(LOC_AHB | ADDR_INC)
#define BUF_ON_APB		(LOC_APB | ADDR_FIX)

#define UART0_MEM		0x0
#define UART0_CTL		(DCON_HS_MODE | DCON_INT_ENABLE | \
				 HWSRC_CH0_UART0 | DCON_HW_SEL | \
				 DCON_NO_AUTO_RELOAD | DSZ_BYTE | 0)
#define UART0_WR_SRC		UART0_MEM
#define UART0_WR_DST		0x50000020	/* UTXH0 */
#define UART0_WR_CTL		UART0_CTL
#define UART0_RD_SRC		0x50000024	/* URXH0 */
#define UART0_RD_DST		UART0_MEM
#define UART0_RD_CTL		UART0_CTL
#define UART0_WR_SRC_CTL	BUF_ON_MEM
#define UART0_WR_DST_CTL	BUF_ON_APB
#define UART0_RD_SRC_CTL	BUF_ON_APB
#define UART0_RD_DST_CTL	BUF_ON_MEM

#define UART1_WR_SRC		DMA_UNDEF
#define UART1_WR_DST		DMA_UNDEF
#define UART1_WR_CTL		DMA_UNDEF
#define UART1_RD_SRC		DMA_UNDEF
#define UART1_RD_DST		DMA_UNDEF
#define UART1_RD_CTL		DMA_UNDEF
#define UART1_WR_SRC_CTL	DMA_UNDEF		
#define UART1_WR_DST_CTL	DMA_UNDEF
#define UART1_RD_SRC_CTL	DMA_UNDEF
#define UART1_RD_DST_CTL	DMA_UNDEF

#define UART2_WR_SRC		DMA_UNDEF
#define UART2_WR_DST		DMA_UNDEF
#define UART2_WR_CTL		DMA_UNDEF
#define UART2_RD_SRC		DMA_UNDEF
#define UART2_RD_DST		DMA_UNDEF
#define UART2_RD_CTL		DMA_UNDEF
#define UART2_WR_SRC_CTL	DMA_UNDEF		
#define UART2_WR_DST_CTL	DMA_UNDEF
#define UART2_RD_SRC_CTL	DMA_UNDEF
#define UART2_RD_DST_CTL	DMA_UNDEF

#define I2SSDO_CTL		(DCON_HS_MODE | DCON_INT_ENABLE |   \
				 HWSRC_CH2_I2SSDO | DCON_HW_SEL | \
				DCON_NO_AUTO_RELOAD | DSZ_HALFWORD | 0)

#define I2SSDI_CTL              (DCON_HS_MODE | DCON_INT_ENABLE |  \
                                 HWSRC_CH1_I2SSDI | DCON_HW_SEL | \
                                 DCON_NO_AUTO_RELOAD | DSZ_HALFWORD | 0)
                
#define I2SSDO_WR_SRC		0x0
#define I2SSDO_WR_DST		0x55000010
#define I2SSDO_WR_CTL		I2SSDO_CTL
#define I2SSDO_RD_SRC		DMA_UNDEF
#define I2SSDO_RD_DST		DMA_UNDEF
#define I2SSDO_RD_CTL		DMA_UNDEF
#define I2SSDO_WR_SRC_CTL	BUF_ON_MEM	
#define I2SSDO_WR_DST_CTL	BUF_ON_APB
#define I2SSDO_RD_SRC_CTL	DMA_UNDEF
#define I2SSDO_RD_DST_CTL	DMA_UNDEF

#define I2SSDI_WR_SRC           DMA_UNDEF
#define I2SSDI_WR_DST           DMA_UNDEF
#define I2SSDI_WR_CTL           DMA_UNDEF
#define I2SSDI_RD_SRC           0x55000010             
#define I2SSDI_RD_DST           0x2        
#define I2SSDI_RD_CTL           I2SSDI_CTL
#define I2SSDI_WR_SRC_CTL       DMA_UNDEF               
#define I2SSDI_WR_DST_CTL       DMA_UNDEF
#define I2SSDI_RD_SRC_CTL       BUF_ON_APB
#define I2SSDI_RD_DST_CTL       BUF_ON_MEM 

#define USB_WR_SRC		DMA_UNDEF
#define USB_WR_DST		DMA_UNDEF
#define USB_WR_CTL		DMA_UNDEF
#define USB_RD_SRC		DMA_UNDEF
#define USB_RD_DST		DMA_UNDEF
#define USB_RD_CTL		DMA_UNDEF
#define USB_WR_SRC_CTL		DMA_UNDEF		
#define USB_WR_DST_CTL		DMA_UNDEF
#define USB_RD_SRC_CTL		DMA_UNDEF
#define USB_RD_DST_CTL		DMA_UNDEF

#define MMC_WR_SRC		DMA_UNDEF
#define MMC_WR_DST		DMA_UNDEF
#define MMC_WR_CTL		DMA_UNDEF
#define MMC_RD_SRC		DMA_UNDEF
#define MMC_RD_DST		DMA_UNDEF
#define MMC_RD_CTL		DMA_UNDEF
#define MMC_WR_SRC_CTL		DMA_UNDEF		
#define MMC_WR_DST_CTL		DMA_UNDEF
#define MMC_RD_SRC_CTL		DMA_UNDEF
#define MMC_RD_DST_CTL		DMA_UNDEF

#define SPI_WR_SRC		DMA_UNDEF
#define SPI_WR_DST		DMA_UNDEF
#define SPI_WR_CTL		DMA_UNDEF
#define SPI_RD_SRC		DMA_UNDEF
#define SPI_RD_DST		DMA_UNDEF
#define SPI_RD_CTL		DMA_UNDEF
#define SPI_WR_SRC_CTL		DMA_UNDEF		
#define SPI_WR_DST_CTL		DMA_UNDEF
#define SPI_RD_SRC_CTL		DMA_UNDEF
#define SPI_RD_DST_CTL		DMA_UNDEF

#define TIMER_WR_SRC		DMA_UNDEF
#define TIMER_WR_DST		DMA_UNDEF
#define TIMER_WR_CTL		DMA_UNDEF
#define TIMER_RD_SRC		DMA_UNDEF
#define TIMER_RD_DST		DMA_UNDEF
#define TIMER_RD_CTL		DMA_UNDEF
#define TIMER_WR_SRC_CTL	DMA_UNDEF		
#define TIMER_WR_DST_CTL	DMA_UNDEF
#define TIMER_RD_SRC_CTL	DMA_UNDEF
#define TIMER_RD_DST_CTL	DMA_UNDEF

#define XDREQ0_WR_SRC		DMA_UNDEF
#define XDREQ0_WR_DST		DMA_UNDEF
#define XDREQ0_WR_CTL		DMA_UNDEF
#define XDREQ0_RD_SRC		DMA_UNDEF
#define XDREQ0_RD_DST		DMA_UNDEF
#define XDREQ0_RD_CTL		DMA_UNDEF
#define XDREQ0_WR_SRC_CTL	DMA_UNDEF		
#define XDREQ0_WR_DST_CTL	DMA_UNDEF
#define XDREQ0_RD_SRC_CTL	DMA_UNDEF
#define XDREQ0_RD_DST_CTL	DMA_UNDEF

#define XDREQ1_WR_SRC		DMA_UNDEF
#define XDREQ1_WR_DST		DMA_UNDEF
#define XDREQ1_WR_CTL		DMA_UNDEF
#define XDREQ1_RD_SRC		DMA_UNDEF
#define XDREQ1_RD_DST		DMA_UNDEF
#define XDREQ1_RD_CTL		DMA_UNDEF
#define XDREQ1_WR_SRC_CTL	DMA_UNDEF		
#define XDREQ1_WR_DST_CTL	DMA_UNDEF
#define XDREQ1_RD_SRC_CTL	DMA_UNDEF
#define XDREQ1_RD_DST_CTL	DMA_UNDEF

/*
static dma_type_t dma_types[4][5] = {
{
	{ "XDREQ0", XDREQ0_WR_SRC, XDREQ0_WR_DST, XDREQ0_WR_CTL, \
		    XDREQ0_WR_SRC_CTL, XDREQ0_WR_DST_CTL, \
		    XDREQ0_RD_SRC, XDREQ0_RD_DST, XDREQ0_RD_CTL, \
		    XDREQ0_RD_SRC_CTL, XDREQ0_RD_DST_CTL },
	{ "UART0",  UART0_WR_SRC, UART0_WR_DST, UART0_WR_CTL, \
		    UART0_WR_SRC_CTL, UART0_WR_DST_CTL, \
		    UART0_RD_SRC, UART0_RD_DST, UART0_RD_CTL, \
		    UART0_RD_SRC_CTL, UART0_RD_DST_CTL },
	{ "MMC",    MMC_WR_SRC, MMC_WR_DST, MMC_WR_CTL, \
		    MMC_WR_SRC_CTL, MMC_WR_DST_CTL, \
		    MMC_RD_SRC, MMC_RD_SRC, MMC_RD_CTL, \
		    MMC_RD_SRC_CTL, MMC_RD_DST_CTL },
	{ "TIMER",  TIMER_WR_SRC, TIMER_WR_DST, TIMER_WR_CTL, \
		    TIMER_WR_SRC_CTL, TIMER_WR_DST_CTL, \
		    TIMER_RD_SRC, TIMER_RD_DST, TIMER_RD_CTL, \
		    TIMER_RD_SRC_CTL, TIMER_RD_DST_CTL },
	{ "USB",    USB_WR_SRC, USB_WR_DST, USB_WR_CTL, \
		    USB_WR_SRC_CTL, USB_WR_DST_CTL, \
		    USB_RD_SRC, USB_RD_DST, USB_RD_CTL, \
		    USB_RD_SRC_CTL, USB_RD_DST_CTL }
},
{
	{ "XDREQ1", XDREQ1_WR_SRC, XDREQ1_WR_DST, XDREQ1_WR_CTL, \
		    XDREQ1_WR_SRC_CTL, XDREQ1_WR_DST_CTL, \
		    XDREQ1_RD_SRC, XDREQ1_RD_DST, XDREQ1_RD_CTL, \
		    XDREQ1_RD_SRC_CTL, XDREQ1_RD_DST_CTL },
	{ "UART1",  UART1_WR_SRC, UART1_WR_DST, UART1_WR_CTL, \
		    UART1_WR_SRC_CTL, UART1_WR_DST_CTL, \
		    UART1_RD_SRC, UART1_RD_DST, UART1_RD_CTL, \
		    UART1_RD_SRC_CTL, UART1_RD_DST_CTL },
	{ "I2SSDI", I2SSDI_WR_SRC, I2SSDI_WR_DST, I2SSDI_WR_CTL, \
		    I2SSDI_WR_SRC_CTL, I2SSDI_WR_DST_CTL, \
		    I2SSDI_RD_SRC, I2SSDI_RD_DST, I2SSDI_RD_CTL, \
		    I2SSDI_RD_SRC_CTL, I2SSDI_RD_DST_CTL },
	{ "SPI",    SPI_WR_SRC, SPI_WR_DST, SPI_WR_CTL, \
		    SPI_WR_SRC_CTL, SPI_WR_DST_CTL, \
		    SPI_RD_SRC, SPI_RD_DST, SPI_WR_CTL, \
		    SPI_RD_SRC_CTL, SPI_RD_DST_CTL },
	{ "USB",    USB_WR_SRC, USB_WR_DST, USB_WR_CTL, \
		    USB_WR_SRC_CTL, USB_WR_DST_CTL, \
		    USB_RD_SRC, USB_RD_DST, USB_RD_CTL, \
		    USB_RD_SRC_CTL, USB_RD_DST_CTL }

},
{
	{ "I2SSDO", I2SSDO_WR_SRC, I2SSDO_WR_DST, I2SSDO_WR_CTL, \
		    I2SSDO_WR_SRC_CTL, I2SSDO_WR_DST_CTL, \
		    I2SSDO_RD_SRC, I2SSDO_RD_DST, I2SSDO_RD_CTL, \
		    I2SSDO_RD_SRC_CTL, I2SSDO_RD_DST_CTL },
	{ "I2SSDI", I2SSDI_WR_SRC, I2SSDI_WR_DST, I2SSDI_WR_CTL, \
		    I2SSDI_WR_SRC_CTL, I2SSDI_WR_DST_CTL, \
		    I2SSDI_RD_SRC, I2SSDI_RD_DST, I2SSDI_RD_CTL, \
		    I2SSDI_RD_SRC_CTL, I2SSDI_RD_DST_CTL },
	{ "MMC",    MMC_WR_SRC, MMC_WR_DST, MMC_WR_CTL, \
		    MMC_WR_SRC_CTL, MMC_WR_DST_CTL, \
		    MMC_RD_SRC, MMC_RD_SRC, MMC_RD_CTL, \
		    MMC_RD_SRC_CTL, MMC_RD_DST_CTL },
	{ "TIMER",  TIMER_WR_SRC, TIMER_WR_DST, TIMER_WR_CTL, \
		    TIMER_WR_SRC_CTL, TIMER_WR_DST_CTL, \
		    TIMER_RD_SRC, TIMER_RD_DST, TIMER_RD_CTL, \
		    TIMER_RD_SRC_CTL, TIMER_RD_DST_CTL },
	{ "USB",    USB_WR_SRC, USB_WR_DST, USB_WR_CTL, \
		    USB_WR_SRC_CTL, USB_WR_DST_CTL, \
		    USB_RD_SRC, USB_RD_DST, USB_RD_CTL, \
		    USB_RD_SRC_CTL, USB_RD_DST_CTL }
},
{
	{ "UART2",  UART2_WR_SRC, UART2_WR_DST, UART2_WR_CTL, \
		    UART2_WR_SRC_CTL, UART2_WR_DST_CTL, \
		    UART2_RD_SRC, UART2_RD_DST, UART2_RD_CTL, \
		    UART2_RD_SRC_CTL, UART2_RD_DST_CTL },
	{ "MMC",    MMC_WR_SRC, MMC_WR_DST, MMC_WR_CTL, \
		    MMC_WR_SRC_CTL, MMC_WR_DST_CTL, \
		    MMC_RD_SRC, MMC_RD_SRC, MMC_RD_CTL, \
		    MMC_RD_SRC_CTL, MMC_RD_DST_CTL },
	{ "SPI",    SPI_WR_SRC, SPI_WR_DST, SPI_WR_CTL, \
		    SPI_WR_SRC_CTL, SPI_WR_DST_CTL, \
		    SPI_RD_SRC, SPI_RD_DST, SPI_WR_CTL, \
		    SPI_RD_SRC_CTL, SPI_RD_DST_CTL },
	{ "TIMER",  TIMER_WR_SRC, TIMER_WR_DST, TIMER_WR_CTL, \
		    TIMER_WR_SRC_CTL, TIMER_WR_DST_CTL, \
		    TIMER_RD_SRC, TIMER_RD_DST, TIMER_RD_CTL, \
		    TIMER_RD_SRC_CTL, TIMER_RD_DST_CTL },
	{ "USB",    USB_WR_SRC, USB_WR_DST, USB_WR_CTL, \
		    USB_WR_SRC_CTL, USB_WR_DST_CTL, \
		    USB_RD_SRC, USB_RD_DST, USB_RD_CTL, \
		    USB_RD_SRC_CTL, USB_RD_DST_CTL }
}
};
i*/

#if !defined(CONFIG_S3C2410_MPORT3) && !defined(CONFIG_S3C2410_MPORT1)
/* FIXME:
 * 왜 TCON설정을 이렇게 해야 될까요? 
 * TCON |= (TIMER3_ATLOAD_ON | TIMER3_ON | TIMER3_NOP);
 * 라는 명령어 제대로 먹혀들지 않습니다. 저의 불찰인데....
 * S3C2410.h에 정의되어 있는 내용과 관련이 있을 것 같습니다.
 */
/*
 * TCNTB3 는 하드코딩 했습니다. 여기서 전제 조건은 TIMER 4를
 * 사용하기 위해서 linux/include/asm-arm/arch-s3c2410/time.h에서
 * TIMER 3에 관련된 셋팅이 이미 되어 있어야 된다는 것입니다.
 */
//#define HOOK_LOST_INT
#ifdef HOOK_LOST_INT
#define stop_dma_timer()	({ TCON |= TIMER3_OFF; })
#define start_dma_timer() \
	({ TCNTB3 = 15626; 	/* less than 10ms */  \
	   TCON = (TCON | TIMER3_MANUP | TIMER3_OFF); \
	   TCON = (TCON | TIMER3_ATLOAD_ON | TIMER3_ON | TIMER3_NOP); })
#endif
#endif /* nCONFIG_S3C2410_MPORT3 */
