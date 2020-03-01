#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

int transmit (int deviceID, int totalNumberOfDevices, int deviceFrameStatus[], int timeElapsed, int deviceWaitingSlot[], int n);
int binaryExponentialBackoff (int n);

int main(int argc, char *argv[]){
    int numberOfDevices = 0;
    if (argc>1){
        numberOfDevices = atoi (argv[1]);
    }
    else {
        printf ("Failed: Program requires number of devices to be inputted as an argument.\n");
        exit (0);
    }
    //declaring an array that keeps track of whether frames are attempting to send (=0) or sent (=1)
    int deviceFrameStatus [numberOfDevices];
    //initialsing all deviceFrameStatuses as sending
    for (int i=0; i<numberOfDevices; i++){
        deviceFrameStatus[i] = 0;
    }
    //initialising a variable that keeps track of time units passed
    int timeElapsed = 0;
    //each device when made to wait will be given a waiting slot by binary exponential backoff
    int deviceWaitingSlot [numberOfDevices];
    //initialising all waiting slots as 0
    for (int i=0; i<numberOfDevices; i++){
        deviceWaitingSlot[i] = 0;
    }
    //running a loop till all devices have successfully sent their frame
    int deviceID = 0;
    while (deviceID<numberOfDevices){
        //informing the system that the device can stop waiting and attempt to send
        deviceFrameStatus[deviceID] = 0;
        //calling a method to send a frame from a device
        timeElapsed += transmit (deviceID, numberOfDevices, deviceFrameStatus, 0, deviceWaitingSlot, 0);
        deviceID++;
    }
    printf ("Time elapsed: %d units.\n", timeElapsed);
    return timeElapsed;
}

//transmits recursively using binaryExponentialBackoff until there are no collisions
int transmit (int deviceID, int numberOfDevices, int deviceFrameStatus[], int timeTakenByThisDevice, int deviceWaitingSlot [], int n){
    //check if other devices are attempting to send frames
    int noOfOtherDevicesSending = 0;
    for (int i=0; i<numberOfDevices; i++){
        if (deviceWaitingSlot[i]==deviceWaitingSlot[deviceID] && deviceFrameStatus[i]==0 && i!=deviceID){
            noOfOtherDevicesSending++;
        }
    }
    //successfully sending the frame when there is no collision
    if (noOfOtherDevicesSending == 0){
        //setting frame status to sent
        deviceFrameStatus [deviceID] = 1;
        //incrementing time elapsed by one unit
        timeTakenByThisDevice++;
        return timeTakenByThisDevice;
    }

    //when there is a collision
    else {
        //one unit of time passes every with every collision
        timeTakenByThisDevice++;
        //doing binary exponential backoff to give each device new waiting slots
        for (int i=0; i<numberOfDevices;i++){
            deviceWaitingSlot [i] = binaryExponentialBackoff (n);
        }
        //retransmitting after doing binary exponential backoff
        return transmit (deviceID, numberOfDevices, deviceFrameStatus, timeTakenByThisDevice, deviceWaitingSlot, n+1);
    }
}

//assings a random waiting slot between 0 and 2^n to all the devices waiting to send
int binaryExponentialBackoff (int n){
    int limit = 1;
    //setting limit to 2^n
    for (int i=0;i<n;i++){
        limit*=2;
    }
    //generating a random number
    int random = rand();
    //returning a random number between 0 and the limit
    return random%limit;
}
