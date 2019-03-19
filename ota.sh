# https://stackoverflow.com/questions/27389089/dynamic-option-selection-in-bash

if [ -n "$1" ]
then
  answer=$1
else
  # use https://apple.stackexchange.com/questions/20547/how-do-i-find-my-ip-address-from-the-command-line
  # to automatically detect ip to upload
  network=`./ip_address.sh |cut -f 1-3 -d .`
  network="$network.*"
  echo "scanning $network"
  esps=`nmap -sn $network|grep -i esp_|perl -pe 's/.* (esp_[^ ]*) \(([^)]*)\)/$1: $2/i'`
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
  echo "Sorry, no ports found"
fi
