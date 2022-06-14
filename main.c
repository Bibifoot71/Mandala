#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <SDL.h>

// "essential" values

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

unsigned int a_tick = 0;
unsigned int b_tick = 0;
unsigned int dt = 0;

SDL_Event events;
SDL_bool run = SDL_TRUE;

// program values:

double pix_x,
       pix_y,
       pix_p1,
       pix_p1o,
       pix_p2,
       pseudo_timer,
       dt_multiply,
       dt_m_alt;

SDL_bool pix_pr;

int nb_init = 0;

// functions:

uint8_t randColorByte() {return rand() % (256-10) + 100;}

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

void init_values()
{

    pix_x = 120; pix_y = 240;
    pix_p1 = rand() % (21-10) + 10;             // between 10 and 20
    pix_p1o = pix_p1;                           // origin value
    pix_p2 = (double)rand() / (double)RAND_MAX; // between 0 and 0.999...
    pix_pr = SDL_FALSE;                         // reverse "projection"
    pseudo_timer = .0;

    dt_multiply = rand() % (12-10) + 10;
    if (dt_multiply==11) {dt_multiply = 20; pix_x = 240;}

}

// program:

int main( int argc, char* args[] )
{

    SDL_Window* window     = NULL;
    SDL_Renderer* renderer = NULL;

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }

    else
    {
        window = SDL_CreateWindow( "Mandala - write by Le Juez Victor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        
        if( window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        }

        else
        {

            renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);

            dt_m_alt=fibonacci(7)*8; // alternative dt multiplicator (value that I found "randomly")

            srand(time(NULL));  // init for rand call
            init_values();      // init program values

            while(run) // mandala program
            {

                while (SDL_PollEvent(&events))
                {
                    switch(events.type)
                    {

                        case SDL_WINDOWEVENT:
                            if (events.window.event == SDL_WINDOWEVENT_CLOSE)
                                run = SDL_FALSE; break;

                        case SDL_MOUSEBUTTONDOWN: // new mandala
                            SDL_SetRenderDrawColor(renderer, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0, (uint8_t) 255);
                            SDL_RenderClear(renderer); init_values(); break;

                    }
                }

                a_tick = SDL_GetTicks();
                dt = a_tick - b_tick;

                if (dt>1000/60.0)
                {
                    b_tick = a_tick;

                    // update

                    pix_x += sin(pseudo_timer)*pix_p1*2;
                    pix_y += cos(pseudo_timer)*pix_p1*2;
                    pseudo_timer += (dt*dt_multiply);

                    if      ( pix_pr == SDL_FALSE && pix_p1 > 0 )       pix_p1 -= pix_p2;
                    else if ( pix_pr == SDL_TRUE && pix_p1 < pix_p1o )  pix_p1 += pix_p2;

                    else{if      ( pix_pr == SDL_FALSE ) pix_pr = SDL_TRUE;
                         else if ( pix_pr == SDL_TRUE )  pix_pr = SDL_FALSE;}

                    // draw

                    SDL_SetRenderDrawColor(renderer, randColorByte(), randColorByte(), randColorByte(), (uint8_t) 255);
                    SDL_RenderDrawPoint(renderer, (int) pix_x, (int) pix_y);
                    SDL_RenderPresent(renderer);
                }

            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
