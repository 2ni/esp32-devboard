.PHONY: compile st serial ota terminals terminal mqtt test format clean

compile:
	pio run

clean:
	pio run -t clean

# eg make st port=1
st: compile serial terminal

fst: compile format serial terminal

serial:
	@./handle_serial.py --port=$$port
	@#esptool.py --port `python -m serial.tools.list_ports 2>/dev/null|grep "usb"` erase_flash
	@#pio run --target upload

# available esp nodes. Their name must beginn with "esp-xy"
# @ suppresses output of command run
# nmap solution: https://stackoverflow.com/questions/503171/send-a-ping-to-each-ip-on-a-subnet
esps:
	@nmap -sn 192.168.1.*|grep -i esp_|perl -pe 's/.* (esp_[^ ]*) \(([^)]*)\)/http:\/\/\1.local: \2/i'
	@#@nmap -n -sP 192.168.1.0/24
	@#@arp -a|grep esp|sed 's/\([^ ]*\) (\(.*\)).*/\1: \2/'

# ping broadcast to activate mDNS services
ping:
	@ifconfig|grep -o "broadcast [0-9.]*"|cut -f 2 -d " "|xargs ping -c 2 2>&1 >/dev/null

# call make ota ip=192.168.1.6 or w/o argument
ota:
	@./ota.sh $$ip

info:
	@./handle_serial.py --info --port=$$port

fuse:
	@./handle_serial.py --fuse --port=$$port

terminals:
	@./handle_serial.py --list --port=$$port
	@#pio device list

terminal:
	@./handle_serial.py --monitor --port=$$port
	@#pio device monitor

format:
	@./handle_serial.py --format --port=$$port

test:
	@read -p "Do you wish to install this program? " yn; \
	case $$yn in \
	[Yy]*) echo "Good choice." ;; \
	[Nn]*) echo "Better choice." ;; \
	*) echo "no choice" ;; \
	esac
