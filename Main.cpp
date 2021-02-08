//#include 
//#include   // OpenGL graphics and input
#include <iomanip>
#include "chip8.h" // Your cpu core implementation

chip8 myChip8;
void setupGraphics();
void setupInput();
void drawGraphics();
void printShort2Hex(unsigned short var);
void printChar2Hex(unsigned char var);
int main(int argc, char** argv)
{
    
    // Set up render system and register input callbacks
    setupGraphics();
    setupInput();

    // Initialize the Chip8 system and load the game into the memory  
    myChip8.Initialize();
    myChip8.LoadGame("Pong (1 player).ch8");

    //Testing components
    // Emulation loop
    for (;;)
    {
        // Emulate one cycle
        myChip8.EmulateCycle();

        // If the draw flag is set, update the screen
        if (myChip8.drawFlag)
            drawGraphics();

        // Store key press state (Press and Release)
        myChip8.SetKeys();

        //update timers
        /*
        These timers will count down by one each cycle. Make the program execute at 60 cycles/second
        */
    }

    return 0;
}
void setupGraphics() {
    /*
    Finish
    */
    //using graphics libraries to impliment the screen
    return;
}
void setupInput() {
    /*
    Finish
    */
    //using libraries to impliment the inputs
    return;
}
void drawGraphics() {
    //draw to the opened window.
    /*
    Finish
    */
    return;
}
void printShort2Hex(unsigned short var) {
    //This will print the hex value of a piece of short memory
    int buff = var;
    cout << hex << setfill('0') << setw(4) << buff << " ";

};
void printChar2Hex(unsigned char var) {
    int buff = var;
    cout << hex << setfill('0') << setw(2) << buff << " ";
};