/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _AZURE_CONFIG_H
#define _AZURE_CONFIG_H

typedef enum
{
    None         = 0,
    WEP          = 1,
    WPA_PSK_TKIP = 2,
    WPA2_PSK_AES = 3
} WiFi_Mode;

#define WIFI_SSID     ""
#define WIFI_PASSWORD ""
#define WIFI_MODE     WPA2_PSK_AES

#define IOT_HUB_HOSTNAME  ""
#define IOT_HUB_DEVICE_ID ""
#define IOT_DEVICE_SAS_KEY ""

#endif // _AZURE_CONFIG_H
