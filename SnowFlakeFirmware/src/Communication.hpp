//
// Snow Flake Project
// ---------------------------------------------------------------------------
// (c)2017-2019 by Lucky Resistor. See LICENSE for details.
// https://luckyresistor.me
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#pragma once


#include <cstdint>


/// The communication module is to communicate with other boards.
///
namespace Communication {

/// A button press by the user.
///
enum class ButtonPress {
	Short, ///< A short press 100-1500ms
	Long, ///< A long press 1500-5000ms
};

/// The function called after a synchronization request is received on a client.
///
typedef void (*SynchronizationFn)();

/// The function which is called if a new value is read on a client.
///
typedef void (*ReadDataFn)(uint32_t data);

/// The function which is called if the user presses a button.
///
typedef void (*ButtonPressFn)(ButtonPress buttonPress);

/// Errors for the communication.
///
enum class Error : uint8_t {
	/// No error
	///
	None = 0,
	
	/// A timeout occurred while waiting for the data input to be set back to the low state.
	///
	/// Possible reasons for this error:
	/// - A short is holding the input data line permanently in high state.
	/// - The previous snow flake has a short holding data output in high state.
	/// - The previous snow flake has a hardware or firmware problem holding data out permanently in high state.
	///
	TimeoutWaitingForLow = 1,
	
	/// A timeout occurred while waiting for the identifier from the previous element.
	///
	/// Possible reasons for this error:
	/// - The firmware or hardware is not working as expected.
	///
	TimeoutWaitingForIdentifier = 2,
	
	/// A timeout while waiting for previous data to been sent.
	///
	/// Possible reasons for this error:
	/// - The firmware is not working as expected.
	/// 
	TimeoutWaitingToSendData = 3,
	
	/// A timeout waiting for the initial synchronization.
	///
	/// Possible reasons for this error:
	/// - The firmware is not working as expected.
	///
	TimeoutWaitingForInitialSynchronization = 4,
		
	/// Received a value, bit it is no valid identifier.
	///
	/// Possible reasons for this error:
	/// - The data connection between the boards has interferences.
	/// - The firmware on the previous board is not working as expected.
	/// - The firmware on this board is not working as expected.
	///
	InvalidIdentifier = 5,
};


/// Initialize the communication component.
///
void initialize();

/// Wait for the negotiation with the other boards.
///
/// This function will start a communication with all other boards to
/// see the own location of the board and to assign numbers to all
/// other boards. It also detects the length of the strand.
///
/// The methods getIdentifier() and getStandLength() can be used after
/// the negotiation.
///
/// @return true on success, false on any error.
///
bool waitForNegotiation();

/// Get the error code.
///
/// @return The error code.
///
Error getError();

/// Check the identifier of this board.
///
/// The identifier is an index value, starting from zero which is 
/// increased by one for each board in the sequence.
///
/// The board with the identifier 0 (zero) is the master controller
/// of the stand. This is the only board which can send information
/// and send the synchronization signal to the other boards.
///
/// @return The identifier of this board.
///
uint8_t getIdentifier();
	
/// Send data to all other boards.
///
/// The sent 32bit are stored on all other devices. This method
/// has only an effect on the master board with the identifier 0.
/// It will trigger sending the data to all other boards.
///
/// @param data The data value to send to all other boards.
/// @return true on success, false if there was a time-out waiting for previous data begin sent.
///
bool sendData(uint32_t data);

/// Send a synchronization request to all other boards in the strand.
///
/// @return true on success, false if there was a time-out waiting for previous data begin sent.
///
bool sendSynchronization();

/// Check if the previous data/signal send operation finished.
///
/// @return true if the all previous data was sent and the component is ready to send more data.
///
bool isReadyToSend();

/// Wait until previous data/signal send operations finished.
///
/// @return true on success, false if there was a time-out.
///
bool waitUntilReadyToSend();

/// Wait for data being received.
///
/// @param timeout The timeout in milliseconds.
/// @return true on success, false if there was a time-out.
///
bool waitForData(uint32_t timeout);

/// Read the last received data.
///
/// @return The last data value which was received.
///
uint32_t readData();

/// Wait for a synchronization pulse.
///
/// @param timeout The timeout in milliseconds.
/// @return true is the synchronization was received, or false if there was a timeout.
///
bool waitForSynchonization(uint32_t timeout);

/// Register a synchronization function.
///
/// This function is called after a synchronization is received from the
/// master controller. Keep this function as short as possible, because it
/// runs in the interrupt of the communication component.
///
void registerSynchronisationFunction(SynchronizationFn synchronizationFn);

/// Register a read data function.
///
/// This function is called after a data value was successfully read from
/// the master board.
///
void registerReadDataFunction(ReadDataFn readDataFn);

/// Register a button press function.
///
/// This function is called if the used pressed the button on the data-in
/// line for the master snow-flake.
///
void registerButtonPressFunction(ButtonPressFn buttonPressFn);

	
}

