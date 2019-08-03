/* 
 *  arduino_connect4: sketch for connect4 game using 8x8 LED matrix backpack
 *  by mit-mit
 *  
 *  Code uses Adafruit LED Backpack and GFX Library
 *  https://github.com/adafruit/Adafruit_LED_Backpack
 *  https://github.com/adafruit/Adafruit-GFX-Library
 *  
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define Button_left 22
#define Button_right 9
#define Button_drop 10

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

int board_state[42];
int board_stateAI[42];
long col_wins[7];
long col_losses[7];
long score_rec[7];

uint8_t movelist1[10] = {0,1,1,7,7,7,0,3,0,0};
uint8_t movelist2[10] = {0,1,2,2,7,3,5,4,0,0};

void ClearBoard() {
  for (int i = 0; i < 42; i++) {
    board_state[i] = 0;
  }
  //board_state[2] = 1;
  //board_state[3] = 1;
  for (int i = 0; i < 42; i++) {
    if (board_state[i] == 1) {
      matrix.drawPixel(i/7, i%7, 1);
      matrix.writeDisplay();
    }
    else if (board_state[i] == 2) {
      matrix.drawPixel(i/7, i%7, 2);
      matrix.writeDisplay();
    }
  }
}


// PlaceToken: controls token place animation and board state update
uint8_t PlaceToken(uint8_t col, uint8_t player)
{
  if (board_state[7*5+col] > 0) { // can't place token here, already full on this column
    return(0);
  }
  uint8_t row = 5;
  while (1) {
    if (row == 0) { // at the bottom, place the token
      board_state[col] = player;
      matrix.drawPixel(0, col, player);
      matrix.writeDisplay();
      delay(200);
      return(1);
    }
    if (board_state[7*(row-1)+col] > 0) { // token already played at row immediately under us
      board_state[7*row+col] = player;
      matrix.drawPixel(row, col, player);
      matrix.writeDisplay();
      delay(200);
      return(1);
    }
    else {
      matrix.drawPixel(row, col, player); // update animation of token falling
      matrix.writeDisplay();
      delay(200);
      matrix.drawPixel(row, col, 0);
      matrix.writeDisplay();
      row--;
    }
  }
}

// CheckforWin: checks if last move resulted in victory
uint8_t CheckforWin()
{
  uint8_t p;
  for (int c = 0; c < 4; c++) {
    for (int r = 0; r < 6; r++) {
      p = board_state[7*r+c];
      if (p == 0) {
        continue;
      }
      if (board_state[7*r+c+1] == p && board_state[7*r+c+2] == p && 
        board_state[7*r+c+3] == p) { // horizontal line
        matrix.drawPixel(r, c, LED_GREEN);
        matrix.drawPixel(r, c+1, LED_GREEN);
        matrix.drawPixel(r, c+2, LED_GREEN);
        matrix.drawPixel(r, c+3, LED_GREEN);
        matrix.writeDisplay();
        delay(1000);
        return p;
      }
    }
  }
  for (int c = 0; c < 7; c++) {
    for (int r = 0; r < 3; r++) {
      p = board_state[7*r+c];
      if (p == 0) {
        continue;
      }
      if (board_state[7*(r+1)+c] == p && board_state[7*(r+2)+c] == p && 
        board_state[7*(r+3)+c] == p) { // vertical line
        matrix.drawPixel(r, c, LED_GREEN);
        matrix.drawPixel(r+1, c, LED_GREEN);
        matrix.drawPixel(r+2, c, LED_GREEN);
        matrix.drawPixel(r+3, c, LED_GREEN);
        matrix.writeDisplay();
        delay(1000);
        return p;
      }
    }
  }
  for (int c = 0; c < 4; c++) {
    for (int r = 0; r < 3; r++) {
      p = board_state[7*r+c];
      if (p == 0) {
        continue;
      }
      if (board_state[7*(r+1)+c+1] == p && board_state[7*(r+2)+c+2] == p && 
        board_state[7*(r+3)+c+3] == p) { // diagonal line
        matrix.drawPixel(r, c, LED_GREEN);
        matrix.drawPixel(r+1, c+1, LED_GREEN);
        matrix.drawPixel(r+2, c+2, LED_GREEN);
        matrix.drawPixel(r+3, c+3, LED_GREEN);
        matrix.writeDisplay();
        delay(1000);
        return p;
      }
      p = board_state[7*r+6-c];
      if (p == 0) {
        continue;
      }
      if (board_state[7*(r+1)+6-c-1] == p && board_state[7*(r+2)+6-c-2] == p &&
        board_state[7*(r+3)+6-c-3] == p) { // diagonal line (opposite direction)
        matrix.drawPixel(r, 6-c, LED_GREEN);
        matrix.drawPixel(r+1, 6-c-1, LED_GREEN);
        matrix.drawPixel(r+2, 6-c-2, LED_GREEN);
        matrix.drawPixel(r+3, 6-c-3, LED_GREEN);
        matrix.writeDisplay();
        delay(1000);
        return p;
      }
    }
  }
  // check for tie
  if ((board_state[35] > 0) && (board_state[36] > 0) && (board_state[37] > 0) && (board_state[38] > 0) &&
    (board_state[39] > 0) && (board_state[40] > 0) && (board_state[41] > 0)) {
      return 3; // board is full, tie game
  }
  return 0; // no winner yet
}

void RunWinnerAnimation(uint8_t player) {
  matrix.setTextWrap(false);
  matrix.setTextSize(1);
  matrix.setRotation(1);
  if (player == 1) {
    matrix.setTextColor(1);
    for (int8_t x=7; x>=-43; x--) {
      matrix.clear();
      matrix.setCursor(x,0);
      matrix.print("winner");
      matrix.writeDisplay();
      delay(100);
    }
  }
  else if (player == 2) {
    matrix.setTextColor(2);
    for (int8_t x=7; x>=-36; x--) {
      matrix.clear();
      matrix.setCursor(x,0);
      matrix.print("loser");
      matrix.writeDisplay();
      delay(100);
    }
  }
  else {
    matrix.setTextColor(3);
    for (int8_t x=7; x>=-29; x--) {
      matrix.clear();
      matrix.setCursor(x,0);
      matrix.print("draw");
      matrix.writeDisplay();
      delay(100);
    }
  }
}

/////////////////////////////////////////////////////////////////
//////////////// AI Functions
/////////////////////////////////////////////////////////////////

#define n_MCTS_runs 10000

// InitBoardAI: resets simulation board and column win/lose stats for MCTS
void InitBoardAI() {
  for (int i = 0; i < 42; i++) {
    board_stateAI[i] = board_state[i];
  }
}

// checks if board is in winning state, return who won
uint8_t CheckforWinAI()
{
  uint8_t p;
  for (int c = 0; c < 4; c++) {
    for (int r = 0; r < 6; r++) {
      p = board_stateAI[7*r+c];
      if (p == 0) {
        continue;
      }
      if (board_stateAI[7*r+c+1] == p && board_stateAI[7*r+c+2] == p && 
        board_stateAI[7*r+c+3] == p) { // horizontal line
        return p;
      }
    }
  }
  for (int c = 0; c < 7; c++) {
    for (int r = 0; r < 3; r++) {
      p = board_stateAI[7*r+c];
      if (p == 0) {
        continue;
      }
      if (board_stateAI[7*(r+1)+c] == p && board_stateAI[7*(r+2)+c] == p && 
        board_stateAI[7*(r+3)+c] == p) { // vertical line
        return p;
      }
    }
  }
  for (int c = 0; c < 4; c++) {
    for (int r = 0; r < 3; r++) {
      p = board_stateAI[7*r+c];
      if (p == 0) {
        continue;
      }
      if (board_stateAI[7*(r+1)+c+1] == p && board_stateAI[7*(r+2)+c+2] == p && 
        board_stateAI[7*(r+3)+c+3] == p) { // diagonal line
        return p;
      }
      p = board_stateAI[7*r+6-c];
      if (p == 0) {
        continue;
      }
      if (board_stateAI[7*(r+1)+6-c-1] == p && board_stateAI[7*(r+2)+6-c-2] == p && 
        board_stateAI[7*(r+3)+6-c-3] == p) { // diagonal line (opposite direction)
        return p;
      }
    }
  }
  // check for tie
  if ((board_stateAI[35] > 0) && (board_stateAI[36] > 0) && (board_stateAI[37] > 0) && (board_stateAI[38] > 0) &&
    (board_stateAI[39] > 0) && (board_stateAI[40] > 0) && (board_stateAI[41] > 0)) {
      return 3; // board is full, tie game
  }
  return 0; // no winner yet
}

// PlaceTokenAI: updates simulation game board, if possible, returns 0 otherwise
uint8_t PlaceTokenAI(uint8_t col, uint8_t player)
{
  if (board_stateAI[7*5+col] > 0) { // can't place token here, already full on this column
    return(0);
  }
  uint8_t row = 0;
  while (board_stateAI[7*row+col] > 0) {
    row++;
  }
  board_stateAI[7*row+col] = player;
  return(1);
}

void DrawBoardAI() {
  Serial.print("\n");
  for (int r = 5; r >= 0; r--) {
    for (int c = 0; c < 7; c++) {
      Serial.print(board_stateAI[7*r+c]);
      Serial.print(" ");
    }
    Serial.print("\n");
  }
}

// AIPlay: function runs AI moves
uint8_t AIPlay(uint8_t player_ai) {

  uint8_t valid;
  uint8_t pwin;
  uint8_t first;
  uint8_t col = 0;
  uint8_t playcol;
  uint8_t simplayer;

  long score, best_score;
  uint8_t best_col;
  uint8_t moves_left;
  uint8_t moves_left2;
  
  uint8_t opponent;

  // record player ID of opponent
  opponent = player_ai + 1;
  if (opponent == 3) {
    opponent = 1;
  }
  
  // Start off by looking for immediate win
  for (int i = 0; i < 7; i++) {
    InitBoardAI();
    valid = PlaceTokenAI(i,player_ai);
    pwin = CheckforWinAI();
    if (pwin == player_ai) {
      return i;
    }
  }

  // Next look for blocks necessary to stop player immediate win
  for (int i = 0; i < 7; i++) {
    InitBoardAI();
    valid = PlaceTokenAI(i,opponent);
    pwin = CheckforWinAI();
    if (pwin == opponent) {
      return i;
    }
  }
  
  // Count remaining moves
  moves_left = 0;
  for (int i = 0; i < 42; i++) {
    if (board_state[i] == 0) {
      moves_left++;
    }
  }
  
  // no win next move, run simuations
  for (int i = 0; i < 7; i++) {
    col_wins[i] = 0;
    col_losses[i] = 0;
  }
  for (int i = 0; i < n_MCTS_runs; i++) {
    InitBoardAI();
    simplayer = player_ai;

    // play first move
    valid = PlaceTokenAI(col,simplayer);
    while (valid == 0) {
      col++;
      if (col == 7) {
        col = 0;
      }
      valid = PlaceTokenAI(col,simplayer);
    }
    simplayer++;
    if (simplayer == 3) {
      simplayer = 1;
    }
    pwin = CheckforWinAI();
    if (pwin == 3) { // check for tie
      continue; // move to next random sample
    }

    // loop through remaining moves until result
    moves_left2 = moves_left - 1;
    while (1) {
      playcol = random(0, 6);
      valid = PlaceTokenAI(playcol,simplayer);
      while (valid == 0) {
        playcol++;
        if (playcol == 7) {
          playcol = 0;
        }
        valid = PlaceTokenAI(playcol,simplayer);
      }

      pwin = CheckforWinAI();
      if (pwin == player_ai) { // game resulted in win
        // record victory against this first played column
        col_wins[col] = col_wins[col] + moves_left2;
        //col_wins[col]++;
        break; // move to next random sample
      }
      else if (pwin == opponent) { // game resulted in loss
        // record defeat against this first played column
        col_losses[col] = col_losses[col] + moves_left2;
        //col_losses[col]++;
        //DrawBoardAI();
        break; // move to next random sample
      }
      else if (pwin == 3) { // resulted in draw, move onto next sim
        break;
      }
      simplayer++;
      if (simplayer == 3) {
        simplayer = 1;
      }
      moves_left2--;
    }

    // Cycle through first-played cols
    col++;
    if (col == 7) {
      col = 0;
    }
    
  } // MCTS loop

  // evaluate win/loss stats to determine best move
  best_col = 0;
  best_score = col_wins[0]-col_losses[0];
  if (board_state[7*5+0] > 0) {
    best_score = -42*n_MCTS_runs;
  }
  score_rec[0] = best_score;
  for (int i = 1; i < 7; i++) {
    score = col_wins[i]-col_losses[i];
    score_rec[i] = score;
    if ( (score > best_score) && (board_state[7*5+i] == 0) ) {
      best_col = i;
      best_score = score;
    }
  }
  
  for (int i = 0; i < 7; i++) {
    Serial.print(col_wins[i]);
    Serial.print("/");
    Serial.print(col_losses[i]);
    Serial.print("/");
    Serial.print(score_rec[i]);
    Serial.print(", ");
  }
  Serial.print("\n");
  
  return best_col;
  
}

void setup() {

  Serial.begin(9600);
  
  matrix.begin(0x70); // start up the bi-colour LED matrix
  delay(500);
  matrix.clear();

  // Run start-up animation
  matrix.setTextWrap(false);
  matrix.setTextSize(1);
  matrix.setRotation(1);
  matrix.setTextColor(LED_GREEN);
  for (int8_t x=7; x>=-57; x--) {
    matrix.clear();
    matrix.setCursor(x,0);
    matrix.print("connect4");
    matrix.writeDisplay();
    delay(100);
  }
  delay(500);
  matrix.clear();

  randomSeed(analogRead(0));

  pinMode(Button_right, INPUT_PULLUP);
  pinMode(Button_left, INPUT_PULLUP);
  pinMode(Button_drop, INPUT_PULLUP);
  pinMode(13, OUTPUT); 
}

void loop() {

  uint8_t player_win = 0;
  uint8_t valid = 0;
  uint8_t col = 3;
  uint8_t col_last = 3;
  uint8_t movenum = 0;
  
  // initialise board
  matrix.clear();
  matrix.setRotation(0);
  ClearBoard();
  
  // start game loop
  while (1) {

    // human player move
    col = col_last;
    //uint8_t hold_count = 0;
    uint8_t held_left = 0;
    uint8_t held_right = 0;
    uint8_t held_drop = 0;
    matrix.drawPixel(7, col, 1);
    matrix.writeDisplay();
    while(1) {
      uint8_t button_r = digitalRead(Button_right);
      uint8_t button_l = digitalRead(Button_left);
      uint8_t button_d = digitalRead(Button_drop);
      delay(20);
      if ( (button_r == 1) && (held_right == 0) ) { // check for button right
        matrix.drawPixel(7, col, 0);
        matrix.writeDisplay();
        col++;
        if (col == 7) {
          col = 6;
        }
        matrix.drawPixel(7, col, 1);
        matrix.writeDisplay();
        held_right = 1;
      }
      else if (button_r == 0) {
        held_right = 0;
      }
      if ( (button_l == 1) && (held_left == 0) ) { // check for button left
        matrix.drawPixel(7, col, 0);
        matrix.writeDisplay();
        if (col == 0) {
          col = 0;
        }
        else {
          col--;
        }
        matrix.drawPixel(7, col, 1);
        matrix.writeDisplay();
        held_left = 1;
      }
      else if (button_l == 0) {
        held_left = 0;
      }
      if (button_d == 0) {
        break;
      }
    }
    col_last = col;
    matrix.drawPixel(7, col, 0);
    matrix.writeDisplay();
    //col = AIPlay(1);
    //col = movelist1[movenum];
    valid = PlaceToken(col, 1);
    player_win = CheckforWin();
    if (player_win > 0) {
      RunWinnerAnimation(player_win);
      break;
    }

    // AI player move
    col = AIPlay(2);
    //col = movelist2[movenum];
    valid = PlaceToken(col, 2);
    player_win = CheckforWin();
    if (player_win > 0) {
      RunWinnerAnimation(player_win);
      break;
    }
    
  }

}
