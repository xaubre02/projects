#!/bin/bash

out=/dev/null 2>&1

# check python3 installation
python3 --version > /dev/null  2>&1
if [[ $? -ne 0 ]]; then
	echo "Installing Python3.."
	sudo apt-get install python3
fi

# check python3 library
python3 -c "import paho.mqtt" > /dev/null  2>&1
if [[ $? -ne 0 ]]; then
	echo "Installing paho.mqtt package.."
	sudo apt-get install paho.mqtt
fi

# check mosquitto installation
mosquitto -h > /dev/null  2>&1
if [[ $? -ne 3 ]]; then
	echo "Installing mosquitto MQTT broker.."
	sudo apt-get install mosquitto
fi
