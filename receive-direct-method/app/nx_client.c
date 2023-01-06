/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "nx_client.h"

#include <stdio.h>

#include "screen.h"
#include "sensor.h"
#include "stm32f4xx_hal.h"

#include "nx_api.h"
#include "nx_azure_iot_hub_client.h"
#include "nx_azure_iot_json_reader.h"
#include "nx_azure_iot_provisioning_client.h"

#include "azure_iot_nx_client.h"

#include "azure_config.h"
#include "azure_pnp_info.h"
#include "wwd_networking.h"

#define IOT_MODEL_ID "dtmi:azurertos:devkit:gsgmxchip;2"

// Properties
#define LED_STATE_PROPERTY          "ledState"

// Commands
#define SET_LED_STATE_COMMAND       "setLedState"
#define SET_DISPLAY_TEXT_COMMAND    "setDisplayText"

static AZURE_IOT_NX_CONTEXT azure_iot_nx_client;

static void set_led_state(bool level)
{
    if (level)
    {
        printf("\tLED is turned ON\r\n");
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    }
    else
    {
        printf("\tLED is turned OFF\r\n");
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    }
}

static void command_received_cb(AZURE_IOT_NX_CONTEXT* nx_context_ptr,
    const UCHAR* component,
    USHORT component_length,
    const UCHAR* method,
    USHORT method_length,
    UCHAR* payload,
    USHORT payload_length,
    VOID* context_ptr,
    USHORT context_length)
{
    UINT status;

    if (strncmp((CHAR*)method, SET_LED_STATE_COMMAND, method_length) == 0)
    {
        bool arg = (strncmp((CHAR*)payload, "true", payload_length) == 0);
        set_led_state(arg);

        if ((status = nx_azure_iot_hub_client_command_message_response(
                 &nx_context_ptr->iothub_client, 200, context_ptr, context_length, NULL, 0, NX_WAIT_FOREVER)))
        {
            printf("Direct method response failed! (0x%08x)\r\n", status);
            return;
        }

        azure_iot_nx_client_publish_bool_property(&azure_iot_nx_client, NULL, LED_STATE_PROPERTY, arg);
    }
    else if (strncmp((CHAR*)method, SET_DISPLAY_TEXT_COMMAND, method_length) == 0)
    {
        // drop the first and last character to remove the quotes
        screen_printn((CHAR*)payload + 1, payload_length - 2, L0);
        if ((status = nx_azure_iot_hub_client_command_message_response(
                 &nx_context_ptr->iothub_client, 200, context_ptr, context_length, NULL, 0, NX_WAIT_FOREVER)))
        {
            printf("Direct method response failed! (0x%08x)\r\n", status);
            return;
        }
    }
    else
    {
        printf("Direct method is not for this device\r\n");

        if ((status = nx_azure_iot_hub_client_command_message_response(
                 &nx_context_ptr->iothub_client, 501, context_ptr, context_length, NULL, 0, NX_WAIT_FOREVER)))
        {
            printf("Direct method response failed! (0x%08x)\r\n", status);
            return;
        }
    }
}
UINT azure_iot_nx_client_entry(
    NX_IP* ip_ptr, NX_PACKET_POOL* pool_ptr, NX_DNS* dns_ptr, UINT (*unix_time_callback)(ULONG* unix_time))
{
    UINT status;

    if ((status = azure_iot_nx_client_create(&azure_iot_nx_client,
             ip_ptr,
             pool_ptr,
             dns_ptr,
             unix_time_callback,
             IOT_MODEL_ID,
             sizeof(IOT_MODEL_ID) - 1)))
    {
        printf("ERROR: azure_iot_nx_client_create failed (0x%08x)\r\n", status);
        return status;
    }

    // Register the callbacks
    azure_iot_nx_client_register_command_callback(&azure_iot_nx_client, command_received_cb);

    if ((status = azure_iot_nx_client_sas_set(&azure_iot_nx_client, IOT_DEVICE_SAS_KEY)))
    {
        printf("ERROR: azure_iot_nx_client_sas_set (0x%08x)\r\n", status);
        return status;
    }

    // Enter the main loop
    azure_iot_nx_client_hub_run(&azure_iot_nx_client, IOT_HUB_HOSTNAME, IOT_HUB_DEVICE_ID, wwd_network_connect);

    return NX_SUCCESS;
}
