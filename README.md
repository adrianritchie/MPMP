# Triangle Solitaire - Arduino
Arduino implementation of Triangle Solitaire solution, displayed using neopixels.

The solution was created using PlatformIO and makes use of the FastLED library to control the neopixels and ArduinoSTL library to make use of vectors.

The solution is a recursive depth-first backtracking algorithm.  The neopixels are wired in the following configuration:

```
   0
  /
  1-2
     \
 5-4-3
/
6-7-8-9
```

Each position that is "empty" is displayed as CRGB::Crimson, each position that has a peg in it is CRGB::AliceBlue.  Each move that is being performed is:
Peg being move start poition: CRGB::Red
Peg being hopped: CRGB::Orange
Peg being moved end position: CRGB:Green

If a move results in a solution (only one peg left), all the neopixels flash CRGB::Green.  If a move results in a deadend (no more moves possible), the board flashes CRGB::Red.

Once a solution or deadend is found, the moves are reversed and alternatives are tried.

Watch it in action (https://youtu.be/of4Qa4DjB7E):

[![Watch the video](https://img.youtube.com/vi/of4Qa4DjB7E/hqdefault.jpg)](https://youtu.be/of4Qa4DjB7E)
