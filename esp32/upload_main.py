#!/usr/bin/env python3
"""Upload esp32/main.py to a MicroPython board over the USB REPL.

Usage:  python3 upload_main.py [REPL_PORT] [main.py]
Defaults: /dev/ttyACM1 and the main.py next to this script.
Requires pyserial (ships with esptool's venv).
"""
import sys, os, time, base64, serial

port = sys.argv[1] if len(sys.argv) > 1 else "/dev/ttyACM1"
srcfile = sys.argv[2] if len(sys.argv) > 2 else os.path.join(os.path.dirname(__file__), "main.py")

def rd(s, t=0.4):
    time.sleep(t); return s.read(s.in_waiting or 1)

s = serial.Serial(port, 115200, timeout=1)
time.sleep(0.3)
s.write(b"\r\x03\x03"); rd(s, 0.5)        # Ctrl-C: stop running program
s.reset_input_buffer()
s.write(b"\r\x01"); rd(s, 0.5)            # Ctrl-A: raw REPL
data = open(srcfile, "rb").read()
b64 = base64.b64encode(data).decode()
prog = ("import binascii\n"
        "f=open('main.py','wb')\n"
        "f.write(binascii.a2b_base64('%s'))\n"
        "f.close()\n"
        "print('WROTE', %d)\n" % (b64, len(data)))
s.write(prog.encode()); s.write(b"\x04")  # Ctrl-D: execute
print(rd(s, 1.5).decode(errors="replace"))
s.write(b"\r\x04")                        # soft reset -> run main.py
s.close()
print("Uploaded %d bytes to %s and reset." % (len(data), port))
