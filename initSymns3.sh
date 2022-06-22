#!/bin/bash

#replace launch.sh and images.json to build image with sym-ns-3
wget -O source/guest-images/Linux/s2e_home/launch.sh https://raw.githubusercontent.com/JeffShao96/Symbolic-NS3/master/launch.sh
wget -O source/guest-images/images.json https://raw.githubusercontent.com/JeffShao96/Symbolic-NS3/master/images.json

# Give the authority to S2E.
sudo usermod -a -G docker $(whoami)
sudo usermod -a -G kvm $(whoami)
sudo chmod ugo+r /boot/vmlinu*

# Build the image
s2e image_build debian-9.2.1-i386 
