from machine import Pin
import onewire, ds18x20, time

ds = ds18x20.DS18X20(onewire.OneWire(Pin(4)))
roms = ds.scan()
print(f"Found: {len(roms)} sensor(s)")

while True:
    ds.convert_temp()
    time.sleep_ms(750)
    for rom in roms:
        c = ds.read_temp(rom)
        print(f"{c:.1f} C  /  {c * 9/5 + 32:.1f} F")
    time.sleep(1)