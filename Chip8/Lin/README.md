# Chip8
Telmac 1800 Emulator for linux

Instructions for use

1. Download the Lin folder from github and extract the folder onto a machine running some version of linux

2. Before building the project assure that the OS you have has Simple Direct Media Layer installed. 
	-It comes with most Linux distros but needs to be installed. 
	-use the following terminal commands to do so:
		sudo apt install libsdl2-dev

		sudo apt install libsdl2-image-dev

		sudo apt install libsdl2-mixer-dev

		sudo apt install libsdl2-ttf-dev

3. Once the files are installed use the command 'make'

4. Once the build was successful, run the command './my_program'

5. A window will pop up with the screen for pong.

6. To move your paddle up press the 'B' key, to move the paddle down press '1' in the num pad.

7. To close the program, the terminal that it was ran from needs to be closed.

Bugs:

1. Slower paddle movement when using the down key.

2. Gameover screen doesn't appear sometimes.
