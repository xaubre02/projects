#!/usr/bin/env python3

"""
 -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 - Content:  Robot control
 - Author:   Tomas 'aubi' Aubrecht
 - Login:    xaubre02
 - Date:     2017-10-07
-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
"""

import parts
from args import *
from time import sleep
from getch import getch

def run_away(sensor, driver, distance):
    while True:
        dis = sensor.get_distance()
        # if the sensor is not responding or if is out of range
        if dis == -1 or dis == -2:
            sleep(0.25)
            continue

        elif dis < distance:
            # move away from an obstacle until the required distance is reached
            driver.move_backward()
            while sensor.get_distance() < distance:
                sleep(0.25)
            driver.stop()

        else:
            # measure the distance once per 250ms
            sleep(0.25)


def keep_distance(sensor, driver, dis_from, dis_to):
    while True:
        dis = sensor.get_distance()
        # if the sensor is not responding or if is out of range
        if dis == -1 or dis == -2:
            sleep(0.25)
            continue

        elif dis < dis_from:
            # move away from an obstacle until the required distance is reached
            driver.move_backward()
            while sensor.get_distance() < dis_from:
                sleep(0.25)
            driver.stop()

        elif dis > dis_to:
            # move towards an obstacle until the required distance is reached
            driver.move_forward()
            while sensor.get_distance() > dis_to:
                sleep(0.25)
            driver.stop()

        else:
            # measure the distance once per 250ms
            sleep(0.25)


def control(sensor, driver):
    print(
        " Manual control\n"
        " -+-+-+-+-+-+-+-\n\n"
        " Movement:    W\n"
        "            A S D\n\n"
        " Speed:     + -\n\n"
        " Stop:      Space\n\n"
        " Exit:      ESC\n")

    sys.stdout.write("Speed: " + str(driver.get_speed()) + "% ")
    sys.stdout.flush()
    while True:
        ch = getch()
        # ESC key -> exit
        if ord(ch) == 27:
            print()
            break

        elif ch == "w":
            driver.move_forward()

        elif ch == "s":
            driver.move_backward()

        elif ch == "a":
            driver.turn_left()

        elif ch == "d":
            driver.turn_right()

        elif ch == " ":
            driver.stop()
        
        elif ch == "+":
            driver.speed_up(10)
            sys.stdout.write("\rSpeed: " + str(driver.get_speed()) + "%  ")
            sys.stdout.flush()
            
        elif ch == "-":
            driver.speed_down(10)
            sys.stdout.write("\rSpeed: " + str(driver.get_speed()) + "%  ")
            sys.stdout.flush()
            
        else:
            continue


# Main function
def main():
    opt = Arguments().get_options()
    try:
        sensor = parts.HY_SRF05(12, 6)
        driver = parts.L298N(13, 19, 26, 16, 20, 21)
        driver.stop()
        
        if opt.run is not False:
            run_away(sensor, driver, opt.run)

        elif opt.keep is not False:
            keep_distance(sensor, driver, opt.keep[0], opt.keep[1])

        elif opt.control is not False:
            control(sensor, driver)

        else: # this should not happen
            print("Unknown parameter error")
            exit(1)

    except KeyboardInterrupt:
        pass

    finally:
        parts.gpio_cleanup()


if __name__ == "__main__":
    main()
