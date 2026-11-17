#include "drv_uart.h"
#include "ti_msp_dl_config.h"
#include <stddef.h>
#include <string.h>

/* ── 接收回调 ── */
static UART_RX_Callback g_gyro_callback = NULL;
static UART_RX_Callback g_bluetooth_callback = NULL;

/* ── 非阻塞发送缓冲区（每个UART一个）── */
#define TX_BUF_SIZE 256

typedef struct {
    uint8_t buffer[TX_BUF_SIZE];
    volatile uint16_t head;  /* 写入位置 */
    volatile uint16_t tail;  /* 读取位置 */
    volatile bool busy;      /* 发送忙碌标志 */
} TX_BufferDef;

static TX_BufferDef g_tx_bluetooth = {0};
static TX_BufferDef g_tx_gyro = {0};

void Drv_UART_Init(UART_Regs *uart_inst)
{
    DL_UART_Main_enable(uart_inst);
    DL_UART_Main_enableInterrupt(uart_inst, DL_UART_MAIN_INTERRUPT_RX);

    /* ── Enable NVIC for this UART ──
       SysConfig only sets priority (NVIC_SetPriority). The NVIC must be
       explicitly enabled here or the interrupt never reaches the CPU.    */
    if (uart_inst == UART_GYRO_INST) {
        NVIC_ClearPendingIRQ(UART_GYRO_INST_INT_IRQN);
        NVIC_EnableIRQ(UART_GYRO_INST_INT_IRQN);
    } else if (uart_inst == UART_BT_INST) {
        NVIC_ClearPendingIRQ(UART_BT_INST_INT_IRQN);
        NVIC_EnableIRQ(UART_BT_INST_INT_IRQN);
    }
}

/**
 * @brief 非阻塞式发送单个字节（放入缓冲区）
 */
void Drv_UART_SendByte(UART_Regs *uart_inst, uint8_t data)
{
    Drv_UART_SendData(uart_inst, &data, 1);
}

/**
 * @brief 非阻塞式发送数据（放入缓冲区）
 * @note 保持接口不变，但改为非阻塞实现
 */
void Drv_UART_SendData(UART_Regs *uart_inst, const uint8_t *data, uint16_t len)
{
    if (data == NULL || len == 0) return;

    /* 选择对应的缓冲区 */
    TX_BufferDef *tx_buf = NULL;
    if (uart_inst == UART_BT_INST) {
        tx_buf = &g_tx_bluetooth;
    } else if (uart_inst == UART_GYRO_INST) {
        tx_buf = &g_tx_gyro;
    } else {
        return;  /* 不支持的UART */
    }

    /* 复制数据到环形缓冲区（非阻塞） */
    for (uint16_t i = 0; i < len; i++) {
        uint16_t next_head = (tx_buf->head + 1) % TX_BUF_SIZE;
        
        /* 如果缓冲区满，丢弃数据（非阻塞） */
        if (next_head == tx_buf->tail) {
            break;  /* 缓冲区满，停止发送 */
        }
        
        tx_buf->buffer[tx_buf->head] = data[i];
        tx_buf->head = next_head;
    }
}

/**
 * @brief 处理发送（在主循环中调用）
 */
void Drv_UART_ProcessTX(void)
{
    /* 处理蓝牙UART发送 - 只发送一个字节 */
    if (g_tx_bluetooth.head != g_tx_bluetooth.tail) {
        /* 检查TX FIFO是否不满（用if，不用while） */
        if (!DL_UART_Main_isTXFIFOFull(UART_BT_INST)) {
            /* 从缓冲区读取并发送一个字节 */
            uint8_t data = g_tx_bluetooth.buffer[g_tx_bluetooth.tail];
            g_tx_bluetooth.tail = (g_tx_bluetooth.tail + 1) % TX_BUF_SIZE;
            
            DL_UART_Main_transmitData(UART_BT_INST, data);
        }
        /* 如果FIFO满，不等待，等待下次循环 */
    }
    
    /* 处理陀螺仪UART发送 - 只发送一个字节 */
    if (g_tx_gyro.head != g_tx_gyro.tail) {
        if (!DL_UART_Main_isTXFIFOFull(UART_GYRO_INST)) {
            uint8_t data = g_tx_gyro.buffer[g_tx_gyro.tail];
            g_tx_gyro.tail = (g_tx_gyro.tail + 1) % TX_BUF_SIZE;
            
            DL_UART_Main_transmitData(UART_GYRO_INST, data);
        }
    }
}

void Drv_UART_RegisterCallback(UART_Regs *uart_inst, UART_RX_Callback callback)
{
    if (uart_inst == UART_GYRO_INST) {
        g_gyro_callback = callback;
    } else if (uart_inst == UART_BT_INST) {
        g_bluetooth_callback = callback;
    } else if (uart_inst == UART_COMM_INST) {
    }
}

/* ── UART1 (GYRO) Interrupt Handler ──
   Matches UART1_IRQHandler in startup_mspm0g350x_ticlang.c           */
void UART1_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_GYRO_INST)) {
        case DL_UART_MAIN_IIDX_RX:
            while (!DL_UART_Main_isRXFIFOEmpty(UART_GYRO_INST)) {
                uint8_t data = DL_UART_Main_receiveData(UART_GYRO_INST);
                if (g_gyro_callback != NULL) {
                    g_gyro_callback(data);
                }
            }
            break;
        default:
            break;
    }
}

/* ── UART2 (Bluetooth) Interrupt Handler ──
   Matches UART2_IRQHandler in startup_mspm0g350x_ticlang.c          */
void UART2_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_BT_INST)) {
        case DL_UART_MAIN_IIDX_RX:
            while (!DL_UART_Main_isRXFIFOEmpty(UART_BT_INST)) {
                uint8_t data = DL_UART_Main_receiveData(UART_BT_INST);
                if (g_bluetooth_callback != NULL) {
                    g_bluetooth_callback(data);
                }
            }
            break;
        default:
            break;
    }
}

bool Drv_UART_Receive(UART_Regs *uart_inst, uint8_t *data, uint16_t len)
{
    if (DL_UART_Main_isRXFIFOEmpty(uart_inst) == true) {
        return false;
    }
    *data = DL_UART_Main_receiveData(uart_inst);
    return true;
}
