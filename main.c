#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_gfxPrimitives.h>

#define SCREEN_WIDTH		(320)
#define SCREEN_HEIGHT		(240)
#define SCREEN_BPP			(32)

SDL_Surface *screen = NULL;
int fps = 0;
bool fps_on = false;

Uint32 getPixel(SDL_Surface *s, int x, int y)
{
	int i = y * s->w + x;
	Uint32 *p = (Uint32*)s->pixels;
	return p[i];
}

void setPixel(SDL_Surface *s, int x, int y, Uint32 c)
{
	int i = y * s->w + x;
	Uint32 *p = (Uint32*)s->pixels;
	p[i] = c;
}

void fps_counter(double dt)
{
	static double total = 0;
	static int count = 0;
	total += dt;
	++count;
	if (total > 1.0)
	{
		fps = count;
		total -= 1.0;
		count = 0;
	}
}

void fps_draw(void)
{
	char string[8] = "";
	sprintf(string, "%d", fps);
	stringRGBA(screen, 0, 0, string, 255, 255, 255, 255);
}

int main(int argc, char *argv[])
{
	srand(time(NULL));
	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);
	//SDL_EnableKeyRepeat(100, 50);
	SDL_ShowCursor(SDL_DISABLE);

    SDL_Surface *palette = IMG_Load("palette.png");
    palette = SDL_DisplayFormat(palette);
    SDL_Surface *noise = IMG_Load("noise2.png");
    noise = SDL_DisplayFormat(noise);

    Uint8 rc1 = rand() % 40 + 8;
    Uint8 rc2 = rand() % 40 + 8;
    Uint8 rc3 = rand() % 25 + 8;
    Uint8 rc4 = rand() % 25 + 8;
    Uint8 rc5 = rand() % 25 + 8;
    Uint8 rc6 = rand() % 25 + 8;
    Uint8 rc7 = rand() % 25 + 8;
    Uint8 map[SCREEN_HEIGHT][SCREEN_WIDTH];
    for (int y = 0; y < SCREEN_HEIGHT; ++y)
        for (int x = 0; x < SCREEN_WIDTH; ++x)
        {
            Uint32 n = getPixel(noise, x, y);
            Uint8 nr, ng, nb, nv;
            SDL_GetRGB(n, noise->format, &nr, &ng, &nb);
            nv = (nr + ng + nb) / 3;
            double nx = x + nv / rc1;
            double ny = y + nv / rc2;
            int xx = nx - SCREEN_WIDTH / 2;
            int yy = ny - SCREEN_HEIGHT / 2;
            Uint8 v = (128.0 + 128.0 * sin(nx / rc3) +
                    128.0 + 128.0 * sin(ny / rc4) +
                    128.0 + 128.0 * sin((nx + ny) / rc5) +
                    128.0 + 128.0 * sin(sqrt(nx * nx + ny * ny) / rc6) +
                    128.0 + 128.0 * sin(sqrt(xx * xx + yy * yy) / rc7)
                    ) / 5;
            map[y][x] = v;
        }

	bool quit = false;
	Uint32 curr = SDL_GetTicks();
	Uint32 prev = curr;
    Uint32 total_time = 0;
	while (!quit)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_RETURN:
							fps_on = !fps_on;
							break;
						case SDLK_ESCAPE:
							quit = true;
							break;
					}
					break;
				case SDL_QUIT:
					quit = true;
					break;
			}
		}
		curr = SDL_GetTicks();
		Uint32 delta = curr - prev;
		prev = curr;
		double dt = delta / 1000.0;
		fps_counter(dt);

        // processing
        total_time += delta;
        int tt = total_time * 35 / 1000;

		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
		SDL_LockSurface(screen);
        for (int y = 0; y < SCREEN_HEIGHT; ++y)
            for (int x = 0; x < SCREEN_WIDTH; ++x)
            {
                Uint32 c = getPixel(palette, (map[y][x] + tt) % 256, 0);
                setPixel(screen, x, y, c);
                //setPixel(screen, x, y, SDL_MapRGB(screen->format, map[y][x], map[y][x], map[y][x]));
            }
		SDL_UnlockSurface(screen);
		if (fps_on)
			fps_draw();
		SDL_Flip(screen);
	}
	SDL_Quit();
	return 0;
}
