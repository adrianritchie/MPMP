#include "FastLED.h"
#include "ArduinoSTL.h"
#include "vector"


#define DATA_PIN    6
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    15
#define BRIGHTNESS  75


CRGB leds[NUM_LEDS];

struct Move {
  size_t start, hop, finish;
};

#if NUM_LEDS == 10
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
#elif NUM_LEDS ==15
std::vector<Move> valid_moves = {
  {0, 1, 5},
  {0, 2, 3},
  {1, 5, 6},
  {1, 4, 8},
  {2, 4, 7},
  {2, 3, 9},
  {5, 6, 14},
  {5, 7, 12},
  {5, 4, 3},
  {4, 7, 13},
  {4, 8, 11},
  {3, 8, 12},
  {3, 9, 10},
  {6, 7, 8},
  {7, 8, 9},
  {14, 13, 12},
  {13, 12, 11},
  {12, 11, 10},
  {5, 1, 0},
  {3, 2, 0},
  {6, 5, 1},
  {8, 4, 1},
  {7, 4, 2},
  {9, 3, 2},
  {14, 6, 5},
  {12, 7, 5},
  {3, 4, 5},
  {13, 7, 4},
  {11, 8, 4},
  {12, 8, 3},
  {10, 9, 3},
  {8, 7, 6},
  {9, 8, 7},
  {12, 13, 14},   
  {11, 12, 13},
  {10, 11, 12}};
#else
std::vector<Move> valid_moves = { };
#endif

bool board[NUM_LEDS];
size_t positions = NUM_LEDS;
size_t current_board = 0;
size_t stack_count = 0;

void setup() {

  delay(3000); // 3 second delay for recovery
  randomSeed(analogRead(0));
  current_board = random(NUM_LEDS );
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  Serial.begin(115200);
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
        moves.push_back(m);
      }
    }
    
  }
  return moves;
}

void displayStart() {
  static uint8_t blendRate = 10;
  FastLED.showColor(CRGB::Black);
  FastLED.show();

  CHSV colorStart = CHSV(0,0,0);  // starting color
  CHSV colorTarget = rgb2hsv_approximate(CRGB::Cyan);  // target color
  CHSV colorCurrent = colorStart;

  while (true) {
    EVERY_N_MILLISECONDS(blendRate){
      static uint8_t k;
      if ( colorCurrent.h == colorTarget.h ) {  // Check if target has been reached
        break;
      }

      colorCurrent = blend(colorStart, colorTarget, k, SHORTEST_HUES);
      leds[current_board] = colorCurrent;
      Serial.print("colorCurrent:\t"); Serial.print(colorCurrent.h); Serial.print("\t");
      Serial.print("colorTarget:\t"); Serial.print(colorTarget.h);
      Serial.print("\tk: "); Serial.println(k);
      k++;
    }
    FastLED.show();  // update the display
  }

  colorTarget = CHSV(0,0,0);
  colorStart = colorCurrent;
  while (true) {
    EVERY_N_MILLISECONDS(blendRate){
      static uint8_t k;
      if ( colorCurrent.h == colorTarget.h ) {  // Check if target has been reached
        break;
      }

      colorCurrent = blend(colorStart, colorTarget, k, SHORTEST_HUES);
      leds[current_board] = colorCurrent;
      Serial.print("colorCurrent:\t"); Serial.print(colorCurrent.h); Serial.print("\t");
      Serial.print("colorTarget:\t"); Serial.print(colorTarget.h);
      Serial.print("\tk: "); Serial.println(k);
      k++;
    }
    FastLED.show();  // update the display
  }
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
  delay(10);
  
}

void displayMove(Move m) {
  //Serial.println("displaying move:");
  
  //Serial.print("Start:");
  //Serial.print(m.start);
  //Serial.print(", Hop:");
  //Serial.print(m.hop);
  //Serial.print(", Finish:");
  //Serial.println(m.finish);

  // for (size_t i = 0; i < 5; i++)
  // {
    leds[m.start] = CRGB::Red;
    leds[m.hop] = CRGB::Orange;
    leds[m.finish] = CRGB::Green;
    FastLED.show(); 
    delay(10);
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

  Serial.print("success: ");
  Serial.println(stack_count);
  FastLED.showColor(CRGB::Green);
  delay(20000);
}

void displayFail() {
  Serial.print("failed: ");
  Serial.println(stack_count);
  FastLED.showColor(CRGB::Red);
  delay(50 );
}

void solveBoard(Move move) {
  if (move.start < 0 || move.start > NUM_LEDS + 1 ||
      move.hop < 0 || move.hop > NUM_LEDS + 1 ||  
      move.finish < 0 || move.finish > NUM_LEDS + 1) return;  

  stack_count++;
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
  //Serial.println("Back track");
  board[move.start] = true;
  board[move.hop] = true;
  board[move.finish] = false;
  displayBoard();
  stack_count--;
}

void loop()
{
    Serial.print("Solving for start: ");
    Serial.println(current_board);
    displayStart();

  initialiseBoard();
  displayBoard();

  for (Move m : getMoves())
  {
    solveBoard(m);
  }

  current_board++;
  current_board = current_board % NUM_LEDS;
}