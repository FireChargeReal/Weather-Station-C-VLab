# Weather Station C-VLab
This repository contains firmware for an ESP32-based weather station that collects and transmits data to TTN. Follow the instructions below to download, configure, and upload the firmware to your ESP32.

Requirements
Varnalab Weaither station (Wireing guide WIP)<br>
Visual Studio Code<br>
PlatformIO (Installed as a VSCode extension or standalone)<br>
Git (to clone the repository)<br>

Getting Started
## 1. Clone the Repository
First, clone the repository to your local machine:

```
git clone https://github.com/FireChargeReal/Weather-Station-C-VLab.git
cd Weather-Station-C-VLab
```

Alternatively, download the repository as a ZIP file from the GitHub page and extract it.

## 2. Install PlatformIO
Ensure you have PlatformIO installed. For instructions, visit PlatformIO Installation Guide.

## 3. Create a TTN Application
If your haven't created an account on the [The Things Network](https://www.thethingsnetwork.org/) do so.

Click on your profile to show the drop down and then select console. Then chose the cluster closest to you.

Then navigate to Applications and create an Application (This will be the central point where all of your stations/devices will send their data to and will also make it easier to send to data to other servers).

Once your Application is created, navigate to the end devices tab and create a end device with the instructions bellow.
```
For input method set it to "Enter end device specifics manually".

Set the JoinEUI set it to all zeros, this is only done when the device is programmable.

Finally generate the APPKey and the DevEUI using the genrate button. And create a end device id (device id cannot be changed once set).
```

## 4. Configure the config.h File
The config.h file is used to set up important parameters like LoRa settings and configurations for sensors and others.

Open the project folder in VSCode.

Navigate to the src/config.h file.

Edit the configuration parameters and add your TTN information in the correct fields.<br>
Example:
```
static const char *appEUI = "0000000000000000";
static const char *devEUI = "70********A9";
static const char *appSKey = "62*****************165";

```

If you want to change how the sleep time of the weather station u can do so by changing the sleepTime value.

## 5. Build and Upload the Firmware
Connect your Weather station board to your computer via USB.

In VSCode, select the PlatformIO extension.

Open the project folder if it's not already open.

Build the project by clicking the checkmark button in the bottom toolbar.

Upload the firmware by clicking the "->" button in the bottom of the toolbar and wait for it to finish.

## 6. Verify the Setup
Ensure the ESP32 connects to the configured peripherals and sends/receives data as expected.<BR>
To view the data on The Things Network navigate to the console, click on your application, click on end devices and click on your weather station device. From there you can see the data in the last decoded messages.


<br>
<br>
<br>

### Troubleshooting
Check the serial monitor output for debugging information or errors.
Troubleshooting
Build Errors: Double-check your platformio.ini file for correct board and library configurations.
Upload Failures: Ensure no other program (e.g., Arduino IDE Serial Monitor) is accessing the COM port.

For further question contact [VarnaLab](https://www.varnalab.org) or contact [Iskren Dobrev](https://www.iskrendobrev.com) 
