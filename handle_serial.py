#!/usr/bin/env python
'''
Various commands on serial port

depends on serial.tools (pySerial)
pip install pyserial
'''

import os, re, argparse
from serial.tools import list_ports

parser = argparse.ArgumentParser(description='Handle serial port(s)', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('-i', '--info', action='store_true', help='Show info concerning flash')
parser.add_argument('--fuse', action='store_true', help='Show fuse info')
parser.add_argument('-f', '--format', action='store_true', help='Format flash before uploading. Erases SPIFFS too!')
parser.add_argument('-l', '--list', action='store_true', help='Show available ports and its port number. LOCATION=20-2 means port 2')
parser.add_argument('-u', '--upload', action='store_true', help='Upload code to port')
parser.add_argument('-m', '--monitor', action='store_true', help='Open monitor to port')
parser.add_argument('-p', '--port', type=str, default='', help='give port number directly (eg 2) instead of selection')

args = parser.parse_args()

ports = {}
for port in list_ports.comports():
    if 'USB' in port.description:
        desc = '{port.device} | {port.hwid}'.format(port=port)
        ports[re.sub('^.*-(\d)$', r'\1', port.hwid)] = desc
        if args.list:
            print desc

if (len(ports) == 0):
    print('Sorry, no ports found')
    exit(0)

if args.list:
    exit(0)

selectedport = ''
if args.port:
    selectedport = ports[args.port]
else:
    if (len(ports) == 1):
        selectedport = ports.values()[0]
    else:
        print('Please enter your port for uploading:')
        for c, port in ports.iteritems():
            print '{c}) {port}'.format(c=c, port=port)

        var = ''
        #while (not re.match('^\d+$', var) or int(var) > (len(ports)) or int(var) < 1):
        while (var not in ports):
            var = raw_input('Your choice: ')

        selectedport = ports[var]

port = re.sub('^([^|]+).*$', r'\1', selectedport)

if args.info:
    os.system('esptool.py --port {port} flash_id'.format(port=port))
    exit(0)

if args.fuse:
    os.system('espefuse.py --port {port} summary'.format(port=port))
    exit(0)


if args.monitor:
    os.system('platformio device monitor --port {port}'.format(port=port))
    exit(0)

# if upload or no argument given
if args.format:
    print '********** erasing flash on {port}'.format(port=port)
    os.system('esptool.py --port {port} erase_flash'.format(port=port))
    exit(0)

print '********** uploading to {port}'.format(port=port)
os.system('platformio run --target upload --upload-port {port}'.format(port=port))
