# uNabto ESP32 EVB Heatpump Stub 

This project turns the ESP32 EVB into a Nabto/AppMyProduct heatpump stub demo. It uses the esp-idf FreeRTOS based core (vs. some other demos which uses the arduino core).

# The structure

The structure of the project is as follows
<pre>

    +
    +-->+-------------+
    |   |    Main     |        This folder contains the initial setup
    |   +-------------+        and the application_event() function defining
    |                          the application
    +-->+-------------+
        |  Components |
        +-------------+
                      |
                      +------->+-------------+
                      |        | unabto|sdk  |    Don't edit here
                      |        +-------------+
                      |
                      |
                      +------->+-------------+
                               | Platform    |    The integration is
                               | Adapter     |    located here
                               +-------------+
</pre>


ESP-IDF project has a speciel structure. You can read more about that here:
https://esp-idf.readthedocs.io/en/v1.0/build_system.html
The unabto source and the integration code is done as components and therefore resides in the IDF component folder.
The unabto sdk is a submodule link to the unabto generic source.

The initial setup/commisioning and the application is located in the main folder.
unabto_application.c contains the application (inside the application_event function), the main.c contains the setup and configuration of the WiFi module.


# How to set it up

## Step 1: Setup the ESP-IDF build environment

Follow the setup of the ESP-IDF toolchain setup

http://esp-idf.readthedocs.io/en/latest/get-started/index.html

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

Possible you need to adjust the serial deivce to use for flashing which is setup in the menuconfig part, but mostly the standard setup will match your platform.

```
make flash
```


# How to test the application



## Monitor the output from the board

Using the monitor command you should see a printout similar to the following every time the ESP32-EVB starts up:


```
~/dev/unabto-esp32$ make monitor
MONITOR
--- idf_monitor on /dev/cu.wchusbserial1420 115200 ---

[REMOVED DEFAULT ESP32 SETUP LOGGING]

00:00:00:006 main.c(145) Nabto ESP32 demo starting up!!!
I (232) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (232) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (292) phy: phy_version: 355.1, 59464c5, Jun 14 2017, 20:25:06, 0, 0
00:00:00:099 main.c(172) Connecting to nabtotest1

00:00:00:102 main.c(74) Main task: waiting for connection to the wifi network...
I (2692) event: ip: 192.168.0.108, mask: 255.255.255.0, gw: 192.168.0.1
00:00:02:496 main.c(76) connected!

00:00:02:497 main.c(82) IP Address:  192.168.0.108
00:00:02:500 main.c(83) Subnet mask: 255.255.255.0
00:00:02:505 main.c(84) Gateway:     192.168.0.1
00:00:02:509 unabto_common_main.c(121) Device id: 'uqhtktt3.smtth.appmyproduct.com'
00:00:02:516 unabto_common_main.c(122) Program Release 4.1.1
00:00:02:523 unabto_app_adapter.c(690) Application event framework using SYNC model
00:00:02:529 unabto_context.c(55) SECURE ATTACH: 1, DATA: 1
00:00:02:534 unabto_context.c(63) NONCE_SIZE: 32, CLEAR_TEXT: 0
00:00:02:541 unabto_common_main.c(200) Nabto was successfully initialized
00:00:02:547 unabto_application.c(71) In demo_init
00:00:02:551 unabto_application.c(92) Before fp_mem_init
00:00:02:556 unabto_application.c(95) Before acl_ae_init
00:00:02:565 unabto_context.c(55) SECURE ATTACH: 1, DATA: 1
00:00:02:566 unabto_context.c(63) NONCE_SIZE: 32, CLEAR_TEXT: 0
00:00:02:575 unabto_attach.c(804) State change from IDLE to WAIT_DNS
00:00:02:578 unabto_attach.c(805) Resolving DNS for uqhtktt3.smtth.appmyproduct.com
00:00:02:595 unabto_attach.c(818) DNS error (returned by application)
00:00:02:596 unabto_attach.c(819) State change from WAIT_DNS to IDLE
00:00:04:605 unabto_context.c(55) SECURE ATTACH: 1, DATA: 1
00:00:04:606 unabto_context.c(63) NONCE_SIZE: 32, CLEAR_TEXT: 0
00:00:04:609 unabto_attach.c(804) State change from IDLE to WAIT_DNS
00:00:04:612 unabto_attach.c(805) Resolving DNS for uqhtktt3.smtth.appmyproduct.com
00:00:04:625 unabto_attach.c(824) Resolved DNS for uqhtktt3.smtth.appmyproduct.com to:
00:00:04:627 unabto_attach.c(830)   Controller ip: 52.31.229.62
00:00:04:632 unabto_attach.c(836) State change from WAIT_DNS to WAIT_BS
00:00:04:696 unabto_attach.c(487) State change from WAIT_BS to WAIT_GSP
00:00:04:696 unabto_attach.c(488) GSP address: 54.72.234.97:5565
00:00:04:705 unabto_attach.c(275) ########    U_INVITE with LARGE nonce sent, version: - URL: -
00:00:04:761 unabto_attach.c(591) State change from WAIT_GSP to ATTACHED
```

## Download the Heat control app

Download the Android or iPhone app from app store
https://www.appmyproduct.com/tutorial.html


## Pair the Heat control app with the wifi module

Follow the video in the tutorial:

https://www.appmyproduct.com/tutorial.html

