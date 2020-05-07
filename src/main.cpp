#include "FastLED.h"
#include "ArduinoSTL.h"
#include "vector"


#define DATA_PIN    6
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    10
#define BRIGHTNESS  75

CRGB leds[NUM_LEDS];

struct Move {
  size_t start, hop, finish;
};

std::vector<Move> valid_moves = {
  {0, 1, 5},
  {0, 2, 3},
  {1, 5, 6},
  {1, 4, 8}, 
  {2, 4, 7}, 
  {2, 3, 9}, 
  {3, 4, 5}, 
  {6, 7, 8}, 
  {7, 8, 9}, 
  {5, 1, 0}, 
  {3, 2, 0}, 
  {6, 5, 1}, 
  {8, 4, 1}, 
  {7, 4, 2}, 
  {9, 3, 2}, 
  {5, 4, 3}, 
  {8, 7, 6}, 
  {9, 8, 7}
};
bool board[10];
size_t positions = 10;
size_t current_board = 0;

void setup() {
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  Serial.begin(9600);
}

std::vector<Move> getMoves() {
   std::vector<Move> moves;

  for (size_t start = 0; start < positions; start++)
  {
    if (board[start] == false) {
      continue;
    }

    for (Move m : valid_moves)
    {
      if (m.start == start && board[m.hop] && !board[m.finish]) {

        Serial.print("Start:");
        Serial.print(m.start);
        Serial.print(", Hop:");
        Serial.print(m.hop);
        Serial.print(", Finish:");
        Serial.println(m.finish);

        moves.push_back(m);
      }
    }
    
  }
  return moves;
}

void displayBoard() {
  for (size_t i = 0; i < positions; i++)
  {
    if (board[i]) {
      leds[i] = CRGB::AliceBlue;
    }
    else {
      leds[i] = CRGB::Crimson;
    }
  }
  FastLED.show();
  delay(200);
  
}

void displayMove(Move m) {
  Serial.println("displaying move");
  // for (size_t i = 0; i < 5; i++)
  // {
    leds[m.start] = CRGB::Red;
    leds[m.hop] = CRGB::Orange;
    leds[m.finish] = CRGB::Green;
    FastLED.show(); 
    delay(100);
  // }
  
}

void initialiseBoard() {
  for (size_t i = 0; i < positions; i++)
  {
    board[i] = i != current_board;
  }
}

bool isSolved() {
  size_t pegs = 0;
  for (size_t i = 0; i < positions; i++)
  {
    if (board[i])
      pegs++;
  }
  
  return pegs == 1;
}

void displaySuccess() {
  FastLED.showColor(CRGB::Green);
  delay(200);
}

void displayFail() {
  FastLED.showColor(CRGB::Red);
  delay(100 );
}

void solveBoard(Move move) {
  displayMove(move);

  board[move.start] = false;
  board[move.hop] = false;
  board[move.finish] = true;

  displayBoard();

  if (isSolved()) {
    displaySuccess();
  }
  else {
    std::vector<Move> moves = getMoves();
    if (moves.size() == 0) {
      displayFail();
    }
    else {
      for (Move m : moves) {
        solveBoard(m);
      }
    }
  }
  Serial.println("Back track");
  board[move.start] = true;
  board[move.hop] = true;
  board[move.finish] = false;
  displayBoard();
}

void loop()
{
    Serial.print("Solving for start: ");
    Serial.println(current_board);

  initialiseBoard();
  displayBoard();

  
  while (current_board == 1 && !Serial.available()){
    //Do Absolutely Nothing until something is received over the serial port
  }

  for (Move m : getMoves())
  {
    solveBoard(m);
  }

  current_board++;
  current_board = current_board % 10;
}