# uNabto CC3200 Streaming Demo

This project turns the CC3200 into a Nabto streaming echo server. It uses FreeRTOS and leverages the CC3200's hardware accelerated cryptography capabilities.

# How to set it up

## Step 1: Setup the ESP-IDF build environment



## Step 2: Clone the repository


```
git clone --recursive https://github.com/nabto/unabto-esp32.git
```

## Step 3: Make menuconfig

Enter into menu "Custom configuration"

Edit/adjust the following menu items:


* WIFI access point name (SSID) : The wifi access point you want the module to attach to
* WIFI password : The password used by the access point
* Nabto ID : The Nabto device id you get from your AppMyProduct account
* Nabto key - 32 hex chars : The key for the specific device id you entered in the before mentioned item


## Step 3: Build the project

```
make
```

## Step 4: Flash the Image

Possible you need to adjust the 

```
make flash
```

Please refer to the [CC31xx & CC32xx UniFlash Quick Start Guide](http://processors.wiki.ti.com/index.php/CC31xx_%26_CC32xx_UniFlash_Quick_Start_Guide#CC32xx_MCU_image_flashing) for flashing the image located in `<YOUR-CCS-WORSPACE>\unabto-cc3200\Release\unabto-cc3200.bin`.

# How to test the application

Using a serial terminal you should see a printout similar to the following every time the CC3200 starts up:

```
*************************************************
                   CC3200 + uNabto
*************************************************

Host Driver Version: 1.0.1.6
Build Version 2.6.0.5.31.1.4.0.1.1.0.3.34
Device is configured in default state
Device started as STATION
Connecting to AP: ASUS ...
[WLAN EVENT] STA Connected to the AP: ASUS , BSSID: c8:60:0:93:88:f8
[NETAPP EVENT] IP Acquired: IP=192.168.1.18 , Gateway=192.168.1.1
Connected to AP: ASUS
Device IP: 192.168.1.18

Device id: 'deviceid.demo.nab.to'
Program Release 123.456
Application event framework using SYNC model
sizeof(stream__)=u
SECURE ATTACH: 1, DATA: 1
NONCE_SIZE: 32, CLEAR_TEXT: 0
Nabto was successfully initialized
SECURE ATTACH: 1, DATA: 1
NONCE_SIZE: 32, CLEAR_TEXT: 0
State change from IDLE to WAIT_DNS
Resolving dns: deviceid.demo.nab.to
State change from WAIT_DNS to WAIT_BS
State change from WAIT_BS to WAIT_GSP
########    U_INVITE with LARGE nonce sent, version: - URL: -
State change from WAIT_GSP to ATTACHED
```

Using the Nabto [Echo Stream Tester](https://github.com/nabto/echo-stream-tester) you can now test the echo server by sending and receiving e.g. 1 MB of data:

```
./echo_stream_tester <DEVICE ID> 1000000
```

