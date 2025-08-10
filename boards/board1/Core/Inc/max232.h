/*
 * max232.h
 *
 *  Created on: May 18, 2025
 *      Author: nibes
 */

#ifndef INC_MAX232_H_
#define INC_MAX232_H_




#include "stm32f4xx_hal.h"


typedef struct {
    UART_HandleTypeDef *huart;   /**< UART handle pointer */
    GPIO_TypeDef      *DE_Port;  /**< GPIO Port for DE/RE pin */
    uint16_t           DE_Pin;   /**< GPIO Pin for DE/RE */
} MX485_HandleTypeDef;

HAL_StatusTypeDef MX485_Init(MX485_HandleTypeDef *hMX,
                             UART_HandleTypeDef *huart,
                             GPIO_TypeDef *DE_Port,
                             uint16_t DE_Pin);

HAL_StatusTypeDef MX485_Transmit(MX485_HandleTypeDef *hMX,
                                 uint8_t *pData,
                                 uint16_t Len);

HAL_StatusTypeDef MX485_Receive(MX485_HandleTypeDef *hMX,
                                uint8_t *pData,
                                uint16_t Len);
#endif /* INC_MAX232_H_ */
