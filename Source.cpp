#include "Pipe.h"
#include <iostream>
#include <thread>
#include <stdlib.h>
#include <string.h>
#include <chrono>

// using difrent short cuts 
using std::cout;                     // just use cout   without the std::
using std::endl;                     // just use endl   without the std::
using std::string;                   // just use string without the std::

// difrent constants
constexpr size_t BUF_SIZE = 1024;    // the buffer size of char (c like string)

int main()
{
	// -- gui verubals -- //
	char msgToGraphics[BUF_SIZE];     // msgToGraphics should contain the board string accord the protocol
	Pipe p;                           // make conection verubal
	bool isConnect = p.connect();     // save in bool if it conected
	string ans;                       // a verubal to save the answer in 
	string msgFromGraphics = "";      // store msg From Graphics
	string result_in_string = "";     // result in string

	// -- cui verubals -- //
	board chess_board;                // the chess board
	MoveResult result;                // the result

	// whill we arnt conected this will run 
	while (!isConnect)
	{
		// ask the user to conect or not 
		cout << "cant connect to graphics" << endl;
		cout << "Do you try to connect again or exit? (0-try again, 1-exit)" << endl;
		std::cin >> ans; // get the answer from the user

		// if they answered 0 then conect
		if (ans == "0")
		{
			cout << "trying connect again.." << endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));
			isConnect = p.connect();
		}
		else 
		{
			// else then close the backend 
			p.close();
			return 0;
		}
	}

	// send the starting point for now
	strcpy_s(msgToGraphics, BUF_SIZE,"rnbqkbnrpppppppp################################PPPPPPPPRNBQKBNR0"); // send the starting point
	// send the board string
	p.sendMessageToGraphics(msgToGraphics);   
	// get message from graphics
	msgFromGraphics = p.getMessageFromGraphics(); 

	while (msgFromGraphics != "quit")
	{
		// should handle the string the sent from graphics
		// according the protocol. Ex: e2e4           (move e2 to e4)
		
		// set the move we need to try
		chess_board.set_try_move(msgFromGraphics);

		// try the move
		result = chess_board.Move();

		// switch case for the difrent results given 
		switch (result)
		{
			case MoveResult::Valid :
				result_in_string = "0";
				break;

			case MoveResult::Valid_Check :
				result_in_string = "1";
				break;

			case MoveResult::Invalid_NoPieceAtSource :
				result_in_string = "2";
				break;

			case MoveResult::Invalid_DestinationOccupiedByOwnPiece :
				result_in_string = "3";
				break;

			case MoveResult::Invalid_SelfCheck :
				result_in_string = "4";
				break;

			case MoveResult::Invalid_IndexOutOfRange :
				result_in_string = "5";
				break;

			case MoveResult::Invalid_IllegalMovement :
				result_in_string = "6";
				break;

			case MoveResult::Invalid_SameSourceAndDestination :
				result_in_string = "7";
				break;

			case MoveResult::Valid_Checkmate :
				result_in_string = "8";
				break;
		}

		// msgToGraphics should contain the result of the operation
		strcpy_s(msgToGraphics, BUF_SIZE, result_in_string.c_str());

		// we dont swich it all for std string becouse of compatbility with the front end
		p.sendMessageToGraphics(msgToGraphics);  

		// get message from graphics
		msgFromGraphics = p.getMessageFromGraphics();
	}

	p.close(); // end the conection

	return 0; // close the program
}
