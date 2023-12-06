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

/********* STUDENTS WRITE THE NEXT SIX ROUTINES *********/
#include <string.h>

//For A:
//flag to show if A is ready to send
int A_flag;
//sequence number for A
int A_seqnum;
//last packet sent by A
struct pkt last_pkt;
//buffer for A with a size of 1000
struct pkt A_Buff[1000];
//next packet to be sent
int A_Buff_next = 0;

//For B:
//sequence number for B
int B_seqnum;

//declar the function for calculating the checksum
int checksum(struct pkt packet);

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
    //first of all we need to set the sequence number of next packet in the buffer to the current sequence number
    A_Buff[A_Buff_next].seqnum = A_seqnum;
    //then we need to set the acknum of next packet in the buffer to the current sequence number, why?
    //because the acknum is the sequence number of the packet that B is expecting to receive
    A_Buff[A_Buff_next].acknum = A_seqnum;

    //then we need to copy the data from the message to the payload of next packet in the buffer
    memcpy(A_Buff[A_Buff_next].payload, message.data, 20);

    //then we need to calculate the checksum for the next packet in the buffer
    //because the checksum is the sum of the sequence number, acknum and payload
    A_Buff[A_Buff_next].checksum = checksum(A_Buff[A_Buff_next]);

    //then we need to send the packet to layer 3
    tolayer3(0, A_Buff[A_Buff_next]);

    //then we need to start the timer with RTT = 20.0(the same as the simulator)
    starttimer(0, 20.0);

    //then we need to set the flag to 0 showing it is not ready to send
    A_flag = 0;

    //then we need to set the last packet sent by A to the next packet in the buffer
    //because the last packet sent by A is the next packet in the buffer
    last_pkt = A_Buff[A_Buff_next];

    //then we need to increase the next packet to be sent by 1, and wrap around the buffer for next packet
    //because the buffer is a circular buffer, so we need to wrap around the buffer when we reach the end of the buffer
    A_Buff_next = (A_Buff_next + 1) % 1000;
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
    //first check if the acknum is the same as the sequence number of current A
    if(packet.acknum == A_seqnum)
    {
        //if so, stop the timer as ack for the current packet is now received
        stoptimer(0);
        //then set the flag to 1 showing it is ready to send
        A_flag = 1;
        //then increase the sequence number by 1, and wrap around the sequence number for next packet
        A_seqnum = (A_seqnum + 1) % 2;
    }
    //otherwise it means the acknum is not the same as the sequence number of current A
    else
    {
        //so we basically restart the timer and resend the last packet
        //which can be done by calling the A_timerinterrupt() function
        A_timerinterrupt();
    }
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
    //When the timer goes off, it means the packet is lost, so we need to resend the packet
    //restart the timer with RTT = 20.0(the same as the simulator)
    starttimer(0, 20.0);
    //resend the last packet, as the timer expires, the last packet is lost, and ACK is not received
    tolayer3(0, last_pkt);
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
    //initalize the flag to 1 showing it is ready to send
    A_flag = 1;
    //initalize the sequence number to 0, since ABT is a 1-bit protocol,
    // the sequence number is either 0 or 1, while 0 is the first sequence number to start with
    A_seqnum = 0;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
    //first check if the checksum is correct
    if(packet.checksum == checksum(packet))
    {
        //if so, check if the sequence number is the same as the sequence number of B
        if(packet.seqnum == B_seqnum)
        {
            //if so, send the packet to layer 5(the application layer)
            tolayer5(1, packet.payload);
            //then send the ack to layer 3
            tolayer3(1, packet);
            //then increase the sequence number by 1, and wrap around the sequence number for next packet
            B_seqnum = (B_seqnum + 1) % 1000;
        }
        //otherwise it means the sequence number is not the same as the sequence number of B
        else
        {
            //so we basically send the ack for the last packet received
            //which can be done by calling the tolayer3() function
            tolayer3(1, packet);
        }
    }

}

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
    //initalize the sequence number to 0, as this is expected first sequence number from A
    B_seqnum = 0;
}

/* Since in section 7 of the PA handout, "You can use whatever approach for checksumming you want."
 * So I'm going to define a function to calculate the checksum for a packet.*/
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
