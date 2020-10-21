# Symbolic-NS3 (Under Development)

Symbolic NS-3 is our on-going project to extend NS-3 for exhaustive testing where a user simulates a network protocol in all possible network environments with respect to some uncertain factors. Specifically, we modify and extend NS-3 by leveraging [symbolic execution](https://en.wikipedia.org/wiki/Symbolic_execution) so that it can be easily and efficiently used for exhaustive testing.

Below is a short introduction of Symbolic NS-3, and you can find more information in this [short paper](./Sym_NS_3_Draft.pdf). Please feel free to contact us (emails can be found in the short paper), if you have any questions, suggestions, or comments. 

## An exhaustive testing demo

Let's consider an exhaustive testing problem in a network where two senders and one receiver are connected by two point-to-point links. Both senders simultaneously send a UDP packet to the receiver. What is the range of all possible differences between the arrival times of these two packets if each link has an uncertain delay between 1 and 1000ms?

We can use the brute force method with the current NS-3 to simulate all possible cases (a total of 1000*1000=1,000,000 cases). You can download the corresponding scripts and code [here](https://github.com/JeffShao96/Current-NS3) (including [repeatCurrentDemo.sh](https://github.com/JeffShao96/Current-NS3/blob/main/repeatCurrentDemo.sh), [currentDemo.cc](https://github.com/JeffShao96/Current-NS3/blob/main/scratch/currentDemo.cc), and [udp-server.cc](https://github.com/JeffShao96/Current-NS3/blob/main/src/applications/model/udp-server.cc)). It takes a total of 521,900 seconds to run all 1,000,000 simulations, and the simulation result is presented and discussed in the [short paper](./Sym_NS_3_Draft.pdf). 

We propose a more efficient method using our proposed Symbolic NS-3. This repository contains our current Symbolic NS-3 and the corresponding simulation scripts and code (including [symDemo.cc](./symDemo.cc)  and [udp-server.cc](./ns-3-dev/src/applications/model/udp-server.cc)) to run this demo. It takes only 33 seconds to get the simulation result using Symbolic NS-3, and the simulation result is presented and discussed in the [short paper](./Sym_NS_3_Draft.pdf). 

Below is the instruction to download, install, and execute Symbolic NS-3.

## Instruction

### Build S2E with s2e-env
We choose [S2E](https://s2e.systems/) as the symbolic execution platform to run Symbolic NS-3.

We highly recommand to build S2E with s2e-env. However, you can manually build S2E as well. 
We have written a shell file to build s2e. If successful, please skip to [Skip Point](#build-the-image).
```bash
wget https://raw.githubusercontent.com/JeffShao96/Symbolic-NS3/master/initS2E.sh
chmod +x initS2E.sh 
./initS2E.sh
```

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

    pip install .
    pip install --upgrade pip
    
**Note: if your pip version is earlier than v19, use the following command:**

    pip install --process-dependency-links .

Download the source code for S2E

    mkdir $S2EDIR
    cd $S2EDIR
    s2e init
    
build the source code

    s2e build
    
**The process takes about 60 mins.**

### Build S2E-NS-3 image
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
    sudo chmod ugo+r /boot/vmlinu*
    
#### Build the image
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
Create an empty project named pointToPoint, type is linux, it runs in a 32-bit system `debian-9.2.1-i386`

    s2e new_project -m -i debian-9.2.1-i386 -n demo -t linux
    
Download the exhaustive testing demo
    
    cd $S2EDIR/project/demo
    wget -O bootstrap.sh https://raw.githubusercontent.com/JeffShao96/Symbolic-NS3/master/bootstrap.sh
    
Execute the exhasutive tesing demo

    ./launch-s2e.sh
    
You can use [symDemo.cc](./symDemo.cc) and [bootstrap.sh](./bootstrap.sh) as an example to write your own project.


## Simulation Code

To find the range of the difference between the arrival times of two packets using Symbolic NS-3, we write one script. The NS-3 script [symDemo.cc](./symDemo.cc) simulates the network with two symbolic link delays, each in the range of [1, 1000] ms. 

```cpp
...
  std::vector<PointToPointHelper> pointToPoint (2);
  pointToPoint[0].SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint[0].SetChannelAttribute ("SymbolicMode", BooleanValue (true));
  pointToPoint[0].SetChannelAttribute ("DelayMin", TimeValue (Time("1ms")));
  pointToPoint[0].SetChannelAttribute ("DelayMax", TimeValue (Time("1000ms")));

  pointToPoint[1].SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint[1].SetChannelAttribute ("SymbolicMode", BooleanValue (true));
  pointToPoint[1].SetChannelAttribute ("DelayMin", TimeValue (Time("1ms")));
  pointToPoint[1].SetChannelAttribute ("DelayMax", TimeValue (Time("1000ms")));
...
```

In addition, we modify NS-3 file [udp-server.cc](./src/applications/model/udp-server.cc) to keep track of the packet arrival times and then calculate and print out their difference.

## Symbolic NS-3 Code
The current Symbolic NS-3 modifies two files in the original NS-3:

[point-to-point-channel.h](./ns-3-dev/src/point-to-point/model/point-to-point-channel.h):

We add three members to the class for the new channel attributes:
```cpp
private:
  ...
  bool m_symbolicDelay;  //!< Enable or disable the symbolic delay
  bool m_isinitialized = false; //!< Check whether the symbolic delay variable has been initialized
  Time m_delaymax;  //!< Maximum value of the symbolic delay
  Time m_delaymin;  //!< Minimum value of the symbolic delay
```
[point-to-point-channel.cc](./ns-3-dev/src/point-to-point/model/point-to-point-channel.cc):

Add the attributes and initialize them.

```cpp
  static TypeId tid = TypeId ("ns3::PointToPointChannel")
    ...
    .AddAttribute ("DelayMin", "Minimum value of the symbolic delay", TimeValue (Seconds (0)),
                  MakeTimeAccessor (&PointToPointChannel::m_delaymin), MakeTimeChecker ())
    .AddAttribute ("DelayMax", "Maximum value of the symbolic delay", TimeValue (Seconds (10)),
                  MakeTimeAccessor (&PointToPointChannel::m_delaymax), MakeTimeChecker ())
    .AddAttribute ("SymbolicMode", "Enable or disable the symbolic delay", BooleanValue (false),
                  MakeBooleanAccessor (&PointToPointChannel::m_symbolicDelay),
                  MakeBooleanChecker ())
...
```

We symbolize the delay in `TransmitStart`:

```cpp
bool PointToPointChannel::TransmitStart (
  Ptr<const Packet> p,
  Ptr<PointToPointNetDevice> src,
  Time txTime)
{
  ...
  if(m_symbolicDelay&&!m_isinitialized){
    uintptr_t m_delayinit = 0;
    s2e_make_symbolic(&m_delayinit,sizeof(m_delayinit),"m_delayinit");
    m_delay = Time(m_delayinit);
    if(m_delay<m_delaymin){
      s2e_kill_state(0,"Out of Range, Lower");
    }else if(m_delay>m_delaymax){
      s2e_kill_state(0,"Out of Range, Upper");
    }
    m_isinitialized = true;
  }
  ...
}
```
