// filepath: chess_project\part a\chess_pieces.cpp

#include "chess_pieces.hpp" 
 
///////////////////////////////////////////////////////////////
// function's:
///////////////////////////////////////////////////////////////

// --- helper function for all is there check --- //
/*^   * function name:      chess_p::is_there_check
---------------------------------------------------------------------------------------
    * function description: Checks if the current player's king is in check based on the state of the chess board.
    * function input:       std::string board_state, char turn
        - board_state:      The current state of the chess board (64 chars + 1 turn char).
        - turn:             Indicates whose turn it is ('0' for white, '1' for black).
    * function output:      bool
    * return value:         True if the king is in check, false otherwise.
    * efficiency:           O(n) - linear time complexity, where n is the number of squares on the board.
---------------------------------------------------------------------------------------
*/
bool is_there_check(std::string board_state)
{
    // -----------------------------
    // All variables declared here
    // -----------------------------
    char current_turn_indicator = '\0';           // Which side's turn is encoded in the board state
    char king_character = '\0';                   // The king character we are searching for ('K' or 'k')
    bool king_found_on_board = false;             // Whether the king was found on the board
    bool opponent_is_white_piece = false;         // True if the opponent pieces are white (uppercase)
    int square_index = 0;                         // Generic index used to loop over board_state
    int king_row_index = 0;                       // King's row (0..7)
    int king_column_index = 0;                    // King's column (0..7)
    int scan_row_index = 0;                       // Row index used for scanning around the king
    int scan_column_index = 0;                    // Column index used for scanning around the king
    char piece_on_scanned_square = '\0';          // Piece found on a scanned square
    char lowercase_opponent_piece = '\0';         // Lowercase representation of opponent piece (for comparisons)
    int pawn_attack_row = 0;                      // Row from which opponent pawns would attack
    bool sliding_piece_blocked = false;           // Blocked flag for sliding piece rays
    int i = 0;                                    // Loop counter
    int delta_row = 0;                            // Delta row for adjacency scanning
    int delta_column = 0;                         // Delta column for adjacency scanning
    bool is_orthogonal = false;                   // True when scanning orthogonal directions
    bool is_diagonal = false;                     // True when scanning diagonal directions

    // Knight move deltas (row, column)
    int knight_row_deltas[8] = { -2, -2, -1, -1, 1, 1, 2, 2 };    // row deltas for knight jumps
    int knight_column_deltas[8] = { -1, 1, -2, 2, -2, 2, -1, 1 }; // column deltas for knight jumps

    // Sliding directions (orthogonal then diagonal)
    int sliding_row_deltas[8] = { -1, 1, 0, 0, -1, -1, 1, 1 };    // row deltas for 8 rays
    int sliding_column_deltas[8] = { 0, 0, 1, -1, 1, -1, 1, -1 }; // column deltas for 8 rays

    // -----------------------------
    // Read the turn character from the board string (last char)
    // -----------------------------
    current_turn_indicator = board_state[BOARD_STATE_LENGTH - 1];

    // -----------------------------
    // Decide which king to find and who is the opponent.
    // IMPORTANT: opponent_is_white_piece must be TRUE when the opponent pieces are uppercase (white).
    // If it is WHITE's turn, we are checking the WHITE king, so opponent is BLACK => opponent_is_white_piece = false.
    // If it is BLACK's turn, we are checking the BLACK king, so opponent is WHITE => opponent_is_white_piece = true.
    // -----------------------------
    if (current_turn_indicator == WHITE_TURN)
    {
        king_character = 'K';                    // we search for white king
        opponent_is_white_piece = false;         // opponent is black (lowercase)
    }
    else
    {
        if (current_turn_indicator == BLACK_TURN)
        {
            king_character = 'k';                // we search for black king
            opponent_is_white_piece = true;      // opponent is white (uppercase)
        }
        else
        {
            // Invalid turn indicator in the board string: be conservative and say checked (or return true to fail safe)
            std::cout << "Invalid turn indicator!" << std::endl;
            return true;
        }
    }

    // -----------------------------
    // Find king position (row/column) on the board string
    // board_state uses index = row*8 + col for rows 0..7 and cols 0..7
    // -----------------------------
    for (square_index = 0; square_index < BOARD_SIZE; square_index++)
    {
        if (!king_found_on_board)
        {
            if (board_state[square_index] == king_character)
            {
                king_row_index = square_index / 8;   // integer division -> row
                king_column_index = square_index % 8; // modulus -> column
                king_found_on_board = true;
            }
        }
    }

    // If king not found, cannot be in check
    if (!king_found_on_board)
    {
        return false;
    }

    // -----------------------------
    // Pawn attack detection
    // White pawns (uppercase 'P') attack one rank upward (row-1) diagonally
    // Black pawns (lowercase 'p') attack one rank downward (row+1) diagonally
    // Use opponent_is_white_piece to test correct case.
    // -----------------------------
    if (opponent_is_white_piece) // opponent are white pawns (uppercase 'P')
    {
        pawn_attack_row = king_row_index - 1; // pawns that can attack king are one row above
        if (pawn_attack_row >= 0)
        {
            if (king_column_index - 1 >= 0)
            {
                if (board_state[pawn_attack_row * 8 + (king_column_index - 1)] == 'P')
                {
                    return true;
                }
            }
            if (king_column_index + 1 < 8)
            {
                if (board_state[pawn_attack_row * 8 + (king_column_index + 1)] == 'P')
                {
                    return true;
                }
            }
        }
    }
    else // opponent are black pawns (lowercase 'p')
    {
        pawn_attack_row = king_row_index + 1; // pawns that can attack king are one row below
        if (pawn_attack_row < 8)
        {
            if (king_column_index - 1 >= 0)
            {
                if (board_state[pawn_attack_row * 8 + (king_column_index - 1)] == 'p')
                {
                    return true;
                }
            }
            if (king_column_index + 1 < 8)
            {
                if (board_state[pawn_attack_row * 8 + (king_column_index + 1)] == 'p')
                {
                    return true;
                }
            }
        }
    }

    // -----------------------------
    // Knight attack detection
    // Check all 8 L-shaped knight jumps from the king's square for an opponent knight
    // -----------------------------
    for (i = 0; i < 8; i++)
    {
        scan_row_index = king_row_index + knight_row_deltas[i];
        scan_column_index = king_column_index + knight_column_deltas[i];

        // If target square is on the board then check it
        if (scan_row_index >= 0 && scan_row_index < 8 && scan_column_index >= 0 && scan_column_index < 8)
        {
            piece_on_scanned_square = board_state[scan_row_index * 8 + scan_column_index];
            if (opponent_is_white_piece)
            {
                // Opponent is white -> look for uppercase 'N'
                if (piece_on_scanned_square == 'N')
                {
                    return true;
                }
            }
            else
            {
                // Opponent is black -> look for lowercase 'n'
                if (piece_on_scanned_square == 'n')
                {
                    return true;
                }
            }
        }
    }

    // -----------------------------
    // Opponent king adjacency check
    // The two kings must not be adjacent. If opponent king is on any adjacent square, it's a check.
    // -----------------------------
    for (delta_row = -1; delta_row <= 1; delta_row++)
    {
        for (delta_column = -1; delta_column <= 1; delta_column++)
        {
            if (delta_row != 0 || delta_column != 0) // skip the square of our king
            {
                scan_row_index = king_row_index + delta_row;
                scan_column_index = king_column_index + delta_column;
                if (scan_row_index >= 0 && scan_row_index < 8 && scan_column_index >= 0 && scan_column_index < 8)
                {
                    piece_on_scanned_square = board_state[scan_row_index * 8 + scan_column_index];
                    if (opponent_is_white_piece)
                    {
                        if (piece_on_scanned_square == 'K') // opponent white king
                        {
                            return true;
                        }
                    }
                    else
                    {
                        if (piece_on_scanned_square == 'k') // opponent black king
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }

    // -----------------------------
    // Sliding piece detection (rook, bishop, queen)
    // For each of 8 rays (N, S, E, W, NE, NW, SE, SW) scan outward until blocked.
    // If first blocking piece belongs to opponent and is the correct type (r/q for orthogonal, b/q for diagonal)
    // then king is in check.
    // -----------------------------
    for (i = 0; i < 8; i++)
    {
        scan_row_index = king_row_index + sliding_row_deltas[i];
        scan_column_index = king_column_index + sliding_column_deltas[i];
        sliding_piece_blocked = false;

        while (scan_row_index >= 0 && scan_row_index < 8 && scan_column_index >= 0 && scan_column_index < 8 && !sliding_piece_blocked)
        {
            piece_on_scanned_square = board_state[scan_row_index * 8 + scan_column_index];

            if (piece_on_scanned_square != EMPTY_SQUARE)
            {
                // Mark this ray as blocked (first non-empty square blocks further ray scanning)
                sliding_piece_blocked = true;

                if (opponent_is_white_piece)
                {
                    // Opponent uppercase pieces (white)
                    if (piece_on_scanned_square >= 'A' && piece_on_scanned_square <= 'Z')
                    {
                        // convert to lowercase for easier comparison
                        lowercase_opponent_piece = piece_on_scanned_square + ('a' - 'A');
                        is_orthogonal = (i < 4); // first 4 directions are orthogonal
                        is_diagonal = (i >= 4);  // last 4 directions are diagonal

                        if ((is_orthogonal && (lowercase_opponent_piece == 'r' || lowercase_opponent_piece == 'q')) ||
                            (is_diagonal && (lowercase_opponent_piece == 'b' || lowercase_opponent_piece == 'q')))
                        {
                            return true;
                        }
                    }
                }
                else
                {
                    // Opponent lowercase pieces (black)
                    if (piece_on_scanned_square >= 'a' && piece_on_scanned_square <= 'z')
                    {
                        lowercase_opponent_piece = piece_on_scanned_square; // already lowercase
                        is_orthogonal = (i < 4);
                        is_diagonal = (i >= 4);

                        if ((is_orthogonal && (lowercase_opponent_piece == 'r' || lowercase_opponent_piece == 'q')) ||
                            (is_diagonal && (lowercase_opponent_piece == 'b' || lowercase_opponent_piece == 'q')))
                        {
                            return true;
                        }
                    }
                }
            }

            // Move to the next square along the ray
            scan_row_index = scan_row_index + sliding_row_deltas[i];
            scan_column_index = scan_column_index + sliding_column_deltas[i];
        }
    }

    // No attackers found -> not in check
    return false;
}

///////////////////////////////////////////////////////////////
// Constructor's:
///////////////////////////////////////////////////////////////

// --- Rook Constructor --- // 
/*
^   * fanction name:        rook::rook
---------------------------------------------------------------------------------------
    * fanction description: Initializes a Rook chess piece with its starting location and color.
    * fanction input:       const std::string& start_loc, bool is_it_white
        - start_loc:        The starting location of the Rook piece.
        - is_it_white:      Indicates whether the piece is white (true) or black (false).
    * fanction output:      void
    * return value:         None.
    * eficency:             O(1) - constant time complexity.
---------------------------------------------------------------------------------------
*/
rook::rook(const std::string& start_loc, bool is_it_white) : chess_p(start_loc, is_it_white){}

// --- King Constructor --- // 
/*
^   * fanction name:        king::king
---------------------------------------------------------------------------------------
    * fanction description: Initializes a King chess piece with its starting location and color.
    * fanction input:       const std::string& start_loc, bool is_it_white
        - start_loc:        The starting location of the King piece.
        - is_it_white:      Indicates whether the piece is white (true) or black (false).
    * fanction output:      void
    * return value:         None.
    * eficency:             O(1) - constant time complexity.
---------------------------------------------------------------------------------------
*/
king::king(const std::string& start_loc, bool is_it_white) : chess_p(start_loc, is_it_white) {}

// --- Knight Constructor --- // 
/*
^   * fanction name:        knight::knight
---------------------------------------------------------------------------------------
    * fanction description: Initializes a knight chess piece with its starting location and color.
    * fanction input:       const std::string& start_loc, bool is_it_white
        - start_loc:        The starting location of the knight piece.
        - is_it_white:      Indicates whether the piece is white (true) or black (false).
    * fanction output:      void
    * return value:         None.
    * eficency:             O(1) - constant time complexity.
---------------------------------------------------------------------------------------
*/
knight::knight(std::string start_loc, bool is_it_white) : chess_p(start_loc, is_it_white) {}

// --- Bishop Constructor --- // 
/*
^   * fanction name:        bishop::bishop
---------------------------------------------------------------------------------------
    * fanction description: Initializes a bishop chess piece with its starting location and color.
    * fanction input:       const std::string& start_loc, bool is_it_white
        - start_loc:        The starting location of the bishop piece.
        - is_it_white:      Indicates whether the piece is white (true) or black (false).
    * fanction output:      void
    * return value:         None.
    * eficency:             O(1) - constant time complexity.
---------------------------------------------------------------------------------------
*/
bishop::bishop(std::string start_loc, bool is_it_white) : chess_p(start_loc, is_it_white) {}

// --- Queen Constructor --- // 
/*
^   * fanction name:        queen::queen
---------------------------------------------------------------------------------------
    * fanction description: Initializes a queen chess piece with its starting location and color.
    * fanction input:       const std::string& start_loc, bool is_it_white
        - start_loc:        The starting location of the queen piece.
        - is_it_white:      Indicates whether the piece is white (true) or black (false).
    * fanction output:      void
    * return value:         None.
    * eficency:             O(1) - constant time complexity.
---------------------------------------------------------------------------------------
*/
queen::queen(std::string start_loc, bool is_it_white) : chess_p(start_loc, is_it_white) {}

// --- Pawn Constructor --- // 
/*
^   * fanction name:        pawn::pawn
---------------------------------------------------------------------------------------
    * fanction description: Initializes a pawn chess piece with its starting location and color.
    * fanction input:       const std::string& start_loc, bool is_it_white
        - start_loc:        The starting location of the pawn piece.
        - is_it_white:      Indicates whether the piece is white (true) or black (false).
    * fanction output:      void
    * return value:         None.
    * eficency:             O(1) - constant time complexity.
---------------------------------------------------------------------------------------
*/
pawn::pawn(std::string start_loc, bool is_it_white) : chess_p(start_loc, is_it_white) {}


///////////////////////////////////////////////////////////////
// is_move_ok's:
///////////////////////////////////////////////////////////////

// --> rook is move ok?
/*
^   * fanction name:        rook::is_move_ok
---------------------------------------------------------------------------------------
    * fanction description: Determines if the proposed move for the Rook piece is valid based on the current state of the chess board.
    * fanction input:       std::string state_of_board
        - state_of_board:   The current state of the chess board.
    * fanction output:      MoveResult
    * return value:         An enumeration value indicating the result of the move validation.
    * eficency:             O(n) - linear time complexity, where n is the number of squares checked for path clearance.
---------------------------------------------------------------------------------------
*/
MoveResult rook::is_move_ok(std::string state_of_board, bool check_for_check)
{
    /*
    Valid = 0,
    Valid_Check = 1,
    Invalid_NoPieceAtSource = 2,
    Invalid_DestinationOccupiedByOwnPiece = 3,
    Invalid_SelfCheck = 4,
    Invalid_IndexOutOfRange = 5,
    Invalid_IllegalMovement = 6,
    Invalid_SameSourceAndDestination = 7,
    Valid_Checkmate = 8
    */
    std::string loc = give_location();                       // Get the current location of the rook
    std::string dest = get_destination();                    // Get the destination location for the move
    char turn = state_of_board[BOARD_STATE_LENGTH - 1];      // Extract whose turn it is from the board state
    int source_index = 0;                                    // Index of the source square on the board
    int dest_index = 0;                                      // Index of the destination square on the board
    int start_index = 0;                                     // Starting index for path validation
    int end_index = 0;                                       // Ending index for path validation
    int step = 0;                                            // Step direction (vertical or horizontal)
    bool dest_is_white = false;                              // Color of the piece at the destination
    char dest_piece = '\0';                                  // Piece at the destination square
    int i = 0;                                               // Loop variable
    std::string new_state_of_board = state_of_board;         // Copy of the board state for self-check validation
    char opponent_king = '\0';                               // Opponent's king character
    bool king_exists = false;                                // Flag to check if opponent's king exists

    // code number 7
    // Check for same source and destination
    if (loc == dest)
    {
        return MoveResult::Invalid_SameSourceAndDestination;
    }

    // code number 2
    // Check if there is a piece at the source location
    source_index = (loc[RANK_OFFSET] - '1') * VERTICAL_STEP + (loc[FILE_OFFSET] - 'a'); //-note-> working culc

    if (state_of_board[source_index] == EMPTY_SQUARE)
    {
        return MoveResult::Invalid_NoPieceAtSource;
    }

    // code number 3
    // Check if the destination is occupied by own piece
    dest_index = (dest[RANK_OFFSET] - '1') * VERTICAL_STEP + (dest[FILE_OFFSET] - 'a');

    if (state_of_board[dest_index] != EMPTY_SQUARE)
    {
        dest_piece = state_of_board[dest_index];
        dest_is_white = (dest_piece >= 'A' && dest_piece <= 'Z');

        if (dest_is_white == get_is_white())
        {
            return MoveResult::Invalid_DestinationOccupiedByOwnPiece;
        }
        
    }

    // code number 5
    // Check for index out of range
    if (source_index < 0 || source_index >= BOARD_SIZE || dest_index < 0 || dest_index >= BOARD_SIZE)
    {
        return MoveResult::Invalid_IndexOutOfRange;
    }

    // code number 6
    // Validate rook movement (horizontal or vertical)
    if ((turn == WHITE_TURN && get_is_white()) || (turn != WHITE_TURN && !get_is_white())) // Check if it's the correct player's turn
    {
        if (loc[FILE_OFFSET] == dest[FILE_OFFSET] || loc[RANK_OFFSET] == dest[RANK_OFFSET]) // Valid rook move (same file or same rank)
        {
            // Calculate start and end indices for path validation
            // this converts loc and dest from chess notation to board string index so we can use simple integer arithmetic
            start_index = (loc[RANK_OFFSET] - '1') * VERTICAL_STEP + (loc[FILE_OFFSET] - 'a');
            end_index = (dest[RANK_OFFSET] - '1') * VERTICAL_STEP + (dest[FILE_OFFSET] - 'a');
            
            // Determine step direction based on movement type
            if (loc[FILE_OFFSET] == dest[FILE_OFFSET])
            {
                // Vertical movement: check if moving up or down
                if (loc[RANK_OFFSET] < dest[RANK_OFFSET])
                {
                    step = VERTICAL_STEP;  // Moving up
                }
                else
                {
                    step = -VERTICAL_STEP; // Moving down
                }
            }
            else
            {
                // Horizontal movement: check if moving left or right
                if (loc[FILE_OFFSET] < dest[FILE_OFFSET])
                {
                    step = HORIZONTAL_STEP;  // Moving right
                }
                else
                {
                    step = -HORIZONTAL_STEP; // Moving left
                }
            }
            
            for (i = start_index + step; i != end_index; i += step)
            {
                if (state_of_board[i] != EMPTY_SQUARE)
                {
                    return MoveResult::Invalid_IllegalMovement;
                }
            }
        }
        else
        {
            return MoveResult::Invalid_IllegalMovement;
        }
    }
    else
    {
        return MoveResult::Invalid_IllegalMovement;
    }

    if(!check_for_check)
    {
        return MoveResult::Valid;
    }

    // we are testing to see if we did Self Check so we will need 
    // so we will act like the next turn is ours so it will ask if
    // is the w king is in denger if turn is w
    // Check for self-check (DON'T switch turn)
    new_state_of_board = state_of_board;
    new_state_of_board[dest_index] = new_state_of_board[source_index];
    new_state_of_board[source_index] = EMPTY_SQUARE;
    // KEEP the same turn to check OUR king
    new_state_of_board[BOARD_STATE_LENGTH - 1] = turn;

    if (is_there_check(new_state_of_board))
    {
        return MoveResult::Invalid_SelfCheck;
    }

    // now we see if we made the other be in denger so we tell them hi we are black so they see if black is in denger if we are w
    // if the other king is in denger we got a Valid Check
    // Check if opponent is in check (DO switch turn)
    new_state_of_board = state_of_board;
    new_state_of_board[dest_index] = new_state_of_board[source_index];
    new_state_of_board[source_index] = EMPTY_SQUARE;
    // SWITCH turn to check OPPONENT's king
    if (turn == WHITE_TURN) 
    {
        new_state_of_board[BOARD_STATE_LENGTH - 1] = BLACK_TURN;
    } 
    else 
    {
        new_state_of_board[BOARD_STATE_LENGTH - 1] = WHITE_TURN;
    }

    if (is_there_check(new_state_of_board))
    {
        return MoveResult::Valid_Check;
    }
    
    // Determine opponent's king character
    if (!get_is_white())
    {
        opponent_king = 'K';
    }
    else
    {
        opponent_king = 'k';
    }
    
    // Check if opponent's king is on the board
    king_exists = false;
    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (new_state_of_board[i] == opponent_king)
        {
            king_exists = true;
            break; // get out this improves speed becouse i dont need to keep going if i got it 
        }
    }
    
    // If king doesn't exist, it's checkmate; otherwise, it's a valid move
    if (!king_exists)
    {
        return MoveResult::Valid_Checkmate;
    }
    else
    {
        return MoveResult::Valid;
    }
}
 
// --> king is move ok?
/*
^   * fanction name:        king::is_move_ok
--------------------------------------------------------------------------------------- 
    * fanction description: Determines if the proposed move for the King piece is valid based on the current state of the chess board.
    * fanction input:       std::string state_of_board
        - state_of_board:   The current state of the chess board.
    * fanction output:      MoveResult
    * return value:         An enumeration value indicating the result of the move validation.
    * eficency:             O(n) - linear time complexity, where n is the number of squares checked for path clearance.
---------------------------------------------------------------------------------------
*/
MoveResult king::is_move_ok(std::string state_of_board, bool check_for_check)
{
    /*
    Valid = 0,
    Valid_Check = 1,
    Invalid_NoPieceAtSource = 2,
    Invalid_DestinationOccupiedByOwnPiece = 3,
    Invalid_SelfCheck = 4,
    Invalid_IndexOutOfRange = 5,
    Invalid_IllegalMovement = 6,
    Invalid_SameSourceAndDestination = 7,
    Valid_Checkmate = 8
    */
    MoveResult result = MoveResult::Invalid_IllegalMovement; // Default to invalid move
    std::string loc = give_location();                       // Get the current location of the king
    std::string dest = get_destination();                    // Get the destination location for the move
    char turn = state_of_board[BOARD_STATE_LENGTH - 1];      // Extract whose turn it is from the board state
    int source_index = 0;                                    // Index of the source square on the board
    int dest_index = 0;                                      // Index of the destination square on the board
    std::string new_state_of_board = "";                     // new state of the board to test for Check
    bool king_exists = false;                                // Flag to check if opponent's king exists
    char opponent_king = '\0';                               // Opponent's king character
    int i = 0;                                               // loop number

    // Determine opponent's king character
    if (!get_is_white())
    {
        opponent_king = 'K'; // w is big
    }
    else
    {
        opponent_king = 'k'; // b is small
    }

    // the king legal moves are one square in any direction
    if ((abs(loc[FILE_OFFSET] - dest[FILE_OFFSET]) <= 1) && (abs(loc[RANK_OFFSET] - dest[RANK_OFFSET]) <= 1))
    {
        if ((turn == WHITE_TURN && get_is_white()) || (turn != WHITE_TURN && !get_is_white())) // Check if it's the correct player's turn
        {
            // now we know it's the turn of whoever owns this king and the move is within one square in any direction
            if (loc != dest)// make sure source and destination are not the same
            {
                // now we need to see if the king is even there
                source_index = (loc[RANK_OFFSET] - '1') * VERTICAL_STEP + (loc[FILE_OFFSET] - 'a');

                if (state_of_board[source_index] == 'K' || state_of_board[source_index] == 'k')
                {
                    // now we need to see if the destination is occupied by own piece
                    dest_index = (dest[RANK_OFFSET] - '1') * VERTICAL_STEP + (dest[FILE_OFFSET] - 'a');

                    if (state_of_board[dest_index] != EMPTY_SQUARE)
                    {
                        char dest_piece = state_of_board[dest_index];
                        bool dest_is_white = (dest_piece >= 'A' && dest_piece <= 'Z');
                        if (dest_is_white == get_is_white())
                        {
                            result = MoveResult::Invalid_DestinationOccupiedByOwnPiece; // Destination occupied by own piece
                        }
                        else
                        {
                            result = MoveResult::Valid; // Valid move (capturing opponent's piece)
                        }
                    }
                    else
                    {
                        result = MoveResult::Valid; // Valid move (to empty square)
                    }
                }
                else
                {
                    result = MoveResult::Invalid_NoPieceAtSource; // No piece at source
                }
            }
            else
            {
                result = MoveResult::Invalid_SameSourceAndDestination; // Same source and destination
            }
        }
        else
        {
            result = MoveResult::Invalid_IllegalMovement; // Invalid move
        }
    }
    else
    {
        result = MoveResult::Invalid_IllegalMovement; // Invalid move
    }
    
    // code number 5
    // Check for index out of range
    if (source_index < 0 || source_index >= BOARD_SIZE || dest_index < 0 || dest_index >= BOARD_SIZE)
    {
        return MoveResult::Invalid_IndexOutOfRange;
    }

    if(!check_for_check)
    {
        return result;
    }

    // Check for self-check (DON'T switch turn)
    new_state_of_board = state_of_board;
    new_state_of_board[dest_index] = new_state_of_board[source_index];
    new_state_of_board[source_index] = EMPTY_SQUARE;
    // KEEP the same turn to check OUR king
    new_state_of_board[BOARD_STATE_LENGTH - 1] = turn;

    if (is_there_check(new_state_of_board))
    {
        return MoveResult::Invalid_SelfCheck;
    }

    // Check if opponent is in check (DO switch turn)
    new_state_of_board = state_of_board;
    new_state_of_board[dest_index] = new_state_of_board[source_index];
    new_state_of_board[source_index] = EMPTY_SQUARE;
    // SWITCH turn to check OPPONENT's king
    if (turn == WHITE_TURN) 
    {
        new_state_of_board[BOARD_STATE_LENGTH - 1] = BLACK_TURN;
    }    
    else 
    {
        new_state_of_board[BOARD_STATE_LENGTH - 1] = WHITE_TURN;
    }

    if (is_there_check(new_state_of_board))
    {
        return MoveResult::Valid_Check;
    }

    if(result == MoveResult::Valid)
    {
        // Check if opponent's king is on the board
        king_exists = false;
        for (i = 0; i < BOARD_SIZE; i++)
        {
            if (new_state_of_board[i] == opponent_king)
            {
                king_exists = true;
                break;
            }
        }

        // If king doesn't exist, it's checkmate; otherwise, it's a valid move
        if (!king_exists)
        {
            result = MoveResult::Valid_Checkmate;
        }
        else
        {
            result = MoveResult::Valid;
        }
    }

    return result; // return the appropriate MoveResult after implementing the logic
}

// --> knight is move ok?
/*
^   * function name:        knight::is_move_ok
---------------------------------------------------------------------------------------
    * function description: Determines if the proposed move for the Knight piece is valid based on the current state of the chess board.
    * function input:       std::string state_of_board
        - state_of_board:   The current state of the chess board.
    * function output:      MoveResult
    * return value:         An enumeration value indicating the result of the move validation.
    * efficiency:           O(1) - constant time complexity (knight moves are fixed patterns).
---------------------------------------------------------------------------------------
*/
MoveResult knight::is_move_ok(std::string state_of_board, bool check_for_check)
{
    /*
    Valid = 0,
    Valid_Check = 1,
    Invalid_NoPieceAtSource = 2,
    Invalid_DestinationOccupiedByOwnPiece = 3,
    Invalid_SelfCheck = 4,
    Invalid_IndexOutOfRange = 5,
    Invalid_IllegalMovement = 6,
    Invalid_SameSourceAndDestination = 7,
    Valid_Checkmate = 8
    */
    std::string loc = give_location();                       // Get the current location of the rook
    std::string dest = get_destination();                    // Get the destination location for the move
    char turn = state_of_board[BOARD_STATE_LENGTH - 1];      // Extract whose turn it is from the board state
    int source_index = 0;                                    // Index of the source square on the board
    int dest_index = 0;                                      // Index of the destination square on the board
    bool dest_is_white = false;                              // Color of the piece at the destination
    char dest_piece = '\0';                                  // Piece at the destination square
    int i = 0;                                               // Loop variable
    std::string new_state_of_board = state_of_board;         // Copy of the board state for self-check validation
    char opponent_king = '\0';                               // Opponent's king character
    bool king_exists = false;                                // Flag to check if opponent's king exists
    int file_diff = 0;                                       // difference in file (a-h)
    int rank_diff = 0;                                       // difference in rank (1-8)

    // code number 7
    // Check for same source and destination
    if (loc == dest)
    {
        return MoveResult::Invalid_SameSourceAndDestination;
    }

    // code number 2
    // Check if there is a piece at the source location
    source_index = (loc[RANK_OFFSET] - '1') * VERTICAL_STEP + (loc[FILE_OFFSET] - 'a'); //-note-> working culc

    if (state_of_board[source_index] == EMPTY_SQUARE)
    {
        return MoveResult::Invalid_NoPieceAtSource;
    }

    // code number 3
    // Check if the destination is occupied by own piece
    dest_index = (dest[RANK_OFFSET] - '1') * VERTICAL_STEP + (dest[FILE_OFFSET] - 'a');

    if (state_of_board[dest_index] != EMPTY_SQUARE)
    {
        dest_piece = state_of_board[dest_index];
        dest_is_white = (dest_piece >= 'A' && dest_piece <= 'Z');

        if (dest_is_white == get_is_white())
        {
            return MoveResult::Invalid_DestinationOccupiedByOwnPiece;
        }

    }

    // code number 5
    // Check for index out of range
    if (source_index < 0 || source_index >= BOARD_SIZE || dest_index < 0 || dest_index >= BOARD_SIZE)
    {
        return MoveResult::Invalid_IndexOutOfRange;
    }

    if (!((turn == WHITE_TURN && get_is_white()) || (turn != WHITE_TURN && !get_is_white()))) // Check if it's the correct player's turn
    {
        return MoveResult::Invalid_IllegalMovement;
    }

    // Calculate absolute differences between source and destination
    file_diff = abs(loc[FILE_OFFSET] - dest[FILE_OFFSET]);
    rank_diff = abs(loc[RANK_OFFSET] - dest[RANK_OFFSET]);

    // Knight legal movement check
    if (file_diff == 2 && rank_diff == 1)
    {
        // legal knight move keep going
    }
    else if (file_diff == 1 && rank_diff == 2)
    {
        // legal knight move keep going
    }
    else
    {
        return MoveResult::Invalid_IllegalMovement;
    }

    if (!check_for_check)
    {
        return MoveResult::Valid;
    }

    // we are testing to see if we did Self Check so we will need 
    // so we will act like the next turn is ours so it will ask if
    // is the w king is in denger if turn is w
    // Check for self-check (DON'T switch turn)
    new_state_of_board = state_of_board;
    new_state_of_board[dest_index] = new_state_of_board[source_index];
    new_state_of_board[source_index] = EMPTY_SQUARE;
    // KEEP the same turn to check OUR king
    new_state_of_board[BOARD_STATE_LENGTH - 1] = turn;

    if (is_there_check(new_state_of_board))
    {
        return MoveResult::Invalid_SelfCheck;
    }

    // now we see if we made the other be in denger so we tell them hi we are black so they see if black is in denger if we are w
    // if the other king is in denger we got a Valid Check
    // Check if opponent is in check (DO switch turn)
    new_state_of_board = state_of_board;
    new_state_of_board[dest_index] = new_state_of_board[source_index];
    new_state_of_board[source_index] = EMPTY_SQUARE;
    // SWITCH turn to check OPPONENT's king
    if (turn == WHITE_TURN)
    {
        new_state_of_board[BOARD_STATE_LENGTH - 1] = BLACK_TURN;
    }
    else
    {
        new_state_of_board[BOARD_STATE_LENGTH - 1] = WHITE_TURN;
    }

    if (is_there_check(new_state_of_board))
    {
        return MoveResult::Valid_Check;
    }

    // Determine opponent's king character
    if (!get_is_white())
    {
        opponent_king = 'K';
    }
    else
    {
        opponent_king = 'k';
    }

    // Check if opponent's king is on the board
    king_exists = false;
    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (new_state_of_board[i] == opponent_king)
        {
            king_exists = true;
            break; // get out this improves speed becouse i dont need to keep going if i got it 
        }
    }

    // If king doesn't exist, it's checkmate; otherwise, it's a valid move
    if (!king_exists)
    {
        return MoveResult::Valid_Checkmate;
    }
    else
    {
        return MoveResult::Valid;
    }
}

// --> bishop is move ok?
/*
^   * function name:        bishop::is_move_ok
---------------------------------------------------------------------------------------
    * function description: Determines if the proposed move for the Bishop piece is valid based on the current state of the chess board.
    * function input:       std::string state_of_board
        - state_of_board:   The current state of the chess board.
    * function output:      MoveResult
    * return value:         An enumeration value indicating the result of the move validation.
    * efficiency:           O(n) - linear time complexity, where n is the number of squares checked for path clearance.
---------------------------------------------------------------------------------------
*/
MoveResult bishop::is_move_ok(std::string state_of_board, bool check_for_check)
{
    /*
        Valid = 0,
        Valid_Check = 1,
        Invalid_NoPieceAtSource = 2,
        Invalid_DestinationOccupiedByOwnPiece = 3,
        Invalid_SelfCheck = 4,
        Invalid_IndexOutOfRange = 5,
        Invalid_IllegalMovement = 6,
        Invalid_SameSourceAndDestination = 7,
        Valid_Checkmate = 8
     */
    std::string loc = give_location();                       // Get the current location of the rook
    std::string dest = get_destination();                    // Get the destination location for the move
    char turn = state_of_board[BOARD_STATE_LENGTH - 1];      // Extract whose turn it is from the board state
    int source_index = 0;                                    // Index of the source square on the board
    int dest_index = 0;                                      // Index of the destination square on the board
    bool dest_is_white = false;                              // Color of the piece at the destination
    char dest_piece = '\0';                                  // Piece at the destination square
    int i = 0;                                               // Loop variable
    std::string new_state_of_board = state_of_board;         // Copy of the board state for self-check validation
    char opponent_king = '\0';                               // Opponent's king character
    bool king_exists = false;                                // Flag to check if opponent's king exists
    int file_diff = 0;
    int rank_diff = 0;
    int file_step = 0;
    int rank_step = 0;
    int scan_file = 0;
    int scan_rank = 0;
    int scan_index = 0;
    bool path_blocked = false;


    // code number 7
    // Check for same source and destination
    if (loc == dest)
    {
        return MoveResult::Invalid_SameSourceAndDestination;
    }

    // code number 2
    // Check if there is a piece at the source location
    source_index = (loc[RANK_OFFSET] - '1') * VERTICAL_STEP + (loc[FILE_OFFSET] - 'a'); //-note-> working culc

    if (state_of_board[source_index] == EMPTY_SQUARE)
    {
        return MoveResult::Invalid_NoPieceAtSource;
    }

    // code number 3
    // Check if the destination is occupied by own piece
    dest_index = (dest[RANK_OFFSET] - '1') * VERTICAL_STEP + (dest[FILE_OFFSET] - 'a');

    if (state_of_board[dest_index] != EMPTY_SQUARE)
    {
        dest_piece = state_of_board[dest_index];
        dest_is_white = (dest_piece >= 'A' && dest_piece <= 'Z');

        if (dest_is_white == get_is_white())
        {
            return MoveResult::Invalid_DestinationOccupiedByOwnPiece;
        }

    }

    // code number 5
    // Check for index out of range
    if (source_index < 0 || source_index >= BOARD_SIZE || dest_index < 0 || dest_index >= BOARD_SIZE)
    {
        return MoveResult::Invalid_IndexOutOfRange;
    }

    if (!((turn == WHITE_TURN && get_is_white()) || (turn != WHITE_TURN && !get_is_white()))) // Check if it's the correct player's turn
    {
        return MoveResult::Invalid_IllegalMovement;
    }

    // -----------------------------
    // Bishop diagonal movement test
    // -----------------------------

    file_diff = abs(loc[FILE_OFFSET] - dest[FILE_OFFSET]);
    rank_diff = abs(loc[RANK_OFFSET] - dest[RANK_OFFSET]);

    if (file_diff != rank_diff)
    {
        return MoveResult::Invalid_IllegalMovement;
    }

    // -----------------------------
    // Path blocking check (bishop cannot jump)
    // -----------------------------

    if (dest[FILE_OFFSET] > loc[FILE_OFFSET])
    {
        file_step = 1;
    }
    else
    {
        file_step = -1;
    }

    if (dest[RANK_OFFSET] > loc[RANK_OFFSET])
    {
        rank_step = 1;
    }
    else
    {
        rank_step = -1;
    }

    scan_file = loc[FILE_OFFSET] + file_step;
    scan_rank = loc[RANK_OFFSET] + rank_step;
    path_blocked = false;

    while ((scan_file != dest[FILE_OFFSET] || scan_rank != dest[RANK_OFFSET]) && !path_blocked)
    {
        scan_index = (scan_rank - '1') * VERTICAL_STEP + (scan_file - 'a');

        if (state_of_board[scan_index] != EMPTY_SQUARE)
        {
            path_blocked = true;
        }
        scan_file = scan_file + file_step;
        scan_rank = scan_rank + rank_step;
    }

    if (path_blocked)
    {
        return MoveResult::Invalid_IllegalMovement;
    }

    if (!check_for_check)
    {
        return MoveResult::Valid;
    }

    // we are testing to see if we did Self Check so we will need 
    // so we will act like the next turn is ours so it will ask if
    // is the w king is in denger if turn is w
    // Check for self-check (DON'T switch turn)
    new_state_of_board = state_of_board;
    new_state_of_board[dest_index] = new_state_of_board[source_index];
    new_state_of_board[source_index] = EMPTY_SQUARE;
    // KEEP the same turn to check OUR king
    new_state_of_board[BOARD_STATE_LENGTH - 1] = turn;

    if (is_there_check(new_state_of_board))
    {
        return MoveResult::Invalid_SelfCheck;
    }

    // now we see if we made the other be in denger so we tell them hi we are black so they see if black is in denger if we are w
    // if the other king is in denger we got a Valid Check
    // Check if opponent is in check (DO switch turn)
    new_state_of_board = state_of_board;
    new_state_of_board[dest_index] = new_state_of_board[source_index];
    new_state_of_board[source_index] = EMPTY_SQUARE;
    // SWITCH turn to check OPPONENT's king
    if (turn == WHITE_TURN)
    {
        new_state_of_board[BOARD_STATE_LENGTH - 1] = BLACK_TURN;
    }
    else
    {
        new_state_of_board[BOARD_STATE_LENGTH - 1] = WHITE_TURN;
    }

    if (is_there_check(new_state_of_board))
    {
        return MoveResult::Valid_Check;
    }

    // Determine opponent's king character
    if (!get_is_white())
    {
        opponent_king = 'K';
    }
    else
    {
        opponent_king = 'k';
    }

    // Check if opponent's king is on the board
    king_exists = false;
    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (new_state_of_board[i] == opponent_king)
        {
            king_exists = true;
            break; // get out this improves speed becouse i dont need to keep going if i got it 
        }
    }

    // If king doesn't exist, it's checkmate; otherwise, it's a valid move
    if (!king_exists)
    {
        return MoveResult::Valid_Checkmate;
    }
    else
    {
        return MoveResult::Valid;
    }
}

// --> queen is move ok?
/*
^   * function name:        queen::is_move_ok
---------------------------------------------------------------------------------------
    * function description: Determines if the proposed move for the Queen piece is valid based on the current state of the chess board.
    * function input:       std::string state_of_board
        - state_of_board:   The current state of the chess board.
    * function output:      MoveResult
    * return value:         An enumeration value indicating the result of the move validation.
    * efficiency:           O(n) - linear time complexity, where n is the number of squares checked for path clearance.
---------------------------------------------------------------------------------------
*/
MoveResult queen::is_move_ok(std::string state_of_board, bool check_for_check)
{
    /*
        Valid = 0,
        Valid_Check = 1,
        Invalid_NoPieceAtSource = 2,
        Invalid_DestinationOccupiedByOwnPiece = 3,
        Invalid_SelfCheck = 4,
        Invalid_IndexOutOfRange = 5,
        Invalid_IllegalMovement = 6,
        Invalid_SameSourceAndDestination = 7,
        Valid_Checkmate = 8
    */

    std::string loc = give_location();                       // Get the current location of the queen
    std::string dest = get_destination();                    // Get the destination location for the move
    char turn = state_of_board[BOARD_STATE_LENGTH - 1];      // Extract whose turn it is from the board state
    int source_index = 0;                                    // Index of the source square on the board
    int dest_index = 0;                                      // Index of the destination square on the board
    int scan_index = 0;                                      // Index used when scanning path
    int step = 0;                                            // Step increment for straight moves
    bool dest_is_white = false;                              // Color of the piece at the destination
    char dest_piece = '\0';                                  // Piece at the destination square
    int i = 0;                                               // Loop variable
    std::string new_state_of_board = state_of_board;         // Copy of the board state for self-check validation
    char opponent_king = '\0';                               // Opponent's king character
    bool king_exists = false;                                // Flag to check if opponent's king exists

    // variables for diagonal checks
    int file_diff = 0;
    int rank_diff = 0;
    int file_step = 0;
    int rank_step = 0;
    int scan_file = 0;
    int scan_rank = 0;
    bool path_blocked = false;

    // -----------------------------
    // Check same source and destination
    // -----------------------------
    if (loc == dest)
    {
        return MoveResult::Invalid_SameSourceAndDestination;
    }

    // -----------------------------
    // Compute source and destination indices
    // -----------------------------
    source_index = (loc[RANK_OFFSET] - '1') * VERTICAL_STEP + (loc[FILE_OFFSET] - 'a');
    dest_index = (dest[RANK_OFFSET] - '1') * VERTICAL_STEP + (dest[FILE_OFFSET] - 'a');

    // -----------------------------
    // Check there is a piece at source
    // -----------------------------
    if (state_of_board[source_index] == EMPTY_SQUARE)
    {
        return MoveResult::Invalid_NoPieceAtSource;
    }

    // -----------------------------
    // Check destination occupied by own piece
    // -----------------------------
    if (state_of_board[dest_index] != EMPTY_SQUARE)
    {
        dest_piece = state_of_board[dest_index];
        dest_is_white = (dest_piece >= 'A' && dest_piece <= 'Z');

        if (dest_is_white == get_is_white())
        {
            return MoveResult::Invalid_DestinationOccupiedByOwnPiece;
        }
    }

    // -----------------------------
    // Index out of range check
    // -----------------------------
    if (source_index < 0 || source_index >= BOARD_SIZE || dest_index < 0 || dest_index >= BOARD_SIZE)
    {
        return MoveResult::Invalid_IndexOutOfRange;
    }

    // -----------------------------
    // Turn check
    // -----------------------------
    if (!((turn == WHITE_TURN && get_is_white()) || (turn != WHITE_TURN && !get_is_white())))
    {
        return MoveResult::Invalid_IllegalMovement;
    }

    // -----------------------------
    // Movement geometry and path checks
    // Queen moves like rook OR bishop
    // -----------------------------

    // straight move (same file or same rank) -> rook logic
    if (loc[FILE_OFFSET] == dest[FILE_OFFSET] || loc[RANK_OFFSET] == dest[RANK_OFFSET])
    {
        // vertical movement (same file)
        if (loc[FILE_OFFSET] == dest[FILE_OFFSET])
        {
            if (dest[RANK_OFFSET] > loc[RANK_OFFSET])
            {
                step = VERTICAL_STEP; // moving up the board string
            }
            else
            {
                step = -VERTICAL_STEP; // moving down
            }

            scan_index = source_index + step;
            path_blocked = false;

            while (scan_index != dest_index && !path_blocked)
            {
                if (state_of_board[scan_index] != EMPTY_SQUARE)
                {
                    path_blocked = true;
                }

                scan_index = scan_index + step;
            }

            if (path_blocked)
            {
                return MoveResult::Invalid_IllegalMovement;
            }
        }
        else // horizontal movement (same rank)
        {
            if (dest[FILE_OFFSET] > loc[FILE_OFFSET])
            {
                step = HORIZONTAL_STEP; // moving right
            }
            else
            {
                step = -HORIZONTAL_STEP; // moving left
            }

            scan_index = source_index + step;
            path_blocked = false;

            while (scan_index != dest_index && !path_blocked)
            {
                if (state_of_board[scan_index] != EMPTY_SQUARE)
                {
                    path_blocked = true;
                }

                scan_index = scan_index + step;
            }

            if (path_blocked)
            {
                return MoveResult::Invalid_IllegalMovement;
            }
        }
    }
    else
    {
        // diagonal move -> bishop logic
        file_diff = abs(loc[FILE_OFFSET] - dest[FILE_OFFSET]);
        rank_diff = abs(loc[RANK_OFFSET] - dest[RANK_OFFSET]);

        if (file_diff != rank_diff)
        {
            return MoveResult::Invalid_IllegalMovement;
        }

        if (dest[FILE_OFFSET] > loc[FILE_OFFSET])
        {
            file_step = 1;
        }
        else
        {
            file_step = -1;
        }

        if (dest[RANK_OFFSET] > loc[RANK_OFFSET])
        {
            rank_step = 1;
        }
        else
        {
            rank_step = -1;
        }

        scan_file = loc[FILE_OFFSET] + file_step;
        scan_rank = loc[RANK_OFFSET] + rank_step;
        path_blocked = false;

        while ((scan_file != dest[FILE_OFFSET] || scan_rank != dest[RANK_OFFSET]) && !path_blocked)
        {
            scan_index = (scan_rank - '1') * VERTICAL_STEP + (scan_file - 'a');

            if (state_of_board[scan_index] != EMPTY_SQUARE)
            {
                path_blocked = true;
            }

            scan_file = scan_file + file_step;
            scan_rank = scan_rank + rank_step;
        }

        if (path_blocked)
        {
            return MoveResult::Invalid_IllegalMovement;
        }
    }

    // -----------------------------
    // If we aren't checking for check, the move is valid
    // -----------------------------
    if (!check_for_check)
    {
        return MoveResult::Valid;
    }

    // -----------------------------
    // Self-check: simulate move, keep the same turn to check OUR king
    // -----------------------------
    new_state_of_board = state_of_board;
    new_state_of_board[dest_index] = new_state_of_board[source_index];
    new_state_of_board[source_index] = EMPTY_SQUARE;
    new_state_of_board[BOARD_STATE_LENGTH - 1] = turn;

    if (is_there_check(new_state_of_board))
    {
        return MoveResult::Invalid_SelfCheck;
    }

    // -----------------------------
    // Opponent-check: simulate move and switch turn to check opponent king
    // -----------------------------
    new_state_of_board = state_of_board;
    new_state_of_board[dest_index] = new_state_of_board[source_index];
    new_state_of_board[source_index] = EMPTY_SQUARE;

    if (turn == WHITE_TURN)
    {
        new_state_of_board[BOARD_STATE_LENGTH - 1] = BLACK_TURN;
    }
    else
    {
        new_state_of_board[BOARD_STATE_LENGTH - 1] = WHITE_TURN;
    }

    if (is_there_check(new_state_of_board))
    {
        return MoveResult::Valid_Check;
    }

    // -----------------------------
    // Check for opponent king existence -> if missing, checkmate
    // -----------------------------
    if (!get_is_white())
    {
        opponent_king = 'K';
    }
    else
    {
        opponent_king = 'k';
    }

    king_exists = false;
    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (new_state_of_board[i] == opponent_king)
        {
            king_exists = true;
        }
    }

    if (!king_exists)
    {
        return MoveResult::Valid_Checkmate;
    }
    else
    {
        return MoveResult::Valid;
    }
}

// --> pawn is move ok?
/*
^   * function name:        pawn::is_move_ok
---------------------------------------------------------------------------------------
    * function description: Determines if the proposed move for the Pawn piece is valid based on the current state of the chess board.
    * function input:       std::string state_of_board
        - state_of_board:   The current state of the chess board.
    * function output:      MoveResult
    * return value:         An enumeration value indicating the result of the move validation.
    * efficiency:           O(1) - constant time complexity (pawn moves are limited).
---------------------------------------------------------------------------------------
*/
MoveResult pawn::is_move_ok(std::string state_of_board, bool check_for_check)
{
    /*
        Valid = 0,
        Valid_Check = 1,
        Invalid_NoPieceAtSource = 2,
        Invalid_DestinationOccupiedByOwnPiece = 3,
        Invalid_SelfCheck = 4,
        Invalid_IndexOutOfRange = 5,
        Invalid_IllegalMovement = 6,
        Invalid_SameSourceAndDestination = 7,
        Valid_Checkmate = 8
    */

    std::string loc = give_location();
    std::string dest = get_destination();
    char turn = state_of_board[BOARD_STATE_LENGTH - 1];
    int source_index = 0;
    int dest_index = 0;
    int step = 0;
    int start_rank = 0;
    int middle_index = 0;
    bool dest_is_white = false;
    char dest_piece = '\0';
    int file_diff = 0;
    int rank_diff = 0;
    int i = 0;
    std::string new_state_of_board = state_of_board;
    char opponent_king = '\0';
    bool king_exists = false;

    // -----------------------------
    // Same source and destination
    // -----------------------------
    if (loc == dest)
    {
        return MoveResult::Invalid_SameSourceAndDestination;
    }

    // -----------------------------
    // Compute indices
    // -----------------------------
    source_index = (loc[RANK_OFFSET] - '1') * VERTICAL_STEP + (loc[FILE_OFFSET] - 'a');
    dest_index = (dest[RANK_OFFSET] - '1') * VERTICAL_STEP + (dest[FILE_OFFSET] - 'a');

    // -----------------------------
    // Index range check
    // -----------------------------
    if (source_index < 0 || source_index >= BOARD_SIZE || dest_index < 0 || dest_index >= BOARD_SIZE)
    {
        return MoveResult::Invalid_IndexOutOfRange;
    }

    // -----------------------------
    // Piece exists at source
    // -----------------------------
    if (state_of_board[source_index] == EMPTY_SQUARE)
    {
        return MoveResult::Invalid_NoPieceAtSource;
    }

    // -----------------------------
    // Turn check
    // -----------------------------
    if (!((turn == WHITE_TURN && get_is_white()) || (turn != WHITE_TURN && !get_is_white())))
    {
        return MoveResult::Invalid_IllegalMovement;
    }

    // -----------------------------
    // Destination occupied by own piece
    // -----------------------------
    if (state_of_board[dest_index] != EMPTY_SQUARE)
    {
        dest_piece = state_of_board[dest_index];
        dest_is_white = (dest_piece >= 'A' && dest_piece <= 'Z');

        if (dest_is_white == get_is_white())
        {
            return MoveResult::Invalid_DestinationOccupiedByOwnPiece;
        }
    }

    // -----------------------------
    // Pawn direction setup
    // -----------------------------
    if (get_is_white())
    {
        step = VERTICAL_STEP;
        start_rank = '2';
    }
    else
    {
        step = -VERTICAL_STEP;
        start_rank = '7';
    }

    file_diff = abs(loc[FILE_OFFSET] - dest[FILE_OFFSET]);
    rank_diff = (dest[RANK_OFFSET] - loc[RANK_OFFSET]);

    // -----------------------------
    // Forward move (no capture)
    // -----------------------------
    if (file_diff == 0)
    {
        // one step forward
        if (rank_diff == (step / VERTICAL_STEP))
        {
            if (state_of_board[dest_index] != EMPTY_SQUARE)
            {
                return MoveResult::Invalid_IllegalMovement;
            }
        }
        // two steps forward from starting rank
        else if (rank_diff == 2 * (step / VERTICAL_STEP) && loc[RANK_OFFSET] == start_rank)
        {
            middle_index = source_index + step;

            if (state_of_board[middle_index] != EMPTY_SQUARE || state_of_board[dest_index] != EMPTY_SQUARE)
            {
                return MoveResult::Invalid_IllegalMovement;
            }
        }
        else
        {
            return MoveResult::Invalid_IllegalMovement;
        }
    }
    // -----------------------------
    // Diagonal capture
    // -----------------------------
    else if (file_diff == 1 && rank_diff == (step / VERTICAL_STEP))
    {
        if (state_of_board[dest_index] == EMPTY_SQUARE)
        {
            return MoveResult::Invalid_IllegalMovement;
        }
    }
    else
    {
        return MoveResult::Invalid_IllegalMovement;
    }

    // -----------------------------
    // If not checking for check
    // -----------------------------
    if (!check_for_check)
    {
        return MoveResult::Valid;
    }

    // -----------------------------
    // Self-check (same turn)
    // -----------------------------
    new_state_of_board = state_of_board;
    new_state_of_board[dest_index] = new_state_of_board[source_index];
    new_state_of_board[source_index] = EMPTY_SQUARE;
    new_state_of_board[BOARD_STATE_LENGTH - 1] = turn;

    if (is_there_check(new_state_of_board))
    {
        return MoveResult::Invalid_SelfCheck;
    }

    // -----------------------------
    // Opponent check (switch turn)
    // -----------------------------
    new_state_of_board = state_of_board;
    new_state_of_board[dest_index] = new_state_of_board[source_index];
    new_state_of_board[source_index] = EMPTY_SQUARE;

    if (turn == WHITE_TURN)
    {
        new_state_of_board[BOARD_STATE_LENGTH - 1] = BLACK_TURN;
    }
    else
    {
        new_state_of_board[BOARD_STATE_LENGTH - 1] = WHITE_TURN;
    }

    if (is_there_check(new_state_of_board))
    {
        return MoveResult::Valid_Check;
    }

    // -----------------------------
    // Checkmate detection
    // -----------------------------
    if (!get_is_white())
    {
        opponent_king = 'K';
    }
    else
    {
        opponent_king = 'k';
    }
    king_exists = false;

    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (new_state_of_board[i] == opponent_king)
        {
            king_exists = true;
        }
    }

    if (!king_exists)
    {
        return MoveResult::Valid_Checkmate;
    }
    else
    {
        return MoveResult::Valid;
    }
}
