#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <poll.h>
#include <gdbm.h>

#include <arpa/inet.h>      /* this has to be included before the next two */
#include <linux/if.h>       /* which has got to be unintended */
#include <linux/if_tun.h>

#define TAP_DEVICE_NAME "tap1"

#define MAX_ETH_FRAME_SIZE 1522

//for the ARP request wrapped in the Ethernet II frame,
//we sent the destination MAC to ff ff ff ff ff ff to indicate a broadcast
//source MAC is set to aa:bb:cc:dd:ee:11 because this is the MAC address for tap1
//the protocol type is set to x0806 to indicate an ARP request

//for the ethernet II frame,
//hardware type is 00 01, which means ethernet
//protocol type is set to 08 00, which means IP
//hardware size is set to 06, because ethenet addresses are 6 bytes long
//protocol size is set to 04, because IP addresses are 4 bytes long
//op code is set to 00 01, which means that the packet is an ARP request
//source MAC is aa:bb:cc:dd:ee:33 which is a device on the tap network
//source IP is 192.168.1.2, which is a device on the tap network i.e. 192.168.1.0/24, which in hex can be written as c0 a8 01 01
//destination IP is input as a command line argument, set 0.0.0.0 initially

char frame[MAX_ETH_FRAME_SIZE] =
        "\xff\xff\xff\xff\xff\xff\xaa\xbb"
        "\xcc\xdd\xee\x33\x08\x06\x00\x01"
        "\x08\x00\x06\x04\x00\x01\xaa\xbb"
        "\xcc\xdd\xee\x11\xc0\xa8\x01\x02"
        "\x00\x00\x00\x00\x00\x00\x00\x00"
        "\x00\x00";

int framelen = 42;

static int connect_to_tap(void);
static int wait_for_frame(int fd, int msec);
void binary_to_hex(char *s, int n);
void decimalToHex (char *hex, int decimalNumber);

int main (int argc, char *argv[]){
    //initilaizing variables
    GDBM_FILE arpTable;
    datum key,value;
    int tap_fd;
    int num_bytes;

    //connects to the tap device that represents the network
    tap_fd = connect_to_tap();

    //making sure the IP address parameters were input
    if (!(argc==5)){
        printf ("Error: invalid IP address\n");
        exit (0);
    }

    //takes as a command-line parameter the target IP address for the ARP request
    //ip1-ip4 represent the 4 seperate integers (between 0&255) of the IP address
    int ip1 = atoi (argv[1]);
    int ip2 = atoi (argv[2]);
    int ip3 = atoi(argv[3]);
    int ip4 = atoi(argv[4]);

    //makes sure the IP address is valid
    if (!(ip1 >=0 && ip1<=255) && (ip2 >=0 && ip2<=255) && (ip3 >=0 && ip3 <=255) && (ip4 >=0 && ip4 <=255)){
        printf ("Error: invalid IP address\n");
        exit (0);
    }
    else {
        printf ("IP address: %d.%d.%d.%d\n", ip1, ip2, ip3, ip4);
    }

    //modifies the arp request to set destination IP to the correct value
    frame [38] = ip1;
    frame [39] = ip2;
    frame [40] = ip3;
    frame [41] = ip4;

    //sends a well-formed ARP request within a well-formed Ethernet frame on the tap device
    num_bytes = write(tap_fd, frame, framelen);
    printf("sent %d/%d bytes\n", num_bytes, framelen);

    //initially we have not recieved the ARP reply
    int booleanARPreply = 0;

    //waits for responses, and parsing them to find the ARP reply
    while (booleanARPreply == 0){
        if(wait_for_frame(tap_fd, 1000)) {
            //reads the contents of the frame
            num_bytes = read(tap_fd, frame, MAX_ETH_FRAME_SIZE);
            //prints the number of bytes read
            printf("read %d bytes\n", num_bytes);
            //response might be ARP reply, if the number of bytes read is 60
            if (num_bytes == 42){
                //parses the ARP response and stores its information in
                //the ARP table, which is a gdbm database; key = IP, value = MAC.
                if (frame [20] == '\x00' && frame [21] == '\x02'){
                    //we confirm that this is the reply using op code x0002
                    booleanARPreply = 1;
                    printf ("the response is an ARP reply.\n");
                    //filling the contents of the key
                    char *key_ip = malloc (5);
                    for (int i=0, j=38; i<=3; i++, j++){
                        key_ip[i] = frame[j];
                    }
                    key.dptr = key_ip;
                    key.dsize = 5;
                    //filling the contents of the value
                    char *value_mac = malloc (7);
                    for (int i=0, j=32; i<=5; i++, j++){
                        value_mac[i] = frame[j];
                    }
                    value.dptr = value_mac;
                    value.dsize = 7;
                    //opening the database
                    arpTable = gdbm_open("arp_table", 0, GDBM_NEWDB, 0666, 0);
                    //storing the key-value pair in the ARP Table
                    gdbm_store (arpTable, key, value, GDBM_INSERT);
                    gdbm_close (arpTable);
                    //separately looking up the MAC address in the ARP table
                    arpTable = gdbm_open("arp_table", 0, GDBM_READER, 0666, 0);
                    if (!arpTable){
                        fprintf (stderr, "File either doesn't exist or is not a gdbm file.\n");
                    }
                    else{
                        datum fetchedValue = gdbm_fetch(arpTable, key);
                        //printing it in human readable format
                        printf ("The MAC address is: ");
                        binary_to_hex (fetchedValue.dptr, fetchedValue.dsize-1);
                        printf ("\n");
                    }
                }
            }
            if (booleanARPreply == 0){
                printf ("the response is not an ARP reply\n");
            }
        }
        else {
            printf("no data arrived\n");
        }
    }


    close(tap_fd);
}



/* brazenly stolen from Linux kernel: Documentation/networking/tuntap.txt */
static int
connect_to_tap(void)
{
    int fd;
    struct ifreq ifr;
    int err;

    if((fd = open("/dev/net/tun", O_RDWR)) < 0) {
        perror("open");
        exit(1);
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    strncpy(ifr.ifr_name, TAP_DEVICE_NAME, IFNAMSIZ);

    if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ) {
        perror("ioctl");
        printf("err is %d (%s)\n", err, strerror(err));
        exit(1);
    }

    return fd;
}

/* returns a 1 if a frame is available for reading from the tap device
 * specified by `fd' within `msec' milliseconds; returns a 0 otherwise */
static int
wait_for_frame(int fd, int msec)
{
    struct pollfd pollfds[1];

    pollfds[0].fd = fd;
    pollfds[0].events = POLLIN;

    poll(pollfds, 1, msec);

    if(pollfds[0].revents & POLLIN) {
        return 1;
    } else {
        return 0;
    }
}

//converts each byte of the buffer s to its corresponding pair of hexadecimal
//digits and prints it to stdout; eg. "input "AAAA" -> output "41 41 41 41
void binary_to_hex(char *s, int n){
//creating an integer to count bytes read
int byteCounter = 0;
//accessing each char in the char*
    for (int i=0; i<n; i++){
        byteCounter+=1;
        //printing every character of the string as hexadecimal
        printf ("%02x ", (unsigned char)*(s+i));
        //creating a new line every 16 bytes
        if (byteCounter%16 == 0){
            printf ("\n");
        }
    }
}
