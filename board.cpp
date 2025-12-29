#include "board.hpp"
#include <iostream>

/*^   * function name:      board::board
---------------------------------------------------------------------------------------
    * function description: Initializes the chess board to the standard starting position.
    *                       Allocates and places all chess pieces (rooks, knights, bishops,
    *                       queen, king, and pawns) for both white and black players.
    *                       Also initializes the internal board array and board state string.
    * function input:       none
    * function output:      none
    * return value:         none (constructor)
    * efficiency:           O(1) - constant time complexity.
    *                       The board size and number of pieces are fixed.
---------------------------------------------------------------------------------------
*/
board::board() 
{
    int i = 0;
    int j = 0;

    char file = '\0';

    std::string pos = "";

    state_of_board_as_string = STARTING_BOARD_STATE;

    // set it all to null pointer
    for (i = 0; i < BOARD_SIZE_UP; ++i)
    {
        for (j = 0; j < BOARD_SIZE_RIGHT; ++j)
        {
            chess_board[i][j] = nullptr;
        }
    }

    chess_board[0][0] = new rook("a1", true);    // White Rook (true = white)
    chess_board[0][1] = new knight("b1", true);  // White Knight
    chess_board[0][2] = new bishop("c1", true);  // White Bishop
    chess_board[0][3] = new queen("d1", true);   // White Queen
    chess_board[0][4] = new king("e1", true);    // White King
    chess_board[0][5] = new bishop("f1", true);  // White Bishop
    chess_board[0][6] = new knight("g1", true);  // White Knight    
    chess_board[0][7] = new rook("h1", true);    // White Rook
    
    // White pawns at rank 2 (row 1)
    for (j = 0; j < BOARD_SIZE_RIGHT; ++j)
    {
        // make the loc (pos) string 
        file = 'a' + j;
        pos = "";
        pos += file;
        pos += '2';

        chess_board[1][j] = new pawn(pos, true);  // White Pawns
    }

    // Row index 7 corresponds to rank 8
    chess_board[7][0] = new rook("a8", false);   // Black Rook (false = black)
    chess_board[7][1] = new knight("b8", false); // Black Knight
    chess_board[7][2] = new bishop("c8", false); // Black Bishop
    chess_board[7][3] = new queen("d8", false);  // Black Queen
    chess_board[7][4] = new king("e8", false);   // Black King
    chess_board[7][5] = new bishop("f8", false); // Black Bishop
    chess_board[7][6] = new knight("g8", false); // Black Knight
    chess_board[7][7] = new rook("h8", false);   // Black Rook
    
    // Black pawns at rank 7 (row 6)
    for (j = 0; j < BOARD_SIZE_RIGHT; ++j)
    {
        // make the loc (pos) string 
        file = 'a' + j;
        pos = "";
        pos += file;
        pos += '7';

        chess_board[6][j] = new pawn(pos, false);  // Black Pawns
    }

    move_result = MoveResult::Invalid_IllegalMovement;
}



// Destructor to clean up dynamically allocated pieces
/*^   * function name:      board::~board
---------------------------------------------------------------------------------------
    * function description: Destructor for the board class.
    *                       Releases all dynamically allocated chess pieces stored
    *                       on the chess board and sets all board pointers to nullptr
    *                       to prevent dangling pointers.
    * function input:       none
    * function output:      none
    * return value:         none (destructor)
    * efficiency:           O(1) - constant time complexity.
    *                       The board size is fixed (8x8).
---------------------------------------------------------------------------------------
*/
board::~board()
{ 
	int i = 0;
	int j = 0;

	for (i = 0; i < BOARD_SIZE_UP; ++i)
	{
		for (j = 0; j < BOARD_SIZE_RIGHT; ++j)
		{
			delete chess_board[i][j]; // Delete each piece if it exists

			chess_board[i][j] = nullptr; // Set pointer to nullptr after deletion
		}
	}
}

/*^   * function name:      board::kill_at
---------------------------------------------------------------------------------------
    * function description: Removes (captures) a chess piece from the board at the given
    *                       row and column. If a piece exists at the specified location,
    *                       it is deleted and the board square is set to nullptr.
    * function input:       int row, int col
            - row:          The row index of the square (0–7).
            - col:          The column index of the square (0–7).
    * function output:      bool
    * return value:         True if a piece was found and deleted, false otherwise.
    * efficiency:           O(1) - constant time complexity.
---------------------------------------------------------------------------------------
*/
bool board::kill_at(int row, int col)
{
    if (chess_board[row][col] != nullptr)
    {
        delete chess_board[row][col];
        chess_board[row][col] = nullptr;
        return true;
    }
    return false;
}

/*^   * function name:      board::get_result
---------------------------------------------------------------------------------------
    * function description: Returns the result of the last move attempted on the board.
    * function input:       None
    * function output:      MoveResult
    * return value:         The last move result stored in the board object.
    * efficiency:           O(1) - constant time complexity.
---------------------------------------------------------------------------------------
*/
MoveResult board::get_result() const
{
    return move_result;
}

/*^   * function name:      board::set_try_move
---------------------------------------------------------------------------------------
    * function description: Sets the next move attempt for the board object.
    * function input:       const std::string& move
                            - move: The move string to be attempted (e.g., "e2e4").
    * function output:      None
    * return value:         None
    * efficiency:           O(1) - constant time complexity.
---------------------------------------------------------------------------------------
*/

void board::set_try_move(const std::string& move)
{
    try_move = move;
}

MoveResult board::Move()
{
    int string_index = 0;                   // this int stors the index of the string we are working with
    int y_2d_index = 0;                     // this stors the y (secand) in chess and in cpp [1] index 
    int x_2d_index = 0;                     // this stors the x (first)  in chess and in cpp [2] index 
    int des_index_1 = 0;                    // this stors the y (secand) in chess and in cpp [1] index 
    int des_index_2 = 0;                    // this stors the x (first)  in chess and in cpp [2] index
    chess_p* pointer_to_chess = nullptr;    // pointer for the 
    std::string loc = "";                   // string for loc
    int new_string_index = 0;               // this int stors the index of the string we are working with
    char chess_p_char = ' ';                // new char for the char of the chess_p
    std::string try_des = "";               // try move but only des
    bool white_king_found = false;          // find king
    bool black_king_found = false;          // find king
    int i = 0;                              // for the loop
    char turn = state_of_board_as_string[BOARD_STATE_LENGTH - 1];

    // try move hold something like [{s_x},{s_y},{d_x},{d_y}] for exsmple h1h2 -> move h1 to h2

    y_2d_index = try_move[RANK_OFFSET] - '1'; // gets us the nuber for exmple if 1 then '1' - '1' is 49 - 49 = 0       =>  0 is 1 becouse we start from 0
    x_2d_index = try_move[FILE_OFFSET] - 'a'; // gets the later for exsmple if a then its a - a wich is 97 - 97 = 0    =>  0 is 1 becouse we start from 0

    string_index = (y_2d_index) * VERTICAL_STEP + (x_2d_index); // becouse 0 if the first line and 1 if line 2 time 8 gives us the right start of the line point then we add 0 if we are at the start of the line becouse 8 is already the next line

    // now we need to get the pointer:
    pointer_to_chess = chess_board[y_2d_index][x_2d_index];

    // this returns Invalid_NoPieceAtSource if there is No Piece At Source
    if(pointer_to_chess == nullptr)
    {
        return MoveResult::Invalid_NoPieceAtSource;
    }

    loc = pointer_to_chess->give_location();

    if (loc[FILE_OFFSET] != try_move[FILE_OFFSET] || loc[RANK_OFFSET] != try_move[RANK_OFFSET]) // see if loc is right
    {
        return MoveResult::Invalid_NoPieceAtSource;
    }

    try_des = std::string() + try_move[2] + try_move[3];

    // now we do the move part
    pointer_to_chess->set_destination(try_des);              // set the destination
    move_result = pointer_to_chess->is_move_ok(state_of_board_as_string);   // see if the move is ok

    if (move_result == MoveResult::Valid || move_result == MoveResult::Valid_Check || move_result == MoveResult::Valid_Checkmate)
    {
        // get thw char
        chess_p_char = state_of_board_as_string[string_index];

        // calc all the index
        des_index_1 =  try_move[RANK_OFFSET + 2] - '1';
        des_index_2 =  try_move[FILE_OFFSET + 2] - 'a';
        new_string_index = (des_index_1)*VERTICAL_STEP + (des_index_2);

        kill_at(des_index_1, des_index_2); // kill the chess_p at the des

        chess_board[y_2d_index][x_2d_index] = nullptr; // remove the pointer to our chess_p
        state_of_board_as_string[string_index] = '#';  // remove the char    to our chess_p

        chess_board[des_index_1][des_index_2] = pointer_to_chess; // give our pointer
        state_of_board_as_string[new_string_index] = chess_p_char;// give our char

        if (turn == WHITE_TURN)
        {
            turn = BLACK_TURN;
        }
        else
        {
            turn = WHITE_TURN;
        }

        state_of_board_as_string[BOARD_STATE_LENGTH - 1] = turn;

        pointer_to_chess->move(); // tell the chess_p to update where it is
    }

    // -----------------------------
    // Check if either king is missing -> declare checkmate
    // -----------------------------

    for (i = 0; i < state_of_board_as_string.size(); i++)
    {
        if (state_of_board_as_string[i] == 'K')
        {
            white_king_found = true;
        }
        if (state_of_board_as_string[i] == 'k')
        {
            black_king_found = true;
        }
    }

    // If one of the kings is missing, set move_result to checkmate
    if (!white_king_found || !black_king_found)
    {
        move_result = MoveResult::Valid_Checkmate;
    }

    return move_result;
}
