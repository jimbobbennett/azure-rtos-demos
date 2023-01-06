# Use sensors with an MXChip

## What you need

* The [MXCHIP AZ3166 IoT DevKit](https://aka.ms/iot-devkit)
* USB 2.0 A male to Micro USB male cable

## Steps

1. Install the development tools from the root folder:

    ```cmd
    \tools\get-toolchain.bat
    ```
    
1. Build the binary image:

    ```cmd
    use-sensors\tools\rebuild.bat
    ```

1. Flash the image to the device by copying the image file to the **AZ3166** drive:

    **use-sensors\build\app\mxchip_azure_iot.bin**

1. Configure a serial port app at baud rate **115,200** to monitor the device output.
