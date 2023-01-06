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

// Device telemetry names
#define TELEMETRY_TEMPERATURE       "temperature"
#define TELEMETRY_INTERVAL_PROPERTY "telemetryInterval"

static AZURE_IOT_NX_CONTEXT azure_iot_nx_client;

static int32_t telemetry_interval = 10;

static UINT append_device_telemetry(NX_AZURE_IOT_JSON_WRITER* json_writer)
{
    lps22hb_t lps22hb_data    = lps22hb_data_read();

    if (nx_azure_iot_json_writer_append_property_with_double_value(json_writer,
            (UCHAR*)TELEMETRY_TEMPERATURE,
            sizeof(TELEMETRY_TEMPERATURE) - 1,
            lps22hb_data.temperature_degC,
            2))
    {
        return NX_NOT_SUCCESSFUL;
    }

    char buff[11];
    sprintf(buff, "Temp: %dC", (int)lps22hb_data.temperature_degC);

    screen_print(buff, 16);

    return NX_AZURE_IOT_SUCCESS;
}

static void telemetry_cb(AZURE_IOT_NX_CONTEXT* nx_context)
{
    azure_iot_nx_client_publish_telemetry(&azure_iot_nx_client, NULL, append_device_telemetry);
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
    azure_iot_nx_client_register_timer_callback(&azure_iot_nx_client, telemetry_cb, telemetry_interval);

    if ((status = azure_iot_nx_client_sas_set(&azure_iot_nx_client, IOT_DEVICE_SAS_KEY)))
    {
        printf("ERROR: azure_iot_nx_client_sas_set (0x%08x)\r\n", status);
        return status;
    }

    azure_iot_nx_client_hub_run(&azure_iot_nx_client, IOT_HUB_HOSTNAME, IOT_HUB_DEVICE_ID, wwd_network_connect);

    return NX_SUCCESS;
}
