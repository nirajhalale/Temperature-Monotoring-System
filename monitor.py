"""Optional live serial monitor for the ESP32 temperature project.

Install: pip install pyserial matplotlib
Run:     python monitor.py --port COM3
macOS:   python monitor.py --port /dev/cu.usbserial-XXXX
"""
import argparse
import csv
import time
from collections import deque
from datetime import datetime

import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

parser = argparse.ArgumentParser()
parser.add_argument("--port", required=True, help="Serial port, e.g. COM3")
parser.add_argument("--baud", type=int, default=115200)
parser.add_argument("--log", default="temperature_log.csv")
args = parser.parse_args()

ser = serial.Serial(args.port, args.baud, timeout=1)
time.sleep(2)  # allow ESP32 serial connection to settle

timestamps = deque(maxlen=60)
dht_values = deque(maxlen=60)
ds_values = deque(maxlen=60)
humidity_values = deque(maxlen=60)

log_file = open(args.log, "a", newline="", encoding="utf-8")
writer = csv.writer(log_file)
if log_file.tell() == 0:
    writer.writerow(["timestamp", "dht_temp_c", "humidity_percent", "ds18b20_temp_c"])

fig, (ax_temp, ax_hum) = plt.subplots(2, 1, figsize=(10, 7))
fig.suptitle("ESP32 Temperature & Humidity Monitor")

def update(_):
    line = ser.readline().decode("utf-8", errors="ignore").strip()
    if line.startswith("DATA,"):
        try:
            _, dht_t, humidity, ds_t = line.split(",")
            dht_t, humidity, ds_t = float(dht_t), float(humidity), float(ds_t)
        except ValueError:
            return

        stamp = datetime.now()
        timestamps.append(stamp.strftime("%H:%M:%S"))
        dht_values.append(dht_t)
        humidity_values.append(humidity)
        ds_values.append(ds_t)
        writer.writerow([stamp.isoformat(timespec="seconds"), dht_t, humidity, ds_t])
        log_file.flush()

        ax_temp.clear()
        ax_temp.plot(list(timestamps), list(dht_values), marker="o", label="DHT11")
        ax_temp.plot(list(timestamps), list(ds_values), marker="o", label="DS18B20")
        ax_temp.set_ylabel("Temperature (°C)")
        ax_temp.legend()
        ax_temp.grid(True, alpha=0.3)
        ax_temp.tick_params(axis="x", rotation=45)

        ax_hum.clear()
        ax_hum.plot(list(timestamps), list(humidity_values), marker="o", label="Humidity")
        ax_hum.set_ylabel("Humidity (%)")
        ax_hum.set_ylim(0, 100)
        ax_hum.grid(True, alpha=0.3)
        ax_hum.tick_params(axis="x", rotation=45)

ani = FuncAnimation(fig, update, interval=250, cache_frame_data=False)
try:
    plt.tight_layout()
    plt.show()
finally:
    ser.close()
    log_file.close()
