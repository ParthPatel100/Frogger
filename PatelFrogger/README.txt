CPSC 359 Project Part 2 README
Frogger Game Implementation

To compile the project part 1 code:
  -dowload the project folder and unzip
  -open terminal
  -go to the directory the project is saved
  -type "make" to compile the code
  -type "./myProg" to run the project code
  -IF... console says "permission denied", then add a empty line in the main.c in the Source folder of the program, save the main, then "make", then "./myProg"
  -the program will now be running 
  -player can move the joypad up or down to select menu item buttons
  -press "A" on selection
  -when player starts game, they will be able to play the game
    -move the joypad to move the frog
    -try to avoid the moving obstacles, player loses life upon colliding with obstacles 
    -avoid the water (if the frog touches the water, life is lost)
    -frog must completely be on the logs to continue
    -frog must completely be in the castle to win the game, if frog touches water in the castle lane, game over
   -press the "Start" button to pause the screen
    -move joypad and press "A" to make selection
    -OR press start button again to resume game
   -VALUE PACK: after 30 seconds into the game, a heart will appear into a random safe lane (purple) at a random position
    -frog touching the heart means a life gets given to the player
   -if game is lost or won, press any button on the controller to return to the main menu screen
  
NOTE: The SNES Controller is highly sensitive
