//#include 
//#include   // OpenGL graphics and input

#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <thread>
#include <cstdlib>

#include "chip8.h" // Your cpu core implementation
#define TEST_FLAG 0 //This controls whether the program will be running benchmarks on the program as it runs
#define TEST_CYCLES 3000
Screen scrn;
chip8 myChip8;
void InitTesting();
void setupGraphics();
void drawGraphics(unsigned char *gfx);
void printShort2Hex(unsigned short var);
void printChar2Hex(unsigned char var);

int main(int argc, char** argv)
{
    bool testFlag = TEST_FLAG;
    // Set up render system and register input callbacks
    setupGraphics();

    // Initialize the Chip8 system and load the game into the memory  
    myChip8.Initialize();
    myChip8.LoadGame("Pong (1 player).ch8");

    //Testing components
    
    //check if doing benchmarks
    if (testFlag) {
        //record benchmark start time
        auto start = std::chrono::steady_clock::now();
        //establish cycles
        int cycles = TEST_CYCLES;
        InitTesting();
        for (int i = 0; i < cycles; i++) {
            // Emulate one cycle
            myChip8.EmulateCycle();

            // If the draw flag is set, update the screen
            if (myChip8.drawFlag) {
                drawGraphics(&myChip8.gfx[0]);
                myChip8.drawFlag = 0;
            }
            // Store key press state (Press and Release)
            myChip8.SetKeys();

            //update timers
            /*
            These timers will count down by one each cycle. Make the program execute at 60 cycles/second
            */
        }
        auto end = std::chrono::steady_clock::now();
        std::cout << "The program ran " << cycles << " cycles in " << std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() << " Microseconds" << std::endl;
    }
    else {
        // Emulation loop
        for (;;)
        {
            // Emulate one cycle
            myChip8.EmulateCycle();

            // If the draw flag is set, update the screen
            if (myChip8.drawFlag) {
                drawGraphics(&myChip8.gfx[0]);
                myChip8.drawFlag = 0;
            }
            // Store key press state (Press and Release)
            myChip8.SetKeys();

            //update timers
            sleep(.01);
            /*
            These timers will count down by one each cycle. Make the program execute at 60 cycles/second
            */
        }
     }
    return 0;
}
void setupGraphics() {
    //initialize screen object
    /* Initialise SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not initialise SDL: %s\n", SDL_GetError());
        exit(-1);
    }
    //initialize the screen by calling initialize function
    if (scrn.InitScreen() == 1) {
        std::cout << "Could not initialize screen. Fatal program error. exiting...." << std::endl;
        exit(-1);
    }
    //clear screen
    scrn.ClearScreen();
    for (int i = 0; i < WINDOW_SIZE_Y * WINDOW_SIZE_X; i++) {
        myChip8.gfx[i] = 0;
    }
    return;
}
void setupInput() {
    /*
    Finish
    To be used to give the user the option to play several different preloaded games. As it stands right now the user has to manually add Pong to the project folder
    And add it to the file reader file
    */
    //using libraries to impliment the inputs
    return;
}
void drawGraphics(unsigned char *gfx) {
    //draw to the opened window.
    scrn.DrawScreen(gfx);
    return;
}
void InitTesting() {
    //call the myChip8 test initializing
    myChip8.InitTesting();
    return;
}
//Used for debugging
void printShort2Hex(unsigned short var) {
    //This will print the hex value of a piece of short memory
    int buff = var;
    std::cout << std::hex << std::setfill('0') << std::setw(4) << buff << " ";

};
//Used for debugging
void printChar2Hex(unsigned char var) {
    int buff = var;
    std::cout << std::hex << std::setfill('0') << std::setw(2) << buff << " ";
};
