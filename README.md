# Temperature Monitoring System

Software for an ESP32-based temperature and humidity monitoring prototype using a DHT11, DS18B20 and 16x2 I2C LCD.

## Included
- `temperature_monitor.ino` — ESP32 firmware: reads both sensors, displays values on the LCD and streams readings over USB serial.
- `monitor.py` — optional desktop live chart and CSV logger.
- `requirements.txt` — Python dependencies for the desktop monitor.

## Arduino IDE setup
1. Install Arduino IDE and the ESP32 board package.
2. Select **ESP32 Dev Module** and the correct USB port.
3. Install these libraries from Library Manager:
   - DHT sensor library (Adafruit)
   - Adafruit Unified Sensor
   - OneWire
   - DallasTemperature
   - LiquidCrystal_I2C
4. Open `temperature_monitor.ino`, verify the pin definitions, then upload.

## Default pin configuration
| Component | ESP32 pin |
|---|---|
| DHT11 data | GPIO 4 |
| DS18B20 data | GPIO 5 |
| I2C SDA | GPIO 21 (default ESP32) |
| I2C SCL | GPIO 22 (default ESP32) |

Use the appropriate pull-up resistor for the DS18B20 data line (commonly 4.7 kΩ). Confirm your LCD I2C address; change `LCD_ADDRESS` from `0x27` to `0x3F` if required. Sensor power and logic levels must be compatible with your board.

## Optional desktop dashboard
```bash
python -m venv .venv
# Windows: .venv\\Scripts\\activate
# macOS/Linux: source .venv/bin/activate
pip install -r requirements.txt
python monitor.py --port COM3
```
Replace `COM3` with your actual serial port (for example `/dev/cu.usbserial-XXXX` on macOS). Close Arduino Serial Monitor before launching the Python dashboard.

The dashboard plots the latest readings and appends them to `temperature_log.csv`.

## Serial protocol
At 115200 baud, valid readings are emitted as:
`DATA,<dht_temperature_c>,<humidity_percent>,<ds18b20_temperature_c>`

## Notes
- DHT11 readings are relatively slow and less precise than many modern sensors; a 2-second sampling interval is used.
- This repository contains software only. Hardware, wiring, enclosure and calibration are not included.
