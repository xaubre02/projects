#!/usr/bin/env python3

"""
 -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 - Content:  Robot parts
 - Author:   Tomas 'aubi' Aubrecht
 - Date:     2017-10-07
 -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 - Pins:   - 5V  = 5V Output
           - GN  = Ground
           - EN  = Enable
           - Ox1 = Output 1 of motor x
           - Ox2 = Output 2 of motor x
           - TR  = Trigger
           - EC  = Echo
 -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 - Cable 1: L298N
   --------------------------------
   | EN1  O11  O12  O21  O22  EN2 |
   --------------------------------

 -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 - Cable 2: HY-SRF05
   ------------------
   | 5V 5V GN EC TR |
   ------------------

 -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
"""

import RPi.GPIO as gpio
from time import time, sleep


# Clean up all used ports
def gpio_cleanup():
    gpio.cleanup()


# Device: HY-SRF05 ultrasonic sensor
class HY_SRF05:
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
        duration = self.measure_duration()
        if duration is -1:
            return -1 # "Not responding!"

        elif duration is -2:
            return -2 # "Out of range!"

        else:
            return self.calculate_distance(duration)


    # Send a pulse to the TRIGGER
    # The sensor requires a short 0.01ms pulse to trigger the module, which will cause the sensor to send 8 ultrasound bursts at 40 kHz in order to obtain an echo response
    def send_pulse(self):
        gpio.output(self.trig, gpio.HIGH)
        sleep(0.00001)
        gpio.output(self.trig, gpio.LOW)


    # Measure the time how long was the ECHO responding
    # The sensor sets ECHO to high for the amount of time it takes for bursts to travel to the obstacle and back
    # Hardware timeout is 200 ms, but it is still too much
    def measure_duration(self):
        err = time()
        while gpio.input(self.echo) == gpio.LOW:
            p_start = time()
            # If the sensor is not responding for 10 ms
            if (time() - err) >= 0.01:
                return -1

        p_end = p_start
        while gpio.input(self.echo) == gpio.HIGH:
            p_end = time()

        duration = p_end - p_start
        # If the duration is too long (over 40 ms)
        if duration >= 0.04:
            return -2

        return duration


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
        self.pwm = gpio.PWM(self.enable, 1000)   # Pulse Width Modulation


    # Turn the motor on
    def turn_on(self):
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
        if speed > 100:
            self.speed = 100
        elif speed < 0:
            self.speed = 0
        else:
            self.speed = speed

    # Increase the motor speed by perc (in percentage)
    def speed_up(self, perc):
        if (self.speed + perc) > 100:
            self.speed = 100
        else:
            self.speed += perc

    # Decrease the motor speed by perc (in percentage)
    def speed_down(self, perc):
        if (self.speed - perc) < 0:
            self.speed = 0
        else:
            self.speed -= perc

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


# Device: L298N H-bridge module
class L298N:
    # Initialize two motors - the first on the left side and the second the right side
    def __init__(self, m1_enable, m1_out1, m1_out2, m2_enable, m2_out1, m2_out2):
        self.lm = Motor(m1_enable, m1_out1, m1_out2) # Left motor
        self.rm = Motor(m2_enable, m2_out1, m2_out2) # Right motor
        self.moving = False

    # Move forward
    def move_forward(self):
        self.moving = True
        self.lm.set_direction(False)
        self.rm.set_direction(True)
        self.lm.turn_on()
        self.rm.turn_on()

    # Move backward
    def move_backward(self):
        self.moving = True
        self.lm.set_direction(True)
        self.rm.set_direction(False)
        self.lm.turn_on()
        self.rm.turn_on()

    # Turn left
    def turn_left(self):
        self.moving = True
        self.rm.set_direction(False)
        self.lm.set_direction(False)
        self.rm.turn_on()
        self.lm.turn_on()

    # Turn right
    def turn_right(self):
        self.moving = True
        self.rm.set_direction(True)
        self.lm.set_direction(True)
        self.rm.turn_on()
        self.lm.turn_on()

    # Set the speed of both motors (in percentage)
    def set_speed(self, speed):
        if self.moving:
            self.lm.turn_off()
            self.rm.turn_off()

        self.lm.set_speed(speed)
        self.rm.set_speed(speed)

        if self.moving:
            sleep(0.05)
            self.lm.turn_on()
            self.rm.turn_on()

    # Increase the speed of both motors by percentage
    def speed_up(self, perc):
        if self.moving:
            self.lm.turn_off()
            self.rm.turn_off()

        self.lm.speed_up(perc)
        self.rm.speed_up(perc)

        if self.moving:
            sleep(0.05)
            self.lm.turn_on()
            self.rm.turn_on()

    # Decrease the speed of both motors by percentage
    def speed_down(self, perc):
        if self.moving:
            self.lm.turn_off()
            self.rm.turn_off()

        self.lm.speed_down(perc)
        self.rm.speed_down(perc)

        if self.moving:
            sleep(0.05)
            self.lm.turn_on()
            self.rm.turn_on()

    # Get the current speed
    def get_speed(self):
        return self.lm.speed

    # Stop both motors
    def stop(self):
        self.moving = False
        self.lm.turn_off()
        self.rm.turn_off()
