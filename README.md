# Computer-Networks-Assignments
--- Assignments done for CS431 during the Spring 2020 semester;

Assignment 1: Two programs hexdump and hexread.
hexdump will convert its input from binary to hex; hexread will perform the reverse operation. 
Both programs should operate on files whose names are provided as command-line arguments;
if no arguments are provided, both should read from stdin. 
You must include a Makefile that builds these programs; its default target should build both.

Assignment 2: 
i) Suppose N devices are waiting for another frame to finish on an Ethernet. All transmit at once when the frame is finished and thus collide.
Write a program to simulate the devices’ attempts to send their frame; its output should be the time slot in which the final device was able to send.
Implement binary exponential backoff as discussed in class.
ii) Write a program to simulate the behavior of (simplified) Ethernet hubs and switches. Make the same simplifying assumptions as in the previous part.
Your program will simulate random frames being sent between ports on the switch; it will measure only how many frames were unable to be delivered.
During each time slot, every connected device should, with 50% probability, decide to send a frame.
If a given device does choose to send a frame, it should randomly choose a device on another port to which to send this frame.
Simulate the behavior of both hubs and switches under these circumstances.
You may assume that devices do not attempt to resend in the face of collision and that the switch does not buffer any frames 
if they are unable to immediately deliver them (ie, the switch silently discards the frame). 
You may also assume that the switch knows which port each device is connected to before the simulation begins.

Your program should take two command-line arguments: the number of devices connected to the hub/switch and the duration of the simulation to run (an integer number of time slots). It should output the percentage of frames it successfully delivered.

Assignment 3:
A program that does the following in sequence:
Connects to the tap device that represents the network.
On this device, sends a well-formed ARP request within a well-formed Ethernet frame.
Receives frames from this device until an ARP response shows up (all other frames should be discarded).
Parses the ARP response and stores its information in the ARP table, which must be a gdbm database.
Store the ARP table in a shared memory region
Separately looks up the MAC address in the ARP table and prints it out in human-readable format.
Your program must take as a command-line parameter the target IP address for the ARP request.
For simplicity, it’s acceptable if it accepts four separate integers (rather than a single dotted-quad value).

Assignment 4:
Implement a program called stack that routes between various tap networks.
The program must connect to each tap network, await incoming packets, and route them accordingly until interrupted by the user (ie, with Ctrl-C).
-- Additionally, your program should:
Discard any frames not intended for it (see the $prog_mac_address field in setup-if.sh.
Discard any frames that do not contain IP packets.
Assume all IP packets should be routed.
-- Here are some shortcuts you may (should) take:
You may assume no more than 10 interfaces.
You may assume no more than 20 routes.
You may hardcode the simulated interface characteristics (ie, MAC address, IP address, etc).
You may hardcode the routing table.
You may hardcode the ARP tables (ie, the one within stack and the one within the Linux virtual machine).
