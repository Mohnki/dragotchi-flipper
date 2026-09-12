# Dragotchi WiFi reporter — ESP32-S2 Flipper devboard
# Scans nearby WiFi access points and reports density to the Flipper over the
# GPIO UART so the game can trigger a "signal storm" (richer hunting).
#
# Protocol (one line, repeated ~1/sec, and immediately on any received byte):
#     DRAGO wifi=<ap_count> rssi=<strongest_dbm>
#
# Receive-only scanning; we transmit nothing on WiFi. The line goes out on
# UART1 mapped to the ESP32-S2 UART0 pins (GPIO43 TX / GPIO44 RX), which the
# Flipper WiFi devboard wires to the Flipper's expansion USART (pins 13/14).

import machine
import network
import time

# UART to the Flipper. GPIO43=TX0, GPIO44=RX0 on the ESP32-S2 devboard header.
uart = machine.UART(1, baudrate=115200, tx=43, rx=44)

wlan = network.WLAN(network.STA_IF)
wlan.active(True)
try:
    wlan.disconnect()
except Exception:
    pass

# onboard LED (GPIO15 on the Flipper devboard) as a heartbeat, best-effort
try:
    led = machine.Pin(15, machine.Pin.OUT)
except Exception:
    led = None

def scan():
    try:
        nets = wlan.scan()  # (ssid, bssid, channel, RSSI, authmode, hidden)
    except Exception:
        return 0, 0
    count = len(nets)
    best = 0
    for n in nets:
        r = n[3]
        if r < 0 and (best == 0 or r > best):
            best = r
    return count, best

def report():
    count, best = scan()
    line = "DRAGO wifi=%d rssi=%d\n" % (count, best)
    uart.write(line)
    # also echo to the USB REPL for debugging
    print(line, end="")
    if led:
        led.value(not led.value())

while True:
    # Respond promptly if the Flipper nudged us, else free-run once a second.
    if uart.any():
        try:
            uart.read()  # drain the nudge
        except Exception:
            pass
        report()
    else:
        report()
    time.sleep_ms(1000)
