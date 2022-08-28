#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include <SDL2/SDL.h>

#define FPS 60
#define TPF 1000/FPS
#define WIN_W 640
#define WIN_H 480

/*
    NOTE: To speed up this program it would surely be necessary to do the work in a texture and not directly on the renderer.
*/

/* Typedefs */

typedef struct _Clock      Clock;
typedef struct _MandalaPix MandalaPix;

/* Clock struct */

struct _Clock
{
    uint32_t last_tick;
    uint32_t delta_ms;
};

Clock Clock_Init()
{
    Clock clock;

    clock.last_tick = 0;
    clock.delta_ms  = 0;

    return clock;
}

void Clock_Update(Clock* clock)
{
    uint32_t act_tick = SDL_GetTicks();
    clock->delta_ms   = act_tick - clock->last_tick;
    clock->last_tick  = act_tick;
}

/* MandalaPix struct */

struct _MandalaPix
{
    float x, y;
    float p1, p1o, p2;
    float phase;
};

void MandalaPix_SetValues(MandalaPix* pix)
{
    pix->x = 120.f;
    pix->y = 240.f;

    pix->p1 = pix->p1o = rand() % (21-10) + 10;     // between 10 and 20 (p1o is p1 origin)
    pix->p2 = -((float)rand()/(float)RAND_MAX);     // between 0 and -0.999...

    pix->phase = 0.f;
}

MandalaPix MandalaPix_Create()
{
    MandalaPix pix;
    MandalaPix_SetValues(&pix);
    return pix;
}

/* Function for "general usage" */

int CalculatePitch(uint32_t format, int width)
{
    int pitch;

    if (SDL_ISPIXELFORMAT_FOURCC(format) || SDL_BITSPERPIXEL(format) >= 8)
         pitch = (width * SDL_BYTESPERPIXEL(format));
    else pitch = ((width * SDL_BITSPERPIXEL(format)) + 7) / 8;

    pitch = (pitch + 3) & ~3;

    return pitch;
}

void GetRGBA (uint32_t const colour, uint8_t* const r, uint8_t* const g, uint8_t* const b, uint8_t* const a)
{
    *r = colour & 0xFF;
    *g = (colour >> 8) & 0xFF;
    *b = (colour >> 16) & 0xFF;
    *a = colour >> 24;
}

void RenderClearToBlack(SDL_Renderer* ren)
{
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
    SDL_RenderClear(ren);
}

uint8_t randColorByte()
{
    return rand() % 256;
}

/* Main program */

int main(int argc, char** argv)
{
    (void) argc; (void) argv;

    /* SDL window initialization */

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "ERROR: %s", SDL_GetError());
        return -1;
    };

    SDL_Window* win = SDL_CreateWindow("Mandala", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_W, WIN_H, 0);

    if (!win)
    {
        fprintf(stderr, "ERROR: %s", SDL_GetError());
        return -1;
    }

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    if (!ren)
    {
        fprintf(stderr, "ERROR: %s", SDL_GetError());
        SDL_DestroyWindow(win); return -1;
    }

    SDL_Event event;

    /* Program initialization */

    Clock clock = Clock_Init();

    srand(time(NULL));

    MandalaPix pix = MandalaPix_Create();

    const SDL_Rect screen_aera = { 0, 0, WIN_W, WIN_H };

    uint32_t* pixels_data = malloc(sizeof(uint32_t[WIN_W * WIN_H])); // Will contain rendered content in order to redisplay it

    const int pixels_pitch = CalculatePitch(SDL_PIXELFORMAT_RGBA32, WIN_W);

    RenderClearToBlack(ren);

    SDL_bool running = SDL_TRUE;

    /* Program execution */

    while (running)
    {
        Clock_Update(&clock);

        /* SDL event handling */

        while (SDL_PollEvent(&event))
        {
            running = event.type != SDL_QUIT; // Close program if requested

            if (event.type == SDL_MOUSEBUTTONDOWN) // Generate a new mandala
            {
                MandalaPix_SetValues(&pix);
                RenderClearToBlack(ren);
            }
        }

        /* Update program */

        pix.x += sin(pix.phase) * pix.p1;
        pix.y += cos(pix.phase) * pix.p1;

        pix.p1 += pix.p2;
        pix.phase += 0.1f;

        if (pix.p1 < 0 || pix.p1 > pix.p1o)
        {
            pix.p2 = -pix.p2;
        }

        /* Draw pixel */

        SDL_SetRenderDrawColor(ren, randColorByte(), randColorByte(), randColorByte(), 255);

        SDL_RenderDrawPoint(ren, pix.x, pix.y);

        /* Copy of the rendering to redisplay it after the next 'RenderClear', this avoids display glitches */

        SDL_RenderReadPixels(ren, &screen_aera, SDL_PIXELFORMAT_RGBA32, pixels_data, pixels_pitch);

        /* Render present / Render clear / Rewrite back render */

        SDL_RenderPresent(ren);

        RenderClearToBlack(ren);

        for (int y = 0; y < WIN_H; y++) // This step is very slow
        {
            for (int x = 0; x < WIN_W; x++)
            {
                uint8_t r, g, b, a;
                GetRGBA(pixels_data[y * WIN_W + x], &r, &g, &b, &a);
                SDL_SetRenderDrawColor(ren, r, g, b, a);
                SDL_RenderDrawPoint(ren, x, y); // The slowest
            }
        }

        /* Limit CPU usage */

        if (SDL_GetTicks() < (clock.last_tick + TPF))
            SDL_Delay((clock.last_tick + TPF) - SDL_GetTicks());
    }

    /* Closing the program */

    free(pixels_data);

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}
