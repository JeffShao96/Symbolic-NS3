#!/bin/bash

#Update softwares for S2E
sudo apt-get update
sudo apt-get -y install git gcc python3 python3-dev python3-venv

#Download s2e-env
git clone https://github.com/s2e/s2e-env.git

# Create an virtual environment to build S2E
cd s2e-env
python3 -m venv venv
. venv/bin/activate

# Upgrade the python packages
pip install --upgrade pip
pip install .

#Download and build S2E
mkdir ../s2e
cd ../s2e
s2e init
s2e build
