#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <cstdlib.h>


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_CYAN    "\x1b[36m"

#define ANSI_COLOR_RESET   "\x1b[0m"
#define BOARD_ROWS 8
#define BOARD_COLS 8


typedef struct PlayerScore {
    int score;
    char piece;
    char *playerName;
} scores;

char greetingLines(char opt);
void setSymbol(scores *playerOne, char playerOnePiece);
void updatePlayerScore(scores *scoresPtr, char *playerName, int playerScore);
void printBoard(char *board);
void printGameRules();
int takeTurn(char *board, int player, const char*);
int checkWin(char *board);
int checkFour(char *board, int, int, int, int);
int horizontalCheck(char *board);
int verticalCheck(char *board);
int diagonalCheck(char *board);
void printScoreBoard(scores *scoresPtrOne, scores *scoresPtrTwo);

int main(int argc, char *argv[]) {
    char opt = greetingLines('0');
    scores playerOne;
    scores playerTwo;
    char optRules;
    FILE *cfPtr;
    int game = 1;
    
    if(opt == '3') {
        printf("Goodbye!\n");
        return 0;
    } else if(opt == '2') {
        printf("Let's play the game!\n");
    }
    
    const char *PIECES = "XO";
    char board[BOARD_ROWS *BOARD_COLS];
    memset(board, ' ', BOARD_ROWS * BOARD_COLS);
    
    updatePlayerScore(&playerOne, "Player 1", 0);
    updatePlayerScore(&playerTwo, "Player 2", 0);
    
    int turn, done = 0;
    
    while(playerOne.score < 2 && playerTwo.score < 2) {
        for (turn = 0; turn < BOARD_ROWS * BOARD_COLS && !done; turn++) {
            printBoard(board);
            while (!takeTurn(board, turn % 2, PIECES)) {
                printBoard(board);
                puts("**Column full!**\n");
            }
            done = checkWin(board);
        }
        printBoard(board);
        
        if (turn == BOARD_ROWS * BOARD_COLS && !done) {
            puts("It's a tie!");
        }
        else {
            turn--;
            int playerTurn = turn % 2 + 1;
            printf("Player %d (%c) wins!\n", playerTurn, PIECES[turn % 2]);
            if(playerTurn == 1) {
                playerOne.score++;
            } else {
                playerTwo.score++;
            }
            cfPtr = fopen("/Users/riri/Documents/Riri/CS49C/Connect4/Connect4/winLog.txt", "a");
            if(cfPtr == NULL) {
                puts("File could not be opened");
            } else {
                fprintf(cfPtr, "Game: %d\n", game);
                fprintf(cfPtr, "Player 1: %d\n", playerOne.score);
                fprintf(cfPtr, "Player 2: %d\n", playerTwo.score);
                fflush(cfPtr);
            }
            
            printScoreBoard(&playerOne, &playerTwo);
            system("clear");
            turn = 0;
            done = 0;
            memset(board, ' ', BOARD_ROWS * BOARD_COLS);
        }
    }
    
    if(playerOne.score > playerTwo.score) {
        printf("Player one wins!\n");
        fprintf(cfPtr, "Player one wins!\n");
    } else {
        printf("Player two wins!\n");
        fprintf(cfPtr, "Player two wins!\n");
    }
    
    fclose(cfPtr);
    return 0;
    
}

char greetingLines(char opt) {
    char option;
    do {
        printf("Welcome to Group 6's Connect4 Game! Please type one of these options to start:\n");
        printf("1. Game rules\n");
        printf("2. Let's start the game!\n");
        printf("3. Exit\n");
        scanf("  %c", &option);
        printf("You choose: %c\n", option);
        if(option == '1') {
            printGameRules();
        }
    } while(option != '3' && option != '2' && option != '1');
    return option;
}

void printGameRules() {
    char optRules;
    printf("\n***CONNECT4 RULES***\n\n");
    printf("Connect4 is a two-player game where players race to be the first to put four of their\n");
    printf("colored checkers in a row, either horizontally, vertically, or diagonally. Our game's\n");
    printf("board has 8x8 spots for players to put their pieces on. Players can specify the number\n");
    printf("of column they want to place their checkers in, and their piece would be printed on the\n");
    printf("board on that particular position. Players keep playing the game until either one\n");
    printf("player gets a score of 5. Whoever gets to score 5 points first wins the current set\n");
    printf("and the game for that round will be completed.\n\n");
    printf("Player one will have the set piece 'X', while player two the set piece 'O'\n");
    printf("\nPlease type any key to return to the greeting page.\n");
    scanf("%s", &optRules);
    printf("\n");
}

void printScoreBoard(scores *scorePtrOne, scores *scorePtrTwo) {
    printf("Scoreboard:\n");
    printf("-----------------------------------------\n");
    printf("| %s         | %s           |\n",scorePtrOne->playerName,
           scorePtrTwo->playerName);
    printf("-----------------------------------------\n");
    printf("| %d                | %d                  |\n",scorePtrOne->score, scorePtrTwo->score);
    
}

void updatePlayerScore(scores *scorePtr, char *playerName, int playerScore) {
    scorePtr->playerName = playerName;
    scorePtr->score = playerScore;
}

void printBoard(char *board) {
    int row, col;
    system("clear");
    
    puts("\n    ****Connect Four****\n");
    puts("  1   2   3   4   5   6   7   8\n");
    for (row = 0; row < BOARD_ROWS; row++) {
        for (col = 0; col < BOARD_COLS; col++) {
            if ('X' == board[BOARD_COLS * row + col])
            {
                printf("|");
                printf("%2c ", board[BOARD_COLS * row + col]);
            }
            else if ('O' == board[BOARD_COLS * row + col])
            {
                printf("|");
                printf("%2c ", board[BOARD_COLS * row + col]);
            }
            else
            {
                printf("| %c ", board[BOARD_COLS * row + col]);
            }
            
        }
        puts("|");
        puts(".................................");
        
    }
    
    
}
int takeTurn(char *board, int player, const char *PIECES) {
    int row, col = 0;
    printf("Player %d (%c):\nEnter number coordinate: ", player + 1, PIECES[player]);
    
    for(;;) {
        if (1 != scanf("%d", &col) || col < 1 || col > 8) {
            while (getchar() != '\n');
            puts("Number out of bounds! Try again.");
        }
        else {
            break;
        }
    }
    col--;
    
    for (row = BOARD_ROWS - 1; row >= 0; row--) {
        if (board[BOARD_COLS * row + col] == ' ') {
            board[BOARD_COLS * row + col] = PIECES[player];
            return 1;
        }
    }
    return 0;
    
}
int checkWin(char *board) {
    return (horizontalCheck(board) || verticalCheck(board) || diagonalCheck(board));
    
}
int checkFour(char *board, int a, int b, int c, int d) {
    return (board[a] == board[b] && board[b] == board[c] && board[c] == board[d] && board[a] != ' ');
    
}
int horizontalCheck(char *board) {
    int row, col, idx;
    const int WIDTH = 1;
    
    for (row = 0; row < BOARD_ROWS; row++) {
        for (col = 0; col < BOARD_COLS - 4; col++) {
            idx = BOARD_COLS * row + col;
            if (checkFour(board, idx, idx + WIDTH, idx + WIDTH * 2, idx + WIDTH * 3)) {
                return 1;
            }
        }
    }
    return 0;
}

int verticalCheck(char *board) {
    int row, col, idx;
    const int HEIGHT = 8;
    
    for (row = 0; row < BOARD_ROWS - 3; row++) {
        for (col = 0; col < BOARD_COLS; col++) {
            idx = BOARD_COLS * row + col;
            if (checkFour(board, idx, idx + HEIGHT, idx + HEIGHT * 2, idx + HEIGHT * 3)) {
                return 1;
            }
        }
    }
    return 0;
    
}
int diagonalCheck(char *board) {
    int row, col, idx, count = 0;
    const int DIAG_RGT = 7, DIAG_LFT = 9;
    
    for (row = 0; row < BOARD_ROWS - 3; row++) {
        for (col = 0; col < BOARD_COLS; col++) {
            idx = BOARD_COLS * row + col;
            if ((count <= 3 && checkFour(board, idx, idx + DIAG_LFT, idx + DIAG_LFT * 2, idx + DIAG_LFT * 3)) || (count >= 3 && checkFour(board, idx, idx + DIAG_RGT, idx + DIAG_RGT * 2, idx + DIAG_RGT * 3))) {
                return 1;
            }
            count++;
        }
        count = 0;
    }
    return 0;
    
}

