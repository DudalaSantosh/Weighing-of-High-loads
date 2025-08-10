#include "max232.h"
HAL_StatusTypeDef MX485_Init(MX485_HandleTypeDef *hMX,
                             UART_HandleTypeDef *huart,
                             GPIO_TypeDef *DE_Port,
                             uint16_t DE_Pin)
{
    if (hMX == NULL || huart == NULL) {
        return HAL_ERROR;
    }
    hMX->huart   = huart;
    hMX->DE_Port = DE_Port;
    hMX->DE_Pin  = DE_Pin;

    // Default to receive mode
    HAL_GPIO_WritePin(hMX->DE_Port, hMX->DE_Pin, GPIO_PIN_RESET);
    return HAL_OK;
}

/**
 * @brief  Enable RS-485 transmit (DE high)
 */
static void MX485_EnableTx(MX485_HandleTypeDef *hMX)
{
    HAL_GPIO_WritePin(hMX->DE_Port, hMX->DE_Pin, GPIO_PIN_SET);
}

/**
 * @brief  Enable RS-485 receive (DE low)
 */
static void MX485_EnableRx(MX485_HandleTypeDef *hMX)
{
    HAL_GPIO_WritePin(hMX->DE_Port, hMX->DE_Pin, GPIO_PIN_RESET);
}

/**
 * @brief  Transmit data (blocking)
 */
HAL_StatusTypeDef MX485_Transmit(MX485_HandleTypeDef *hMX,
                                 uint8_t *pData,
                                 uint16_t Len)
{
    if (hMX == NULL || hMX->huart == NULL) {
        return HAL_ERROR;
    }

    MX485_EnableTx(hMX);
    HAL_StatusTypeDef status = HAL_UART_Transmit(hMX->huart, pData, Len, HAL_MAX_DELAY);
    // Wait for end of transmission
    while (__HAL_UART_GET_FLAG(hMX->huart, UART_FLAG_TC) == RESET) {}

    MX485_EnableRx(hMX);
    return status;
}

/**
 * @brief  Receive data (blocking)
 */
HAL_StatusTypeDef MX485_Receive(MX485_HandleTypeDef *hMX,
                                uint8_t *pData,
                                uint16_t Len)
{
    if (hMX == NULL || hMX->huart == NULL) {
        return HAL_ERROR;
    }

    MX485_EnableRx(hMX);
    return HAL_UART_Receive(hMX->huart, pData, Len, HAL_MAX_DELAY);
}
