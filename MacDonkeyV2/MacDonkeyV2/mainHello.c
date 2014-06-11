#include "SDL/SDL.h"
#include <stdlib.h>
#include <stdio.h>

//The attributes of the screen
const int SCREEN_WIDTH = 28*16;
const int SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;
const int FPS = 30;
const char* sprites_filename = "sprites_small.bmp";

//L'écran
SDL_Surface *ecran = NULL;
void initialisation(const char* titre) {
    //Initialisation de SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("L'initialisation de SDL a échoué\n");
        exit(EXIT_FAILURE);
    }
    //Initialisation de l'écran
    ecran = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );
    if (ecran == NULL) {
        printf("L'initialisation de l'écran a échoué\n");
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    SDL_WM_SetCaption(titre, NULL);
}


SDL_Surface *sprites = NULL;
SDL_Rect sol_img;
SDL_Rect echelle_img;
SDL_Rect tonneau_img;
SDL_Rect mario_img;
void charger_images() {
    SDL_Surface* temporaire = SDL_LoadBMP(sprites_filename);
    if (temporaire != NULL) {
        sprites = SDL_DisplayFormat(temporaire);
        SDL_FreeSurface(temporaire);
    }
    else {
        printf("Error in loading sprites.bmp");
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    Uint32 colorkey = SDL_MapRGB(sprites->format, 255, 255, 255);
    SDL_SetColorKey(sprites, SDL_SRCCOLORKEY, colorkey);
    sol_img.x = 0;      sol_img.y = 0;
    sol_img.w = 16;     sol_img.h = 16;
    echelle_img.x = 0;  echelle_img.y = 18;
    echelle_img.w = 16; echelle_img.h = 16;
    tonneau_img.x = 0;  tonneau_img.y = 36;
    tonneau_img.w = 24; tonneau_img.h = 20;
    mario_img.x = 0;    mario_img.y = 58;
    mario_img.w = 24;   mario_img.h = 32;
}

int nsol = 0;
SDL_Rect sol[100];
void initialise_sol(int x0, int xN, int y0, int yN) {
    int i;
    for (i = 0; i < (xN - x0)/sol_img.w; i++) {
//    for (x = x0; x < xN; x += sol_img.w) {
        sol[nsol].x = x0 + i*sol_img.w;
        sol[nsol].y = (y0 + (i*sol_img.w)*(yN - y0)/(xN - x0));
        
        if (sol[nsol].y > SCREEN_HEIGHT - sol_img.h) {
            sol[nsol].y = SCREEN_HEIGHT - sol_img.h;
        }
        nsol++;
    }
}

const int NTONNEAUX = 100;
int nechelle = 0;
SDL_Rect echelle[100];
SDL_Rect tonneau[100];
SDL_Rect mario;
void initialise_jeu() {
    int i;
    nsol = 0;
        // initialise les sols
    initialise_sol(          0, 28*sol_img.w, 597, 569);
    initialise_sol(          0, 26*sol_img.w, 450, 476);
    initialise_sol(2*sol_img.w, 28*sol_img.w, 295, 269);
    
        // initialise les échelles
    echelle[0].x = sol[23].x; // 368
    echelle[0].y = sol[23].y - echelle_img.h; // 574 - echelle_img.h
    for (i = 1; i < 7; i++) {
        echelle[i].x = echelle[i-1].x;
        echelle[i].y = echelle[i-1].y - echelle_img.h;
    }
//    echelle[6].y = sol[52].y;
    echelle[6].y = echelle[0].y;
    echelle[0].y = sol[52].y;
    nechelle = 7;
    
        // initialise les tonneau
    for (i = 0; i < NTONNEAUX; i++) {
        // x = -1 est un signal special pour dire que le
        //   tonneau ne doit pas être afficer.
        tonneau[i].x = -1;
    }
    tonneau[0].x = 10;
    tonneau[0].y = 10;
        
        // initialise mario
    mario.x = 160;
    mario.y = SCREEN_HEIGHT - sol_img.h - mario_img.h;
}


void dessiner_tout() {
    int i;
        // remplissage avec du noir
    SDL_FillRect(ecran, 0, SDL_MapRGB(ecran->format, 0, 0, 0));
        // afficher les échelles
    for (i = 0; i < nechelle; ++i) {
        SDL_BlitSurface(sprites, &echelle_img, ecran, &echelle[i]);
    }
        // afficher le sol
    for (i = 0; i < nsol; ++i) {
        SDL_BlitSurface(sprites, &sol_img, ecran, &sol[i]);
    }

        // afficher le mario
    SDL_BlitSurface(sprites, &mario_img, ecran, &mario);
  
        // afficher les tonneaux
    for (i = 0; i < NTONNEAUX; ++i) {
        if (tonneau[i].x >= 0) {
            SDL_BlitSurface(sprites, &tonneau_img, ecran, &tonneau[i]);
        }
    }
}


int verif_sol(SDL_Rect* objet, int delta) {
        // verifier si l'objet est hors-écran
    if (objet->x < 0 || objet->x > SCREEN_WIDTH) return 0;
        // verifier si le bas de l'objet est sur un "sol"
    int bas = objet->y + objet->h;
    int i;
    for (i = 0; i < nsol; ++i) {
        if ( abs(bas - sol[i].y) <= delta // si le bas de l'objet est proche de l'haut du sol
           && objet->x <= sol[i].x && sol[i].x < objet->x + objet->w) { // et il y a du sol en sous de l'objet
            objet->y = sol[i].y - objet->h;
            return 1;
        }
    }
    return 0;
}


int verif_echelle(SDL_Rect* objet, int delta) {
    int centre = objet->x + objet->w/2;
    int bas = objet->y + objet->h;
    int i;
    for (i = 0; i < nechelle; ++i) {
        if (abs(centre - (echelle[i].x + echelle[i].w / 2)) <= delta
            && echelle[i].y <= bas && bas <= echelle[i].y + echelle[i].h) {
            objet->x = echelle[i].x + echelle[i].w / 2 - objet->w / 2;
            return 1;
        }
    }
    return 0;
}


int gerer_evenements() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return 1;
    }
    Uint8 *keyState = SDL_GetKeyState(NULL);
    if (keyState[SDLK_ESCAPE]) {  // echap = quitter le jeu
        return 1;
    }
    // bouger le mario
    int ancien_position;
    if (keyState[SDLK_UP]) {
        ancien_position = mario.y;
        mario.y -= 4;
        while (!verif_echelle(&mario, 2) && (mario.y != ancien_position)) {
            mario.y++;
        }
    }
    if (keyState[SDLK_DOWN]) {
        ancien_position = mario.y;
        mario.y += 4;
        while (!verif_echelle(&mario, 2) && (mario.y != ancien_position)) {
            mario.y--;
        }
    }
    if (keyState[SDLK_LEFT]) { // bouge vers la gauche
        mario.x -= 4;
        if (!verif_sol(&mario, 2)) {
            mario.x += 4;
        }
    }
    if (keyState[SDLK_RIGHT]) { // bouge vers la droite
        mario.x += 4;
        if (!verif_sol(&mario, 2)) {
            mario.x -= 4;
        }
    }
    return 0;
}

void avance_tonneaux() {
    int i;
    for (i = 0; i < NTONNEAUX; i++) {
        if (tonneau[i].x > 0) {
            if (verif_sol(&tonneau[i], 2)) {
                tonneau[i].x += 6;
            }
//            else if (verif_echelle(&tonneau[i], 2)) {}
            else {
                tonneau[i].y += 6;
            }
        }
    }
}

int main( int argc, char* args[] )
{
    printf("Init\n");
    initialisation("Donkey Kong");
    printf("Images\n");
    charger_images();
    printf("Init game\n");
    initialise_jeu();
    printf("ok\n");
    

    const Uint32 ms_per_frame = 1000 / FPS;
    Uint32 chrono = SDL_GetTicks();
    int quit = 0;
    while (!quit) {
        dessiner_tout();
//        printf("flipping\n");
        chrono = ms_per_frame - (SDL_GetTicks() - chrono);
        if (chrono > 0) {
          SDL_Delay(chrono);
//          printf("delay of: %i ms\n", chrono);
        }
        else {
          printf("lag of: %i ms\n", -chrono);
        } 
        /* mettre a jour l'écran */
        SDL_Flip(ecran);
        chrono = SDL_GetTicks();
        
//        printf("events\n");
        quit = gerer_evenements();
        
        /* Bouger les tonneaux */
        avance_tonneaux();
    }

    //Free the surfaces
    SDL_FreeSurface(sprites);
    SDL_FreeSurface(ecran);

    //Quit SDL
    SDL_Quit();

    return EXIT_SUCCESS;
}
