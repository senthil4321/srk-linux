#!/bin/bash

# Create a network namespace
ip netns add mynamespace

# Create a virtual Ethernet pair
ip link add veth0 type veth peer name veth1

# Move one end of the pair to the namespace
ip link set veth1 netns mynamespace

# Assign IP addresses
ip addr add 192.168.1.1/24 dev veth0
ip netns exec mynamespace ip addr add 192.168.1.2/24 dev veth1

# Bring up the interfaces
ip link set veth0 up
ip netns exec mynamespace ip link set veth1 up

# Add a route in the namespace
ip netns exec mynamespace ip route add default via 192.168.1.1

echo "Network namespace setup complete."
