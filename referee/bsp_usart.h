#ifndef BSP_USART_H
#define BSP_USART_H
#include "struct_typedef.h"

#define T_SBUS_RX_BUF_NUM 36u
#define T_RC_FRAME_LENGTH  18u 

extern void usart6_init(uint8_t *rx1_buf, uint8_t *rx2_buf, uint16_t dma_buf_num);

extern void usart1_tx_dma_init(void);
extern void usart1_tx_dma_enable(uint8_t *data, uint16_t len);
void Gyro_DMA_Start(void);
#endif
