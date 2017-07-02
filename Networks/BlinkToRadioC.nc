 #include <Timer.h>
 #include "BlinkToRadio.h"
 
module BlinkToRadioC {
  uses {
    interface Boot;
    interface SplitControl as RadioControl;

    interface Leds;
    interface Timer<TMilli> as Timer0;
	interface Timer<TMilli> as Timer1;

    interface Packet;
    interface AMPacket;
    interface AMSendReceiveI;
  }
}

implementation {
  DEADLOCK_RESEND_PERIOD = 5000;
  uint16_t counter = 0;
  uint16_t sequence = 0;
  message_t sendMsgBuf;
  message_t* sendMsg = &sendMsgBuf; // initially points to sendMsgBuf

  // Acknowledge message.
  message_t ackMsgBuf;
  message_t* ackMsg = &ackMsgBuf;
  
  // Copy of last sent data message.
  message_t copyMsgBuf;
  message_t* copyMsg = &copyMsgBuf;
  
  // Boolean to communicate between sendReceive and fired, states whether busy (a message has been sent which has no acknowledgement yet).
  bool busy = FALSE;

  event void Boot.booted() {
    call RadioControl.start();
  };

  event void RadioControl.startDone(error_t error) {
    if (error == SUCCESS) {
      call Timer0.startPeriodic(TIMER_PERIOD_MILLI);
    }
  };

  event void RadioControl.stopDone(error_t error){};



  event void Timer0.fired() {
    BlinkToRadioMsg* btrpkt;

	if( !busy ){ // If not busy / NOT still waiting for an acknowledgement of previous message, send another.
		
		// Flip the sequence number between 1 & 0 for each new message.
		sequence = 1 - sequence;
		
		// Initialise the message.
		call AMPacket.setType(sendMsg, AM_BLINKTORADIO);
		call AMPacket.setDestination(sendMsg, DEST_ECHO);
		call AMPacket.setSource(sendMsg, TOS_NODE_ID);
		call Packet.setPayloadLength(sendMsg, sizeof(BlinkToRadioMsg));

		// Initialise the message as a BlinkToRadio packet.
		btrpkt = (BlinkToRadioMsg*)(call Packet.getPayload(sendMsg, sizeof (BlinkToRadioMsg)));
		counter++;
		btrpkt->type = TYPE_DATA;
		btrpkt->seq = sequence;
		btrpkt->nodeid = TOS_NODE_ID;
		btrpkt->counter = counter;

		// Send message and store returned pointer as a copy of the message.
		copyMsg = call AMSendReceiveI.send(sendMsg);
		busy = TRUE;
	} else {
		// Still waiting for acknowledge, stop 0 and start Timer1 to solve potential deadlock.
		call Timer0.stop();
		call Timer1.startOneShot( DEADLOCK_RESEND_PERIOD );
	}
  }

  event void Timer1.fired(){
	  
		// Initialise the packet to be send and the copy packet, who's data is transferred into the send packet.
		BlinkToRadioMsg* btrpkt;
		BlinkToRadioMsg* cpypkt;
	
		// Initialise the message to be resent.
		call AMPacket.setType( sendMsg, AM_BLINKTORADIO );
		call AMPacket.setDestination( sendMsg, DEST_ECHO );
		call AMPacket.setSource( sendMsg, TOS_NODE_ID );
		call Packet.setPayloadLength( sendMsg, sizeof( BlinkToRadioMsg ) );
		
		// Get the copy of the last message as a BlinkToRadio packet.
		cpypkt = ( BlinkToRadioMsg* )( call Packet.getPayload( sendMsg, sizeof( BlinkToRadioMsg ) ) );
		
		// Initialise the resent message as a BlinkToRadio packet, using the copy message's data.
		btrpkt = ( BlinkToRadioMsg* )( call Packet.getPayload( sendMsg, sizeof( BlinkToRadioMsg ) ) );
		btrpkt -> type = cpypkt -> type;
		btrpkt -> seq = cpypkt -> seq;
		btrpkt -> nodeid = cpypkt -> nodeid;
		btrpkt -> counter = cpypkt -> counter;
		
		// Send the message and store the returned pointer as a copy of it, incase it needs to be sent again.
		copyMsg = call AMSendReceiveI.send( sendMsg );
		
		// Set Timer1 to fire again in x milliseconds. Will be stopped from firing by send recieve when acknowledge is confirmed.
		call Timer1.startOneShot( DEADLOCK_RESEND_PERIOD );
  }


  event message_t* AMSendReceiveI.receive(message_t* msg) {
    uint8_t len = call Packet.payloadLength(msg);
    BlinkToRadioMsg* btrpkt = (BlinkToRadioMsg*)(call Packet.getPayload(msg, len));
    call Leds.set(btrpkt->counter);
	
	// Reciever: If data, send an acknowledgement back.
	if( btrpkt -> type == TYPE_DATA && btrpkt -> seq == sequence ){ // If a data message, send the acknowledgement.
		
		// Initialise the acknowledge message.
		call AMPacket.setType( ackMsg, AM_BLINKTORADIO );
		call AMPacket.setDestination( ackMsg, DEST_ECHO );
		call AMPacket.setSource( ackMsg, TOS_NODE_ID );
		call Packet.setPayloadLength( ackMsg, sizeof( BlinkToRadioMsg ) );
		
		// Initialise the acknowledge message as a BlinkToRadio packet.
		btrpkt = ( BlinkToRadioMsg* )( call Packet.getPayload( ackMsg, sizeof ( BlinkToRadioMsg ) ) );
		btrpkt -> type = TYPE_ACK;
		btrpkt -> seq = sequence;
		btrpkt -> nodeid = TOS_NODE_ID;
		btrpkt -> counter = counter;
		
		// Send the acknowledge message.
		ackMsg = call AMSendReceiveI.send( ackMsg );
		
	}
	// Sender: Recieved acknowledge, ready to send the next message.
	else if( btrpkt -> type == TYPE_ACK && btrpkt -> seq == sequence ){ // If acknowledgement & in sequence we're expecting, inform fired using busy bool, so it is free to send the next message.
	
		// Flip the busy bool, so Timer0 can send the next message | start Timer0 again & stop Timer1 so it doesn't resend the last message.
		busy = FALSE;
		call Timer1.stop();
		call Timer0.startPeriodic(TIMER_PERIOD_MILLI);
	}
	
	
    return msg; // no need to make msg point to new buffer as msg is no longer needed
  }
}

 
