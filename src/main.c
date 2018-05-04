#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <math.h>

#define GRID_W 256
#define GRID_H 256
#define CELL_SIZE 1

#define N 16
#define T 1

typedef struct COLOR {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} color;

//Accepts a hue (0-255) and returns an RGB color
color h_to_rgb(int h) {
	unsigned char c[3];
	for (int i=0; i<3; i++) {
		if (h >= 360)
			h = h-360;

		if (h < 60) {
			c[i] = 255;
		} else if (h < 120) {
			double x = (double)(h-60)/60;
			c[i] = 255 - (char)(255*x);
		} else if (h < 240) {
			c[i] = 0;
		} else if (h < 300) {
			double x = (double)(h-240)/60;
			c[i] = 0 + (char)(255*x);
		} else if (h < 360) {
			c[i] = 255;
		}

		h += 240;
	}

	color cl = {c[0], c[1], c[2]};
	return cl;
}

//Return number of neighbors with state 1 greater than own
int nb_count(int **g, int x, int y) {
	int c = 0;

	for (int i=x-1; i<=x+1; i++) {
		if (i == x) continue;
		if (i < 0 || i > GRID_W-1) continue;
		if ((g[x][y]+1) % (N) == g[i][y])
			c++;
	}
	for (int j=y-1; j<=y+1; j++) {
		if (j == y) continue;
		if (j < 0 || j > GRID_H-1) continue;
		if ((g[x][y]+1) % (N) == g[x][j])
			c++;
	}

	return c;
}

int main(int argc, char **argv) {
	//Initialize grid
	srand(time(NULL));
	int **grid = malloc(sizeof(int*)*GRID_W);
	for (int i=0; i<GRID_W; i++) {
		grid[i] = malloc(sizeof(int)*GRID_H);
		for (int j=0; j<GRID_H; j++)
			grid[i][j] = rand() % (N);
	}

	//Initialize comparison grid
	int **c_grid = malloc(sizeof(int*)*GRID_W);
	for (int i=0; i<GRID_H; i++)
		c_grid[i] = malloc(sizeof(int)*GRID_H);

	//Initialize colors
	color tcolor[N];
	for (int i=0; i<N; i++) {
		tcolor[i] = h_to_rgb(i*(360/N));
	}

	//Initialize SDL
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *win = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			GRID_W*CELL_SIZE, GRID_H*CELL_SIZE, SDL_WINDOW_SHOWN);
	SDL_Renderer *rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

	//Main loop
	int state = 1;
	int frame = 0;
	int time_s, time_e;
	while (state) {
		time_s = SDL_GetTicks();

		//Poll events
		SDL_Event e;
		while (SDL_PollEvent(&e)) {	
			switch (e.type) {
				case SDL_QUIT:
					state = 0;
					break;
				default:
					break;
			}
		}

		//Update
		for (int i=0; i<GRID_H; i++)
			memcpy(c_grid[i], grid[i], sizeof(int)*GRID_H);

		for (int i=0; i<GRID_W; i++) {
			for (int j=0; j<GRID_W; j++) {
				int nbc = nb_count(c_grid, i, j);
				if (nbc >= T) {
					grid[i][j] = ((grid[i][j]+1) % N);
				}
			}
		}

		//Draw
		for (int i=0; i<GRID_W; i++) {
			for (int j=0; j<GRID_H; j++) {
				int val = grid[i][j];
				color c = tcolor[val];
				SDL_SetRenderDrawColor(rend, c.r, c.g, c.b, 255);
				SDL_Rect r;
				r.x = i*CELL_SIZE;
				r.y = j*CELL_SIZE;
				r.w = CELL_SIZE;
				r.h = CELL_SIZE;
				SDL_RenderFillRect(rend, &r);
			}
		}
		SDL_RenderPresent(rend);

		//Delay
		frame++;
		time_e = SDL_GetTicks() - time_s;
		if (time_e < 30)
			SDL_Delay(30-time_e);
	}

	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);

	SDL_Quit();
	return 0;
}
