# linux_esp32
Linux compatible esp-idf library

It has an older version of FreeRTOS for use under linux.

It uses the tun/tap interface 

Here is a good tutorial,

http://backreference.org/2010/03/26/tuntap-interface-tutorial/

'''
# ip tuntap help
Usage: ip tuntap { add | del } [ dev PHYS_DEV ] 
          [ mode { tun | tap } ] [ user USER ] [ group GROUP ]
          [ one_queue ] [ pi ] [ vnet_hdr ]

Where: USER  := { STRING | NUMBER }
       GROUP := { STRING | NUMBER }

# create tun interface
ip tuntap add dev tun0 mode tun user olas
ip link set tun0 up
ip addr add 192.168.1.3/24 dev tun0
ifconfig 

tun0: flags=4241<UP,POINTOPOINT,NOARP,MULTICAST>  mtu 1500
        inet 192.168.1.3  netmask 255.255.255.0  destination 192.168.1.3
        unspec 00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00  txqueuelen 500  (UNSPEC)
        RX packets 0  bytes 0 (0.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 0  bytes 0 (0.0 B)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0


#equivalent openvpn create command
 openvpn --mktun --dev tun1 --user olas

'''

First, whatever you do, the device /dev/net/tun must be opened read/write. That device is also called the clone device, because it's used as a starting point for the creation of any tun/tap virtual interface. The operation (as with any open() call) returns a file descriptor. But that's not enough to start using it to communicate with the interface.

The next step in creating the interface is issuing a special ioctl() system call, whose arguments are the descriptor obtained in the previous step

