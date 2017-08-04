# uNabto esp32-evb heat stub demo

This project turns the esp32-evb into a Nabto/AppMyProduct heatpump stub demo. It uses the esp-idf FreeRTOS based core (vs. some other demos which uses the arduino core)

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

Possible you need to adjust the serial deivce to use for flashing which is setup in the menuconfig part, but mostly the standard setup will match your platform.

```
make flash
```


# How to test the application

Using a serial terminal you should see a printout similar to the following every time the CC3200 starts up:

```
```



```
./echo_stream_tester <DEVICE ID> 1000000
```

