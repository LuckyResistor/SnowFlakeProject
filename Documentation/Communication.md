# Communication Protocol
## Summary
This document explains the details about the communication protocol and the idea behind this protocol as implemented in the firmware.

## Background and Idea
Each snow flake board has a data input and output pad to allow communication between multiple boards. The idea is that multiple snow flakes are chained to a single strand. On this strand, the snow flake boards use a communication protocol to synchronise the displayed effect.

1. A communication will make sure all snow flakes will display the same scene on the same strand. If all snow flakes display different scenes, it will look chaotic.
2. Because the internal oscillator of the MCU is used, not all snow flakes will display the scenes in exactly the same speed. A synchronisation signal will make sure there is no "shift" over time and all snow flakes will blend the scenes at the same time.
3. In an initial negotiation, each snow flake can find out the own position in the strand. This position can be used to create scenes which use all snow flakes in the same strand as one huge canvas. An effect could start at the first snow flake and run over all snow flakes in a smooth sequence.
4. Theoretically, the communication can also be used to detect how many snow flake boards are part of the same strand. This final number can be reported back the whole strand. It would keep the firmware flexible, boards can be added and removed from a strand and the scenes would always work with the current number of snow flakes. While this is possible, it is not part of the current firmware implementation. This firmware always assumes a fixed number of snow flakes, which is configured in the firmware as a configuration variable.

## Basic Principle
The communication is a master/client system, where the first snow flake in the strand is the master. The master sends data down the strand which is received by the clients. Each client just passes the received data unmodified to the next client in the strand.

For robustness, the protocol uses a pulse length encoding. Different lengths of pulses encode bits and control information. Because of the variations in the CPU frequencies and unshielded cables, there is a certain timing tolerance built in the protocol.

## Negotiation
In the start process of each snow flake, the position and mode of the element is determined. First, the system checks if it is the first element in the strand:

1. The output line is set to high.
2. The system waits a short time if the input line goes to high.
3. In the case the input line goes to high, the system switches to client mode.
4. In the case the input line stays low, the system switches to master mode.
5. The output line is set to low.

### Negotiation as Master
The master system assigns index zero to itself and sends the next index number (1) to the next client in the strand.

### Negotiation as Client
The client waits for an incoming number from the previous element in the strand. After it receives this number, it assigns this number to itself as index number and sends this number plus one to the next client in the strand.

### Index numbers
The index number is sent with the same protocol described below. The index number is encoded like this:

`0x1B7200XX` Where XX is the index number and all other digits in this number are used as "magic" to make sure this is a valid number.

### Error Handling
There are a number of error situations in the negotiation:

1. No number is received in a certain time frame, the negotiation times out.
2. The received number has the wrong format.

In this error cases, the snow flake will flag the error internally. The firmware will display a special scene to indicate the error to the user.

## Regular Communication
The regular communication consist of 32bit values sent from the master to all clients. Currently there is a single command implemented:

Blend scene: `0xAB0000XX` - XX is the new scene number.

This command is sent before all snow flakes shall blend to a new scene. A synchronisation signal is sent after this command, which will trigger the actual blend of the scene.

## Protocol
The protocol is using a simple pulse length encoding to transfer data. A pulse is pulling the data line to high and back to low. The time while the line was held high determines the meaning of this information element.

A timer is used to measure the incoming pules. Another timer is used to generate the outgoing pules. Both timer use the same clock source and divider to create the best possible precision.

- Length `0x0200`: Zero Bit
- Length `0x0400`: One Bit
- Length `0x0600`: Break Signal
- Length `0x0800`: Synchronisation Signal

For each transmission, a single 32bit value is sent. The lowest bit is sent first. A transmission consist of 32 zero or one bit signals, followed by a break signal.

A synchronisation signal can theoretically be sent at any time, but usually should be sent between value transmissions.

## Error handling

### Technical Problems
 
There are a number of technical situations how the transmission of signals and values can fail:

1. A signal with an unknown length is received.
2. A break signal is received, but the number of previously received zero and one bits is not 32.

In both cases, the communication is reset and the client is waiting for a new value. Any further handling of these problems make no sense, because there is no way to recover from them.

### Logical Problems

There are a number of logical problems which can occur. This logic problems are not handled by the firmware.

1. _There is inference in the transmission and wrong values are received._ For this case, a checksum would have to be added to the protocol. Also either a feedback channel would have to exist to report the problem and trigger resending the data - or the protocol would have to repeat each value a number of times to allow error correction.
2. _No data is received._ To catch this case, the firmware would have to run a kind of watch-dog timer. If no value is received in a given time, the board would have to signal this problem.

It makes no much sense to handle this logical problems. Most likely the reason is a hardware problem. The situation will be visible, because the strand will not synchronise the scenes or the elements will not start at all. 

