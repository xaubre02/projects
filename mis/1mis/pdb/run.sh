#!/bin/sh

mvn package $@
java -cp ./target/pdb18-1.0.jar pdb18.gui.AppGUI
