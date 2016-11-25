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
ip tuntap add dev tap0 mode tap user root
ip link set tap0 up

ifconfig tap0 0.0.0.0 promisc up
ifconfig eth0 0.0.0.0 promisc up

ip tuntap list

#Setup bridge, to forward packages
brctl addbr br0
brctl show
ip addr show
ip addr add 172.20.0.1/16 dev br0
ip link set br0 up

iptables -I FORWARD -m physdev --physdev-is-bridged -j ACCEPT

sysctl net.ipv4.ip_forward=1

brctl addif br0 tap0 eth0

#To delete
brctl delif br0 tap0



# Tun inteface

The driver will later execute these
/sbin/ip addr add 192.168.0.1/24 dev tun0
/sbin/ip route add default via 192.168.0.1

ip route add 192.168.1.0/24 dev tun0

These two are set by tunif driver,

ip addr add 192.168.1.3/24 dev tun0
ifconfig 

tun0: flags=4241<UP,POINTOPOINT,NOARP,MULTICAST>  mtu 1500
        inet 192.168.1.3  netmask 255.255.255.0  destination 192.168.1.3
        unspec 00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00  txqueuelen 500  (UNSPEC)
        RX packets 0  bytes 0 (0.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 0  bytes 0 (0.0 B)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

#iptables
iptables --list
iptables -F
iptables --list

ip route show
ip route add 192.168.0.0/24 dev tun0

#Remove
ip tuntap del dev tun0 mode tun

ip link set dev tun0 up
Cannot find device "tun0"


#equivalent openvpn create command
 openvpn --mktun --dev tun1 --user olas

'''

First, whatever you do, the device /dev/net/tun must be opened read/write. That device is also called the clone device, because it's used as a starting point for the creation of any tun/tap virtual interface. The operation (as with any open() call) returns a file descriptor. But that's not enough to start using it to communicate with the interface.

The next step in creating the interface is issuing a special ioctl() system call, whose arguments are the descriptor obtained in the previous step

Currently not working so well.

For easier debugging, reate a file in your home directory called .gdbinit and place the following two lines in it:

handle SIGUSR1 nostop noignore noprint
handle SIG34 nostop noignore noprint


