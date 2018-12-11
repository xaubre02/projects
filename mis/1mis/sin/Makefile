all:
	
install:
	chmod +x ./install_requirements.sh
	./install_requirements.sh
	mkdir build
	wget 'https://releases.domoticz.com/releases/release/domoticz_linux_x86_64.tgz'
	tar -xvzf domoticz_linux_x86_64.tgz -C build/
	rm domoticz_linux_x86_64.tgz
	cp domoticz.db build/

run:
	mosquitto >> build/mosquitto_log &
	./build/domoticz -wwwbind localhost -www 6000 -sslwww 0 >> build/domoticz_log &
	sleep 8
	python3 agentControllingAirConditions.py \
	--min_required_temperature 22 --max_required_temperature 25 \
	--min_required_humidity 40 --max_required_humidity 60 \
	--min_required_co2 800 --max_required_co2 1500

clean:
	rm -R build
