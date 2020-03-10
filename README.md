# Dance Radio 

[![Watch the video](https://img.youtube.com/vi/gBpEz3MsBD0/maxresdefault.jpg)](https://youtu.be/gBpEz3MsBD0) 

## Compatibility

ESP32-ADF
https://www.olimex.com/Products/IoT/ESP32/ESP32-ADF/open-source-hardware

## Usage

Prepare the audio board:

- Connect speakers or headphones to the board.

- Connect [MOD-LCD2.8RTP](https://www.olimex.com/Products/Modules/LCD/MOD-LCD2-8RTP/open-source-hardware) and [led stripe](https://www.olimex.com/Products/Components/LEDs/LED-STRIPE-8/) to UEXT connector using [UEXTx5](https://www.olimex.com/Products/Modules/Interface/UEXTx5/open-source-hardware) extender as follow:

  ​	led stripe: VCC on pin 1, GND on pin 2, DATA on pin 3 
  
  (note: Only LED-STRIPE-8 is supported)

# Get ESP-ADF
```bash
cd ~/
git clone --recursive https://github.com/espressif/esp-adf.git
cd esp-adf
git submodule update --init
export ADF_PATH=$PWD
cd esp-idf
./install.sh
. ./export.sh
```

Load the example:
```bash
cd ~/
mkdir espwork
cd espwork
git clone https://github.com/d3v1c3nv11/dance_radio.git
cd dance_radio
make menuconfig
```
Configure the example:

- Set up the Wi-Fi connection by running `menuconfig` > `Example Configuration` and filling in `WiFi SSID` and `WiFi Password`.

- Choice used Audio board type:  menuconfig > Audio board select...

  Optional:

- Choice used Led stripe: menuconfig > Led stripe > Led stripe... (currently only Led stripe 8 is supported)

- Choice led stripe mode: menuconfig > Led stripe > Led stripe mode...

Run the example:

```bash
make flash monitor
```
- The audio board will first connect to the Wi-Fi.
- Then the board will start playing automatically.
```bash
Use Touch buttons:
Volume: Vol- Vol+
Next station: <Play>
Presset station: <Set>
Loudness ON: <mode>
Loudness OFF: <record>
```



Graphical part of project is based on loboris/ESP32_TFT_library https://github.com/loboris/ESP32_TFT_library


