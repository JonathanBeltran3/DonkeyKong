//The headers
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "includes/mario.h"

//The attributes of the screen
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;

//The surfaces that will be used
SDL_Surface *screen = NULL;

SDL_Surface *load_image( char* filename )
{
    //Temporary storage for the image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized image that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = SDL_LoadBMP( filename );

    //If nothing went wrong in loading the image
    if( loadedImage != NULL )
    {
        //Create an optimized image
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old image
        SDL_FreeSurface( loadedImage );
    }
    else
    {
        printf("Cannot find: %s\n", filename);
    }

    //Return the optimized image
    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination )
{
    //Make a temporary rectangle to hold the offsets
    SDL_Rect offset;

    //Give the offsets to the rectangle
    offset.x = x;
    offset.y = y;

    //Blit the surface
    SDL_BlitSurface( source, NULL, destination, &offset );
}

struct Rectangle { int minx, maxx, miny, maxy; };
struct Rectangle sol_rect;
struct Rectangle ech_rect;

int mariow;

// width of mario image!
int isAllowed(int x, int y) {
    //printf("%i %i %i %i %i %i\n", sol_rect.minx, x, sol_rect.maxx, sol_rect.miny, y, sol_rect.maxy);
    if ((sol_rect.minx <= x && x + mariow <= sol_rect.maxx && sol_rect.miny <= y && y <= sol_rect.maxy) ||
        (ech_rect.minx <= x && x + mariow <= ech_rect.maxx && ech_rect.miny <= y && y <= ech_rect.maxy))
        return 1;
    else
        return 0;
}

void reshape(){
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

int main( int argc, char* args[] )
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_VIDEO ) == -1 )
    {
        fprintf(stderr, "Impossible d'initialiser la SDL");
        return EXIT_FAILURE;
    }

    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );
    if( screen == NULL )
    {
        fprintf(stderr, "Impossible d'ouvrir la fenetre");
        return EXIT_FAILURE;
    }

    //Set the window caption
    SDL_WM_SetCaption( "Donkey Kong", NULL );

    //Creation des éléments
    Mario player = createMario(0, -1, 1, 0.2, 0.2, 1, 0.2, 0.2);

    //Load the images
    SDL_Surface* mario = load_image("img/mario.bmp" );
    Uint32 colorkey = SDL_MapRGB(mario->format, 0, 255, 0);
    SDL_SetColorKey(mario, SDL_SRCCOLORKEY, colorkey);

    SDL_Surface* tonneau = load_image("img/todd.bmp");
    colorkey = SDL_MapRGB( tonneau->format, 4, 242, 77 );
    SDL_SetColorKey(tonneau, SDL_SRCCOLORKEY, colorkey);

    SDL_Surface* sol = load_image("img/sol.bmp");
    colorkey = SDL_MapRGB( sol->format, 0, 0, 0);
    SDL_SetColorKey(sol, SDL_SRCCOLORKEY, colorkey);

    SDL_Surface* echelle = load_image("img/echelle.bmp");
    colorkey = SDL_MapRGB( echelle->format, 0, 0, 0);
    SDL_SetColorKey(sol, SDL_SRCCOLORKEY, colorkey);

    mariow = mario->w;

    sol_rect.minx = 0;
    sol_rect.maxx = SCREEN_WIDTH;
    sol_rect.miny = SCREEN_HEIGHT - sol->h - mario->h - 5;
    sol_rect.maxy = SCREEN_HEIGHT - sol->h - mario->h + 5;

    ech_rect.minx = 500;
    ech_rect.maxx = ech_rect.minx + echelle->w;
    ech_rect.miny = sol_rect.maxy - echelle->h;
    ech_rect.maxy = sol_rect.maxy;


    //Apply the message to the screen
    int mariox = 0;
    int marioy = SCREEN_HEIGHT - sol->h - mario->h;
    int tonnx = 200;
    int tonny = 10;

    int echellex = 500;

    int quit = 0;
    while (!quit) {

        /* Update the screen */
        // fill the screen with black
        SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));
        // dessiner le sol
        int x;
        for (x = 0; x < SCREEN_WIDTH; x += sol->w) {
            apply_surface(x, SCREEN_HEIGHT-sol->h, sol, screen);
        }
        // dessiner l'echelle
        apply_surface(echellex, SCREEN_HEIGHT-sol->h-echelle->h, echelle, screen);

        // dessiner le mario
        apply_surface(mariox, marioy, mario, screen );
        // dessiner le tonneau
        apply_surface(tonnx,  tonny,  tonneau, screen);
        // maj l'ecran
        if( SDL_Flip( screen ) == -1 )
        {
            return 1;
        }

        // Attendre un 10eme de seconde
        SDL_Delay( 100 ); // en milli-secondes

        // handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
                break;
            }
        }
        Uint8 *keyState = SDL_GetKeyState(0);
        if (keyState[SDLK_ESCAPE]) {  // echap = quitter le jeu
            quit = 1;
        }

        // bouger le mario
        if (keyState[SDLK_LEFT]) { // bouge vers la gauche
            mariox -= 10;
            if (!isAllowed(mariox, marioy)) mariox += 10;
        }
        if (keyState[SDLK_RIGHT]) { // bouge vers la droite
            mariox += 10;
            if (!isAllowed(mariox, marioy)) mariox -= 10;
        }
        if (keyState[SDLK_UP]) {
            marioy -= 10;
            if (!isAllowed(mariox, marioy)) marioy += 10;
        }
        if (keyState[SDLK_DOWN]) {
            marioy += 10;
            if (!isAllowed(mariox, marioy)) marioy -= 10;
        }

        // bouger le tonneau
        tonny += 15;
        if (tonny > SCREEN_HEIGHT) tonny = 0;

    }

    //Free the surfaces
    SDL_FreeSurface(echelle);
    SDL_FreeSurface(sol);
    SDL_FreeSurface(tonneau);
    SDL_FreeSurface(mario);

    //Quit SDL
    SDL_Quit();

    return 0;
}
