#ifndef ASSIGN2_GAMEENGINE_H
#define ASSIGN2_GAMEENGINE_H

#include <string>
#include <random>
#include "Tile.h"
#include "LinkedList.h"
#include "Position.h"
#include "Player.h"

#define BOARD_SIZE  26
#define MAX_PLAYERS 12

class GameEngine
{
public:
	/*
	 * Initialises the GameEngine's state from scratch, and then 
	 * begins the game
	 */
	void newGame();

	~GameEngine();

	/*
	 * Requests a file name from the user, uses the contents
	 * to initialise the game state and then continues with the
	 * game. If the file does not exist or the format is invalid,
	 * then the user is asked for a different file name until a 
	 * correct one is inputted or they quit with Ctrl D. Returns
	 * whether the load was successful or not
	 */
    bool loadGame();

	/*
	 * Moves the tiles of the board to the right, down, left-most, or
	 * right-most of the board when needed.
	 */
	void adjustBoard(bool shrinkBoard);

	/*
	 * Contains the main game loop, performing each player's turn
	 * until the game ends
	 */
	void runGame();

	/*
	 * Prompts the player for input, then processes and validates the
	 * supplied command, calling corresponding function if the input
	 * is valid. If the input is invalid, the player is re-prompted for
	 * input until valid input is supplied
	 */
    void getInput();

	/*
	 * Returns a string that contains the formatted representation of the
	 * board with a size appropriate for the number of tiles in the board
	 * or a full 26 * 26 board for saving the game.
	 */
	std::string boardToString(bool colouredOutput, bool fullBoard);

    /*
     * Prints out the details of the current game state as specified in
	 * the assignment specification
     */
    void displayGameState();

	/*
	 * Completes the AI's turn
	 */
	void processAITurn();

	/*
	 * Tests if the specified tile can be placed at the specified postition,
	 * return the resulting score if it can and returning 0 if it can't
	 */
	int testPlacement(Tile* tile, Position position, bool& qwirkle);

    /*
     * Attempts to remove the specified tile from the current player's 
	 * hand and place it on the board in the specified postion, calculating
	 * and adding points to the current player's total if the placement is 
	 * successful, and then adding a new tile to the player's hand from the
	 * bag. If the tile is not contained within the player's hand or the 
	 * placement is invalid, then the function returns false
     */
    bool placeTile(std::string tileLabel, std::string positionLabel);

	/*
	 * Attemps to remove the specified tile from the current player's hand,
	 * place it back in the bag, and add a new tile from the bag to the
	 * player's hand. If the tile is not contained within the player's hand
	 * then the function returns false
	 */
    bool replaceTile(std::string tileLabel);

	/*
	 * Opens the specified file and saves within it the current game state
	 * using the format specified in the assignment specification. Returns
	 * whether the save was successful
	 */
    bool saveGame(std::string fileName);


private:
	//2D array of tiles as board 26*26
    Tile*** board;
    LinkedList tileBag;
	std::vector<Player*> players;
	int playerTurnIndex;

	// Used for AI choice generation
	std::uniform_real_distribution<> choiceDistributions[3] = { 
		std::uniform_real_distribution<>(0, 0.3) , 
		std::uniform_real_distribution<>(0.3, 0.6) , 
		std::uniform_real_distribution<>(0.6, 1) };

	int viewX;
	int viewY;
	
	bool firstTile;
	bool exitGame;
};

struct Placement
{
	Placement(Tile* tile, int x, int y, int score, bool qwirkle)
		: tile{ tile }, x{ x }, y{ y }, score{ score }, qwirkle{ qwirkle }{}
	
	static bool compare(Placement& p1, Placement& p2)
	{
		return p1.score < p2.score;
	}

	Tile* tile;
	int x;
	int y;
	int score;
	bool qwirkle;
};

#endif // ASSIGN2_GAMEENGINE_H