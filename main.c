#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include <SDL2/SDL.h>

#define WIN_W 640
#define WIN_H 480

/* TYPE/STRUCT FOR PROGRAM */

typedef struct _mandala_pix mandala_pix;

struct _mandala_pix {
    double x, y,
    p1, p1o, p2,
    pseudo_timer;
    SDL_bool pr;
};

void set_mandala_pix_values (mandala_pix* pix)
{
    pix->x   = 120;
    pix->y   = 240;

    pix->p1  = rand() % (21-10) + 10;             // between 10 and 20
    pix->p1o = pix->p1;                           // origin value
    pix->p2  = (double)rand() / (double)RAND_MAX; // between 0 and 0.999...
    pix->pr  = SDL_FALSE;                         // reverse "projection"

    pix->pseudo_timer = .0;
}

mandala_pix* create_mandala_pix ()
{
    mandala_pix* pix = malloc(sizeof(mandala_pix));
    set_mandala_pix_values(pix);
    return pix;
}

/* FUNCTIONS FOR PROGRAM */

void set_black_screen(SDL_Renderer* ren)
{
    SDL_SetRenderDrawColor(ren, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 255);
    SDL_RenderClear(ren);
}

uint8_t randColorByte()
{
    return rand() % (256-10) + 100;
}

/* Personnaly used for experimenting but not used in the final program.

double fibonacci(int toDec)
{
    double U2,U1,U0;
    double phi = 0.0;
    U1 = 0; U0 = 1;

    for (int i=0; i <= toDec; i++)
    {
        U2=U1+U0;
        U0 =U1;
        U1 =U2;
    }

    phi = U1/U0;

    return phi;
}

*/

/* PROGRAM */

int main (int argc, char** argv)
{
    (void) argc; (void) argv; // Because unused

    /* SDL window initialization */

    if (SDL_Init (SDL_INIT_VIDEO) < 0) { printf("ERROR: %s", SDL_GetError()); return 1; };

    SDL_Window* win = SDL_CreateWindow ("Mandala", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_W, WIN_H, 0);
    if (!win) { printf("ERROR: %s", SDL_GetError()); return 1; }

    SDL_Renderer* ren = SDL_CreateRenderer (win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Event event;

    /* Program initialization */

    unsigned int dt = 0,      // Ticks values is used for calculation of delta time
                 a_tick = 0,
                 b_tick = 0;

    srand(time(NULL)); // init for rand call

    mandala_pix* pix = create_mandala_pix();

    SDL_bool running = SDL_TRUE;

    set_black_screen(ren);

    /* Program execution */

    while (running)
    {

        /* SDL event handling */

        while (SDL_PollEvent(&event))
        {

            running = event.type != SDL_QUIT; // Close program if requested

            if (event.type == SDL_MOUSEBUTTONDOWN) // Generate a new mandala
            {
                set_mandala_pix_values(pix);
                set_black_screen(ren);
            }

        }

        /* Delta Time calculation and display according to it */

        a_tick = SDL_GetTicks();
        dt = a_tick - b_tick;

        if (dt>1000/60.0)
        {
            b_tick = a_tick;

            /* Update program */

            pix->x += sin(pix->pseudo_timer) * pix->p1;
            pix->y += cos(pix->pseudo_timer) * pix->p1;

            pix->pseudo_timer += .1;

            if (pix->pr == SDL_FALSE)
            {
                if (pix->p1 > 0)
                     pix->p1 -= pix->p2;
                else pix->pr  = SDL_TRUE;
            }
            else if (pix->pr == SDL_TRUE)
            {
                if (pix->p1 < pix->p1o)
                     pix->p1 += pix->p2;
                else pix->pr  = SDL_FALSE;
            }

            /* Draw program */

            SDL_SetRenderDrawColor(ren, randColorByte(), randColorByte(), randColorByte(), (uint8_t) 255);
            SDL_RenderDrawPoint(ren, (int) pix->x, (int) pix->y);
            SDL_RenderPresent(ren);
        }
    }

    /* Closing the program */

    SDL_DestroyRenderer (ren);
    SDL_DestroyWindow   (win);

    free(pix);
    SDL_Quit();

    return 0;
}
