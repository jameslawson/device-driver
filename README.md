# device-driver
Experimenting with Linux Kernel Development


## Development

Requirements:
- [Oracle VM VirtualBox](https://www.virtualbox.org/) v5+ 
- Version 2.6 of the Linux Kernel

We'll assume the environment is an macOS / OSX machine with CentOS 6 running on VirtualBox ([see below](https://github.com/jameslawson/device-driver#using-centos-6-on-macos))

> :information_source: Assume all commands are to be executed on the CentOS (guest) machine unless otherwise specified

To **compile the source**:

```bash
## On OSX (host machine)
cd <project-root>
scp -P 3022 * root@localhost:~

## ssh to Centos6 (guest machine) then compile
ssh -p 3022 root@localhost
make
```

To **load the device driver** into memory:
```bash
chmod u+x ./load_driver.sh
./load_driver.sh
```

To **unload the device driver** from memory:
```bash
chmod u+x ./unload_driver.sh
./unload_driver.sh
```

### Debugging


To check if the driver is currently loaded:
```
cat /proc/modules | grep jjl
```

To check if the driver has registered a device number:
```
cat /proc/devices | grep jjl
```

To check if the driver has a device file:
```
ls /dev/jjl
```

To check the debugging logs:
```
dmesg | grep "jjl"
> [jjl] Loading Module 'jjl_driver' ...
> [jjl] Inserted Module 'jjl_driver'
```

### Using CentOS 6 on macOS

We can't compile the files on macOS (OSX), nor can we actually run 
the device driver on macOS. You need a Linux machine (specifically, an OS with the Linux Kernel) 
to do these things. For example, we can use **CentOS 6**.
The development cycle involves copying files from macOS to CentOS 6 
and do all compilation and running in Centos 6. 
   
Setting up Centos 6 with Virtual Box on macOS:

- **Create the Guest Image**: Download the Centos 6 (minimal) ISO. Install VirtualBox and create a CentOS 6 virutal machine using default settings. During the setup, take note of the password you chose for `root` as you'll need it later for ssh and scp commands.
I avoided CentOS 7 because this uses v3 of the Linux Kernel and most books/literature about kernel development 
are currently written for v2 (which is incidentally the kernel version Centos v6 uses).
- **Connect the Guest to the Internet**: Ensure *Connect Network Adapter* is enabled in VirtualBox. By default, CentOS 6 does not connect the `eth` interfaces by default. So when you run `ifconfig` you'll only see the loopback interface. To fix this, modify (or create) `/etc/sysconfig/network-scripts/ifcfg-eth0` so that it contains at minimum the following:
  ```
  DEVICE="eth0"
  ONBOOT="yes"
  BOOTPROTO="dhcp"
  ```
  Then restart the networking daemon via `sudo service network restart`. 
  Subsequently, when you run `ifconfig` you should see an `eth` interface in addition to the loopback interface. This means that
  CentOS 6 can now connect to the Internet using your MacBook's network card.
- **Installing developer tools**: By default we don't have gcc and other tools needed for compilation: 
  ```
  yum groupinstall "Development tools"
  ```
- **Softlink `build` to the Kernel Source** By default, CentOS 6 minimal does not include kernel headers nor the kernel source. 
  As a result, the `build` softlink in
  the `/lib/modules/<kernel version>` directory will be broken (and hence appears red when you do `ls` in that directory).
  The device driver we are writing needs these Kernel headers. 
  If we were to run `make`, we would get a "`No such file or directory`" error. 
  To fix this, we download and link up the source and headers:
  ```
  sudo yum install kernel-headers
  sudo yum install kernel-devel
  sudo yum install build-essential
  ln -s /usr/src/kernels/$(uname -r)/ /lib/modules/$(uname -r)/build
  ```
- **Enable port forwarding for SSH**: While CentOS 6 is running in VirtualBox, go to Device -> Network -> *Network Settings*, click
  *Port Forwaring* and add a new row that forwards Host Port 3022 to Guest Port 22. The result is that when we can now
  run `ssh` and `scp` specifying port 3022 (arbitrarily chosen port) on OSX 
  and that will be forwarded to the ssh server on Centos 6 listening on port 22 (the [well known port](https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers#Well-known_ports) for ssh
  where CentOS 6's ssh server will be listening).

- **Removing Passwords for SSH**: Typing your root passport for CentOS over and over again is annoying and slows down the development cycle.
To [speed things up](https://serverfault.com/questions/241588/how-to-automate-ssh-login-with-password), 
we'll use an RSA keypair for authentication.
    ```
    ssh-keygen
    brew install ssh-copy-id
    ssh-copy-id -p 3022 root@localhosh
    ssh -p 3022 root@localhost  # no longer asks for password
    ```
    The `ssh-copy-id` command will copy your public key to the CentOS server. From now on, when you login 
    with `ssh -p 3022 root@localhost` you will not be asked for a password because you'll
    instead be using your RSA keypair for authentication.
