#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_CYAN    "\x1b[36m"

#define ANSI_COLOR_RESET   "\x1b[0m"
#define ROW 8
#define COLUMN 8

#define WAIT 3

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
int switchTurn(char *board, int player, const char*, int *skipped);
int checkWinCondition(char *board);
int checkC4(char *board, int, int, int, int);
int checkHorizontal(char *board);
int checkVertical(char *board);
int checkDiag(char *board);
void printScoreBoard(scores *scoresPtrOne, scores *scoresPtrTwo);

int main(int argc, char *argv[]) {
    char opt = greetingLines('0');
    scores p1;
    scores p2;
    // char optRules;
    FILE *cfPtr;
    int game = 1;
    
    if(opt == '3') {
        printf("Goodbye!\n");
        return 0;
    } else if(opt == '2') {
        printf("Let's play the game!\n");
    }
    
    const char *PIECES = "XO";
    char board[ROW *COLUMN];
    memset(board, ' ', ROW * COLUMN);
    
    updatePlayerScore(&p1, "Player 1", 0);
    updatePlayerScore(&p2, "Player 2", 0);
    
    int turn, done = 0;
    int skipped = 0;
    while(p1.score < 5 && p2.score < 5) {
        for (turn = 0; turn < ROW * COLUMN && !done; turn++) {
            printBoard(board);
            while (!switchTurn(board, turn % 2, PIECES, &skipped)) {
                if(!skipped) {
                    turn++;
                }
                printBoard(board);
                //puts("**Column full!**\n");
            }
            done = checkWinCondition(board);
        }
        printBoard(board);
        
        if (turn == ROW * COLUMN && !done) {
            puts("It's a tie!");
        }
        else {
            turn--;
            int playerTurn = turn % 2 + 1;
            printf("Player %d (%c) wins this round!\n", playerTurn, PIECES[turn % 2]);
            if(playerTurn == 1) {
                p1.score++;
            } else {
                p2.score++;
            }
            cfPtr = fopen("/Users/riri/Documents/Riri/CS49C/Connect4/Connect4/winLog.txt", "a");
            if(cfPtr == NULL) {
                puts("File could not be opened");
            } else {
                fprintf(cfPtr, "Game: %d\n", game);
                fprintf(cfPtr, "Player 1: %d\n", p1.score);
                fprintf(cfPtr, "Player 2: %d\n", p2.score);
                fflush(cfPtr);
            }
            
            printScoreBoard(&p1, &p2);
            sleep(4);
            system("clear");
            turn = 0;
            done = 0;
            memset(board, ' ', ROW * COLUMN);
        }
    }
    
    if(p1.score > p2.score) {
        printf("Player one wins!\n");
        fprintf(cfPtr, "Player one wins!\n");
        printScoreBoard(&p1, &p2);
    } else {
        printf("Player two wins!\n");
        fprintf(cfPtr, "Player two wins!\n");
        printScoreBoard(&p1, &p2);
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
    for (row = 0; row < ROW; row++) {
        for (col = 0; col < COLUMN; col++) {
            if ('X' == board[COLUMN * row + col])
            {
                printf("|");
                printf(ANSI_COLOR_RED"%2c "ANSI_COLOR_RESET "", board[COLUMN * row + col]);
            }
            else if ('O' == board[COLUMN * row + col])
            {
                printf("|");
                printf(ANSI_COLOR_CYAN "%2c " ANSI_COLOR_RESET "", board[COLUMN * row + col]);
            }
            else
            {
                printf("| %c ", board[COLUMN * row + col]);
            }
            
        }
        puts("|");
        puts(".................................");
        
    }
}

int switchTurn(char *board, int player, const char *PIECES, int *skipped) {
    int row, col = 0;
    int ready_for_reading = 0;
    int read_bytes = 0;
    char piece[3] = {0};
    fd_set input_set;
    struct timeval  timeout;
    FD_ZERO(&input_set );
    FD_SET(0, &input_set);
    
    timeout.tv_sec = WAIT;
    timeout.tv_usec = 0;
    
    printf("Player %d (%c):\nEnter number coordinate. If 10 seconds have passed, your turn will be skipped: ", player + 1, PIECES[player], WAIT);
    ready_for_reading = select(1, &input_set, NULL, NULL, &timeout);
    
    if(ready_for_reading) {
        printf("Col: %d\n", col);
        read_bytes = read(0, piece, 2);
        if(piece[read_bytes-1]=='\n'){
            --read_bytes;
            piece[read_bytes]='\0';
        }
        if(read_bytes==0){
            printf("You just hit enter\n");
        } else {
            char *ptr = &piece;
            col = atoi(ptr);
        }
    } else {
        col = -1;
        printf("No input! Your turn is skipped.\n");
        return 0;
    }
    
    //    printf("Col is now: %d\n", col);
    
    for(;;) {
        if (col < 1 || col > 8) {
            while (getchar() != '\n');
            puts("Number out of bounds! Try again.");
            scanf(" %d", &col);
        } else if(col < 0) {
            printf("Skipped!\n");
        }
        else {
            break;
        }
    }
    
    if(col != -1) {
        skipped = 0;
        col--;
        
        for (row = ROW - 1; row >= 0; row--) {
            if (board[COLUMN * row + col] == ' ') {
                board[COLUMN * row + col] = PIECES[player];
                return 1;
            }
        }
    } else {
        skipped = 1;
    }
    return 0;
    
}
int checkWinCondition(char *board) {
    return (checkHorizontal(board) || checkVertical(board) || checkDiag(board));
    
}
int checkC4(char *board, int a, int b, int c, int d) {
    return (board[a] == board[b] && board[b] == board[c] && board[c] == board[d] && board[a] != ' ');
    
}
int checkHorizontal(char *board) {
    int row, col, id;
    const int WIDTH = 1;
    
    for (row = 0; row < ROW; row++) {
        for (col = 0; col < COLUMN - 3; col++) {
            id = COLUMN * row + col;
            if (checkC4(board, id, id + WIDTH, id + WIDTH * 2, id + WIDTH * 3)) {
                return 1;
            }
        }
    }
    return 0;
}

int checkDiag(char *board) {
    int row, col, id, count = 0;
    const int DIAG = 7, DIAG_LFT = 9;
    
    for (row = 0; row < ROW - 3; row++) {
        for (col = 0; col < COLUMN; col++) {
            id = COLUMN * row + col;
            if ((count <= 3 && checkC4(board, id, id + DIAG_LFT, id + DIAG_LFT * 2, id + DIAG_LFT * 3)) || (count >= 3 && checkC4(board, id, id + DIAG, id + DIAG * 2, id + DIAG * 3))) {
                return 1;
            }
            count++;
        }
        count = 0;
    }
    return 0;
    
}

int checkVertical(char *board) {
    int row, col, id;
    const int HEIGHT = 8;
    
    for (row = 0; row < ROW - 3; row++) {
        for (col = 0; col < COLUMN; col++) {
            id = COLUMN * row + col;
            if (checkC4(board, id, id + HEIGHT, id + HEIGHT * 2, id + HEIGHT * 3)) {
                return 1;
            }
        }
    }
    return 0;
    
}
