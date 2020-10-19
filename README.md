# Symbolic-NS3 (Under Development)
This demo will show how Symbolic Execution solve the problem.

## Code
We modified two files in NS-3:

[point-to-point-channel.h](./ns-3-dev/src/point-to-point/model/point-to-point-channel.h):

We add three members to the class for the API:
```cpp
private:
  ...
  bool m_symbolicDelay;  //!< Control signal of symbolic mode
  bool m_isinitialized = false; //!< Check the status of the symbolic delay
  Time m_delaymax;  //!< Maximum propagation delay
  Time m_delaymin;  //!< Minimum propagation delay
```
[point-to-point-channel.cc](./ns-3-dev/src/point-to-point/model/point-to-point-channel.cc):

Add attributes for API and initialize them.

```cpp
  static TypeId tid = TypeId ("ns3::PointToPointChannel")
    ...
    .AddAttribute ("DelayMin", "Symbobolic propagation delay through the channel Minimum", TimeValue (Seconds (0)),
                  MakeTimeAccessor (&PointToPointChannel::m_delaymin), MakeTimeChecker ())
    .AddAttribute ("DelayMax", "Symbobolic propagation delay through the channel Maximum", TimeValue (Seconds (10)),
                  MakeTimeAccessor (&PointToPointChannel::m_delaymax), MakeTimeChecker ())
    .AddAttribute ("SymbolicMode", "Control Signal of Symbolic Mode", BooleanValue (false),
                  MakeBooleanAccessor (&PointToPointChannel::m_symbolicDelay),
                  MakeBooleanChecker ())
...
```

We symbolize the delay in `Transit`:

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

## Demo

### Build S2E with s2e-env
We highly recommand to build S2E with s2e-env. However, you can manually build S2E as well. 
We provided a shell file to build s2e, if success, please skip to [Skip Point](#build-the-image).
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

Create an virtual environment to build S2E

    cd s2e-env
    python3 -m venv venv
    . venv/bin/activate
**You should do the following steps in the virtual environment**

If you want to quit the virtual environment, use the following command:

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
Since S2E will disable the networking when running, we need to install the NS-3 into the image before we run S2E-NS-3.

`launch.sh` is the script runs after image was created. We have already modified it to install the NS-3.If you want to install other softwares, you can use that as an example.

    cd $S2EDIR
    wget -O source/guest-images/Linux/s2e_home/launch.sh https://raw.githubusercontent.com/JeffShao96/S2E-NS-3/master/launch.sh

Since NS-3 is not a 'small' software, sometimes we need to extend the image or memory size.

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
        "default_disk_size": "4G",                  --This is the disk size of image, make sure your hardware have enough space before you extend it
        "default_snapshot_size": "256M",            --This is the Memory size, you should not set it too large, "1G" is recommended.
        "nic": "e1000"
      }
    }

```



Give the authority to S2E.

    sudo usermod -a -G docker $(whoami)
    sudo chmod ugo+r /boot/vmlinu*
    
#### Build the image
**Log out and log back** in so that your group membership is re-evaluated.

Run `s2e image_build` to check what image is available.

    s2e image_build <image_name> -g

For Example:

    s2e image_build debian-9.2.1-i386 -g

If KVM is not available, use the following command:

    s2e image_build -n <image_name>
    
### Create and run the project

create an empty project in S2E:

    s2e new_project -m -i <image_name> -n <project_name> -t linux

Eamxple:
Create an empty project named pointToPoint, type is linux, it will run in a 32-bit system `debian-9.2.1-i386`

    s2e new_project -m -i debian-9.2.1-i386 -n demo -t linux
    
We provide a demo to show how symbolic works in NS-3 system:
    
    cd $S2EDIR/project/demo
    wget -O bootstrap.sh https://raw.githubusercontent.com/JeffShao96/Symbolic-NS3/master/bootstrap.sh
    
Executed the project:

    ./launch-s2e.sh
    
You can use [demo.cc](./symDemo.cc) and [bootstrap.sh](./bootstrap.sh) as example to write your own project.
