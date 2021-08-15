#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <thread>
#pragma once
#include "Screen.h"
#include "SDL.h"
//Function macros----------------
#define CHAR_MAX 255
#define CHAR_MIN 0
#define VX V[(op & 0x0F00) >> 8]
#define VY V[(op & 0x00F0) >> 4]
#define VF V[15]
#define SCREEN_HEIGHT 32;
#define SCREEN_WIDTH 64;
//-----------------------------
//Namespace declaration-------
using namespace std;
//-------------------------------
class chip8
{
	public:
		/*
		Registers, necessary memory, etc.
		*/
		//Screen array
		unsigned char gfx[WINDOW_SIZE_X * WINDOW_SIZE_Y];
		//This will be 1 if the screen needs to draw
		unsigned char drawFlag;
		//16 8 bit registers
		//These are written to via Vx and Vy. Vx and Vy aren't a specific register, they are any of the 16
		unsigned char V[16];
		//4k 8 bit memory
		//most programs will start at memory location 512 (0x0200 in hex) because the interpreter on this hardware occupied the first 512 lines.
		unsigned char mem[4096];
		//The two timers that CHIP-8 has. They count down at 60 hertz
		unsigned char delay_timer;
		unsigned char sound_timer;
		//Index registre and program counter
		unsigned short I;
		unsigned short pc;					//this is storing the current "address" of the program
		//Program stack and stack pointer
		//implimenting this gives the program the ability to jump to subroutines. The current program address is stored here before the program jumps.
		//Only 16 levels of stack for this.
		unsigned short stack[16];
		short sp;
		//This stores the current opcode
		unsigned short opCode;
		//Key board (only 16 bytes)
		unsigned char key[16];
		//This will store the numeric values
		unsigned int pressedKey;
		//sleep variables
		int sleep;
		//establish screen class as vehicle for writing to screen
		Screen scrn;
		//establishes event class for keyboard events
		SDL_Event event;
		//fontset
		unsigned char chip8_fontset[80] =
		{
		  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		  0x20, 0x60, 0x20, 0x20, 0x70, // 1
		  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		  0xF0, 0x80, 0xF0, 0x80, 0x80  // F

		 
		  
		};
		//-----------------------------------------------------------------------------------------------------------------------
		void Initialize() {
			//This initializes all of the emulated hardware. clears all registers, etc.
			//incrementing ints
			int i = 0;
			drawFlag = 0;
			//clear v registers length of 16
			for (i = 0; i < 16;i++) {
				//This is the bitwise implimentation of the clear
				V[i] =0;	//0xFF is hex implimentation of 11111111
			}
			//clear the memory
			for (i = 0; i < 4096;i++) {
				mem[i] =0;	//0xFF is hex implimentation of 11111111. This is clearing all of the bits
			}
			//set the stack pointer to function properly using the first memory address available
			sp = -1;
			pc = 512; 
			//clear stack
			for (i = 0; i < 16; i++) {
				stack[i] = 0;
			}
			//load Fontset
			for (i = 0; i < 80; i++) {
				mem[i] = chip8_fontset[i];
			}
			//set timers
			delay_timer &=~ 0xFF;
			sound_timer &=~ 0xFF;
			//initializing the sleep timing
			sleep = 17;
		};
		//-----------------------------------------------------------------------------------------------------------------------
		void InitTesting() {
			//redefine function macros for sleeping so that the cycles run as fast as possible
			sleep = 0;
		}
		//-----------------------------------------------------------------------------------------------------------------------
		int LoadGame(string gameName) {
			//typedef basic_filebuf<char> filebuf;
			//Loads the binary file into the program memory area
			//use the fopen() in binary mode for this.
			int bufferSize = 0;
			int i = 0;
			int j = 0;
			int c;
			FILE* file = NULL;
			errno_t err;
			err = fopen_s(&file, &gameName[0], "rb");
			if (err == 0) {
				cout << "The file was opened properly" << endl;
			}
			do {
				c = fgetc(file);
				bufferSize++;
			} while (c != EOF);
			fclose(file);
			err = fopen_s(&file, &gameName[0], "rb");
			if (err == 0) {
				cout << "The file was opened properly" << endl;
			}
			for (i = 0; i < bufferSize - 1; i++)
			{
				c = fgetc(file);
				//load game into mem[]
				mem[512 + i] = c;
			}
			fclose(file);
			
			return 0;
		};
		//-----------------------------------------------------------------------------------------------------------------------
		int EmulateCycle() {
			//step through one cycle of the CHIP-8 hardware
			//get opCode
			//This just comes straight from the text file at the file location specified
			//decode opCode
			opCode = mem[pc] << 8 | mem[pc + 1];		//This line gets both bytes from the memory and makes them one two byte opcode
			//Might have to add a if statement here to make sure the opcode doesn't run if it is a zero, if that is a problem
			//preform opCode
			CallOp(opCode);
			//opCode = 0x0000;
			//updateTimers
			UpdateTimers();
			this_thread::sleep_for(chrono::milliseconds(sleep));
			return 0;
		};
		//-----------------------------------------------------------------------------------------------------------------------
		void UpdateTimers() {
			//decrement timers if not already at 0
			if (delay_timer > 0) {
				delay_timer--;
			}
			if (sound_timer > 0) {
				sound_timer--;
			}
		}
		//-----------------------------------------------------------------------------------------------------------------------
		int SetKeys() {
			//store key press state, (press and release)
			//if a key is pressed at the time this function is called.
			//loop through keyboard events stored
			while (SDL_PollEvent(&event)) {
				/*I'm worried this won't ever exit*/
				switch (event.type) {
					/* Keyboard event */
					/* Pass the event data onto PrintKeyInfo() */
				case SDL_KEYDOWN:
					//set the key value in arr key[] to 0x0F;
					StoreKey(&event.key);
					break;
				case SDL_KEYUP:

				default:
					break;
				}
			}
			return 0;
		}
		//------------------------------------------------------------------------------------------------------------------------
		void StoreKey( SDL_KeyboardEvent* keyE) {
			//I think this works because we are pointing to a struct and then to a type
			/*WEBSITE WITH THIS INFO:  */
			/*
			Telmac 1800 keyboard was a hexidecimal keyboard. I will be emulating that with the exact same keys to start.
					1800 keypad:		1800 Mapping:
					|A|B|C|D|			The keys will be mapped
					|*|1|2|3|			Directly to there equivalent
					| |4|5|6|			face value match
					|0|7|8|9|			A->A, B->B, C->C, etc.
					|   |E|F|
			****The key[] array will be incremented as though it is the keypad on the right. a will be key[0].
			* F will be key[15]


			*/
			int i = 0;
			for (i = 0; i < 16; i++) {
				key[i] &= ~0x0F;
			}
			switch (keyE->keysym.sym) {
				case SDLK_a:
					//cout << "a" << endl;
					key[0] |= 0x0F;
					break;
				case SDLK_b:
					//cout << "b" << endl;
					key[1] |= 0x0F;
					break;
				case SDLK_c:
					//cout << "c" << endl;
					key[2] |= 0x0F;
					break;
				case SDLK_d:
					//cout << "d" << endl;
					key[3] |= 0x0F;
					break;
				case SDLK_KP_1:
					//cout << "1" << endl;
					key[4] |= 0x0F;
					break;
				case SDLK_KP_2:
					//cout << "2" << endl;
					key[5] |= 0x0F;
					break;
				case SDLK_KP_3:
					//cout << "3" << endl;
					key[6] |= 0x0F;
					break;
				case SDLK_KP_4:
					//cout << "4" << endl;
					key[7] |= 0x0F;
					break;
				case SDLK_KP_5:
					//cout << "5" << endl;
					key[8] |= 0x0F;
					break;
				case SDLK_KP_6:
					//cout << "6" << endl;
					key[9] |= 0x0F;
					break;
				case SDLK_KP_0:
					//cout << "0" << endl;
					key[10] |= 0x0F;
					break;
				case SDLK_KP_7:
					//cout << "7" << endl;
					key[11] |= 0x0F;
					break;
				case SDLK_KP_8:
					//cout << "8" << endl;
					key[12] |= 0x0F;
					break;
				case SDLK_KP_9:
					//cout << "9" << endl;
					key[13] |= 0x0F;
					break;
				case SDLK_e:
					//cout << "e" << endl;
					key[14] |= 0x0F;
					break;
				case SDLK_f:
					//cout << "f" << endl;
					key[15] |= 0x0F;
					break;
				default:
					break;
			}
		}
		//------------------------------------------------------------------------------------------------------------------------
		void CallOp(unsigned short op) {
			//This will be the switch statement for all 35 opcodes
			//This will mostly be evaluated via the first letter of the opcode
			unsigned short add;
			unsigned char temp;
			int inc;
			int inc1 = 0;
			int inc2 = 0;
			//All descriptions come from the wikipedia page.
			switch (op & 0xF000) {
				case 0x0000:
					//----------------------------------------------------------------------------------------------------------------------------
					/*
					* WARNING: I am not adding 0x0NNN instruction in because I am not sure hop to identify it.
					
					//0NNN: Calls subroutine at NNN.
					//store address
					//write the opcode to the address
					add = op & 0x0FFF;
					cout << "address:";
					printShort2Hex(add);
					cout << endl;
					//store the address pointer
					/*
					* NOTE: This is the address of the jump instruction. This is incremented as it is pushed onto the stack. DO NOT increment it after it is popped off the stack
					
					PushStack(pc + 2);
					//store the jump to address in the current address.
					pc = add;
					break;*/
					cout << "No instruction" << endl;
					//another switch for all of the zero opcodes
					switch (op & 0x00FF) {
					case 0x00E0:
						//Clears the screen.
						ClearScreen();
						//increment Counter
						NextInstruction();
						break;
						//-------------------------------------------------------
					case 0x00EE:
						//Returns from a subroutine.
						//set program counter equal to the last address put in the stack
						pc = PopStack();
						//increment counter
						NextInstruction();
						break;
						//-------------------------------------------------------
					default:
						cout << "ERROR: There is an error with the current value of UNSIGNED SHORT opCode. This code cannot be processed." << endl;
						cout << op << endl;
						exit(0);
						break;
					};
					break;
				//----------------------------------------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------------------------------------
				case 0x1000:
					//1NNN: go to address NNN
					//write the opcode to the address
					add = op & 0x0FFF;
					//store the address pointer
					/*
					* NOTE: This is the address of the jump instruction. This is incremented as it is pushed onto the stack. DO NOT increment it after it is popped off the stack
					*/
					//store the jump to address in the current address.
					pc = add;
					break;
				//----------------------------------------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------------------------------------
				case 0x2000:
					//2NNN: Calls subroutine at NNN.
					//store address
					//write the opcode to the address
					add = op & 0x0FFF;
					//store the address pointer
					/*
					* NOTE: This is the address of the jump instruction. This is incremented as it is pushed onto the stack. DO NOT increment it after it is popped off the stack
					*/
					PushStack(pc);
					//store the jump to address in the current address.
					pc = add;
					break;
				//----------------------------------------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------------------------------------
				case 0x3000:
					//3XNN: Skips the next instruction if VX equals NN.
					//OpCode & 0x0F00 >> 8 isolates the 4 bit register identifier and bit shifts it to make it useful
					if (VX == (op & 0x00FF)) {
						SkipNextInstruction();
					}
					else {
						NextInstruction();
					}
					//usually the next instruction is a jumpto (used to skip code block)
					break;
				//----------------------------------------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------------------------------------
				case 0x4000:
					//4XNN: Skips the next instruction if VX doesn't equal NN
					if (VX != (op & 0x00FF)) {
						SkipNextInstruction();	//skip next instruction
					}
					else {
						NextInstruction();	//preform next instruction
					}
					//Usually the next instruction is a jump to skip a code block
					break;
				//----------------------------------------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------------------------------------
				case 0x5000:
					//5XY0: Skips the next instruction if VX equals VY
					if (VX == VY) {
						SkipNextInstruction();	//skip next instruction
					}
					else {
						NextInstruction();	//preform next instruction
					}
					//Usually the next instruction is a jump to skip a code block
					break;
				//----------------------------------------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------------------------------------
				case 0x6000:
					//6XNN: sets VX to NN
					VX = (0x00FF & op);
					//go to next instruction
					NextInstruction();
					break;
				//----------------------------------------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------------------------------------
				case 0x7000:
					//7XNN: Adds NN to VX. (Carry flag is not changed)
					VX += (0x00FF & op);
					//go to next instruction
					NextInstruction();
					break;
				//----------------------------------------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------------------------------------
				case 0x8000:
					//another switch for all of the 0x8xxx instructions
					switch (op & 0x000F) {
					case 0x0000:
						//0x8XY0: Sets VX to the value of VY.
						VX = VY;
						//iterate program
						NextInstruction();
						break;
						//-------------------------------------------------------
					case 0x0001:
						//0x8XY1: Sets VX to VX or VY. (Bitwise OR operation)
						VX = VX | VY;
						//iterate program
						NextInstruction();
						break;
						//-------------------------------------------------------
					case 0x0002:
						//0x8XY2: Sets VX to VX and VY. (Bitwise AND operation)
						VX = VX & VY;
						//move to next instruction
						NextInstruction();
						break;
						//-------------------------------------------------------
					case 0x0003:
						//0x8XY3: Sets VX to VX xor VY.
						VX = VX ^ VY;
						//move to next
						NextInstruction();
						break;
						//-------------------------------------------------------
					case 0x0004:
						//0x8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
						//check for overflow conditions
						VF = 0;
						if (VY > (CHAR_MAX - VX)) {/* `a + x` would overflow */
							//set VF flag
							VF =0x01;
							//subtract half of 256 from both (effectively subtracting 256 from the final product without creating an overflow along the way)
							//Add them
							//VX = (VX - (256 / 2)) + (VY - (256 / 2));
						}
						//Add VY to VX either way
						VX = VX + VY;
						NextInstruction();
						//set VF if there is a carry
						break;
						//-------------------------------------------------------
					case 0x0005:
						//0x8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
						DetectBorrowPBP(&VX, &VY, &VF);
						
						NextInstruction();
						break;
						//-------------------------------------------------------
					case 0x0006:
						//0x8XY6: Stores the least significant bit of VX in VF and then shifts VX to the right by 1
						//Setting the bit with an if statement to keep with the set standard
						VF = 0;
						if (VX & 0x01) {
							VF =0x01;
						}
						//bitshift to the right 1 bit. Dumping off the LSB.
						VX = VX >> 1;
						//next instruction
						NextInstruction();
						break;
						//-------------------------------------------------------
					case 0x0007:
						//0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
						//Variables
						
						//assigning variables
						temp = VY;
						while ((inc1 < 8) && !(temp & 0x80)) {
							temp = temp << 1;
							inc1++;
						}
						temp = VY - VX;
						while ((inc2 < 8) && !(temp & 0x80)) {
							temp = temp << 1;
							inc2++;
						}
						VF = 0x01;
						if (inc2 > inc1) {
							//There is a borrow so clear VF
							VF = 0;
						}
						//if DetectBorrow becomes pass by reference/pointer, we can set these numbers within the function
						VX = VY - VX;
						//next instruction
						NextInstruction();
						break;
						//-------------------------------------------------------
					case 0x000E:
						//0x8XYE: Stores the most significant bit of VX in VF and then shifts VX to the left by 1
						VF = 0;
						if (VX & 0x80) {
							//Store in VF
							VF=0x01;
						}
						VX = VX << 1;
						//next instruction
						NextInstruction();
						break;
						//-------------------------------------------------------
					default:
						cout << "ERROR: There is an error with the current value of UNSIGNED SHORT opCode. This code cannot be processed." << endl;
						cout << op << endl;
						exit(0);
						break;
					};
					break;
				//----------------------------------------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------------------------------------
				case 0x9000:
					//9XY0:Skips the next instruction if VX doesn't equal VY.
					if (VX != VY) {
						SkipNextInstruction();	//Skips the next instruction
					}
					else {
						NextInstruction();	//goes to the next instruction
					}
					//(Usually the next instruction is a jump to skip a code block)
					break;
				//----------------------------------------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------------------------------------
				case 0xA000:
					//ANNN: 	Sets I to the address NNN
					I = (0x0FFF & op);
					//increment program counter
					NextInstruction();
					break;
				//----------------------------------------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------------------------------------
				case 0xB000:
					//0xBNNN: Jumps to the address NNN plus V0
					pc = (op & 0x0FFF) + V[0];
					break;
				//----------------------------------------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------------------------------------
				case 0xC000:
					//CXNN: Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
					VX = (0x00FF & op) & (rand() % 256);
					NextInstruction();
					break;
				//----------------------------------------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------------------------------------
				case 0xD000:
					//DXYN: 	Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height
					//of N+1 pixels. Each row of 8 pixels is read as bit-coded starting from memory
					//location I; I value doesn’t change after the execution of this instruction. As
					//described above, VF is set to 1 if any screen pixels are flipped from set to
					//unset when the sprite is drawn, and to 0 if that doesn’t happen
					V[0x0F] = DrawSprite(V[(0x0F00 & op) >> 8]-1, V[(0x00F0 & op) >> 4], (op & 0x000F));
					NextInstruction();
					break;
				//----------------------------------------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------------------------------------
				case 0xE000:
					switch (op & 0x00FF) {
					case 0x009E:
						//if key pressed is equal to VX skip next line
						if (key[VX] == 0x0F) {
							SkipNextInstruction();
						}
						else {
							NextInstruction();
						}
						ClearKeys();
						break;
						//-------------------------------------------------------
					case 0x00A1:
						//if key pressed does not equal VX skip next line
						if (key[VX] != 0x0F) {
							SkipNextInstruction();
						}
						else {
							NextInstruction();
						}
						ClearKeys();
						break;
					};
					break;
				//----------------------------------------------------------------------------------------------------------------------------
				//----------------------------------------------------------------------------------------------------------------------------
				case 0xF000:
					//A nested switch for all of the F instructions
					switch (op & 0x00FF) {
						case 0x0007:
							//0xFX07:Sets VX to the value of the delay timer.
							VX = delay_timer;
							NextInstruction();
							break;
							//-------------------------------------------------------
						case 0x000A:
							/*
							This may be a problematic way of doing this action considering the way the keypad is organized
							*/
							//0xFX0A:A key press is awaited, and then stored in VX. 
							//(Blocking Operation. All instruction halted until next key event)
							if (CheckKeys() > 0) {
								//run for loop and store key pressed
								for (inc = 0; inc < 16; inc++) {
									if (key[inc] == 0x0F) {
										//store key pressed
										VX = inc;
										//jincrement the program counter
										ClearKeys();
										NextInstruction();
									}
								}
							}
							else {
								//do nothing this is the blocking code
							}
							break;
							//-------------------------------------------------------
						case 0x0015:
							//Sets the delay timer to VX.
							delay_timer = VX;
							NextInstruction();
							break;
							//-------------------------------------------------------
						case 0x0018:
							//Sets the sound timer to VX.
							sound_timer = VX;
							NextInstruction();
							break;
							//-------------------------------------------------------
						case 0x001E:
							//	Adds VX to I. VF is not affected.
							I = VX + I;
							NextInstruction();
							break;
							//-------------------------------------------------------
						case 0x0029:
							/*
							NOT COMPLETE: This will probably need to cooridinate with the Screen class.
							*/
							//Sets I to the location of the sprite for the character in VX. Characters 0-F 
							//I = mem[VX*5];
							I = VX * 5;
							NextInstruction();
							//(in hexadecimal) are represented by a 4x5 font.
							break;
							//-------------------------------------------------------
						case 0x0033:
							//Stores the binary - coded decimal representation of VX, with the most 
							//significant of three digits at the address in I, the middle digit at I 
							//plus 1, and the least significant digit at I plus 2. (In other words, 
							//take the decimal representation of VX, place the hundreds digit in memory 
							//at location in I, the 
							//tens digit at location I + 1, and the ones digit at location I + 2.)
							temp = VX;
							//setting the hundreds place
							mem[I] = temp / 100;
							//setting the tens place
							mem[I + 1] = (temp % 100) / 10;
							//setting the ones place
							mem[I + 2] = ((temp % 100) % 10);
							NextInstruction();
							break;
							//-------------------------------------------------------
						case 0x0055:
							//0xFX55: Stores V0 to VX (including VX) in memory starting at address I. The offset from 
							//I is increased by 1 for each value written, but I itself is left unmodified
							for (inc = 0; inc <= ((op & 0x0F00) >> 8); inc++) {
								mem[I + inc] = V[inc];
							}
							NextInstruction();
							break;
							//-------------------------------------------------------
						case 0x0065:
							//Fills V0 to VX (including VX) with values from memory starting at address I.  
							//The offset from I is increased by 1 for each value written, but I itself is left
							//unmodified.
							for (inc = 0; inc <= ((op & 0x0F00) >> 8); inc++) {
								V[inc] = mem[I + inc];
							}
							NextInstruction();
							break;
							//-------------------------------------------------------
						default:
							cout << "ERROR: There is an error with the current value of UNSIGNED SHORT opCode. This code cannot be processed." << endl;
							cout << op << endl;
							exit(0);
							break;
							//-------------------------------------------------------
					};
					break;
				default:
					cout << "ERROR: There is an error with the current value of UNSIGNED SHORT opCode. This code cannot be processed." << endl;
					cout << op << endl;
					exit(0);
					break;
			};
		};
		//----------------------------------------------------------------------------------------------------------------------
		void NextInstruction() {
			pc += 2;		//goes to next instruction start
		}
		void SkipNextInstruction() {
			pc += 4;		//
		}
		//-----------------------------------------------------------------------------------------------------------------------
		void PushStack(unsigned short currentAdd) {
			//pushes an address onto the address stack and 
			//increments the stack pointer
			sp++;
			//places the the new address on the stack
			stack[sp] = currentAdd;
		};
		//-----------------------------------------------------------------------------------------------------------------------
		/*
		*NOTE:What is returned from this function needs to be incremented AFTER it is popped off the stackand before it is executed upon
		*/
		unsigned short PopStack() {
			//pops the top address off of the stack
			unsigned short newAdd;
			newAdd = stack[sp];
			//Clear the bits of this array location
			stack[sp] = 0;
			//decrements the stack pointer
			sp--;
			return newAdd;			//This should work becasue I am doing pass by value not trying to return a pointer to this variable.
		};
		//----------------------------------------------------------------------------------------------------------------------------
		//This function is called and the variable addresses are passed to it.
		void DetectBorrowPBR(unsigned char &vx, unsigned char &vy, unsigned char &vf) {
			//Variables
			unsigned char temp;
			int inc1 = 0;
			int inc2 = 0;
			//assigning variables
			temp = vx;
			while (!(temp & 0x80)) {
				temp = temp << 1;
				inc1++;
			}
			temp = vx - vy;
			while (!(temp & 0x80)) {
				temp = temp << 1;
				inc2++;
			}
			vf = 0x01;
			if (inc2>inc1) {
				//There is a borrow so clear VF
				vf =0;
			}
			//if DetectBorrow becomes pass by reference/pointer, we can set these numbers within the function
			vx = vx - vy;
		}
		//----------------------------------------------------------------------------------------------------------------------------
		void DetectBorrowPBP(unsigned char* vx, unsigned char* vy, unsigned char* vf) {
			//Variables
			unsigned char temp;
			int inc1 = 0;
			int inc2 = 0;
			//assigning variables
			/*temp = *vx;
			while ((inc1<8)&&!(temp & 0x80)) {
				temp = temp << 1;
				inc1++;
			}
			temp = *vx - *vy;
			while ((inc2<8)&&!(temp & 0x80)) {
				temp = temp << 1;
				inc2++;
			}*/
			*vf = 0x01;
			if (*vx < *vy) {
				//There is a borrow so clear VF
				*vf =0;
			}
			//if DetectBorrow becomes pass by reference/pointer, we can set these numbers within the function
			*vx = *vx - *vy;
		}
		//-----------------------------------------------------------------------------
		int CheckKeys() {
			//iterates through arr keys[] and counts the number of keys pressed. It then returns these key values
			int numOfKeys = 0;
			int i = 0;
			for (i = 0; i < 16; i++) {
				if (key[i] == 0x0F) {
					numOfKeys++;
				}
			}
			return numOfKeys;
		}
		//-------------------------------------------------------------------------------
		void ClearKeys() {
			//clears the key array so that movement will stop
			for (int i = 0; i < 16; i++) {
				key[i] &= ~0xFF;
			}
		}
		//______________________________________________________________________________
		/*unsigned char DrawSpriteOld(unsigned char X, unsigned char Y, unsigned char i) {
			//preform the things described in the 0xDXXX opcode instruction
			int j = 0;
			int k = 0;
			int x = (int)X;
			int y = (int)Y;
			unsigned char collisionFlag = 0;
			unsigned char temp = 0x00;
			//the row
			for (j = 0; j < (int)i; j++) {
				//the column (8 pix wide)
				//setting the temp equal to appropriate memory location, getting ready for a bit shift
				temp = mem[I + j];
				for (k = 0; k < 8; k++) {
					//This will test the most significant bit first
					/*
					Kasey make sure this is the correct way to do this
					
					if (temp & 0x80) {
						//draw the pixel
						if (scrn.DrawPixel(x + k, y + j)) {
							collisionFlag =0x01;
						}
					}
					//left bitshifting temp
					temp = temp << 1;
				}
			}
			//if pixel goes to zero,
			return collisionFlag;
		}*/
		unsigned char DrawSprite(unsigned char X, unsigned char Y, unsigned char i) {
			//preform the things described in the 0xDXXX opcode instruction
			int j = 0;
			int k = 0;
			int x = (int)X;
			int y = (int)Y;
			unsigned char collisionFlag = 0;
			unsigned char temp = 0x00;
			//the row
			for (j = 0; j < (int)i; j++) {
				//the column (8 pix wide)
				//setting the temp equal to appropriate memory location, getting ready for a bit shift
				temp = mem[I + j];
				for (k = 0; k < 8; k++) {
					//This will test the most significant bit first
					/*
					Kasey make sure this is the correct way to do this
					*/
					if (temp & 0x80) {
						//draw the pixel
						//if the xor is equal to zero, set collision flag
						gfx[((y + j) * WINDOW_SIZE_X) + (x + k)] ^= 1;
						if ((gfx[((y+j)* WINDOW_SIZE_X)+(x+k)])==0) {
							collisionFlag = 0x01;
						}
					}
					//left bitshifting temp
					temp = temp << 1;
				}
			}
			//set draw flag
			drawFlag = 1;
			//if pixel goes to zero,
			return collisionFlag;
		}
		//----------------------------------------------------------------------------------
		void ClearScreen() {
			std::cout << "cleared" << std::endl;
			for (int i = 0; i < WINDOW_SIZE_X * WINDOW_SIZE_Y; i++) {
				gfx[i] = 0;
			}
			//set draw flag
			drawFlag = 1;
		}
		//------------------------------------------------------------------------------------
		void printShort2Hex(unsigned short var) {
			//This will print the hex value of a piece of short memory
			int buff = var;
			cout << hex << setfill('0') << setw(4) << buff << " ";
		};
		void printChar2Hex(unsigned char var) {
			int buff = var;
			cout << hex << setfill('0') << setw(2) << buff << " ";
		};
};

