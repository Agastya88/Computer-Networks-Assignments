#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int transmit (int numberOfDevices, int sourceID);

int main(int argc, char *argv[]){
    int numberOfDevices = 0;
    int numberOfTimeslots = 0;
    if (argc>2){
        numberOfDevices = atoi (argv[1]);
        numberOfTimeslots = atoi (argv[2]);
    }
    else {
        printf ("Failed: Program requires number of devices & number of time slots to be inputted as an argument.\n");
        exit (0);
    }
    //declaring an array that keeps track of frame status; = 0 implies N/A
    //= 1 implies sending; = 2 implees recieving; = 3 implies sending and recieving.
    int deviceFrameStatus [numberOfDevices];
    //initialsing deviceFrameStatus
    for (int i=0; i<numberOfDevices; i++){
        deviceFrameStatus[i]=0;
    }
    //declaring variables necessary to determine percentage of frames sent using hubs and switches
    int hubFramesSentSuccessfully = 0;
    int switchFramesSentSuccessfully = 0;
    int framesAttempted = 0;
    //opening dev/urandom for random number generation
    FILE *fpointer;
    fpointer = fopen("/dev/urandom", "rb");
    //running a loop for the process that takes place at each time slot
    for (int currentTimeSlot=0; currentTimeSlot<numberOfTimeslots; currentTimeSlot++){
        //a boolean type variable for presence of a collision in the switch
        int switchCollision = 0;
        //for debugging: printf ("Timeslot %d\n", currentTimeSlot);
        //initialsing all deviceFrameStatuses with a 50% probability for the current time slot
        int deviceID = 0;
        //an integer that calculates the number of devices sending frames in the current time slot
        int devicesTransmitting = 0;
        //running the loop for each device
        for (deviceID=0;deviceID<numberOfDevices;deviceID++){
            //getting a random number from dev/urandom
            int random;
            fread(&random,sizeof(int),1,fpointer);
            //setting deviceFrameStatus to 0 or 1 randomly
            deviceFrameStatus[deviceID] = random%2;
            //dealing with negative random numbers
            if (deviceFrameStatus [deviceID] ==-1){
                deviceFrameStatus [deviceID] = 1;
            }
            //when the device transmits
            if (deviceFrameStatus [deviceID]){
                devicesTransmitting++;
                int destinationID = transmit (numberOfDevices, deviceID);
                //changing N/A devices to recievers
                if (deviceFrameStatus[destinationID]==0){
                    deviceFrameStatus [destinationID] = 2;
                }
                //changing sender devices to senders and recievers
                if (deviceFrameStatus[destinationID]==1){
                    deviceFrameStatus [destinationID] = 3;
                }
            }
        }
        //Calculating the number of frames successfully sent in this time slot using hubs
        if (devicesTransmitting==1){
            //frame is only sent successfully if only one device is using the collision domain
            hubFramesSentSuccessfully++;
        }
        framesAttempted += devicesTransmitting;
        //Calculating the number of frames successfully sent in this time slot using switches
        for (int i=0; i<numberOfDevices;i++){
            //if any device is both sending and recieving, there is a collision
            if (deviceFrameStatus[i]==3){
                switchCollision=1;
            }
        }
        //when there is no collision, the frame is sent successfully
        if (switchCollision==0){
            switchFramesSentSuccessfully++;
        }
    }
    double percentageOfHubFrames = (double)(hubFramesSentSuccessfully*100)/framesAttempted;
    printf ("Percentage of Hub Frames Successfully Sent: %.2f\n", percentageOfHubFrames);
    double percentageOfSwitchFrames = (double)(switchFramesSentSuccessfully*100)/framesAttempted;
    printf ("Percentage of Switch Frames Successfully Sent: %.2f\n", percentageOfSwitchFrames);
    //closing dev/urandom
    fclose(fpointer);
}

int transmit (int numberOfDevices, int sourceID){
    //for debugging:printf ("sourceID: %d\n", sourceID);
    //setting up dev/urandom for random number generation
    int random;
    FILE *fpointer;
    fpointer = fopen("/dev/urandom", "rb");
    //finding destination ID randomly from all devices
    fread(&random,sizeof(int),1,fpointer);
    int destinationID = random%numberOfDevices;
    //dealing with negative random numbers
    if (destinationID<0)
    {
        destinationID*=-1;
    }
    //making sure it is not the source device
    while (destinationID == sourceID){
        fread(&random,sizeof(int),1,fpointer);
        destinationID = random%numberOfDevices;
        //dealing with negative random numbers
        if (destinationID<0)
        {
            destinationID*=-1;
        }
    }
    //for debugging:printf ("Final destinationID: %d\n", destinationID);
    fclose(fpointer);
    return destinationID;
}
