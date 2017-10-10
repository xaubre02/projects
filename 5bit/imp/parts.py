#!/usr/bin/env python3

"""
 -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 - Content:  Robot parts
 - Author:   Tomas 'aubi' Aubrecht
 - Date:     2017-10-07
-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
"""

from time import time, sleep
import RPi.GPIO as gpio


# Device: HY-SRF05 ultrasonic sensor
class Sensor:
    # Initialize the pin numbering and pins assignment
    def __init__(self, trig, echo):
        # Broadcom chip-specific pin numbering
        if gpio.getmode() is not gpio.BCM:
            gpio.setmode(gpio.BCM)

        self.trig = trig
        self.echo = echo

        gpio.setup(self.trig, gpio.OUT)    # Output pin
        gpio.setup(self.echo, gpio.IN)     # Input pin


    # Return a distance (in cm) between the sensor and an obstacle
    def get_distance(self):
        self.send_pulse()
        return self.calculate_distance(self.measure_duration())


    # Send a pulse to the TRIGGER
    # The sensor requires a short 0.01ms pulse to trigger the module, which will cause the sensor to send 8 ultrasound bursts at 40 kHz in order to obtain an echo response
    def send_pulse(self):
        gpio.output(self.trig, gpio.HIGH)
        sleep(0.00001)
        gpio.output(self.trig, gpio.LOW)


    # Measure the time how long was the ECHO responding
    # The sensor sets ECHO to high for the amount of time it takes for bursts to travel to the obstacle and back
    def measure_duration(self):
        while gpio.input(echo) == gpio.LOW:
            p_start = time()

        while gpio.input(echo) == gpio.HIGH:
            p_end = time()

        return p_end - p_start


    # Calculate the distance
    def calculate_distance(self, duration):
        # Distance = Speed * Time
        # Speed of the sound in the air is 343m/s, which is 34300 cm/s
        # The sound must reach the obstacle and get back - divide by 2
        return round(17150 * duration, 1)


# Device: DC Motor 6V
class Motor:
    # Initialize the pin numbering, pins assignment and motor attributes
    def __init__(self, enable, out1, out2):
        # Broadcom chip-specific pin numbering
        if gpio.getmode() is not gpio.BCM:
            gpio.setmode(gpio.BCM)

        self.enable = enable
        self.out1   = out1
        self.out2   = out2

        gpio.setup(self.enable, gpio.OUT)  # Output pin
        gpio.setup(self.out1,   gpio.OUT)  # Output pin
        gpio.setup(self.out2,   gpio.OUT)  # Output pin

        self.clockwise = True              # Spin direction
        self.speed = 100                   # Duty cycle

        # Low frequencies:   100..1000Hz
        # High frequencies:  10..50kHZ
        self.pwm = gpio.PWM(self.enable, 3000)   # Pulse Width Modulation
        #lower their PWM frequency because they can't get the motor turning with small duty cycles


    # Turn the motor on
    def turn_on(self):
        # Enable one of the OUT pins
        self.set_direction(self.clockwise)
        self.pwm.start(self.speed)


    # Turn the motor off
    def turn_off(self):
        self.pwm.stop()
        # Energy saving
        gpio.output(self.out1, gpio.LOW)
        gpio.output(self.out2, gpio.LOW)


    # Set the motor speed (in percentage)
    def set_speed(self, speed):
        self.pwm.stop()
        self.speed = speed
        self.pwm.start(self.speed)


    # Set the spin direction
    def set_direction(self, clockwise):
        self.clockwise = clockwise

        # clockwise
        if self.clockwise:
            gpio.output(self.out1, gpio.HIGH)
            gpio.output(self.out2, gpio.LOW)

        # counter-clockwise
        else:
            gpio.output(self.out1, gpio.LOW)
            gpio.output(self.out2, gpio.HIGH)


# Device: L293D IC Motor Driver
class Driver:
    def __init__(self, pin1, pin2, pin3, pin4, pin5, pin6):
        self.m1 = Motor(pin1, pin2, pin3)
        self.m2 = Motor(pin4, pin5, pin6)
