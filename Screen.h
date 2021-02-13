#pragma once
#include <iostream>
#include "SDL.h"
#define WINDOW_DELTA_X 8 //translates to pixel width
#define WINDOW_DELTA_Y 8 //translates to pixel height
#define WINDOW_SIZE_X 64 //screen size (tarmac)
#define WINDOW_SIZE_Y 32
class Screen
{
	public:
		//This class will be used to impliment any of the graphical information as well as the graphics module
		//This array will be implimented using 1 or 0 for the pixel value. Total of 2048 pixels
		//The pixels are XOR'd when writing to them so if a pixel goes from 1 to 0, the collision flag is set.
		//unsigned char gfx[WINDOW_SIZE_X * WINDOW_SIZE_Y];
		SDL_Window* window = NULL;
		SDL_Surface* screenSurface = NULL;
		SDL_Renderer* renderer = NULL;	//This is used to actually draw stuff to the window
		//-----------------------------------------------------------------------------------------------------------------------
		int InitScreen() {
			//initializing the screen to be used
			//Set a video mode 
			//These objects are initialized on the stack
			/*SDL_Window* window = NULL;
			SDL_Surface* screenSurface = NULL;
			SDL_Renderer* renderer = NULL;	//This is used to actually draw stuff to the window
			*/
			window = SDL_CreateWindow("Telmac 1800 Emulator",
				SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				WINDOW_SIZE_X * WINDOW_DELTA_X, WINDOW_SIZE_Y * WINDOW_DELTA_Y, 0);
			renderer = SDL_CreateRenderer(window, -1, 0);
			if (window == NULL) {
				fprintf(stderr, "Window could not be created: %s\n", SDL_GetError());
				return 1;
			}

			screenSurface = SDL_GetWindowSurface(window);

			if (!screenSurface) {
				fprintf(stderr, "Screen surface could not be created: %s\n", SDL_GetError());
				SDL_Quit();
				return 1;
			}
			//init renderer

			return 0;
		};
		//This will be fed the x and y coordinates of the pixel. the function will translate that to a group of pixels
		/*int DrawPixel(int x, int y) {
			int i = 0;
			int j = 0;
			gfx[y * WINDOW_SIZE_X + x] ^= 1;
			if (gfx[y * WINDOW_SIZE_X + x] == 0) {
				//clear the pixel
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				for (i = x * WINDOW_DELTA_X; i < x * WINDOW_DELTA_X + WINDOW_DELTA_X; i++) {
					//this will iterate from the left to the right side of the pixel
					for (j = y * WINDOW_DELTA_Y; j < y * WINDOW_DELTA_Y + WINDOW_DELTA_Y; j++) {

						SDL_RenderDrawPoint(renderer, i, j);
					}
				}
				SDL_RenderPresent(renderer);
				return 1;
			}
			else {
				//write the pixel
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
				for (i = x * WINDOW_DELTA_X; i < x * WINDOW_DELTA_X + WINDOW_DELTA_X; i++) {
					//this will iterate from the left to the right side of the pixel
					for (j = y * WINDOW_DELTA_Y; j < y * WINDOW_DELTA_Y + WINDOW_DELTA_Y; j++) {

						SDL_RenderDrawPoint(renderer, i, j);
					}
				}
				SDL_RenderPresent(renderer);
				return 0;
			}
		}*/
		void DrawScreen(unsigned char *gfx) {
			int x = 0;
			int y = 0;
			int i = 0;
			int j = 0;
			for (y = 0; y < 32; y++) {
				for (x = 0; x < 64; x++) {
					if (gfx[y * WINDOW_SIZE_X + x] == 0) {
						//clear the pixel
						SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
						for (i = x * WINDOW_DELTA_X; i < x * WINDOW_DELTA_X + WINDOW_DELTA_X; i++) {
							//this will iterate from the left to the right side of the pixel
							for (j = y * WINDOW_DELTA_Y; j < y * WINDOW_DELTA_Y + WINDOW_DELTA_Y; j++) {

								SDL_RenderDrawPoint(renderer, i, j);
							}
						}
					}
					else {
						//std::cout << "set" << std::endl;
						//write the pixel
						SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
						for (i = x * WINDOW_DELTA_X; i < x * WINDOW_DELTA_X + WINDOW_DELTA_X; i++) {
							//this will iterate from the left to the right side of the pixel
							for (j = y * WINDOW_DELTA_Y; j < y * WINDOW_DELTA_Y + WINDOW_DELTA_Y; j++) {
								SDL_RenderDrawPoint(renderer, i, j);
							}
						}
					}
				}
			}
			SDL_RenderPresent(renderer);
			return;
		}
		void ClearScreen() {
			std::cout << "cleared" << std::endl;
			//clears the screen when called
			int i = 0;
			int j = 0;
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			for (i = 0; i < WINDOW_DELTA_X*WINDOW_SIZE_X; i++) {
				for (j = 0; j < WINDOW_DELTA_Y * WINDOW_SIZE_Y; j++) {
					//write zero to the screen
					SDL_RenderDrawPoint(renderer, i, j);
				}
			}
			//This is something that could be consolidated into the previous loop
			for (i = 0; i < WINDOW_SIZE_X*WINDOW_SIZE_Y; i++) {
				//clear the gfx array
				//gfx[i] =0;
			}
			SDL_RenderPresent(renderer);
			return;
		}
};

