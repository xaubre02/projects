#!/bin/sh
# DIR="https://raw.githubusercontent.com/xbandi01/test/master/pic.zip"
wget https://github.com/xbandi01/test/raw/master/pic.zip | tr -d '\r'
unzip pic.zip -d lib | tr -d '\r'
rm pic.zip | tr -d '\r'
