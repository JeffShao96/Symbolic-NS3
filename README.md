# Symbolic ns-3

sym-ns-3 is proposed for efficient exhaustive testing, where we need to exhaustively test a network protocol or a network application, for all possible cases. It extends a well-developed and widely-used network simulator, [ns-3](https://www.nsnam.org/), and leverages a powerful symbolic execution platform, [s2e](https://github.com/S2E/s2e). Please refer to the [sym-ns-3 website](https://symbolicns3.github.io) for more information. This repository contains all the necessary code for installing, building, and running sym-ns-3 and the examples. 

Instructions
* [1. Operating systems](#1-operating-systems)
* [2. Install S2E](#2-install-s2e)
* [3. Install sym-ns-3](#3-install-sym-ns-3)
* [4. Run examples](#4-run-examples)

## 1. Operating systems

We have tested our code mainly using Ubuntu, so we recommand Ubuntu 18.04 LTS, 20.04 LTS, or higher versions. 

For Windows users, you may install Ubuntu via Vmware Workstation Player or Oracle VirtualBox. Please enable *Intel VT-x/AMD-v* for your virtual machine, which enables the KVM feature and consequently reduces the building and running time. We recommand the following configurations for your virtual machine:
* at least 1 vCPU
* at least 4 GB RAM
* at least 80 GB Hard Disk (SSD Disk is better)


## 2. Install S2E

We choose [S2E](https://s2e.systems/) as the symbolic execution platform to run sym-ns-3, as it is good at symbolically executing big software systems.

There are two different methods to install S2E. We recommend the script method. If it does not work, please then try the step by step method.
* [2.1 Script method](#21-script-method) (Recommended)
* [2.2 Step by step method](#22-step-by-step-method)

### 2.1 Script method
You may install S2E using our [`initS2E.sh`](./initS2E.sh) script by typing the following commands. 

```bash
wget https://raw.githubusercontent.com/JeffShao96/Symbolic-NS3/master/initS2E.sh
chmod +x initS2E.sh 
./initS2E.sh
```

### 2.2 Step by step method
This method is essentially the same as the [s2e-env method](http://s2e.systems/docs/s2e-env.html#installing-s2e-env) described on the S2E website.

Install the packages

    sudo apt-get update
    sudo apt-get install git gcc python3 python3-dev python3-venv

Clone the source code of s2e-env

    git clone https://github.com/s2e/s2e-env.git

Create a virtual environment

    cd s2e-env
    python3 -m venv venv
    . venv/bin/activate
    
Install s2e-env

    pip install --upgrade pip
    pip install .
    

Download the source code of S2E

    mkdir ../s2e
    cd ../s2e
    s2e init
    
Build S2E (this step takes a long time)

    s2e build
    

## 3. Install sym-ns-3

This step creates an S2E virtual machine image that contains sym-ns-3. Please run the following commands in the `s2e` folder.

There are two different methods to install sym-ns-3. We recommend the script method. If it does not work, please then try the step by step method.
* [3.1 Script method](#31-script-method) (Recommended)
* [3.2 Step by step method](#32-step-by-step-method)

### 3.1 Script method
You may install sym-ns-3 using our [`initSymns3.sh`](./initSymns3.sh) script by typing the following commands. 

```bash
wget https://raw.githubusercontent.com/JeffShao96/Symbolic-NS3/master/initSymns3.sh
chmod +x initSymns3.sh 
./initSymns3.sh
```

### 3.2 Step by step method

Modify S2E file `launch.sh` to download and install sym-ns-3 to an S2E virtual machine image.

    wget -O source/guest-images/Linux/s2e_home/launch.sh https://raw.githubusercontent.com/JeffShao96/Symbolic-NS3/master/launch.sh

Modify S2E file `images.json` to increase the memory and disk sizes of the virtual machine image as sym-ns-3 is quite big.

    wget -O source/guest-images/images.json https://raw.githubusercontent.com/JeffShao96/Symbolic-NS3/master/images.json

Set S2E permissions. Sometimes, you may need to log out and then back for these commands to take effect.

    sudo usermod -a -G docker $(whoami)
    sudo usermod -a -G kvm $(whoami)
    sudo chmod ugo+r /boot/vmlinu*

Build an S2E virtual machine image with sym-ns-3 (this step takes a long time). Note that, we choose the Linux Debian 9.2.1 i386 image, as it is small.

    s2e image_build debian-9.2.1-i386 

    
## 4. Run examples

Create and run the project

create an empty project in S2E:

    s2e new_project -m -i <image_name> -n <project_name> -t linux

Example:
Create an empty project named pointToPoint, linux type, it runs in a 32-bit system `debian-9.2.1-i386`

    s2e new_project -m -i debian-9.2.1-i386 -n demo -t linux
    
Download the exhaustive testing demo
    
    cd $S2EDIR/project/demo
    wget -O bootstrap.sh https://raw.githubusercontent.com/JeffShao96/Symbolic-NS3/master/bootstrap.sh
    
Execute the exhaustive testing demo

    ./launch-s2e.sh
    
You can use [symDemo.cc](./ns-3-dev/scratch/symDemo.cc) and [bootstrap.sh](./bootstrap.sh) as an example to write your own project.

To run other demo, for example, to run [reachabilitySymEx](./ns-3-dev/scratch/reachabilitySymEx.cc), you should change the last line of [bootstrap.sh](./bootstrap.sh) into
    
    ${S2EGET} "reachabilitySymEx.cc"            #use [S2EGET] to pass the script in to the image
    cp reachabilitySymEx.cc ns-3-dev/scratch    #copy the script to scratch folder
    cd ns-3-dev                             
    ./waf --run reachabilitySymEx               #run the NS-3 scratch
    

Then run the s2e script

    ./launch-s2e.sh

