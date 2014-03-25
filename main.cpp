/* --------------- AUTHORS ---------------
- David Pérez (hallaplay835@gmail.com).
- Iván Calvo (calvuxi@gmail.com).
*/

// TO DO
// - declarar constantes a modo de alias sobre los char usados para imprimir el tablero
// - chcp?
// - BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY

// #### Libraries ####
#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
#include <iomanip>
#include <climits>
#include <cmath>

using namespace std;
typedef unsigned short int usi;

// #### Constants ####
const string PROMPT = "", INVALID_TYPE = "ERROR. Invalid input data type.", ATTEMPTS = "Attempts", SCORE = "Score";
const usi DIM = 5, MIN_USER_LENGTH = 5, MAX_USER_LENGTH = 20, MAX_TRIES = 30, CHIP_WIDTH = 8, LEFT_PADDING = 3;
const string HELP_FILE = "help.txt", USER_FILE = "users.txt";
const bool DEBUG = false;

// #### typedef declarations ####
typedef enum tChip {magenta, yellow, blue, green, none};
typedef enum tDirection {moveup, movedown, moveleft, moveright}; // We all love One Direction

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
usi countDigits(unsigned int n);

usi menu();
tGame newGame(usi dim = DIM, usi max_tries = MAX_TRIES);
void genBoard(tBoard &board);

bool inBounds(const tGame& game, int row, int col);
void conductGame(tGame& game);
bool getValidMove(const tGame& game, int& row, int& col, tDirection& dir);
bool testGroup(const tGame& game, int row, int col, tDirection dir, tChip color);
bool testValidMove(const tGame& game, int row, int col, tDirection dir);
void processTurn(tGame& game);
bool seekAndDestroy(tGame& game);
bool drop(tGame& game);
void dropLoop(tGame& game);

bool promptAction(int dim, int& row, int& col, tDirection& dir);
bool pausar();

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

	return (i == opt1);
}

/** Returns the number of digits of an unsigned integer. **/
usi countDigits(unsigned int n) {
	unsigned int dig = 1;
	n /= 10;
	while (n > 0) {
		dig += 1;
		n /= 10;
	}
	return dig;
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

	// Computes leading whitespace to center header.
	// left_padding + space + chips + separators.
	usi x = (left_padding + 1 + chip_width * game.board.dim + game.board.dim + 1);
	// ATTEMPTS + ": " + score + " / " + SCORE + ": " + tries.
	usi y = ATTEMPTS.length() + 2 + countDigits(game.score) + 3 + SCORE.length() + 2 + countDigits(game.tries);
	usi center_space = 0;
	if (x > y) {
		center_space = (x - y) / 2;
	}
	cout << setw(center_space) << ' ';
	cout << ATTEMPTS << ": " << game.tries << " / " << SCORE << ": " << game.score << endl;

	if (game.board.dim > 9 && chip_width == 1) chip_width++; // Cannot print two-digit numbers under a char without not leaving spaces.

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

	// X-axis labels:
	usi margin = chip_width / 2;
	usi cw_parity = chip_width % 2; // 0 is even, 1 is odd.
	cout << setw(left_padding + 2 + margin - (cw_parity == 0 ? 1 : 0)) << ' '; // Centers labels depending on parity of chip_width.
	// Labels are left-aligned if they cannot be centered.
	for (usi i = 1; i <= game.board.dim; i++) {
		cout << i << setw(2 * margin + cw_parity - (countDigits(i) - 1)) << ' ';
	}

}

/** Displays a row. **/
void printRow(usi i, tBoard &board, usi left_padding, usi chip_width) {
	cout << setw(left_padding) << board.dim - i << " ";
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
	tColor resultado;
	switch (chip) {
	case magenta:
		resultado = dark_magenta;
		break;
	case yellow:
		resultado = dark_yellow;
		break;
	case blue:
		resultado = dark_cyan;
		break;
	case green:
		resultado = dark_green;
		break;
	default:
		black;
	}
	return resultado;
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

void conductGame(tGame& game) {
	bool canceled = false;
	int row, col, row2, col2;
	tDirection dir;
	tChip aux;
	while (!canceled && game.tries > 0) {
		canceled = getValidMove(game, row, col, dir);
		if (!canceled) {
			switch(dir) {
				case moveup:
					row2 = row -1;
					col2 = col;
					break;
				case movedown:
					row2 = row +1;
					col2 = col;
					break;
				case moveleft:
					row2 = row;
					col2 = col -1;
					break;
				case moveright:
					row2 = row;
					col2 = col +1;
					break;
			}
			aux = game.board.a[row][col];
			game.board.a[row][col] = game.board.a[row2][col2];
			game.board.a[row2][col2] = aux;
		}
		processTurn(game);
	}
}

bool getValidMove(const tGame& game, int& row, int& col, tDirection& dir) {
	bool canceled = promptAction(game.board.dim, row, col, dir);
	if (!canceled) {
		col = col -1;
		row = game.board.dim - row;
		bool valid = testValidMove(game, row, col, dir);
		while (!valid && !canceled) {
			cout << "That's not valid, dude. ";
			canceled = promptAction(game.board.dim, row, col, dir);
			if (!canceled) valid = testValidMove(game, row, col, dir);
		}
	}
	return canceled;
}

bool testGroup(const tGame& game, int row, int col, tDirection dir, tChip color) {
	int w = 0, a = 0, s = 0, d = 0;
	bool found = false;
	bool inBounds = true;
	while (!found && inBounds && dir != moveup) {
		inBounds = (row-(w+1) >= 0);
		if (inBounds) {
			found = (color == game.board.a[row-(w+1)][col]);
			if (!found) w++;
		}
	}
	found = false;
	inBounds = true;
	while (!found && inBounds && dir != moveleft) {
		inBounds = (col-(a+1) >= 0);
		if (inBounds) {
			found = (color == game.board.a[row][col-(a+1)]);
			if (!found) a++;
		}
	}
	found = false;
	inBounds = true;
	while (!found && inBounds && dir != movedown) {
		inBounds = (row+(s+1) < game.board.dim);
		if (inBounds) {
			found = (color == game.board.a[row+(s+1)][col]);
			if (!found) s++;
		}
	}
	found = false;
	inBounds = true;
	while (!found && inBounds && dir != moveright) {
		inBounds = (col+(d+1) < game.board.dim);
		if (inBounds) {
			found = (color == game.board.a[row][col+(d+1)]);
			if (!found) d++;
		}
	}

	return (a+d+1 >= 3)||(w+s+1 >= 3);
}

bool testValidMove(const tGame& game, int row, int col, tDirection dir) {
	bool result;
	switch (dir) {
	case moveup:
		result = inBounds(game, row, col) && inBounds(game, row-1, col);
		if (result) {
			result = testGroup(game, row, col, moveup, game.board.a[row-1][col])
						|| testGroup(game, row, col, movedown, game.board.a[row][col]);
		}
		break;
	case movedown:
		result = inBounds(game, row, col) && inBounds(game, row+1, col);
		if (result) {
			result = testGroup(game, row, col, movedown, game.board.a[row+1][col])
						|| testGroup(game, row, col, moveup, game.board.a[row][col]);
		}
		break;
	case moveleft:
		result = inBounds(game, row, col) && inBounds(game, row, col-1);
		if (result) {
			result = testGroup(game, row, col, moveleft, game.board.a[row][col-1])
						|| testGroup(game, row, col, moveright, game.board.a[row][col]);
		}
		break;
	case moveright:
		result = inBounds(game, row, col) && inBounds(game, row, col+1);
		if (result) {
			result = testGroup(game, row, col, moveright, game.board.a[row][col+1])
						|| testGroup(game, row, col, moveleft, game.board.a[row][col]);
		}
	}
	return result;
}

bool inBounds(const tGame& game, int row, int col) {
	return row >= 0 && row < game.board.dim && col >= 0 && col < game.board.dim;
}

void processTurn(tGame& game) {
	bool stabilized = seekAndDestroy(game);
	while (!stabilized) {
		dropLoop(game);
		stabilized = seekAndDestroy(game);
	}
}

