#include <cstdio>
#include <string.h>
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char* TAG = "App";

void charToBinary(bool(&dest)[8], char c) {
    ESP_LOGD(TAG, "'%c'", c);
    for (uint8_t i = 0; i < 8; i++) {
        dest[i] = static_cast<uint8_t>(c) & (0b10000000 >> i);
    }
}

void printCharBinaryMSB(bool bins[8]) {
    for (uint8_t i = 0; i < 8; i++) {
        ESP_LOGD(TAG, "%u", bins[i]);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void initSerial() {
    const uart_port_t uartNum = UART_NUM_0;
    uart_config_t uartConfig = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };

    uart_param_config(uartNum, &uartConfig);
    uart_set_pin(uartNum, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(uartNum, 1024 * 2, 0, 0, NULL, 0);
}

void strReadTask(void* pvParam) {
    ESP_LOGI(TAG, "Starting app");
    uint8_t* data = (uint8_t*) malloc(1024);
    memset(data, 0, 1024);
    initSerial();

    for(;;) {
        if (uart_read_bytes(UART_NUM_0, data, 1024, 10 / portTICK_PERIOD_MS) > 0) {
            int64_t tlog_us = esp_timer_get_time();
            bool binaryResult[8];

            for (uint32_t i = 0; data[i] != '\0' && data[i] != '\n'; i++) {
                charToBinary(binaryResult, data[i]);
                printCharBinaryMSB(binaryResult);
                vTaskDelay(50 / portTICK_PERIOD_MS);
            }

            ESP_LOGD(TAG, "Execution time: %llu ms", (esp_timer_get_time() - tlog_us) / 1000);
            uart_flush_input(UART_NUM_0);
            memset(data, 0, 1024);
        }
    }
}

extern "C" void app_main() {
    xTaskCreate(strReadTask, "String read task", 1024 * 2, NULL, 5, NULL);
}
