# device-driver
Experimenting with Linux Kernel Development


## Development


### Using Centos6 in OSX

We can't compile the files on OSX, nor can we actually run 
the device driver on OSX. We'll copy files from OSX to **Centos 6** (an OS with the Linux Kernel)
and do all compilation and running there instead. 
   
Setting up Centos 6 with Virtual Box on OSX:

- **Create the Guest Image** Download the Centos 6 (minimal) ISO. Install Virtual Box and create a Centos 6 virutal machine using default settings. During the setup, take note of the passport you chose for `root` as you'll need it later for ssh and scp commands.
I avoided using Centos 7 because this use v3 of the Linux Kernel most books/literature about kernel development 
were written for v2 (and this is the kernel version Centos v6 uses)
- **Connect the Guest to the Internet** Ensure *Connect Network Adapter* is enabled. By default, Centos 6 does not connect the `eth` interfaces by default. So when you run `ifconfig` you'll only see the loopback interface. To fix this, modify (or create) `/etc/sysconfig/network-scripts/ifcfg-eth0` so that it contains at minimum the following:
  ```
  DEVICE="eth0"
  ONBOOT="yes"
  BOOTPROTO="dhcp"
  ```
  Then restart the networking daemon via `sudo service network restart`. 
  Now when you run `ifconfig` you should see an `eth` interface as well as the loopback interface. This means that
  Centos 6 can now connect to the Internet using your MacBook's network card.
- **Installing developer tools**: By default we don't have gcc and other tools needed for compilation: 
  ```
  yum groupinstall "Development tools"
  ```
- **Softlink `build` to the Kernel Source** By default, Centos6 minimal does not include kernel headers nor the kernel source. 
  As a result, the `build` softlink in
  the `/lib/modules/<kernel version>` will be broken and appear red when you do `ls` in that directory.
  The device driver we are writing needs the Kernel headers. 
  If we were to run `make`, we would get a "`No such file or directory`" error. 
  To fix this we download and link up the source and headers:
  ```
  sudo yum install kernel-headers
  sudo yum install kernel-devel
  sudo yum install build-essential
  ln -s /usr/src/kernels/$(uname -r)/ /lib/modules/$(uname -r)/build
  ```
- **Enable port forwarding for ssh** Run Centos 6. Then In VirtualBox, in Device -> Network -> *Network Settings*, click
  *Port Forwaring* and add a new row that forwards Host Port 3022 to Guest Port 22. The result is that when we can now
  run `ssh` and `scp` specifying port 3022 (arbitrarily chosen port) on OSX 
  and that will be forwarded to the ssh server on Centos 6 listening on port 22 (the well known port for ssh
  where Centos 6's ssh server will be listening).


### Running


To **compile the source**:

```bash
## On OSX (host machine)
cd <project-root>
scp -P 3022 * root@localhost:~

## ssh to Centos 6 (guest machine) then compile
ssh -p 3022 root@localhost
make
```

To **load the device driver** into memory:
```bash
insmod jjl_device.ko
```

To **unload the device driver** from memory:
```bash
rmmod jjl_driver
```

### Debugging


To check if the driver is currently loaded, you can check the `/proc/modules` file
```
cat /proc/modules | grep jjl
```

To check if the driver loaded succesfully, you can check the logs for a success message:
```
dmesg | grep "jjl"
> [jjl] Loading Module 'jjl_driver' ...
> [jjl] Inserted Module 'jjl_driver'
```
