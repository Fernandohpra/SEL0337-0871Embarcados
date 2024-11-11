#!/bin/bash


echo 17 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio17/direction
echo 27 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio27/direction
echo 22 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio22/direction
while [ 1 ]
    do
        echo 1 > /sys/class/gpio/gpio17/value
        sleep 0.2s
        echo 0 > /sys/class/gpio/gpio17/value
        sleep 0.2s
        echo 1 > /sys/class/gpio/gpio27/value
        sleep 0.2s
        echo 0 > /sys/class/gpio/gpio27/value
        sleep 0.2s
        echo 1 > /sys/class/gpio/gpio22/value
        sleep 0.2s
        echo 0 > /sys/class/gpio/gpio22/value
        sleep 0.2s
    done
