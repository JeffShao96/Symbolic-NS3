# Symbolic ns-3

sym-ns-3 is proposed for efficient exhaustive testing, where we need to exhaustively test a network protocol or a network application, for all possible cases. It extends a well-developed and widely-used network simulator, [ns-3](https://www.nsnam.org/), and leverages a powerful symbolic execution platform, [s2e](https://github.com/S2E/s2e). Please refer to the [sym-ns-3 website](https://symbolicns3.github.io) for more information. 

This repository contains all the necessary code for installing, building, and running sym-ns-3 and the examples. 


Install S2E
Install sym-ns-3
Run examples


We recommand using Ubuntu 18.04 LTS or 20.04 LTS or higher Ubuntu version for this project. 
## Tips for Windows users using Virtualization Tools
You may have Linux installed via Vmware Workstation Player or Oracle VirtualBox.
While setup your virtual machine, we recommand to allocate at least:
* 1 vCPU
* 4 GB RAM
* 80 GB Hard Disk (SSD Disk is better)

We also recommand enable *Intel VT-x/AMD-v* for virtual machine, which enables KVM feature for guest system. Using KVM in this project would reduce building and running time.

Some combinations of system version and virtualization tools may work fine on Windows, but we found that the combination of latest Vmware and Ubuntu LTS is the best approach.

## Build S2E with s2e-env
We choose [S2E](https://s2e.systems/) as the symbolic execution platform to run Symbolic NS-3.

We highly recommend to build S2E with s2e-env. However, you can manually build S2E as well. 
### Convenient Script
We have written a shell file to build s2e. If successful, please skip to [Skip Point](#build-the-image).
```bash
wget https://raw.githubusercontent.com/JeffShao96/Symbolic-NS3/master/initS2E.sh
chmod +x initS2E.sh 
./initS2E.sh
```
### Manual Install Steps
Install the packages for s2e-env

    sudo apt-get update
    sudo apt-get install git gcc python3 python3-dev python3-venv

Clone the source code for s2e-env

    git clone https://github.com/s2e/s2e-env.git

Create a virtual environment to build S2E

    cd s2e-env
    python3 -m venv venv
    . venv/bin/activate
**You should do the following steps in the virtual environment**

If you want to quit from the virtual environment, use the following command:

    deactivate

Install the s2e-env

    pip install --upgrade pip
    pip install .
    
**Note: if your pip version is earlier than v19, use the following command:**

    pip install --process-dependency-links .

Download the source code for S2E

    mkdir $S2EDIR
    cd $S2EDIR
    s2e init
    
build the source code

    s2e build
    
**The process takes about 60 mins.**

## Build S2E-NS-3 image
Since S2E disables the networking when running, we need to install NS-3 into the image before we run S2E-NS-3.

`launch.sh` is the script that runs after the image is created. We have modified it to install NS-3. If you want to install other softwares, you can use that as an example.

    cd $S2EDIR
    wget -O source/guest-images/Linux/s2e_home/launch.sh https://raw.githubusercontent.com/JeffShao96/Symbolic-NS3/master/launch.sh

Since NS-3 is quite big, sometimes we need to extend the image or memory size.

Modify `$S2EDIR/source/guest-images/images.json`

Example:
```
    "cgc_debian-9.2.1-i386": {
      "name": "Debian i386 image with CGC kernel and user-space packages",
      "image_group": "linux",
      "url": "https://drive.google.com/open?id=1vexW3emZ5-jQ2hohelCfM3iAdFmcFqbq",
      "iso": {
        "url": "https://cdimage.debian.org/mirror/cdimage/archive/9.2.1/i386/iso-cd/debian-9.2.1-i386-netinst.iso"
      },
      "os": {
        "name": "cgc_debian",
        "version": "9.2.1",
        "arch": "i386",
        "build": "",
        "binary_formats": ["decree"]
      },
      "hw": {
        "default_disk_size": "4G",                  --This is the disk size of image, make sure your hardware has enough space before you extend it
        "default_snapshot_size": "256M",            --This is the Memory size, you should not set it too large, "1G" is recommended.
        "nic": "e1000"
      }
    }

```

Set S2E permissions

    sudo usermod -a -G docker $(whoami)
    sudo usermod -a -G kvm $(whoami)
    sudo chmod ugo+r /boot/vmlinu*
    
### Build the image
**Log out and log back** in so that your group membership is re-evaluated.

Run `s2e image_build` to check what image is available.

    s2e image_build <image_name> -g

For example:

    s2e image_build debian-9.2.1-i386 -g

If KVM is not available, use the following command:

    s2e image_build -n <image_name>
    
### Create and run the project

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

