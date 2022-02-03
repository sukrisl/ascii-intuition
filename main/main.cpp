#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void charToBinary(bool(&dest)[8], char c) {
    for (uint8_t i = 0; i < 8; i++) {
        dest[i] = static_cast<uint8_t>(c) & (0b10000000 >> i);
    }

    printf("%c,0x%02x,%u%u%u%u%u%u%u%u\n",
        c, static_cast<uint8_t>(c),
        dest[0], dest[1], dest[2], dest[3], dest[4], dest[5], dest[6], dest[7]
    );
}

void strReadTask(void* pvParam) {
    for(;;) {
        const char* inputStr = 
R"(
One of these days i'm going to get that red dot, just you wait and see get my claw stuck in the dog's ear.
Crash against wall but walk away like nothing happened purr when being pet, so stick butt in face scratch
at door to be let outside, get let out then scratch at door immmediately after to be let back in kitty pounce,
trip, faceplant you didn't see that no you didn't definitely didn't lick, lick, lick, and preen away the embarrassment
but caticus cuteicus. Flex claws on the human's belly and purr like a lawnmower. Present belly, scratch hand when stroked.
Scratch bawl under human beds.
)";

        printf("\nWait a sec...\n\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        int64_t tlog_us = esp_timer_get_time();

        for (uint64_t i = 0; inputStr[i] != '\0'; i++) {
            bool binaryResult[8];
            charToBinary(binaryResult, inputStr[i]);
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }

        printf("Execution time: %llu ms\n", (esp_timer_get_time() - tlog_us) / 1000);
    }
}

extern "C" void app_main() {
    xTaskCreate(strReadTask, "String read task", 1024 * 2, NULL, 5, NULL);
}
