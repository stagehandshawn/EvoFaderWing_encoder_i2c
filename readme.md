# Encoder Slave Firmware for ATmega328P (3.3V, 8MHz)

This project handles 5 rotary encoders and communicates via I2C with a master device (such as a Teensy 4.1). Each encoder slave board supports:

- 5 rotary encoders with velocity scaling
- I2C communication using a unified protocol
- INT pin signaling when data is available

## I2C Address and Encoder Indexing

Each slave board is assigned an I2C address and global encoder index range based on a configuration constant.

| Start Encoder | I2C Address | Encoder Range |
|---------------|-------------|----------------|
| 1             | `0x11`      | 1–5            |
| 6             | `0x12`      | 6–10           |
| 11            | `0x13`      | 11–15          |
| 16            | `0x14`      | 16–20          |

## Changing Encoder Group via Build Flag

You can configure which slave board to build using a build flag called `ENCODER_GROUP`. Add the desired encoder group number to your `platformio.ini`, like so:

```
; For encoders 6–10
build_flags = -DENCODER_GROUP=6
```

This will automatically:
- Set `startEncoder` to `ENCODER_GROUP`
- Set '`I2C_ADDRESS` to `(0x10 + ((ENCODER_GROUP + 4) / 5))`'


## Example platformio.ini configuration

```
[env:pro8MHzatmega328]
platform = atmelavr
board = pro8MHzatmega328
framework = arduino
lib_deps =
  Encoder
  Wire
upload_speed = 115200
monitor_speed = 115200
build_flags = -DENCODER_GROUP=6
```

## Default Group

If no build flag is provided, the firmware defaults to `ENCODER_GROUP=1` (encoders 1–5, I2C address `0x11`).