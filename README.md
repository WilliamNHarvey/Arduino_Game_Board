# Arduino_Game_Board

## Schematic

![schematic](https://raw.githubusercontent.com/WilliamNHarvey/Arduino_Game_Board/master/schematic.png)

Columns are anodes, rows are cathodes.

## Functionality

The matrix is to be controlled using a multiplexing principle. I find that a 10 millis delay is about long enough for the lights to appear to be constantly on. The multiplexing principle works as follows:

* Develop your current game board map in an array of 8 8-bit bytes, mapping byte 1 through 8 to columns and bit 1 through 8 to rows (each bit being a single LED state).
* On a board refresh, loop through each of the 8 column bytes.
  * On each loop, turn the corresponding column to LOW, and the rest to HIGH. This allows only LEDs in the proper row to have a voltage drop.
  * After setting the column to ground, flip LED states in the row according to your game board map developed in step 1.
  * Delay for some short amount of time

There are two controllers, with two buttons each. In the snake example, the two controllers are used for vertical movement and horizontal movement. 

Each player is connected to an interrupt input, which can be used if user mechanics need to be measured instantaneously. See pong for an example of this.
