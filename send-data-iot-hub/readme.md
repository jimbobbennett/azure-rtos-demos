# Send data to Azure Iot Hub from an MXChip

## What you need

* The [MXCHIP AZ3166 IoT DevKit](https://aka.ms/iot-devkit)
* USB 2.0 A male to Micro USB male cable
* WiFI
* An Azure IoT Hub instance
* One device created in the IoT Hub instance using a SAS token for security

## Steps

1. Install the development tools from the root folder:

    ```cmd
    \tools\get-toolchain.bat
    ```

1. Set the WiFi and IoT Hub details in the `send-data-iot-hub\app\azure_config.h` header file:

    ```cpp
    #define WIFI_SSID     ""
    #define WIFI_PASSWORD ""
    #define IOT_HUB_HOSTNAME  ""
    #define IOT_HUB_DEVICE_ID ""
    #define IOT_DEVICE_SAS_KEY ""
    ```
    
1. Build the binary image:

    ```cmd
    send-data-iot-hub\tools\rebuild.bat
    ```

1. Flash the image to the device by copying the image file to the **AZ3166** drive:

    **send-data-iot-hub\build\app\mxchip_azure_iot.bin**

1. Configure a serial port app at baud rate **115,200** to monitor the device output.
