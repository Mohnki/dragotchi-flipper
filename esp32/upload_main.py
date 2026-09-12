import sys, time, base64, serial
port = sys.argv[2] if len(sys.argv) > 2 else "/dev/ttyACM1"
srcfile = sys.argv[1]

def rd(s, t=0.4):
    time.sleep(t); return s.read(s.in_waiting or 1)

s = serial.Serial(port, 115200, timeout=1)
time.sleep(0.3)
# The running loop may be blocked in wlan.scan() for a few seconds; hammer
# Ctrl-C over ~6s until we see a REPL prompt.
got_repl = False
for _ in range(12):
    s.write(b'\r\x03')
    out = rd(s, 0.5)
    if b'>>>' in out or b'KeyboardInterrupt' in out:
        got_repl = True
        break
print("REPL reached:", got_repl)
s.reset_input_buffer()
s.write(b'\r\x01')                 # raw REPL
banner = rd(s, 0.6)
if b'raw REPL' not in banner:
    print("WARN raw REPL banner:", banner[-80:])
data = open(srcfile, 'rb').read()
b64 = base64.b64encode(data).decode()
prog = ("import binascii\n"
        "f=open('main.py','wb')\n"
        "f.write(binascii.a2b_base64('%s'))\n"
        "f.close()\n"
        "print('WROTE', %d)\n" % (b64, len(data)))
s.write(prog.encode()); s.write(b'\x04')   # execute
print(rd(s, 1.5).decode(errors='replace'))
s.write(b'\r\x02'); rd(s, 0.3)             # normal REPL
s.write(b'\r\x04')                         # soft reset -> run new main.py
time.sleep(0.5)
s.close()
print("done -> %s" % port)
