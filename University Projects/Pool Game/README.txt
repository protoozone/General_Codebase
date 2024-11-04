# Pool Game Builder

To run the application, please use:

gradle run

To generate a javadoc, please use:

gradle javadoc

# Game Notes
- In order to hit the ball, click and hold onto the edge of the cue ball where you'd like to hit. 
- Then, drag your cursor away (in the angle you'd like to hit), and then release.
- The power of your hit will be based on the length of your drag (although ball velocity is capped). 

# Config Notes
When entering config details, please note the following restrictions:
- Friction must be value between 0 - 1 (not inclusive). [Would reccomend switching between 0.95, 0.9, 0.85 to see changes].
- Ball X and Y positions must be within the size of the table width and length, including the ball radius (10).
- Ball colours must be Paint string values as expected.

# Extra Notes
By comparison to the previous edition of this program, there are the following additions made:
- There is now an undo and cheat button in-game that allows the reversion of a shot and the removal of all blue balls, respectively.
You can click on these buttons after loading a difficulty.

- The game now requests you to click on a difficulty at the start.
- This was done through simple button additions.

- More colours have been implemented for balls.

- Black and brown balls now last 3 shots, instead of just 2 or 1 shot.

- Green and purple balls last 2 shots like blue balls.

- All other balls except the cue ball last 1 shot as usual.

- These balls implement the strategy and builder patterns like the original patterns. 
<"src/java/PoolGame/Strategy/BlackStrategy"> & <"src/java/PoolGame/Objects/BallBuilder">

- Time and score are now tracked, and are both reverted with the undo feature.
- This was done using simple trackers.
- Reversions were done with a new Snapshot Class.
<"src/java/PoolGame/Snapshot">

- There are more levels based on difficulty choice.
- This was read in using the factory JSON reading method.
<"src/java/PoolGame/config/ReaderFactory">

- Enjoy :) [I know I forgot the cue stick!]