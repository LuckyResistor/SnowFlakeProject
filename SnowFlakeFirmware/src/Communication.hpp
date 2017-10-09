//
// Snow Flake Project
// ---------------------------------------------------------------------------
// (c)2017 by Lucky Resistor. See LICENSE for details.
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


typedef void (*SynchronizationFn)();
typedef void (*ReadDataFn)(uint32_t data);


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
void waitForNegotiation();

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
	
/// Get the length of the stand.
///
/// @return The number of snow flakes in the stand.
///
uint8_t getStandLength();

/// Send data to all other boards.
///
/// The sent 32bit are stored on all other devices. This method
/// has only an effect on the master board with the identifier 0.
/// It will trigger sending the data to all other boards.
///
void sendData(uint32_t data);

/// Send a synchronization request to all other boards in the strand.
///
void sendSynchronization();

/// Get the currently stored data.
///
/// This data is automatically received from the master board.
///
uint32_t getData();

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

	
}

