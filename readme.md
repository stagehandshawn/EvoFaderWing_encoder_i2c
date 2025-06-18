# Encoder slave for EvoFaderwing master
- One of 4 needed to get input from 20 encoders over I2c

- Before build set encoder_group flag to 1-4 in platformio.ini to compile for the group of encoders you need
For example you want to compile for encoders 1-5
  - `build_flags = -DENCODER_GROUP=1`

- compiled for Atmega328p3.3v8Mhz

- This repo supports the full project [EvoFaderWing](https://github.com/stagehandshawn/EvoFaderWing)

## Wiring diagram

![Encoder diagram](https://github.com/stagehandshawn/EvoFaderWing_encoder_i2c/blob/main/images/evofaderwing_encoder_slave_wiring.png)