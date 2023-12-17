#include "../include/simulator.h"

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
#include <string.h>
#include <stdlib.h>
//First define some constants/variables
//Let's start with the A side(sender)
//We need a flag, a sequence number, a last successful ack,
// a number of packets ready, a last sequence number, a window,
// a buffer for messages, a buffer for packets, a buffer next
int A_flag;
int A_seqnum;
int A_last_successful_ack;
int num_packets_ready;
int A_last_sequence;
int A_windowSize;
struct msg A_msg_buffer[1000];
struct pkt A_packet_buffer[1000];
int A_buffer_next;

//For B side
//We need a expected sequence number
int B_expected_seqnum;

//Then we need to declare the helper functions
int checksum(struct pkt packet);
struct pkt create_packet(struct msg message, int seqnum);


/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
    //let's create a packet use the helper function first
    struct pkt packet = create_packet(message, A_seqnum);
    //First we need to check if this is the first packet in current window
    if (A_last_sequence == A_seqnum)
    {
        //if so, we need to start the timer for that packet
        starttimer(0, 25.0);
    }
    //if the number of packets ready is 0
    else if (num_packets_ready == 0)
    {
        //we need to set the last successful ack to the sequence number
        //because we need to know which packet is the last packet that has been sent successfully
        A_last_successful_ack = A_seqnum;
    }
        //check if the number of packets ready is smaller than the window size
    if (num_packets_ready < A_windowSize)
    {
        //if so, we need to send the packet to layer 3(network layer)
        tolayer3(0, packet);
        //then we need to store the packet in the packet buffer in sender side
        A_packet_buffer[A_seqnum % 1000] = packet;
        //then we need to increase the number of packets ready by 1
        num_packets_ready++;
        //same for the sequence number
        A_seqnum++;
    }
    else
    {
        //if so, we need to store the message in the message buffer
        A_msg_buffer[A_buffer_next] = message;
        //then we need to increase the buffer next
        A_buffer_next++;
    }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(  struct pkt packet)
{
    //first check if the acknum is the same as the last sequence number
    if (packet.acknum == A_last_sequence+1 && packet.acknum < A_last_sequence+A_windowSize) {
        A_last_sequence = packet.acknum;
        //also check for the message buffer
        if (A_buffer_next > 0)
        {
            //if so, we need to send out the message in the message buffer
            A_output(A_msg_buffer[0]);
            //then we need to decrease the buffer next
            A_buffer_next--;
        }
        //then we need to check if the acknum is the same as the last successful ack plus the window size
        if (packet.acknum == A_last_successful_ack + A_windowSize) {
            //if so, we need to set the last successful ack to the acknum
            A_last_successful_ack = packet.acknum;
            //then we need to stop the timer
            stoptimer(0);
            //then we set the number of packets ready to 0
            num_packets_ready = 0;
        }
    }

}

/* called when A's timer goes off */
void A_timerinterrupt()
{
    //iterate through the packets in the window that needs to be resent
    for (int i = A_last_sequence; i < A_last_sequence+A_windowSize && i < num_packets_ready; i++)
    {
        //resend the packet
        tolayer3(0, A_packet_buffer[i % 1000]);
    }
    //then we need to start the timer
    starttimer(0, 25.0);
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
    //let's start with the A side(sender)
    //first set the window size using the getwinsize() function
    A_windowSize = getwinsize();
    //then set the flag to 1 showing it is ready to send
    A_flag = 1;
    //then set the sequence number to 0
    A_seqnum = 0;
    //then set the last successful ack to 0
    A_last_successful_ack = 0;
    //then set the number of packets ready to 0
    num_packets_ready = 0;
    //then set the last sequence number to 0
    A_last_sequence = 0;
    //then set the buffer next to 0
    A_buffer_next = 0;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
//first we need to check if the checksum is correct
    if (packet.checksum == checksum(packet))
    {
        //if so, we need to check if the sequence number is the same as the expected sequence number
        if (packet.seqnum == B_expected_seqnum)
        {
            //if so, we need to send the message to layer 5(application layer)
            tolayer5(1, packet.payload);
            //then we need to create a packet to send back to A
            struct pkt ack_packet;
            //set the acknum to the sequence number
            ack_packet.acknum = B_expected_seqnum;
            //set the checksum to the checksum of the packet
            ack_packet.checksum = checksum(ack_packet);
            //then we need to send the packet to layer 3(network layer)
            tolayer3(1, ack_packet);
            //then we need to increase the expected sequence number by 1
            B_expected_seqnum += 1;
        }
        else
        {
            //if so, we need to create a packet to send back to A
            struct pkt ack_packet;
            //set the acknum to the sequence number
            ack_packet.acknum = B_expected_seqnum -1;
            //set the checksum to the checksum of the packet
            ack_packet.checksum = checksum(ack_packet);
            //then we need to send the packet to layer 3(network layer)
            tolayer3(1, ack_packet);
        }
    }
    else
    {
        //if so, we need to create a packet to send back to A
        struct pkt ack_packet;
        //set the acknum to the sequence number
        ack_packet.acknum = B_expected_seqnum -1;
        //set the checksum to the checksum of the packet
        ack_packet.checksum = checksum(ack_packet);
        //then we need to send the packet to layer 3(network layer)
        tolayer3(1, ack_packet);
    }
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
    //set the expected sequence number to 0
    B_expected_seqnum = 0;
}

//Then we need to define the helper functions
//First is the checksum function
int checksum(struct pkt packet)
{
    int sum = 0;
    sum += packet.seqnum;
    sum += packet.acknum;
    for(int i = 0; i < 20; i++)
    {
        sum += packet.payload[i];
    }
    return sum;
}

//Then we need to define the create packet function
struct pkt create_packet(struct msg message, int seqnum)
{
    //declare a packet use malloc struct pointer
    struct pkt *packet = (struct pkt *)malloc(sizeof(struct pkt));
    //set the acknum to -1
    (*packet).acknum = -1;
    //set the sequence number to the seqnum
    (*packet).seqnum = seqnum;
    //copy the payload to the packet
    strcpy((*packet).payload, message.data);
    //set the checksum to the checksum of the packet
    (*packet).checksum = checksum((*packet));
    //return the packet
    return *packet;
}
