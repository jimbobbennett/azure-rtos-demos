/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <stdio.h>

#include "tx_api.h"

#include "board_init.h"
#include "cmsis_utils.h"
#include "screen.h"

#include "sensor.h"

TX_TIMER telemetry_timer;

static void telemetry_timer_callback(ULONG parameter)
{
    lps22hb_t lps22hb_data = lps22hb_data_read();
    float temp_c = lps22hb_data.temperature_degC;

    char buff[11];
    sprintf(buff, "Temp: %dC", (int)temp_c);

    screen_print(buff, 16);
}

void tx_application_define(void* first_unused_memory)
{
    systick_interval_set(TX_TIMER_TICKS_PER_SECOND);

    UINT status = tx_timer_create(&telemetry_timer,
        "Telemetry Timer",
        telemetry_timer_callback,
        0,
        TX_TIMER_TICKS_PER_SECOND,
        TX_TIMER_TICKS_PER_SECOND,
        TX_AUTO_ACTIVATE);

    if (status != TX_SUCCESS)
    {
        printf("ERROR: Timer creation failed\r\n");
    }
}

int main(void)
{
    // Initialize the board
    board_init();

    screen_print("Hello", 0);

    // Enter the ThreadX kernel
    tx_kernel_enter();

    return 0;
}
