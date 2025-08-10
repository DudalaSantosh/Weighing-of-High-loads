#ifndef INC_ADS1232_H_
#define INC_ADS1232_H_

#include "stm32f4xx_hal.h"

typedef enum { NoERROR, TIMEOUT_HIGH, TIMEOUT_LOW, WOULD_BLOCK, STABLE_TIMEOUT, DIVIDED_by_ZERO } ERROR_t;
typedef enum { SLOW = 0, FAST } Speed;

typedef struct {
  GPIO_TypeDef* DOUT_Port;
  uint16_t DOUT_Pin;
  GPIO_TypeDef* SCLK_Port;
  uint16_t SCLK_Pin;
  GPIO_TypeDef* PDWN_Port;
  uint16_t PDWN_Pin;
  GPIO_TypeDef* SPEED_Port;
  uint16_t SPEED_Pin;
  GPIO_TypeDef* GAIN0_Port;
  uint16_t GAIN0_Pin;
  Speed speed;
  float OFFSET;
  float SCALE;
} ADS1232;

void ADS1232_Init(ADS1232* adc, GPIO_TypeDef* dout_port, uint16_t dout_pin,
                  GPIO_TypeDef* sclk_port, uint16_t sclk_pin,
                  GPIO_TypeDef* pdwn_port, uint16_t pdwn_pin,
                  GPIO_TypeDef* speed_port, uint16_t speed_pin,
                  GPIO_TypeDef* gain0_port, uint16_t gain0_pin,
                  Speed speed);
uint8_t ADS1232_IsReady(ADS1232* adc);
void ADS1232_PowerUp(ADS1232* adc);
void ADS1232_PowerDown(ADS1232* adc);
void ADS1232_SetSpeed(ADS1232* adc, Speed speed);
void ADS1232_SetGain(ADS1232* adc, uint8_t gain0);
ERROR_t ADS1232_Read(ADS1232* adc, int32_t* value, uint8_t calibrating);
ERROR_t ADS1232_ReadAverage(ADS1232* adc, float* value, uint8_t times, uint8_t calibrating);
ERROR_t ADS1232_GetValue(ADS1232* adc, float* value, uint8_t times, uint8_t calibrating);
ERROR_t ADS1232_GetUnits(ADS1232* adc, float* value, uint8_t times, uint8_t calibrating);
ERROR_t ADS1232_Tare(ADS1232* adc, uint8_t times, uint8_t calibrating);
float ADS1232_GetScale(ADS1232* adc);
void ADS1232_SetScale(ADS1232* adc, float scale);
float ADS1232_GetOffset(ADS1232* adc);
void ADS1232_SetOffset(ADS1232* adc, float offset);

#endif
