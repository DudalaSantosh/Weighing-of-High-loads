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

/**
 * @brief  Initialize MX485 handle
 * @param  hMX     Pointer to MX485 handle struct
 * @param  huart   UART handle (e.g. &huart2)
 * @param  DE_Port GPIO port for DE/RE
 * @param  DE_Pin  GPIO pin for DE/RE
 * @retval HAL status
 */
HAL_StatusTypeDef MX485_Init(MX485_HandleTypeDef *hMX,
                             UART_HandleTypeDef *huart,
                             GPIO_TypeDef *DE_Port,
                             uint16_t DE_Pin);

/**
 * @brief  Transmit data over RS-485 (blocking)
 * @param  hMX   MX485 handle
 * @param  pData Pointer to data buffer
 * @param  Len   Number of bytes to send
 * @retval HAL status
 */
HAL_StatusTypeDef MX485_Transmit(MX485_HandleTypeDef *hMX,
                                 uint8_t *pData,
                                 uint16_t Len);

/**
 * @brief  Receive data over RS-485 (blocking)
 * @param  hMX   MX485 handle
 * @param  pData Pointer to receive buffer
 * @param  Len   Number of bytes to receive
 * @retval HAL status
 */
HAL_StatusTypeDef MX485_Receive(MX485_HandleTypeDef *hMX,
                                uint8_t *pData,
                                uint16_t Len);

/**
 * @brief  Transmit data over RS-485 (interrupt)
 */
HAL_StatusTypeDef MX485_Transmit_IT(MX485_HandleTypeDef *hMX,
                                    uint8_t *pData,
                                    uint16_t Len);

/**
 * @brief  Receive data over RS-485 (interrupt)
 */
HAL_StatusTypeDef MX485_Receive_IT(MX485_HandleTypeDef *hMX,
                                   uint8_t *pData,
                                   uint16_t Len);

/**
 * @brief  Transmit data over RS-485 (DMA)
 */
HAL_StatusTypeDef MX485_Transmit_DMA(MX485_HandleTypeDef *hMX,
                                     uint8_t *pData,
                                     uint16_t Len);

/**
 * @brief  Receive data over RS-485 (DMA)
 */
HAL_StatusTypeDef MX485_Receive_DMA(MX485_HandleTypeDef *hMX,
                                    uint8_t *pData,
                                    uint16_t Len);
#endif /* INC_MAX232_H_ */
