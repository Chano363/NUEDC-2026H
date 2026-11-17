#include "motor_drv.h"
#include "../Drv/drv_pwm.h"
#include "ti_msp_dl_config.h"

/*
 * 引脚映射（硬件实际连接）：
 *   左电机 = AT8236 A通道 (AIN1=PA15, AIN2=PB6):
 *     PWM: PWM_MOTOR_L_INST (TIMA1, PA15)
 *     DIR: MOTOR_DIR_A_MTR_AIN_PIN (PB6)
 *   右电机 = AT8236 B通道 (BIN1=PA14, BIN2=PA13):
 *     PWM: PWM_MOTOR_R_INST (TIMG12, PA14)
 *     DIR: MOTOR_DIR_B_MTR_BIN_PIN (PA13)
 */

#define MOTOR_DEADBAND  0

/* 左电机硬件资源 */
#define LEFT_PWM_INST   PWM_MOTOR_L_INST
#define LEFT_DIR_PORT   MOTOR_DIR_A_PORT
#define LEFT_DIR_PIN    MOTOR_DIR_A_MTR_AIN_PIN
#define LEFT_DIR_IOMUX  MOTOR_DIR_A_MTR_AIN_IOMUX

/* 右电机硬件资源 */
#define RIGHT_PWM_INST  PWM_MOTOR_R_INST
#define RIGHT_DIR_PORT  MOTOR_DIR_B_PORT
#define RIGHT_DIR_PIN   MOTOR_DIR_B_MTR_BIN_PIN
#define RIGHT_DIR_IOMUX MOTOR_DIR_B_MTR_BIN_IOMUX

static int16_t MotorDrv_ClampSpeed(int16_t speed)
{
    if (speed > MOTOR_PWM_MAX) return MOTOR_PWM_MAX;
    if (speed < -MOTOR_PWM_MAX) return -MOTOR_PWM_MAX;
    if (speed > -MOTOR_DEADBAND && speed < MOTOR_DEADBAND) return 0;
    return speed;
}

static uint16_t MotorDrv_SpeedToDuty(int16_t speed)
{
    return (uint16_t)speed * 200u / MOTOR_PWM_MAX;
}

static void MotorDrv_StopLeft(void)
{
    Drv_PWM_SetDuty(LEFT_PWM_INST, 0, 0);
    DL_GPIO_clearPins(LEFT_DIR_PORT, LEFT_DIR_PIN);
}

static void MotorDrv_StopRight(void)
{
    Drv_PWM_SetDuty(RIGHT_PWM_INST, 0, 0);
    DL_GPIO_clearPins(RIGHT_DIR_PORT, RIGHT_DIR_PIN);
}

void MotorDrv_Init(void)
{
    DL_GPIO_initDigitalOutput(LEFT_DIR_IOMUX);
    DL_GPIO_clearPins(LEFT_DIR_PORT, LEFT_DIR_PIN);
    DL_GPIO_enableOutput(LEFT_DIR_PORT, LEFT_DIR_PIN);
    DL_GPIO_initDigitalOutput(RIGHT_DIR_IOMUX);
    DL_GPIO_clearPins(RIGHT_DIR_PORT, RIGHT_DIR_PIN);
    DL_GPIO_enableOutput(RIGHT_DIR_PORT, RIGHT_DIR_PIN);

    Drv_PWM_Init();
    MotorDrv_Stop();

    Drv_PWM_Start(LEFT_PWM_INST);
    Drv_PWM_Start(RIGHT_PWM_INST);
}

void MotorDrv_SetSpeed(int16_t left_speed, int16_t right_speed)
{
    left_speed = MotorDrv_ClampSpeed(left_speed);
    right_speed = MotorDrv_ClampSpeed(right_speed);

    if (left_speed > 0) {
        Drv_PWM_SetDuty(LEFT_PWM_INST, 0, MotorDrv_SpeedToDuty(left_speed));
        DL_GPIO_clearPins(LEFT_DIR_PORT, LEFT_DIR_PIN);
    } else if (left_speed < 0) {
        uint16_t duty = MotorDrv_SpeedToDuty(-left_speed);
        Drv_PWM_SetDuty(LEFT_PWM_INST, 0, 200u - duty);
        DL_GPIO_setPins(LEFT_DIR_PORT, LEFT_DIR_PIN);
    } else {
        MotorDrv_StopLeft();
    }

    /* B10 is physical reverse and B01 is physical forward. */
    if (right_speed > 0) {
        uint16_t duty = MotorDrv_SpeedToDuty(right_speed);
        Drv_PWM_SetDuty(RIGHT_PWM_INST, 0, 200u - duty);
        DL_GPIO_setPins(RIGHT_DIR_PORT, RIGHT_DIR_PIN);
    } else if (right_speed < 0) {
        Drv_PWM_SetDuty(RIGHT_PWM_INST, 0, MotorDrv_SpeedToDuty(-right_speed));
        DL_GPIO_clearPins(RIGHT_DIR_PORT, RIGHT_DIR_PIN);
    } else {
        MotorDrv_StopRight();
    }
}

void MotorDrv_Stop(void)
{
    MotorDrv_StopLeft();
    MotorDrv_StopRight();
}
