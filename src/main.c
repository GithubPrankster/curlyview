#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define SWC SDL_WINDOWPOS_CENTERED
#define SDL_ERR() printf("[SDL2]: %s\n", SDL_GetError())
#define USAGE_PRT(arg) printf("USAGE: %s img.ext\n", arg)

#include "curly_bmp.h"
#include "curly_icon.h"
SDL_Texture *curly = NULL, *imagery = NULL;
SDL_Rect rect;
int failed_load = 0;
unsigned scaler = 1;

int main(int argc, char** argv)
{
	if(argc < 2){
		puts("[CRVW]: Specify an image, dummy!\n");
		USAGE_PRT(argv[0]);
		return -1;
	}

	if(argc == 3){
		int num = atoi(argv[2]);
		scaler = num <= 1 ? 1 : num % 8;
	}

	/* Easter Egg */
	if(!strncmp(argv[1], "quote", 6)){
		puts("[CRVW]: Pretty subtle, Quote.\n");
		USAGE_PRT(argv[0]);
		return -1;
	}

	
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
		SDL_ERR();
		return -1;
	}

	SDL_Window* win = SDL_CreateWindow("Curlyview", SWC, SWC, 1024, 1024, SDL_WINDOW_ALWAYS_ON_TOP);
	if(!win){
		SDL_ERR();
		SDL_Quit();
		return -1;
	}

	SDL_Renderer* rdr = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if(!rdr){
		SDL_ERR();
		SDL_DestroyWindow(win);
		SDL_Quit();
		return -1;
	}

	int w, h;
	uint8_t* img = stbi_load(argv[1], &w, &h, NULL, 4);
	if(!img){
		puts("[CRVW]: Should've specified an image that WORKS.");
		failed_load = 1;
		w = 1024, h = 1024;
	}else{
		SDL_Surface* surf = SDL_CreateRGBSurfaceFrom((void*)img, w, h, 32, 4 * w,
						0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
		imagery = SDL_CreateTextureFromSurface(rdr, surf);
		rect = (SDL_Rect){0, 0, w * scaler, h * scaler};

		SDL_FreeSurface(surf);
		stbi_image_free(img);
		SDL_SetWindowSize(win, w * scaler, h * scaler);
		char winname[256];
		strcpy(winname, "Curlyview - ");
		strcat(winname, argv[1]);
		SDL_SetWindowTitle(win, winname);
	}

	if(failed_load){
		int x, y;
		uint8_t* data = stbi_load_from_memory(bin2c_curly_rdr_png, sizeof(bin2c_curly_rdr_png), &x, &y, NULL, 4);

		SDL_Surface* surf = SDL_CreateRGBSurfaceFrom((void*)data, x, y, 32, 4 * x,
						0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
		curly = SDL_CreateTextureFromSurface(rdr, surf);
		rect = (SDL_Rect){0, h - (y * 3), x * 3, y * 3};

		SDL_FreeSurface(surf);
		stbi_image_free(data);
	}

	int z, q;
	uint8_t* data = stbi_load_from_memory(bin2c_curly_icon_png, sizeof(bin2c_curly_icon_png), &z, &q, NULL, 4);

	SDL_Surface* surf = SDL_CreateRGBSurfaceFrom((void*)data, z, q, 32, 4 * z,
					0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
	SDL_SetWindowIcon(win, surf);
	SDL_FreeSurface(surf);
	stbi_image_free(data);

	int run = 0;
	SDL_Event e;
	while(!run){
		while(SDL_PollEvent(&e)){
			if(e.type == SDL_QUIT)
				run = 1;
			if(e.type == SDL_KEYDOWN){
				switch(e.key.keysym.sym){
					case SDLK_ESCAPE: run = 1; break;
					default: break;
				}
			}
		}
		if(failed_load)
			SDL_SetRenderDrawColor(rdr, 0, 0xA0, 0x80, 0xFF);
		else
			SDL_SetRenderDrawColor(rdr, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(rdr);
		if(failed_load)
			SDL_RenderCopy(rdr, curly, NULL, &rect);
		else
			SDL_RenderCopy(rdr, imagery, NULL, &rect);
		SDL_RenderPresent(rdr);
	}
	if(failed_load)
		SDL_DestroyTexture(curly);
	else
		SDL_DestroyTexture(imagery);
	SDL_DestroyRenderer(rdr);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
