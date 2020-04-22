#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <poll.h>
#include <fcntl.h>
#include <arpa/inet.h>      /* this has to be included before the next two */
#include <linux/if.h>       /* which has got to be unintended */
#include <linux/if_tun.h>

#define TAP_DEVICE_ONE_NAME "tap1"
#define TAP_DEVICE_TWO_NAME "tap2"
#define MAX_ETH_FRAME_SIZE 1522
#define SLEEP_TIME 10
#define SHARED_MEMORY_NAME "/test"

char tap1_mac [6] = "\xaa\xbb\xcc\xdd\xee\x11";
char tap2_mac [6] = "\xaa\xbb\xcc\xdd\xee\x22";
char tap1_ip [4] = "\xc0\xa8\x01\x01";
char tap2_ip [4] = "\xc0\xa8\x02\x01";
char prog_ip [5] = "\xc0\xa8\x01\x12\0";
char prog_mac [6] = "\x1e\x44\x5b\x59\x48\x6b";
char frame[MAX_ETH_FRAME_SIZE];
static int connect_to_tap(char* tapDeviceName);
static int wait_for_frame(int* fds, int msec);
void binary_to_hex(char *s, int n);

int main (int argc, char *argv[]){
    //declaring variables
    int tap1_fd;
    int tap2_fd;
    int num_bytes;

    //connecting to the tap devices that represent the networks
    tap1_fd = connect_to_tap(TAP_DEVICE_ONE_NAME);
    tap2_fd = connect_to_tap(TAP_DEVICE_TWO_NAME);

    //initialising the file descriptor array
    int fda [3] = {tap1_fd, tap2_fd, -1};

    //waiting for incoming packets, until interrupted by the user
    while (1>0){
        int fd = wait_for_frame(fda, 1000);
        if(fd!=0){
            //recieved some data on network fd
            printf("Data arrived on fd = %d\n", fd);
            num_bytes = read(fd, frame, MAX_ETH_FRAME_SIZE);
            printf("read %d bytes\n", num_bytes);
            if (num_bytes == 42){
                //parses the ARP request/reply
                if (frame [20] == '\x00' && frame [21] == '\x01'){
                    //parsing ARP request
                    printf ("the packet is an ARP request.\n");
                    //filling the contents of the destination ip
                    char *dest_ip = malloc (5);
                    for (int i=0, j=38; i<=3; i++, j++){
                        dest_ip[i] = frame[j];
                    }
                    dest_ip [5] = '\0';
                    //destinationIP matches the programIP, we reply to the ARP packet
                    if (strcmp(dest_ip, prog_ip)==0){
                        //send an ARP reply with the destination MAC set to prog_mac
                        int framelen = 42;
                        //setting the destination MAC to represent the tap1 mac
                        for (int i=0, j=32; i<=5; i++, j++){
                            frame[j] = tap1_mac[i];
                        }
                        //setting destination IP to be tap1 IP
                        for (int i=0, j=38; i<=3; i++, j++){
                            frame[j] = tap1_ip[i];
                        }
                        //setting the sourceIP to be prog_ip
                        for (int i=0, j=28; i<=3; i++, j++){
                            frame[j] = prog_ip[i];
                        }
                        //setting the sourceMAC to be prog_mac
                        for (int i=0, j=22; i<=5; i++, j++){
                            frame[j] = prog_mac[i];
                        }
                        //setting the op code to be ARP reply
                        frame [21] = '\x02';
                        num_bytes = write(fd, frame, framelen);
                        printf("sent %d/%d bytes to %d\n", num_bytes, framelen, fd);
                        //sending an ARP reply
                    }
                    //when the destinationIP does not match the program IP, we drop the packet,
                    //since it wasn't meant to pass through 192.168.1.18
                    else{
                        printf ("packet discarded");
                    }
                }
            }
            else if (num_bytes==90){
                //parsing the ping packet
                //bytes 30-33 are the final destination's IP address
                //if final destination = prog IP, then we have recieved the packet
                //if final destination != prog IP, then we use netmask to check if it was for this network
                    //if it is for this network we send an ARP request
                    //if it is not for this network we send it to another network
            }
            else {
                printf("no data arrived\n");
            }
        }
    }
    //closing networks
    close (tap1_fd);
    close (tap2_fd);
}

/* brazenly stolen from Linux kernel: Documentation/networking/tuntap.txt */
static int
connect_to_tap(char* tapDeviceName)
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
    strncpy(ifr.ifr_name, tapDeviceName, IFNAMSIZ);

    if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ) {
        perror("ioctl");
        printf("err is %d (%s)\n", err, strerror(err));
        exit(1);
    }

    return fd;
}


/* Returns the file descriptor if a frame is available for reading from the
 * corresponding tap device within `msec' milliseconds; returns a -1
 * otherwise.
 *
 * The parameter fds is an array of file descriptors on which to listen for
 * incoming data; the end of the list is signified by the value -1.  The
 * parameter msec is the number of milliseconds to wait (a value of -1 causes
 * it to wait forever).
 */
int wait_for_frame(int *fds, int msec) {
    struct pollfd pollfds[10];
    int num_ifs;
    int *fdp;
    int i;

    /* check inputs like a good little programmer */
    if(fds == NULL) {
        return -1;
    }

    /* fill in the poll struct, and simultaneously count descriptors */
    for(fdp=fds, i=0, num_ifs=0; *fdp != -1; fdp++, i++) {
        pollfds[i].fd = *fdp;
        pollfds[i].events = POLLIN;
        num_ifs++;
    }

    /* do the thing */
    poll(pollfds, num_ifs, msec);

    /* check which one has activity */
    for(fdp=fds, i=0; *fdp != -1; fdp++, i++) {
        if(pollfds[i].revents && POLLIN) {
            return *fdp;
        }
    }

    return -1;
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
