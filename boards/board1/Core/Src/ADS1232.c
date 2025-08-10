#include "ADS1232.h"

void ADS1232_Init(ADS1232* adc, GPIO_TypeDef* dout_port, uint16_t dout_pin,
                  GPIO_TypeDef* sclk_port, uint16_t sclk_pin,
                  GPIO_TypeDef* pdwn_port, uint16_t pdwn_pin,
                  GPIO_TypeDef* speed_port, uint16_t speed_pin,
                  GPIO_TypeDef* gain0_port, uint16_t gain0_pin,
                  Speed speed) {
  adc->DOUT_Port = dout_port;
  adc->DOUT_Pin = dout_pin;
  adc->SCLK_Port = sclk_port;
  adc->SCLK_Pin = sclk_pin;
  adc->PDWN_Port = pdwn_port;
  adc->PDWN_Pin = pdwn_pin;
  adc->SPEED_Port = speed_port;
  adc->SPEED_Pin = speed_pin;
  adc->GAIN0_Port = gain0_port;
  adc->GAIN0_Pin = gain0_pin;
  adc->speed = speed;
  adc->OFFSET = 0;
  adc->SCALE = 1;

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Pin = dout_pin;
  HAL_GPIO_Init(dout_port, &GPIO_InitStruct);

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = sclk_pin;
  HAL_GPIO_Init(sclk_port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = pdwn_pin;
  HAL_GPIO_Init(pdwn_port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = speed_pin;
  HAL_GPIO_Init(speed_port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = gain0_pin;
  HAL_GPIO_Init(gain0_port, &GPIO_InitStruct);

  ADS1232_SetSpeed(adc, speed);
  HAL_Delay(1);
  ADS1232_PowerUp(adc);
}

uint8_t ADS1232_IsReady(ADS1232* adc) {
  return HAL_GPIO_ReadPin(adc->DOUT_Port, adc->DOUT_Pin) == GPIO_PIN_RESET;
}

void ADS1232_PowerUp(ADS1232* adc) {
  HAL_GPIO_WritePin(adc->PDWN_Port, adc->PDWN_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(adc->SCLK_Port, adc->SCLK_Pin, GPIO_PIN_RESET);
}

void ADS1232_PowerDown(ADS1232* adc) {
  HAL_GPIO_WritePin(adc->PDWN_Port, adc->PDWN_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(adc->SCLK_Port, adc->SCLK_Pin, GPIO_PIN_SET);
}

void ADS1232_SetSpeed(ADS1232* adc, Speed speed) {
  adc->speed = speed;
  HAL_GPIO_WritePin(adc->SPEED_Port, adc->SPEED_Pin, speed == FAST ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void ADS1232_SetGain(ADS1232* adc, uint8_t gain0) {
  HAL_GPIO_WritePin(adc->GAIN0_Port, adc->GAIN0_Pin, gain0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

ERROR_t ADS1232_Read(ADS1232* adc, int32_t* value, uint8_t calibrating) {
  uint32_t start = HAL_GetTick();
  uint32_t wait = (adc->speed == FAST ? 20 : 150) + 600;
  while(HAL_GPIO_ReadPin(adc->DOUT_Port, adc->DOUT_Pin) != GPIO_PIN_SET) {
    if(HAL_GetTick() - start > wait) return TIMEOUT_HIGH;
  }
  start = HAL_GetTick();
  while(HAL_GPIO_ReadPin(adc->DOUT_Port, adc->DOUT_Pin) != GPIO_PIN_RESET) {
    if(HAL_GetTick() - start > wait) return TIMEOUT_LOW;
  }
  int32_t val = 0;
  for(int i = 0; i < 24; i++) {
    HAL_GPIO_WritePin(adc->SCLK_Port, adc->SCLK_Pin, GPIO_PIN_SET);
    val = (val << 1) | (HAL_GPIO_ReadPin(adc->DOUT_Port, adc->DOUT_Pin) == GPIO_PIN_SET);
    HAL_GPIO_WritePin(adc->SCLK_Port, adc->SCLK_Pin, GPIO_PIN_RESET);
  }
  if(calibrating) {
    for(int i = 0; i < 2; i++) {
      HAL_GPIO_WritePin(adc->SCLK_Port, adc->SCLK_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(adc->SCLK_Port, adc->SCLK_Pin, GPIO_PIN_RESET);
    }
    HAL_Delay(adc->speed == FAST ? 100 : 800);
  }
  *value = (val << 8) / 256;
  if(!calibrating) {
    HAL_GPIO_WritePin(adc->SCLK_Port, adc->SCLK_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(adc->SCLK_Port, adc->SCLK_Pin, GPIO_PIN_RESET);
  }
  return NoERROR;
}

ERROR_t ADS1232_ReadAverage(ADS1232* adc, float* value, uint8_t times, uint8_t calibrating) {
  if(calibrating) {
    int32_t tmp;
    ADS1232_Read(adc, &tmp, 1);
  }
  int64_t sum = 0;
  for(uint8_t i = 0; i < times; i++) {
    int32_t tmp;
    ERROR_t err = ADS1232_Read(adc, &tmp, 0);
    if(err) return err;
    sum += tmp;
    HAL_Delay(1);
  }
  if(times == 0) return DIVIDED_by_ZERO;
  *value = (float)sum / times;
  return NoERROR;
}

ERROR_t ADS1232_GetValue(ADS1232* adc, float* value, uint8_t times, uint8_t calibrating) {
  float tmp;
  ERROR_t err = ADS1232_ReadAverage(adc, &tmp, times, calibrating);
  if(err) return err;
  *value = tmp - adc->OFFSET;
  return NoERROR;
}

ERROR_t ADS1232_GetUnits(ADS1232* adc, float* value, uint8_t times, uint8_t calibrating) {
  float tmp;
  ERROR_t err = ADS1232_GetValue(adc, &tmp, times, calibrating);
  if(err) return err;
  if(adc->SCALE == 0) return DIVIDED_by_ZERO;
  *value = tmp / adc->SCALE;
  return NoERROR;
}

ERROR_t ADS1232_Tare(ADS1232* adc, uint8_t times, uint8_t calibrating) {
  float tmp;
  ERROR_t err = ADS1232_ReadAverage(adc, &tmp, times, calibrating);
  if(err) return err;
  adc->OFFSET = tmp;
  return NoERROR;
}

float ADS1232_GetScale(ADS1232* adc) {
  return adc->SCALE;
}

void ADS1232_SetScale(ADS1232* adc, float scale) {
  adc->SCALE = scale;
}

float ADS1232_GetOffset(ADS1232* adc) {
  return adc->OFFSET;
}

void ADS1232_SetOffset(ADS1232* adc, float offset) {
  adc->OFFSET = offset;
}
