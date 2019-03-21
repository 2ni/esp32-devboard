# https://stackoverflow.com/questions/27389089/dynamic-option-selection-in-bash

# upload via wifi
# no spaces allowed in hostname
# ./ota.sh  tries to locate the device automatically
# ./ota.sh <devicename> uploads to the given devicename (network must return hostnames, name can be defined by connect_to_wifi(nodename)
# ./ota.sh <ip> uploads to ip of device (this should always work)

if [[ "$1" =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
  answer=$1
else
  # use https://apple.stackexchange.com/questions/20547/how-do-i-find-my-ip-address-from-the-command-line
  # to automatically detect ip to upload
  network=`./ip_address.sh |cut -f 1-3 -d .`
  network="$network.*"
  if [ -n "$1" ]; then
    name=$1
  else
    name="esp_"
  fi

  echo "scanning $network for $name"
  # sometimes arp works better, sometimes nmap...
  esps=`nmap -sn $network|sed -n 's@.* for \(.*\) (\([^)]*\))@\1: \2@p'|grep -i $name`
  # esps=`arp -a|sed -n 's@\(.*\) (\([^)]*\)).*@\1: \2@p'|grep -i $name`
  number_of_esps=`echo "$esps"|wc -l`
  if [ "$number_of_esps" -gt "1" ]
  then
    oldIFS=$IFS
    IFS=$'\n'
    choices=( $esps )
    IFS=$oldIFS
    PS3="Please enter your choice: "
    select answer in "${choices[@]}"; do
      for item in "${choices[@]}"; do
        if [[ $item == $answer ]]; then
          break 2
        fi
      done
    done
  else
    answer=$esps
  fi
fi

if [ -n "$answer" ]
then
  echo "uploading to $answer"
  platformio run --target upload --upload-port `echo "$answer" | sed 's/.* \(.*\).*/\1/'`
else
  echo "Sorry, no device found. Some networks do not return hostnames. Try uploading by ota.sh <ip>"
fi
