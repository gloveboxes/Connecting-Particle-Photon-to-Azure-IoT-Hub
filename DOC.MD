

# Connect a Particle Photon to your Azure IoT Central application (Arduino/C/C++)

This article describes how, as a device developer, to connect a Partcile Photon to your Microsoft Azure IoT Central application using the Arduino programming language.

## Before you begin

To complete the steps in this article, you need the following components:

* An Azure IoT Central application created from the **Sample Devkits** application template. For more information, see the [create an application quickstart](quick-deploy-iot-central.md).
* A [Particle Photon](https://store.particle.io/collections/photon) device connected to the internet.
* Registered on the [Particle.io](https://www.particle.io/build) cloud.

## **Sample Devkits** application

An application created from the **Sample Devkits** application template includes a **Raspberry Pi** device template with the following characteristics: 

- Telemetry, which includes the following measurements the device will collect:
    - Humidity
    - Temperature
    - Pressure
- Commands
    - LightOn
    - LightOff
    - FanOn
    - FanOff



## Add a real device

In your Azure IoT Central application, add a real device and keep track of the device connection details (**Scope ID, Device ID, Primary key**). For more information, see [Add a real device to your Azure IoT Central application](tutorial-add-device.md).


### Configure the Raspberry Pi

The following steps describe how to download and configure the sample Python application from GitHub. This sample application:

* Sends telemetry and property values to Azure IoT Central.
* Responds to setting changes made in Azure IoT Central.

To configure the device, [follow the step-by-step instructions on GitHub.](https://aka.ms/iotcentral-docs-Raspi-releases)


> [!NOTE]
> For more information about the Raspberry Pi Python sample, see the [README](https://aka.ms/iotcentral-docs-Raspi-releases) file on GitHub.


1. Once the device is configured, your device should start sending data to Azure IoT Central momentarily.
1. In your Azure IoT Central application, you see how the code running on the Raspberry Pi interacts with the application:

    * On the **Measurements** page for your real device, you can see the telemetry sent from the Raspberry Pi. If you are using the **Sense HAT Emulator**, you can modify the telemetry values in the GUI on the Raspberry Pi.
    * On the **Properties** page, you can see the value of the reported **Die Number** property.
    * On the **Settings** page, you can change various settings on the Raspberry Pi such as voltage and fan speed. When the Raspberry Pi acknowledges the change, the setting shows as **synced** in Azure IoT Central.




### Telemetry measurements

| Field name     | Units  | Minimum | Maximum | Decimal places |
| -------------- | ------ | ------- | ------- | -------------- |
| humidity       | %      | 0       | 100     | 0              |
| temp           | °C     | -10     | 60      | 0              |
| pressure       | hPa    | 800     | 1260    | 0              |



Toggle settings

| Display name | Field name | On text | Off text | Initial |
| ------------ | ---------- | ------- | -------- | ------- |
| IR           | activateIR | ON      | OFF      | Off     |



## Next steps

Now that you have learned how to connect a Raspberry Pi to your Azure IoT Central application, here are the suggested next steps:

* [Connect a generic Node.js client application to Azure IoT Central](howto-connect-nodejs.md)