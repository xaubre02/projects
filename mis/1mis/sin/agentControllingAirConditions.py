#!/usr/bin/env python3

import argparse
import json
import paho.mqtt.client as mqtt
import sys
import random
import time

BROKER_ADDRESS = '127.0.0.1'
DEFAULT_IN_TOPIC = 'domoticz/in'
DEFAULT_OUT_TOPIC = 'domoticz/out'

DEFAULT_MIN_TEMPERATURE = 22
DEFAULT_MAX_TEMPERATURE = 25
DEFAULT_MIN_HUMIDITY = 40
DEFAULT_MAX_HUMIDITY = 60
DEFAULT_MIN_CO2 = 900
DEFAULT_MAX_CO2 = 1500

CONTROL_SYSTEM_NAME = 'AgentControllingAirConditions'
REFRESH_TIME = 10

# Sensor measures temperature and humidity. Provides methods to generate new values
# which are influenced by actors (Heating, AC, Humidifier).
class TempHumSensor:
	def __init__(self, deviceId, client):
		self._id = deviceId
		self._client = client
		self._temperature = random.randint(10, 30)
		self._humidity = random.randint(20, 80)

		self._heating = False
		self._ac = False
		self._humidifing = False
		self._airFlow = False

	def id(self):
		return self._id

	def temperature(self):
		return self._temperature

	def humidity(self):
		return self._humidity

	def setHeating(self, status):
		self._heating = status

	def setAC(self, status):
		self._ac = status

	def setHumidifing(self, status):
		self._humidifing = status

	def setAirFlow(self, status):
		self._airFlow = status

	def newTemperature(self):
		if self._heating == True and self._ac == True:
			if random.randint(0, 10) > 5:
				self._temperature += random.randint(0, 10) / 10
			else:
				self._temperature -= random.randint(0, 10) / 10
		elif self._heating == True:
			self._temperature += random.randint(0, 10) / 10
		elif self._ac == True:
			self._temperature -= random.randint(0, 10) / 10
		else:
			if random.randint(0, 10) > 5:
				self._temperature += random.randint(0, 10) / 10
			else:
				self._temperature -= random.randint(0, 10) / 10

		if (self._temperature < -20):
			self._temperature = -20
		elif (self._temperature > 80):
			self._temperature = 80

		return self._temperature

	def newHumidity(self):
		if self._humidifing == True and self._airFlow == False:
			if self._heating == True or self._ac == True: # normal random changes of humidity
				if random.randint(0, 10) > 5:
					self._humidity -= random.randint(0, 3)
				else:
					self._humidity += random.randint(0, 3)
			else: # humidity is increasing
					self._humidity += random.randint(0, 5)
		elif self._humidifing == False and self._airFlow == True:
			if self._heating == True or self._ac == True: # humidity should be increasing but with less probability
				if random.randint(0, 10) > 3:
					self._humidity -= random.randint(0, 5)
				else:
					self._humidity += random.randint(0, 3)
			else: # humidity should be decreasing
				if self._humidity < 50: # outside humidity is around 50 so if the humidity in household is below 50, normal random changes
					if random.randint(0, 10) > 5:
						self._humidity -= random.randint(0, 3)
					else:
						self._humidity += random.randint(0, 3)
				else:
					if random.randint(0, 10) > 3:
						self._humidity -= random.randint(0, 5)
					else:
						self._humidity += random.randint(0, 3)
		else:
			if self._heating == True or self._ac == True: # humidity should be decreasing
				if random.randint(0, 10) > 3:
					self._humidity -= random.randint(0, 3)
				else:
					self._humidity += random.randint(0, 3)
			else: # normal random changes of humidity
				if random.randint(0, 10) > 5:
					self._humidity -= random.randint(0, 3)
				else:
					self._humidity += random.randint(0, 3)

		if (self._humidity < 20):
			self._humidity = 20
		elif (self._humidity > 95):
			self._humidity = 95

		return self._humidity

	def sendNewData(self):
		temp = self.newTemperature()
		hum = self.newHumidity()

		msg = '{'\
					'"idx" : ' + str(self.id()) + ','\
					'"nvalue" : 0,'\
					'"svalue" : "' + str(temp) + ';' + str(hum) + ';0"'\
			  '}'

		print("INFO: publishing data from TempHumSensor" + str(self.id()) + " temp: " + str(temp) + " a hum: " + str(hum));
		self._client.publish(DEFAULT_IN_TOPIC, msg);

# Sensor measures CO2. Provides methods to generate new values
# which are influenced by actors (Air Flow).
class CO2Sensor:
	def __init__(self, deviceId, client):
		self._id = deviceId
		self._client = client
		self._co2 = random.randint(400, 1500)

		self._airFlow = False

	def id(self):
		return self._id

	def co2(self):
		return self._co2

	def setAirFlow(self, status):
		self._airFlow = status

	def newCO2(self):
		if self._airFlow == True: # CO2 should be decreasing
			if random.randint(0, 10) > 2:
				self._co2 -= random.randint(0, 70)
			else:
				self._co2 += random.randint(0, 40)
		else: # CO2 should be increasing
			if random.randint(0, 10) > 2:
				self._co2 += random.randint(0, 50)
			else:
				self._co2 -= random.randint(0, 50)

		# 350 PPM is really low co2 outside so it could not be less then this value
		if (self._co2 < 350):
			self._co2 = 350
		elif (self._co2 > 5000):
			self._co2 = 5000

		return self._co2

	def sendNewData(self):
		co2 = self.newCO2()

		msg = '{'\
					'"idx" : ' + str(self.id()) + ','\
					'"nvalue" : ' + str(co2) +\
			  '}'

		print("INFO: publishing data from CO2Sensor" + str(self.id()) + " co2: " + str(co2));
		self._client.publish(DEFAULT_IN_TOPIC, msg);

# RemoteSwitch represents generic swich. Provides method to change state (On/Off) and
# send the command to domoticz.
class RemoteSwitch:
	def __init__(self, deviceId, client):
		self._id = deviceId
		self._client = client

		self._state = 'Off'

	def id(self):
		return self._id

	def state(self):
		if self._state == 'On':
			return True
		else:
			return False

	def setState(self, newState):
		self._state = 'On' if newState == True else 'Off'

	def on(self):
		self.sendCommand('On')

	def off(self):
		self.sendCommand('Off')

	def sendCommand(self, state):
		msg = '{'\
					'"command": "switchlight",'\
					'"idx": ' + str(self.id()) + ','\
					'"switchcmd": "' + state + '"'\
			  '}'

		print("INFO: publishing command to " + self._state + " the switch");
		self._client.publish(DEFAULT_IN_TOPIC, msg)

# Reflects the manual changes of actors in to the agent.
def on_message(client, devices, message):
	data = json.loads(message.payload.decode('utf-8'))

	if data['stype'] != 'Switch':
		return

	idx = data['idx']
	settingValue = True if data['nvalue'] == 1 else False

	sensors1 = devices[0][0]
	sensors2 = devices[0][1]

	heatingSwitches = devices[1][0]
	acSwitches =  devices[1][1]
	humidifierSwitch = devices[1][2]
	airFlowSwitches =  devices[1][3]

	for i in range(0,3):
		if heatingSwitches[i].id() == idx:
			heatingSwitches[i].setState(settingValue)
			sensors1[i].setHeating(settingValue)
		elif acSwitches[i].id() == idx:
			acSwitches[i].setState(settingValue)
			sensors1[i].setAC(settingValue)
		elif humidifierSwitch[i].id() == idx:
			humidifierSwitch[i].setState(settingValue)
			sensors1[i].setHumidifing(settingValue)
		elif airFlowSwitches[i].id() == idx:
			airFlowSwitches[i].setState(settingValue)
			sensors1[i].setAirFlow(settingValue)
			sensors2[i].setAirFlow(settingValue)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description=sys.modules[__name__].__doc__)
	parser.add_argument("--min_required_temperature", metavar="MIN_REQUIRED_TEMPERATURE",
		help="The agent will try to hold temperature in household above this value. (Default 22 degree of Celisus)")
	parser.add_argument("--max_required_temperature", metavar="MAX_REQUIRED_TEMPERATURE",
		help="The agent will try to hold temperature in household below this value. (Default 25 degree of Celisus)")
	parser.add_argument("--min_required_humidity", metavar="MIN_REQUIRED_HUMIDITY",
		help="The agent will try to hold humidity in household above this value. (Default 40 percents)")
	parser.add_argument("--max_required_humidity", metavar="MAX_REQUIRED_HUMIDITY",
		help="The agent will try to hold humidity in household below this value. (Default 60 percents)")
	parser.add_argument("--min_required_co2", metavar="MIN_REQUIRED_CO2",
		help="The agent will try to hold CO2 in household above this value. (Default 900 PPM)")
	parser.add_argument("--max_required_co2", metavar="MAX_REQUIRED_CO2",
		help="The agent will try to hold CO2 in household below this value. (Default 1500 PPM)")
	args = parser.parse_args()

	minTemp = args.min_required_temperature if args.min_required_temperature is not None else DEFAULT_MIN_TEMPERATURE
	maxTemp = args.max_required_temperature if args.max_required_temperature is not None else DEFAULT_MAX_TEMPERATURE
	minHum = args.min_required_humidity if args.min_required_humidity is not None else DEFAULT_MIN_HUMIDITY
	maxHum = args.max_required_humidity if args.max_required_humidity is not None else DEFAULT_MAX_HUMIDITY
	minCo2 = args.min_required_co2 if args.min_required_co2 is not None else DEFAULT_MIN_CO2
	maxCo2 = args.max_required_co2 if args.max_required_co2 is not None else DEFAULT_MAX_CO2

	if type(minTemp) == int:
		print("Error: min_required_temperature has to be number")
		sys.exit()
	elif type(maxTemp) == int:
		print("Error: max_required_temperature has to be number")
		sys.exit()
	elif type(minHum) == int:
		print("Error: min_required_humidity has to be number")
		sys.exit()
	elif type(maxHum) == int:
		print("Error: max_required_humidity has to be number")
		sys.exit()
	elif type(minCo2) == int:
		print("Error: min_required_co2 has to be number")
		sys.exit()
	elif type(maxCo2) == int:
		print("Error: max_required_co2 has to be number")
		sys.exit()
	else:
		minTemp = int(minTemp)
		maxTemp = int(maxTemp)
		minHum = int(minHum)
		maxHum = int(maxHum)
		minCo2 = int(minCo2)
		maxCo2 = int(maxCo2)

	print("INFO: creating MQTT client with name %s" % (CONTROL_SYSTEM_NAME));
	client = mqtt.Client(CONTROL_SYSTEM_NAME)

	sensors1 = [TempHumSensor(1, client), TempHumSensor(2, client), TempHumSensor(3, client)]
	sensors2 = [CO2Sensor(4, client), CO2Sensor(5, client), CO2Sensor(6, client)]

	heatingSwitches = [RemoteSwitch(7, client), RemoteSwitch(8, client), RemoteSwitch(9, client)]
	acSwitches = [RemoteSwitch(10, client), RemoteSwitch(11, client), RemoteSwitch(12, client)]
	humidifierSwitch = [RemoteSwitch(13, client), RemoteSwitch(14, client), RemoteSwitch(15, client)]
	airFlowSwitches = [RemoteSwitch(16, client), RemoteSwitch(17, client), RemoteSwitch(18, client)]

	# initilize subscribe callback
	devices = [[sensors1, sensors2], [heatingSwitches, acSwitches, humidifierSwitch, airFlowSwitches]]
	client.user_data_set(devices)
	client.on_message = on_message

	print("INFO: connecting to broker on address %s" % (BROKER_ADDRESS));
	client.connect(BROKER_ADDRESS)

	print("INFO: subscribing to topic %s" % (DEFAULT_OUT_TOPIC));
	client.subscribe(DEFAULT_OUT_TOPIC)
	client.loop_start()

	# initilize all switches
	for i in range(0, 3):
		heatingSwitches[i].off()
		acSwitches[i].off()
		humidifierSwitch[i].off()
		airFlowSwitches[i].off()

	try:
		while True:
			for i in range(0, 3):
				sensors1[i].sendNewData()
				sensors2[i].sendNewData()

				# Controls the temperature
				if sensors1[i].temperature() < minTemp:
					if acSwitches[i].state() == True:
						acSwitches[i].off()
					if heatingSwitches[i].state() == False:
						heatingSwitches[i].on()
				elif sensors1[i].temperature() >= minTemp and sensors1[i].temperature() <= maxTemp:
					if acSwitches[i].state() == True:
						acSwitches[i].off()
					if heatingSwitches[i].state() == True:
						heatingSwitches[i].off()
				elif sensors1[i].temperature() >= maxTemp:
					if acSwitches[i].state() == False:
						acSwitches[i].on()
					if heatingSwitches[i].state() == True:
						heatingSwitches[i].off()

				# Controls the humidity
				if sensors1[i].humidity() < minHum:
					if humidifierSwitch[i].state() == False:
						humidifierSwitch[i].on()
				elif sensors1[i].humidity() >= (maxHum - ((maxHum - minHum) / 2)):
					if humidifierSwitch[i].state() == True:
						humidifierSwitch[i].off()

				if sensors1[i].humidity() > maxHum:
					if airFlowSwitches[i].state() == False:
						airFlowSwitches[i].on()

				# Controls the co2
				if sensors2[i].co2() > maxCo2:
					if airFlowSwitches[i].state() == False:
						airFlowSwitches[i].on()

				# Turn off the air flow if both humidity and co2 is below min value
				if sensors1[i].humidity() < maxHum and sensors2[i].co2() < minCo2:
					if airFlowSwitches[i].state() == True:
						airFlowSwitches[i].off()

			time.sleep(REFRESH_TIME)
	except KeyboardInterrupt:
		client.disconnect()
