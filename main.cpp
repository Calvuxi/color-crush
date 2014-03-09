/* --------------- AUTHORS ---------------
- David Pérez (hallaplay835@gmail.com).
- Iván Calvo (calvuxi@gmail.com).
*/

// #### Libraries ####
#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <ctime>
#include <string>
// #include <fstream>
#include <iomanip>
// #include <climits>
// #include <cmath>

using namespace std;
typedef unsigned short int usi;

// #### Constants ####
const string PROMPT = "", INVALID_TYPE = "ERROR. Invalid input data type.";
const usi DIM = 8, MIN_USER_LENGTH = 5, MAX_USER_LENGTH = 20, MAX_TRIES = 30, CHIP_WIDTH = 3, LEFT_PADDING = 2;
// const string HELP_FILE = "help.txt", USER_FILE = "users.txt";
const bool DEBUG = false;

// #### typedef declarations ####
typedef enum tChip {magenta, yellow, blue, green, none};

typedef struct {
	tChip a[DIM][DIM];
	usi dim;
} tBoard;

typedef struct {
	tBoard board;
	usi tries;
	int score;
} tGame;

typedef enum tColor {
	black, dark_blue, dark_green, dark_cyan, dark_red,
	dark_magenta, dark_yellow, light_gray, dark_gray,
	light_blue, light_green, light_cyan, light_red,
	light_magenta, light_yellow, white
};

// #### Prototypes ####
void pause();

int readInt(string ERR_MSG = INVALID_TYPE, int m = INT_MIN, int n = INT_MAX);
bool readBool(string prompt, string opt1, string opt2);

usi menu();
tGame newGame(usi dim = DIM, usi max_tries = MAX_TRIES);
void genBoard(tBoard &board);

void displayGame(tGame &game, usi chip_width = CHIP_WIDTH, usi left_padding = LEFT_PADDING);
void printRow(usi i, tBoard &board, usi left_padding = LEFT_PADDING, usi chip_width = CHIP_WIDTH);
void printSeparator(usi dim = DIM, usi left_padding = LEFT_PADDING, usi chip_width = CHIP_WIDTH);
void printFLSeparator(bool first, usi dim = DIM, usi left_padding = LEFT_PADDING, usi chip_width = CHIP_WIDTH);
void printChip(tChip chip, usi chip_width = CHIP_WIDTH);

void setColor(tColor color);
tColor tChipTotColor(tChip chip);
string getUserName();

// #### main() ####
int main() {

	// Initialize seed:
	srand((unsigned int)time(NULL));
	
	// Default config:
	usi max_tries = MAX_TRIES;
	usi chip_width = CHIP_WIDTH;
	usi dim = DIM;

	// Ask for and validate a username:
	/*string user = getUserName();
	cout << "¡Hello " << user << "! Choose an option..." << endl;*/

	// Display the menu and read an option:
	usi opt = menu();
	while (opt != 0) {
		cout << endl;
		switch (opt) {
		case 1:
			tGame game = newGame(dim, max_tries);
			displayGame(game, chip_width);

			break;
		}

		// Display the menu and read another option:
		opt = menu();
	}

	return 0;
}

// #### UDFs ####

/** Pauses execution until Intro is pressed. **/
void pause() {
	cin.clear();
	cin.sync();
	cout << "Press Intro to continue...";
	cin.ignore(INT_MAX, '\n');
}

/** Reads and returns an integer between m and n (inclusive). **/
int readInt(string ERR_MSG, int m, int n) {
	int input;
	cin >> input;
	while (cin.fail() || input < m || input > n) {
		cin.clear(); cin.sync();
		cin.ignore(INT_MAX, '\n');
		cout << ERR_MSG;
		cin >> input;
	}
	cin.sync();

	return input;
}

/** Reads and returns a boolean, displaying the prompt and opt1 and opt2 as options. **/
bool readBool(string prompt, string opt1, string opt2) {
	cout << prompt << " (" << opt1 << "/" << opt2 << "): ";

	string i;
	cin >> i;
	while (cin.fail() || !(i == opt1 || i == opt2)) { // Validate user input.
		cin.clear(); cin.sync();
		cout << "ERROR. " << prompt << " (" << opt1 << "/" << opt2 << "): ";
		cin >> i;
	}
	cin.sync();

	if (i == opt1) return true; else return false;
}

/** Displays the menu and returns an usi representing user selection. **/
usi menu() {
	cout << endl;
	cout << "--- MENU ---";
	cout << endl << endl;
	cout << "1 - Play." << endl;
	cout << "0 - Exit." << endl;
	cout << "Option: ";
	return (usi)readInt("Invalid option. Option: ", 0, 1);
}

/** Initialises a game. **/
tGame newGame(usi dim, usi max_tries) {
	tGame game;
	game.score = 0;
	game.tries = max_tries;
	game.board.dim = dim;
	genBoard(game.board);
	return game;
}

/** Generates a random board. **/
void genBoard(tBoard &board) {
	for (usi i = 0; i < board.dim; i++) {
		for (usi j = 0; j < board.dim; j++) {
			board.a[i][j] = (tChip)(rand() % 4);
		}
	}
}

/** Displays the board, tries and score of game. **/
void displayGame(tGame &game, usi chip_width, usi left_padding) {
	cout << right << setw(20) << "Attempts: " << game.tries << "  Score: " << game.score << endl;

	// First separator:
	printFLSeparator(true, game.board.dim, left_padding, chip_width);

	// Board:
	for (usi i = 0; i < game.board.dim - 1; i++) {
		
		// Row:
		printRow(i, game.board, left_padding, chip_width);
		
		// Separator:
		printSeparator(game.board.dim, left_padding, chip_width);
	}

	// Last row:
	printRow(game.board.dim - 1, game.board, left_padding, chip_width);

	// Last separator:
	printFLSeparator(false, game.board.dim, left_padding, chip_width);

}

/** Displays a row. **/
void printRow(usi i, tBoard &board, usi left_padding, usi chip_width) {
	cout << setw(left_padding) << i + 1 << " ";
	cout << char(179);
	for (usi j = 0; j < board.dim; j++) {
		printChip(board.a[i][j], chip_width);
		cout << char(179);
	}
	cout << endl;
}

/** Displays a common separator between rows. **/
void printSeparator(usi dim, usi left_padding, usi chip_width) {
	cout << setw(left_padding + 1) << " ";
	cout << char(195);
	for (usi j = 0; j < dim - 1; j++) {
		for (usi k = 1; k <= chip_width; k++) cout << char(196);
		cout << char(197);
	}
	for (usi k = 1; k <= chip_width; k++) cout << char(196);
	cout << char(180);
	cout << endl;
}

/** Displays first/last separator of the grid surrounding the board. **/
void printFLSeparator(bool first, usi dim, usi left_padding, usi chip_width) {
	cout << setw(left_padding + 1) << " ";
	cout << (first ? char(218) : char (192));
	for (usi i = 0; i < dim - 1; i++) {
		for (usi j = 1; j <= chip_width; j++) cout << char(196);
		cout << (first ? char(194) : char(193));
	}
	for (usi j = 1; j <= chip_width; j++) cout << char(196);
	cout << (first ? char(191) : char(217));

	cout << endl;
}

/** Displays a chip. **/
void printChip(tChip chip, usi chip_width) {
	setColor(tChipTotColor(chip));
	for (usi k = 1; k <= chip_width; k++) cout << char(219);
	setColor(light_gray);
}

/** Sets current printing color. **/
void setColor(tColor color) {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, color);
}

/** Returns appropiate printing color of a chip. **/
tColor tChipTotColor(tChip chip) {
	if (chip == magenta) {
		return dark_magenta;
	} else if (chip == yellow) {
		return dark_yellow;
	} else if (chip == blue) {
		return dark_cyan;
	} else if (chip == green) {
		return dark_green;
	} else { // chip == none.
		return black;
	}
}

/** Asks for and returns a username.
 ** Username must be a single word with length between MIN_USER_LENGTH and MAX_USER_LENGTH chars. **/
string getUserName() {
	string user;
	cout << "Welcome to Color Crush. Please, input a username: ";
	getline(cin, user);
	while (user.length() < MIN_USER_LENGTH || user.find(" ") != -1 || user.length() > MAX_USER_LENGTH) {

		// Distuinguish between error cases.
		if (user.length() < MIN_USER_LENGTH) {
			cout << "Usernames cannot have less than " << MIN_USER_LENGTH << " characters." << endl;
		} else if (user.find(" ") != -1) {
			cout << "Usernames cannot contain spaces." << endl;
		} else {
			cout << "Usernames cannot have more than " << MAX_USER_LENGTH << " characters." << endl;
		}
		cout << "Username: ";
		getline(cin, user);
	}
	return user;
}