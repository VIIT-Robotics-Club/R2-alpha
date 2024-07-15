## R2 control unit 

### How to build ?
make sure you have a functional installation of [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#installation)

   ```
  git clone git@github.com:VIIT-Robotics-Club/R2-alpha.git --recursive

   ```
   ```
   cd R2-alpha 

   ```
   ```
   idf.py build

   ```


### How to flash pre-build binaries?
Download latest Release of R2-alpha from the [releases](https://github.com/VIIT-Robotics-Club/R2-alpha/releases) section and flash the binaries on ESP32-S3 at address 0x00.

you can use [ESP flash Download tool](https://www.espressif.com/en/support/download/other-tools) to do the same.
