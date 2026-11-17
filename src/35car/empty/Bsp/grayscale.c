#include "grayscale.h"
#include "ti_msp_dl_config.h"

static const int16_t g_weights[8] = {35, 25, 15, 5, -5, -15, -25, -35};
/* S1检测到→deviation=+35→小车偏右→yaw为负→左转回中 */

void Grayscale_Init(void)
{
    DL_GPIO_initDigitalInput(GRAYSCALE_A_GRAY_CH1_IOMUX);
    DL_GPIO_initDigitalInput(GRAYSCALE_A_GRAY_CH2_IOMUX);
    DL_GPIO_initDigitalInput(GRAYSCALE_B_GRAY_CH3_IOMUX);
    DL_GPIO_initDigitalInput(GRAYSCALE_B_GRAY_CH4_IOMUX);
    DL_GPIO_initDigitalInput(GRAYSCALE_B_GRAY_CH5_IOMUX);
    DL_GPIO_initDigitalInput(GRAYSCALE_B_GRAY_CH6_IOMUX);
    DL_GPIO_initDigitalInput(GRAYSCALE_B_GRAY_CH7_IOMUX);
    DL_GPIO_initDigitalInput(GRAYSCALE_B_GRAY_CH8_IOMUX);
}

void Grayscale_Update(Grayscale_DataDef *data)
{
    if (data == NULL) return;
    
    data->digital_status = 0;
    
    /* 传感器：白地=GPIO LOW, 黑线=GPIO HIGH
       bit=1 = 检测到黑线（赛道） */
    data->digital_status |= (DL_GPIO_readPins(GRAYSCALE_A_PORT, GRAYSCALE_A_GRAY_CH1_PIN) ? 0x01 : 0x00) << 0;
    data->digital_status |= (DL_GPIO_readPins(GRAYSCALE_A_PORT, GRAYSCALE_A_GRAY_CH2_PIN) ? 0x01 : 0x00) << 1;
    data->digital_status |= (DL_GPIO_readPins(GRAYSCALE_B_PORT, GRAYSCALE_B_GRAY_CH3_PIN) ? 0x01 : 0x00) << 2;
    data->digital_status |= (DL_GPIO_readPins(GRAYSCALE_B_PORT, GRAYSCALE_B_GRAY_CH4_PIN) ? 0x01 : 0x00) << 3;
    data->digital_status |= (DL_GPIO_readPins(GRAYSCALE_B_PORT, GRAYSCALE_B_GRAY_CH5_PIN) ? 0x01 : 0x00) << 4;
    data->digital_status |= (DL_GPIO_readPins(GRAYSCALE_B_PORT, GRAYSCALE_B_GRAY_CH6_PIN) ? 0x01 : 0x00) << 5;
    data->digital_status |= (DL_GPIO_readPins(GRAYSCALE_B_PORT, GRAYSCALE_B_GRAY_CH7_PIN) ? 0x01 : 0x00) << 6;
    data->digital_status |= (DL_GPIO_readPins(GRAYSCALE_B_PORT, GRAYSCALE_B_GRAY_CH8_PIN) ? 0x01 : 0x00) << 7;
    
    int32_t sum_weight = 0;
    int32_t sum_sensor = 0;
    
    for (int i = 0; i < 8; i++) {
        if (data->digital_status & (1 << i)) {
            sum_weight += g_weights[i];
            sum_sensor++;
        }
    }
    
    if (sum_sensor == 0) {
        data->deviation = 0;
        data->line_status = GRAYSCALE_LINE_LOST;
    } else {
        data->deviation = (int16_t)(sum_weight / sum_sensor);
        data->line_status = GRAYSCALE_LINE_ON;
    }

    /* 硬件接线 bit0(S1)=最左, bit7(S8)=最右*/
}

uint8_t Grayscale_GetDigitalStatus(void)
{
    uint8_t status = 0;
    
    /* 传感器：白地=GPIO LOW, 黑线=GPIO HIGH */
    status |= (DL_GPIO_readPins(GRAYSCALE_A_PORT, GRAYSCALE_A_GRAY_CH1_PIN) ? 0x01 : 0x00) << 0;
    status |= (DL_GPIO_readPins(GRAYSCALE_A_PORT, GRAYSCALE_A_GRAY_CH2_PIN) ? 0x01 : 0x00) << 1;
    status |= (DL_GPIO_readPins(GRAYSCALE_B_PORT, GRAYSCALE_B_GRAY_CH3_PIN) ? 0x01 : 0x00) << 2;
    status |= (DL_GPIO_readPins(GRAYSCALE_B_PORT, GRAYSCALE_B_GRAY_CH4_PIN) ? 0x01 : 0x00) << 3;
    status |= (DL_GPIO_readPins(GRAYSCALE_B_PORT, GRAYSCALE_B_GRAY_CH5_PIN) ? 0x01 : 0x00) << 4;
    status |= (DL_GPIO_readPins(GRAYSCALE_B_PORT, GRAYSCALE_B_GRAY_CH6_PIN) ? 0x01 : 0x00) << 5;
    status |= (DL_GPIO_readPins(GRAYSCALE_B_PORT, GRAYSCALE_B_GRAY_CH7_PIN) ? 0x01 : 0x00) << 6;
    status |= (DL_GPIO_readPins(GRAYSCALE_B_PORT, GRAYSCALE_B_GRAY_CH8_PIN) ? 0x01 : 0x00) << 7;
    
    return status;
}
