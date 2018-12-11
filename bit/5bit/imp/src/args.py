#!/usr/bin/env python3

"""
 -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 - Content:  Robot arguments
 - Author:   Tomas 'aubi' Aubrecht
 - Login:    xaubre02
 - Date:     2017-10-07
-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
"""

import sys


# Robot behavior options
class Options:
    def __init__(self):
        self.run     = False
        self.keep    = False
        self.control = False


# Program arguments processing
class Arguments:
    # arguments check and options settings
    def __init__(self):
        # private variable
        self.__o = Options()

        if len(sys.argv) == 1:
            print("Choose a mode! Run with '--help' for more information.", file=sys.stderr)
            exit(1)

        elif len(sys.argv) > 4:
            print("Too many arguments! Run with '--help' for more information.", file=sys.stderr)
            exit(1)

        else:
            if sys.argv[1] == "--help":
                if len(sys.argv) != 2:
                    print("Parameter '--help' can not be combined with others.", file=sys.stderr)
                    exit(1)
                self.__print_help()
                exit(0)

            elif sys.argv[1] == "-c":
                if len(sys.argv) != 2:
                    print("Parameter '-c' can not be combined with others.", file=sys.stderr)
                    exit(1)
                self.__o.control = True

            elif sys.argv[1] == "-r":
                if len(sys.argv) < 3:
                    print("Incomplete command: missing distance", file=sys.stderr)
                    exit(1)
                elif len(sys.argv) > 3:
                    print("Too many arguments!")
                    exit(1)
                self.__number_check(sys.argv[2])
                self.__o.run = int(sys.argv[2])

            elif sys.argv[1] == "-k":
                if len(sys.argv) != 4:
                    print("Incomplete command: missing distance", file=sys.stderr)
                    exit(1)
                self.__number_check(sys.argv[2])
                self.__number_check(sys.argv[3])
                self.__o.keep = (int(sys.argv[2]), int(sys.argv[3]))
                if self.__o.keep[0] > self.__o.keep[1]:
                    print("Invalid distance range", file=sys.stderr)
                    exit(1)

            else:
                self.__args_error(sys.argv[1])

    # return an object with set parammeters
    def get_options(self):
        return self.__o

    # exit with an error and print a message to stderr
    def __args_error(self, arg):
        print("Parameter '" + arg + "' is unknown!", file=sys.stderr)
        exit(1)

    # check whether provided string is an unsigned integer
    def __number_check(self, string):
        if not string.isdigit():
            print("'" + string + "' is not an unsigned integer!", file=sys.stderr)
            exit(1)

    # print help
    def __print_help(self):
        print(
            "\n\t-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-\n"
            "\t-  IMP - Simple robot                 -\n"
            "\t-  Author: Tomas Aubrecht (xaubre02)  -\n"
            "\t-  Year: 2017/2018                    -\n"
            "\t-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-\n\n"
            "\tRobot modes\n"
            "\t-+-+-+-+-+-\n\n"
            "\t--help\t show help\n\n"
            "\t-r N\t run away from an obstacle from N centimeters\n\n"
            "\t-k N M\t keep distance from an obstacle between N and M centimeters\n\n"
            "\t-c\t maunal robot control\n"
            )
