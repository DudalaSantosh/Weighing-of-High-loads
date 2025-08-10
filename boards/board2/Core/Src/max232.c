#include "max232.h"
static MX485_HandleTypeDef *g_hMX = NULL;

static void MX485_EnableTx(MX485_HandleTypeDef *hMX)
{
    HAL_GPIO_WritePin(hMX->DE_Port, hMX->DE_Pin, GPIO_PIN_SET);
}

static void MX485_EnableRx(MX485_HandleTypeDef *hMX)
{
    HAL_GPIO_WritePin(hMX->DE_Port, hMX->DE_Pin, GPIO_PIN_RESET);
}

HAL_StatusTypeDef MX485_Init(MX485_HandleTypeDef *hMX,
                             UART_HandleTypeDef *huart,
                             GPIO_TypeDef *DE_Port,
                             uint16_t DE_Pin)
{
    if (!hMX || !huart) return HAL_ERROR;
    hMX->huart   = huart;
    hMX->DE_Port = DE_Port;
    hMX->DE_Pin  = DE_Pin;
    g_hMX = hMX;

    // Default to receive mode
    MX485_EnableRx(hMX);
    return HAL_OK;
}

HAL_StatusTypeDef MX485_Transmit(MX485_HandleTypeDef *hMX,
                                 uint8_t *pData,
                                 uint16_t Len)
{
    if (!hMX) return HAL_ERROR;
    MX485_EnableTx(hMX);
    HAL_StatusTypeDef st = HAL_UART_Transmit(hMX->huart, pData, Len, HAL_MAX_DELAY);
    while (!__HAL_UART_GET_FLAG(hMX->huart, UART_FLAG_TC)) {}
    MX485_EnableRx(hMX);
    return st;
}

HAL_StatusTypeDef MX485_Receive(MX485_HandleTypeDef *hMX,
                                uint8_t *pData,
                                uint16_t Len)
{
    if (!hMX) return HAL_ERROR;
    MX485_EnableRx(hMX);
    return HAL_UART_Receive(hMX->huart, pData, Len, HAL_MAX_DELAY);
}

HAL_StatusTypeDef MX485_Transmit_IT(MX485_HandleTypeDef *hMX,
                                    uint8_t *pData,
                                    uint16_t Len)
{
    if (!hMX) return HAL_ERROR;
    MX485_EnableTx(hMX);
    return HAL_UART_Transmit_IT(hMX->huart, pData, Len);
}

HAL_StatusTypeDef MX485_Receive_IT(MX485_HandleTypeDef *hMX,
                                   uint8_t *pData,
                                   uint16_t Len)
{
    if (!hMX) return HAL_ERROR;
    MX485_EnableRx(hMX);
    return HAL_UART_Receive_IT(hMX->huart, pData, Len);
}

HAL_StatusTypeDef MX485_Transmit_DMA(MX485_HandleTypeDef *hMX,
                                     uint8_t *pData,
                                     uint16_t Len)
{
    if (!hMX) return HAL_ERROR;
    MX485_EnableTx(hMX);
    return HAL_UART_Transmit_DMA(hMX->huart, pData, Len);
}

HAL_StatusTypeDef MX485_Receive_DMA(MX485_HandleTypeDef *hMX,
                                    uint8_t *pData,
                                    uint16_t Len)
{
    if (!hMX) return HAL_ERROR;
    MX485_EnableRx(hMX);
    return HAL_UART_Receive_DMA(hMX->huart, pData, Len);
}

// Callback to switch back to RX after interrupt or DMA TX complete
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (g_hMX && huart == g_hMX->huart) {
        MX485_EnableRx(g_hMX);
    }
}

// Other callbacks (stubbed, override if needed)
void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart) {}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {}
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {}
