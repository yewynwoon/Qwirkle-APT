#include "GameEngine.h"
#include "getline.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <regex>

using std::cout;
using std::endl;
using std::string;

void GameEngine::newGame()
{
    exitGame = false;
	player1Turn = true;
	firstTile = true;
	versingAI = false; // Need to be changed
	string player1Name;
	string player2Name;
	//regex to ensure player name is only uppercase alphabets
	std::regex r("[a-zA-Z]+");
	std::smatch m;
	cout << "Starting a new game" << endl;

	//prompts the user again if the given name does not follow the regex
	while (!std::regex_search(player1Name, m, r))
	{
		cout << "Enter a name for player 1 (no numbers or symbols)" << endl;
		getline(std::cin, player1Name);
	}
	player1 = new Player(player1Name);

	while (!std::regex_search(player2Name, m, r))
	{
		cout << "Enter a name for player 2 (no numbers or symbols)" << endl;
		getline(std::cin, player2Name);
	}
	player2 = new Player(player2Name);

	//create an array to store all the colours
	char colours[6] = { 'R','O','Y','G','B','P' };
	//create a temporary linked list
	LinkedList temp;
	int shapes = 6;
	//fill the temporary linked list with two of every tile
	for (int i = 0; i < 6; i++)
	{
	    for (int j = 1; j <= shapes; j++)
	    {
	        temp.add_back(new Tile(colours[i], j));
			temp.add_back(new Tile(colours[i], j));
	    }
	}

	std::random_device rd; // obtain a random number from hardware
	std::mt19937 eng(rd()); // seed the generator

	// randomly add tiles
	for (int i = 72; i > 0; i--)
	{
		std::uniform_int_distribution<> distr(0, i - 1);
		tileBag.add_back(temp.removeAt(distr(eng)));
	}
	//distribute six tiles to each player
	for (int i = 0; i < 6; ++i)
	{
		player1->hand.add_back(tileBag.pop_front());
	}
	for (int i = 0; i < 6; ++i)
	{
		player2->hand.add_back(tileBag.pop_front());
	}

	// Creates the empty board
	board = new Tile**[BOARD_SIZE];
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		board[i] = new Tile*[BOARD_SIZE];
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			board[i][j] = nullptr;
		}
	}
	cout << "Let's Play!" << endl;
	runGame();
}

bool GameEngine::loadGame()
{
    exitGame = false;
	std::ifstream file;
	string input;
	bool valid = true;

	cout << "\nEnter the filename from which to load a game\n> ";

	getline(std::cin, input);
	file.open(input);

	//Check if file exists
	if (file)
	{
		std::regex r("[a-zA-Z]+");
		std::smatch m;
		//Checks both player's information
		for (int i = 0; i < 2 && valid; i++)
		{
			Player* player = nullptr;
			getline(file, input);
			if (!file.eof() && std::regex_search(input, m, r))
			{
				player = new Player(input);
			}
			else valid = false;
			if (valid)
			{
				getline(file, input);
				std::istringstream iss(input);
				int score;
				iss >> score;
				if (!file.eof() && !iss.bad()) player->score = score;
				else valid = false;
			}
			if (valid)
			{
				getline(file, input);
				if (!file.eof())
				{
					int tileCount = 0;
					for (unsigned int i = 0; i < input.size() && valid; i += 3)
					{
						Tile* tile = Tile::stringToTile(input[i], input[i + 1]);
						if (tile == nullptr) valid = false;
						else
						{
							player->hand.add_back(tile);
							++tileCount;
						}
						if (tileCount > 6) valid = false;
					}
				}
				else valid = false;
			}
			// Checks if player 2 is an AI
			if (valid && i == 1)
			{
				getline(file, input);
				versingAI = false;
				if (!file.eof())
				{
					if (input == "EASY")
					{
						versingAI = true;
						AIDifficulty = EASY;
					}
					if (input == "MEDIUM")
					{
						versingAI = true;
						AIDifficulty = MEDIUM;
					}
					if (input == "HARD")
					{
						versingAI = true;
						AIDifficulty = HARD;
					}
					else if (input != "HUMAN") valid = false;
				}
				else valid = false;
			}
			if (i == 0)
			{
				player1 = player;
			}
			else player2 = player;
		}
		if (valid)
		{
			//Initialize the board
			board = new Tile * *[BOARD_SIZE];
			for (int i = 0; i < BOARD_SIZE; ++i)
			{
				board[i] = new Tile * [BOARD_SIZE];
				for (int j = 0; j < BOARD_SIZE; j++)
				{
					board[i][j] = nullptr;
				}
			}
			getline(file, input);

			if (!file.eof() && input == "   0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25")
			{
				getline(file, input);
				if (file.eof() || input != "  -------------------------------------------------------------------------------") valid = false;
			}
			else valid = false;
			if (valid)
			{
				firstTile = true;
				string rowLabel = "A |";
				for (int i = 0; i < BOARD_SIZE && valid; i++)
				{
					getline(file, input);
					if (file.eof() || input.substr(0, 3) != rowLabel || input.size() > (3 + (BOARD_SIZE * 3))) valid = false;
					//Check all tiles in board are valid
					for (unsigned int j = 3; j < input.size() && valid; j += 3)
					{
						if (input.substr(j, 3) != "  |")
						{
							Tile* tile = Tile::stringToTile(input[j], input[j + 1]);
							if (tile == nullptr) valid = false;
							else
							{
								firstTile = false;
								board[((j - 3) / 3)][i] = tile;
							}
						}
					}
					//Update row label to next letter
					++rowLabel[0];
				}
			}
		}
		if (valid)
		{
			getline(file, input);
			if (!file.eof())
			{
				for (unsigned int i = 0; i < input.size() && valid; i += 3)
				{
					Tile* tile = Tile::stringToTile(input[i], input[i + 1]);
					if (tile == nullptr) valid = false;
					else
					{
						tileBag.add_back(tile);
					}
				}
			}
			else valid = false;
		}
		if (valid)
		{
			getline(file, input);
			//Checks if this is the eof rather than not eof
			if (file.eof())
			{
				
				if (input == player1->name)
				{
					player1Turn = true;
				}
				else if (input == player2->name) player1Turn = false;
				else valid = false;
			}
			else valid = false;
		}

	}
	else valid = false;
	if (valid)
	{
		cout << "\nQwirkle game successfully loaded" << endl;
		runGame();

	}
	return valid;
}

void GameEngine::runGame()
{
	while (!exitGame)
	{
		displayGameState();
		if (!player1Turn && versingAI) processAITurn();
		else getInput();
		player1Turn = !player1Turn;
	}
}

void GameEngine::getInput()
{
	bool valid = false;
    while (!valid && !exitGame)
	{
		cout << "> ";
		string input;
		getline(std::cin, input);

		// Causes the game to exit if eof is encountered
		if (std::cin.eof())
		{
			exitGame = true;
			valid = true;
		}

		if (input.size() != 0)
		{
			std::vector<string> commands;
			std::istringstream iss(input);
			string word;

			// Splits the input line into individual words
			// and puts them into a vector
			while (iss >> word) commands.push_back(word);

			// Checks whether the correct number of words was inputted
			if (commands.size() == 1)
			{
				if (commands[0] == "quit")
				{
					valid = true;
					exitGame = true;
				}
			}
			else if (commands.size() == 2)
			{
				if (commands[0] == "replace")
				{
					// Input is valid if the action was successful
					valid = replaceTile(commands[1]);
				}
				else if (commands[0] == "save")
				{
					// Input is valid if the action was successful
					valid = saveGame(commands[1]);
				}
			}
			else if (commands.size() == 4)
			{
				if (commands[0] == "place" && commands[2] == "at")
				{
					// Input is valid if the action was successful
					valid = placeTile(commands[1], commands[3]);
				}
			}
		}

		if (!valid) cout << "\nInvalid input" << endl;
	}
}

string GameEngine::boardToString(bool colouredOutput)
{
	std::ostringstream output;
	output << "   ";

	Tile* tile = nullptr;
	for (int header = 0; header < BOARD_SIZE; header++)
	{
		output << header << " ";
		if (header < 10) output << " ";
	}

	output << "\n  -";
	for (int dash = 0; dash < BOARD_SIZE; dash++)
	{
		output << "---";
	}

	for (int y = 0; y < BOARD_SIZE; y++)
	{
		char ch = 'A' + y;
		output << "\n" << ch << " |";

		for (int x = 0; x < BOARD_SIZE; x++)
		{
			tile = board[x][y];
			if (tile == nullptr) {
				output << "  |";
			}
			else
			{
					output << tile->getLabel(colouredOutput) <<"|";
			}
		}
	}

	return output.str();
}

void GameEngine::displayGameState()
{
	Player* player = player1Turn ? player1 : player2;

	cout << "\n" << player->name << ", it's your turn" << endl;

	cout << "Score for " << player1->name << ": " << player1->score << endl;
	cout << "Score for " << player2->name << ": " << player2->score << endl;

	cout << boardToString(true);

	cout << "\n\nYour hand is" << endl;
	cout << player->hand.display(true) << endl;
}

void GameEngine::processAITurn()
{
	std::vector<Placement> validPlacements;
	std::vector<Tile*> hand = player2->hand.toVector();

	// Finds all valid placements
	for (int x = 0; x < BOARD_SIZE; ++x)
	{
		for (int y = 0; y < BOARD_SIZE; ++y)
		{
			if (board[x][y] != nullptr)
			{
				for (Tile* tile : hand)
				{
					bool qwirkle = false;
					int score = testPlacement(tile, Position(x, y), qwirkle);
					if (score > 0)
					{
						validPlacements.push_back(Placement(tile, x, y, score, qwirkle));
					}
				}
			}
		}
	}

	// Sorts placements in terms of score
	sort(validPlacements.begin(), validPlacements.end(), Placement::compare);

	// Randomely picks from validPlacements from a distribution based on the AIDifficulty
}

int GameEngine::testPlacement(Tile* tile, Position position, bool& qwirkle)
{
	int score = 0;
	if (board[position.x][position.y] == nullptr)
	{
		// Indicates whether the surrounding tiles allow the placement
		// of the supplied tile
		bool valid = true;
		Position offsets[4];

		// Translation up
		offsets[0].x = 0;
		offsets[0].y = -1;

		// Translation right
		offsets[1].x = 1;
		offsets[1].y = 0;

		// Translation down
		offsets[2].x = 0;
		offsets[2].y = 1;

		// Translation left
		offsets[3].x = -1;
		offsets[3].y = 0;

		// Indicates whether the corresponding offset tile is a valid connection
		bool connected[4] = { false, false, false, false };

		// Indicates each dimension's similarity type (colour or shape)
		bool verColourSimilarity = false;
		bool horColourSimilarity = false;

		for (int i = 0; i < 4 && valid; ++i)
		{
			Position offsetPosition = position + offsets[i];
			if (offsetPosition.x < BOARD_SIZE && offsetPosition.x >= 0
				&& offsetPosition.y < BOARD_SIZE && offsetPosition.y >= 0)
			{
				Tile* currTile = board[offsetPosition.x][offsetPosition.y];

				if (currTile != nullptr)
				{
					// Checks whether the two tiles only have one type of similarity
					if ((tile->colour == currTile->colour) !=
						(tile->shape == currTile->shape))
					{
						connected[i] = true;
						// If there are two connected segments in the current dimension
						if (i >= 2 && connected[i - 2])
						{
							// If the opposite tile has a different similarity type
							if ((i == 2 ? verColourSimilarity : horColourSimilarity)
								!= (tile->colour == currTile->colour))
							{
								valid = false;
							}
						}
						else
						{
							// Sets the current dimension's similarity type
							(i % 2 == 0 ? verColourSimilarity : horColourSimilarity)
								= tile->colour == currTile->colour;
						}
					}
					else
					{
						valid = false;
					}
				}
			}
		}

		if (valid && (connected[0] || connected[1] || connected[2] || connected[3]))
		{
			// Indicates which elements of the similarity type have been found
			// in each dimension
			std::unordered_set<int> verTypeSet;
			std::unordered_set<int> horTypeSet;
			verTypeSet.insert(verColourSimilarity ? tile->shape : tile->colour);
			horTypeSet.insert(horColourSimilarity ? tile->shape : tile->colour);

			for (int i = 0; i < 4 && valid; ++i)
			{
				if (connected[i])
				{
					// Indicates the current dimension
					bool vertical = i % 2 == 0;

					// The type set for the current dimension
					std::unordered_set<int>& typeSet =
						(vertical ? verTypeSet : horTypeSet);

					// Indicates the current dimension's similarity type
					bool colorSimilarity =
						(vertical ? verColourSimilarity : horColourSimilarity);

					Position currPosition = position + offsets[i];
					Tile* currTile;
					bool empty = false;
					while (valid && currPosition.x < BOARD_SIZE && currPosition.x >= 0
						&& currPosition.y < BOARD_SIZE && currPosition.y >= 0 && !empty)
					{
						currTile = board[currPosition.x][currPosition.y];
						if (currTile != nullptr)
						{
							// Used to check for duplicate tiles in the segment
							int signature =
								(colorSimilarity ? currTile->shape : currTile->colour);

							// If a tile is found that is already contained 
							// within the segment
							if (typeSet.count(signature) != 0)
							{
								score = 0;
								valid = false;
							}
							else
							{
								typeSet.insert(signature);
								++score;
							}
							currPosition += offsets[i];
						}
						else empty = true;
					}
				}
			}

			if (valid)
			{
				// Adds 1 point for the tile itself
				++score;
				
				// If tile is part of a vertical and horizontal
				// segment score is increased by 1
				if (verTypeSet.size() > 1 && horTypeSet.size() > 1)
				{
					++score;
				}

				// Qwirkle checking
				if (verTypeSet.size() == 6)
				{
					score += 6;
					qwirkle = true;
				}
				if (horTypeSet.size() == 6)
				{
					score += 6;
					qwirkle = true;
				}
			}
		}
	}
	return score;
}

bool GameEngine::placeTile(string tileLabel, string positionLabel)
{
	bool success = false;
   	Player* player = player1Turn ? player1 : player2;
	Position* position;
	Tile* tile = player->hand.find(tileLabel);
	// Tile found in player's hand
	if (tile != nullptr)
	{
		position = Position::labelToPosition(positionLabel);
		// Position is within board bounds
		if(position != nullptr)
		{
			if (firstTile)
			{
				++player->score;
				player->hand.remove(tile);
				board[position->x][position->y] = tile;
				Tile* newTile = tileBag.pop_front();
				if (newTile != nullptr)
				{
					player->hand.add_back(newTile);
				}
				firstTile = false;
				success = true;
			}
			else
			{
				bool qwirkle = false;
				int score = testPlacement(tile, *position, qwirkle);

				// If placement was successful
				if (score > 0)
				{
					if (qwirkle) cout << "QWIRKLE!!!" << endl;

					// Score updating and tile placement
					player->score += score;
					player->hand.remove(tile);
					board[position->x][position->y] = tile;

					// Tile replenishment
					Tile* newTile = tileBag.pop_front();
					if (newTile != nullptr)
					{
						player->hand.add_back(newTile);
					}

					// Game over checking
					if (player->hand.isEmpty())
					{
						cout << "\nGame Over" << endl;
						cout << "Score for " << player1->name << ": " << player1->score << endl;
						cout << "Score for " << player2->name << ": " << player2->score << endl;
						if (player1->score > player2->score)
						{
							cout << "Player " << player1->name << " won!" << endl;
						}
						else if (player2->score > player1->score)
						{
							cout << "Player " << player2->name << " won!" << endl;
						}
						else cout << "Draw..." << endl;
						exitGame = true;
					}
					success = true;
				}
				delete position;
			}
		}
	}
	return success;
}

bool GameEngine::replaceTile(string tileLabel)
{
    Player* player = player1Turn ? player1 : player2;
    Tile* tile = player->hand.find(tileLabel);
    bool replaced = false;

    if (tile != nullptr)
    {
		if (!tileBag.isEmpty())
		{
			Tile* newTile = tileBag.pop_front();
            player->hand.remove(tile);
			tileBag.add_back(tile);
			player->hand.add_back(newTile);
			replaced = true;
		}
		else
        {
		    cout << "Tile Bag is empty!" << endl;
        }
    }
    return replaced;
}

bool GameEngine::saveGame(string fileName)
{
	Player* player = player1Turn ? player1 : player2;
	std::ofstream outFile(fileName);
	outFile.open(fileName, std::ofstream::app);
	outFile << player1->name << endl;
	outFile << player1->score << endl;
	outFile << player1->hand.display(false) << endl;
	outFile << player2->name << endl;
	outFile << player2->score << endl;
	outFile << player2->hand.display(false) << endl;
	if (versingAI)
	{
		if (AIDifficulty == EASY) outFile << "EASY" << endl;
		else if (AIDifficulty == MEDIUM) outFile << "MEDIUM" << endl;
		else outFile << "HARD" << endl;
	}
	else outFile << "HUMAN" << endl;
	outFile << boardToString(false) << endl;
	outFile << tileBag.display(false) << endl;
	outFile << player->name << endl;
	outFile.close();
	return true;
}
