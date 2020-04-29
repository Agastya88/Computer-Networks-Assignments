#!/bin/sh

username="agastya"
#ext_if="enp0s3"
ext_if="enp0s31f6"

prog_ip_address="192.168.1.18"
prog_mac_address="1e:44:5b:59:48:6b"

create_tap_device() {
    if_name="$1"
    if_mac_address="$2"
    if_ip_address="$3"
    if_netmask="$4"

    # create the virtual network device
    ip tuntap add dev $if_name mode tap user $username

    # assign an IP address and netmask to the virtual interface
    ifconfig $if_name $if_ip_address netmask $if_netmask

    # assign a MAC address to the virtual interface
    ip link set dev $if_name address $if_mac_address
}

create_tap_device "tap1" "aa:bb:cc:dd:ee:11" "192.168.1.1" "255.255.255.0"
create_tap_device "tap2" "aa:bb:cc:dd:ee:22" "192.168.2.1" "255.255.255.0"
create_tap_device "tap3" "aa:bb:cc:dd:ee:33" "192.168.2.18" "255.255.255.0"

# tell Linux to route packets
#sysctl net.ipv4.ip_forward=1

# turn on network address translation (NAT)
#iptables -t nat -A POSTROUTING -o $ext_if -j MASQUERADE
#iptables -A FORWARD -m conntrack --ctstate RELATED,ESTABLISHED -j ACCEPT

# add arp table entry for program connecting to virtual network
#arp -s $prog_ip_address $prog_mac_address
