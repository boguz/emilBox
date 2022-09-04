# emilBox
ESP32 based Toniebox clone

## Materials
- Board: ESP32 doit devkit v1 [pins](https://www.mischianti.org/2021/02/17/doit-esp32-dev-kit-v1-high-resolution-pinout-and-specs/)

## Connections
#### RFID RC522 [diagram here](https://esp32io.com/tutorials/esp32-rfid-nfc):
- SDA:   D2
- SCL:   D18
- MOSI:  D23
- MISO:  D19
- IRQ:   --
- GND:   GND
- RST:   D27
- 3.3V:  3.3V


## Build the index.html
1. We must build the css and js
2. The build css and js must be added to the index.html head section
3. We must GZIP the index.html, for instance using the [Online GZIP comporessor](https://gzip.swimburger.net/)
4. The gziped file must be converted to a file array, like described [here](https://www.mischianti.org/2020/10/26/web-server-with-esp8266-and-esp32-byte-array-gzipped-pages-and-spiffs-2/)
5. The result must be added to a `web_index.h` file (sibling of index.html)
6. On the `web_index.h` we must define `index_html_gz_len` (using dashed) and 


## Inspiration and links

- [ESP32 Web Sockets server](https://shawnhymel.com/1882/how-to-create-a-web-server-with-websockets-using-an-esp32-in-arduino/)
- [RFID tag removal detection](https://github.com/miguelbalboa/rfid/issues/352#issue-282870788)