#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <SDL_mixer.h>

#include "constants.h"

enum GameState {
    premierePage,
    menuPage,
    optionPage,
    getInfo,
    jouerPage,
    reglementPage,
    WIN,
    LOST
};

struct base {
    char username[20];
    int score;
};
struct base player[6]={0};



Mix_Music* bgMusic = NULL;
Mix_Chunk* winSFX = NULL;
Mix_Chunk* loseSFX = NULL;
Mix_Chunk* wrongGuessSFX = NULL;

SDL_Texture* hangmanTextures[7];

bool firstTime = true;
bool showpopup = true;
char inputText[21] = "";
int tries = 7;
int numberchoice = 4;
bool backgroundeyes = false;

bool isInside(SDL_Point point, SDL_Rect rect) {
    return point.x >= rect.x && point.x <= rect.x + rect.w && point.y >= rect.y && point.y <= rect.y + rect.h;
}

bool guessedLetters[8];

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* backgroundplainImageTexture;
SDL_Texture* reglementPageImageTexture;
SDL_Texture* premierePageImageTexture;
SDL_Texture* menuPageImageTexture;
SDL_Texture* manWithoutEyesTexture;
SDL_Texture* manWithEyesTexture;
SDL_Texture* gameImageTexture;
SDL_Texture* doorimagetexture;
SDL_Texture* yesIndiceIconTexture;
SDL_Texture* yesIndiceGreenIconTexture;
SDL_Texture* CongratsTexture;
SDL_Texture* lostTexture;
SDL_Texture* leaderBoardTexture;

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL) {
        printf("Failed to load surface: %s\n", IMG_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (texture == NULL) {
        printf("Failed to create texture: %s\n", SDL_GetError());
    }
    return texture;
}

bool intialisationSDL(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    window = SDL_CreateWindow("Jeu pendu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1200, 800, 0);
    if (window == NULL) {
        fprintf(stderr, "Error creating SDL Window.\n");
        return false;
    }

    SDL_Surface* iconSurface = IMG_Load("./src/quickIcon1.png");
    SDL_SetWindowIcon(window, iconSurface);
    SDL_FreeSurface(iconSurface);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "Error creating SDL Renderer.\n");
        return false;
    }

    backgroundplainImageTexture = loadTexture("./src/newTheme/backgroundPaper.png", renderer);
    reglementPageImageTexture = loadTexture("./src/newTheme/reglementPage.png", renderer);
    premierePageImageTexture = loadTexture("./src/newTheme/firstPage.png", renderer);
    menuPageImageTexture = loadTexture("./src/newTheme/menuPageBackground.png", renderer);
    manWithoutEyesTexture = loadTexture("./src/newTheme/menuPageBackgroundMan.png", renderer);
    manWithEyesTexture = loadTexture("./src/newTheme/menuPageBackgroundManWithEyes.png", renderer);
    gameImageTexture = loadTexture("./src/newTheme/jouerPageBg.png", renderer);
    doorimagetexture = loadTexture("./src/newTheme/door.png", renderer);
    yesIndiceIconTexture = loadTexture("./src/yesIndiceIcon.png", renderer);
    yesIndiceGreenIconTexture = loadTexture("./src/yesIndiceGreenIcon.png", renderer);
    CongratsTexture = loadTexture("./src/newTheme/winFreeManbg.png", renderer);
    lostTexture = loadTexture("./src/newTheme/loseFreeManbg.png", renderer);
    leaderBoardTexture = loadTexture("./src/newTheme/leaderBoard.png", renderer);


    if ((reglementPageImageTexture == NULL) || (backgroundplainImageTexture == NULL) || (premierePageImageTexture == NULL)
        || (menuPageImageTexture == NULL) || (manWithoutEyesTexture == NULL) || (manWithEyesTexture == NULL) || (gameImageTexture == NULL)
        || (doorimagetexture == NULL) || (yesIndiceIconTexture == NULL) || (yesIndiceGreenIconTexture == NULL) ||
        (CongratsTexture == NULL) || (lostTexture == NULL)) {
        printf("One or more textures failed to load. Exiting.\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    for (int i = 0; i < 7; i++) {
        char hangmanImagePath[50];
        sprintf(hangmanImagePath, "./src/newTheme/hangman%d.png", i + 1);
        SDL_Surface* hangmanImages = IMG_Load(hangmanImagePath);
        if (hangmanImages == NULL) {
            fprintf(stderr, "Error loading image: %s\n", IMG_GetError());
        }
        hangmanTextures[i] = SDL_CreateTextureFromSurface(renderer, hangmanImages);
        SDL_FreeSurface(hangmanImages);
        if (hangmanTextures[i] == NULL) {
            fprintf(stderr, "Error creating texture: %s\n", SDL_GetError());
        }
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        SDL_Quit();
        return 1;
    }

    bgMusic = Mix_LoadMUS("./background_music.mp3");
    if (bgMusic == NULL) {
        printf("Failed to load background music! SDL_mixer Error: %s\n", Mix_GetError());
        bgMusic = Mix_LoadMUS("background_music.ogg");
        if (bgMusic == NULL) {
            printf("Failed to load alternative background music! SDL_mixer Error: %s\n", Mix_GetError());
            Mix_CloseAudio();
            SDL_Quit();
            return 1;
        }
    }

    winSFX = Mix_LoadWAV("./src/SFXwon.mp3");
    if (winSFX == NULL) {
        printf("Failed to load background music! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }

    loseSFX = Mix_LoadWAV("./src/SFXlost.mp3");
    if (loseSFX == NULL) {
        printf("Failed to load background music! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }

    wrongGuessSFX = Mix_LoadWAV("./src/SFXnope.mp3");
    if (wrongGuessSFX == NULL) {
        printf("Failed to load background music! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }
    return true;
}

void motAleatoire(const int indice, char *word, const int aleo) {
    char filename[100];
    sprintf(filename, "./src/%dlettres.txt", indice);
    FILE *fptr = fopen(filename, "r");
    if (fptr != NULL) {
        for (int i = 1; i <= aleo; i++) {
            fscanf(fptr, "%49s", word);
        }
        for (int i = 0; i < strlen(word); i++) {
            word[i] = tolower(word[i]);
        }
        rewind(fptr);
        fclose(fptr);
           printf("%s\n",word);

    }
}

void indiceDuMot(const int indice, char *wordIndice, const int aleo) {
    char filename[100];
    sprintf(filename, "./src/%dlettresIndice.txt", indice);
    FILE *fptr2 = fopen(filename, "r");
    if (fptr2 != NULL) {
        for (int i = 1; i <= aleo; i++) {
            fgets(wordIndice, 300, fptr2);
        }
        size_t len = strlen(wordIndice);
        if (len > 0 && wordIndice[len - 1] == '\n') {
            wordIndice[len - 1] = '\0';
        }
        rewind(fptr2);
        fclose(fptr2);

        printf("%s",wordIndice);
    }
}

bool cliqueSurIndice = false;
bool hoverIndice = false;

int main(int argc, char* argv[]) {
    srand(time(NULL));

    int score;

    bool indiceOrNon = true;
    bool SoundOrNon = true;
    bool found = false;
    char *word = malloc(20 * sizeof(char));
    char *wordIndice = malloc(250 * sizeof(char));
    char entry;
    int i;
    bool allGuessed;

    TTF_Init();
    TTF_Font* font = TTF_OpenFont("./src/bold.ttf", 30);
    if (font == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;

    }
    TTF_Font* fontleaderBoard = TTF_OpenFont("./src/bold.ttf", 14);
    if (font == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    TTF_Font* optionOptionsFont = TTF_OpenFont("./src/FeGPrm2.ttf", 48);
    if (optionOptionsFont == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    TTF_Font* font1 = TTF_OpenFont("./src/FroadmileDEMO.ttf", 48);
    if (font1 == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    TTF_Font* font3 = TTF_OpenFont("./src/FroadmileDEMO.ttf", 48);
    if (font3 == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    TTF_Font* font4 = TTF_OpenFont("./src/G_ari_bd.ttf", 70);
    if (font4 == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    TTF_Font* indiceFont = TTF_OpenFont("./src/G_ari_bd.ttf", 36);
    if (indiceFont == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    TTF_Font* choseNumberFourFont  = TTF_OpenFont("./src/arial.ttf", 80);
    if (choseNumberFourFont == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    TTF_Font* choseNumberFiveFont  = TTF_OpenFont("./src/arial.ttf", 60);
    if (choseNumberFiveFont == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    TTF_Font* choseNumberSixFont  = TTF_OpenFont("./src/arial.ttf", 60);
    if (choseNumberSixFont == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    TTF_Font* yesIndiceFont  = TTF_OpenFont("./src/arial.ttf", 72);
    if (yesIndiceFont == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    TTF_Font* nonIndiceFont  = TTF_OpenFont("./src/arial.ttf", 60);
    if (nonIndiceFont == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    TTF_Font* yesSoundFont  = TTF_OpenFont("./src/arial.ttf", 72);
    if (yesIndiceFont == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    TTF_Font* nonSoundFont  = TTF_OpenFont("./src/arial.ttf", 60);
    if (nonIndiceFont == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    TTF_Font* menuTextJouerFont  = TTF_OpenFont("./src/FeENsc2Bold.ttf", 48);
    if (menuTextJouerFont == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    TTF_Font* menuTextReglementFont= TTF_OpenFont("./src/FeENsc2Bold.ttf", 48);
    if (menuTextReglementFont == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    TTF_Font* menuTextOptionFont  = TTF_OpenFont("./src/FeENsc2Bold.ttf", 48);
    if (menuTextOptionFont == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    bool showText = true;
    bool showText2 = true;
    Uint32 lastToggleTime = SDL_GetTicks();
    Uint32 lastToggleTime2 = SDL_GetTicks();

    bool play = intialisationSDL();
    enum GameState gameState = premierePage;

    Mix_PlayMusic(bgMusic, -1); // Play music indefinitely (-1 for looping)

    SDL_Event event;
    while (play) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                play = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    play = false;
                } else if (gameState == premierePage && (event.type == SDL_KEYDOWN)) {
                    gameState = menuPage;
                    showpopup = true;


                }


            } else if (showpopup && gameState == optionPage) {

                if (event.key.keysym.sym == SDLK_RETURN)  {

                    showpopup = false;
                    printf("%s  ",inputText);
                }
                if (event.type == SDL_TEXTINPUT && strlen(inputText) < 8) {

                    strcat(inputText, event.text.text);

                } else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(inputText) > 0) {
                        inputText[strlen(inputText) - 1] = '\0';
                    }
                }
            } else if (event.type == SDL_MOUSEMOTION && gameState == menuPage ) {

                int x, y;
                SDL_GetMouseState(&x, &y);

                SDL_Point mousePoint = {x, y};

                SDL_Rect jouerRect = {400, 200, 177, 84};

                if (isInside(mousePoint, jouerRect)) {
                    TTF_CloseFont(menuTextJouerFont);
                    menuTextJouerFont = TTF_OpenFont("./src/FeENsc2Bold.ttf", 58);
                    backgroundeyes = true;
                } else {
                    TTF_CloseFont(menuTextJouerFont);
                    menuTextJouerFont = TTF_OpenFont("./src/FeENsc2Bold.ttf", 48);
                    backgroundeyes = false;
                }
                SDL_Rect reglementRect = {400, 350, 366, 65};
                if (isInside(mousePoint, reglementRect)) {
                    TTF_CloseFont(menuTextReglementFont);
                    menuTextReglementFont = TTF_OpenFont("./src/FeENsc2Bold.ttf", 58);
                } else {
                    TTF_CloseFont(menuTextReglementFont);
                    menuTextReglementFont = TTF_OpenFont("./src/FeENsc2Bold.ttf", 48);
                }
                SDL_Rect optionRect = {400, 500, 246, 65};
                if (isInside(mousePoint, optionRect)) {
                    TTF_CloseFont(menuTextOptionFont);
                    menuTextOptionFont = TTF_OpenFont("./src/FeENsc2Bold.ttf", 55);
                } else {
                    TTF_CloseFont(menuTextOptionFont);
                    menuTextOptionFont = TTF_OpenFont("./src/FeENsc2Bold.ttf", 48);
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN && gameState == menuPage  ) {
                // Existing code for menuPage mouse button down...
                int x = event.button.x;
                int y = event.button.y;

                SDL_Point clickPoint = {x, y};

                SDL_Rect jouerRect = {400, 200, 177, 84};
                if (isInside(clickPoint, jouerRect)) {
                    if (firstTime) {
                        gameState = optionPage;
                        firstTime = false;
                    } else {
                        gameState = jouerPage;
                        int aleo = rand() % 83 + 1;
                        motAleatoire(numberchoice, word, aleo);
                        printf("%s",word);
                        indiceDuMot(numberchoice, wordIndice, aleo);
                        printf("%s",wordIndice);
                    }
                }

                SDL_Rect reglementRect = {400, 350, 366, 65};
                if (isInside(clickPoint, reglementRect)) {
                    gameState = reglementPage;
                }

                SDL_Rect optionRect = {400, 500, 246, 65};
                if (isInside(clickPoint, optionRect)) {
                    showpopup=true;
                    memset(inputText, 0, sizeof(inputText));

                    gameState = optionPage;
                }
            } else if (gameState == reglementPage && (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE)) {
                gameState = menuPage;
            } else if (gameState == optionPage && (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE)) {
                gameState = jouerPage;
                int aleo = rand() % 83 + 1;
                motAleatoire(numberchoice, word, aleo);

                indiceDuMot(numberchoice, wordIndice, aleo);

            } else if (event.type == SDL_MOUSEBUTTONDOWN && gameState == optionPage) {
                // Existing code for optionPage mouse button down...
                int x = event.button.x;
                int y = event.button.y;

                SDL_Point clickPoint = {x, y};

                SDL_Rect Number1Rect = {600, 150, 100, 50};
                SDL_Rect Number2Rect = {750, 150, 100, 50};
                SDL_Rect Number3Rect = {900, 150, 100, 50};

                if (isInside(clickPoint, Number1Rect)) {
                    numberchoice = 4;
                } else if (isInside(clickPoint, Number2Rect)) {
                    numberchoice = 5;
                } else if (isInside(clickPoint, Number3Rect)) {
                    numberchoice = 6;
                }

                switch (numberchoice) {
                    case 4:
                        TTF_CloseFont(choseNumberFourFont);
                        choseNumberFourFont = TTF_OpenFont("arial.ttf", 80);
                        choseNumberFiveFont = TTF_OpenFont("arial.ttf", 60);
                        choseNumberSixFont = TTF_OpenFont("arial.ttf", 60);
                        break;
                    case 5:
                        TTF_CloseFont(choseNumberFiveFont);
                        choseNumberFourFont = TTF_OpenFont("arial.ttf", 60);
                        choseNumberFiveFont = TTF_OpenFont("arial.ttf", 80);
                        choseNumberSixFont = TTF_OpenFont("arial.ttf", 60);
                        break;
                    case 6:
                        TTF_CloseFont(choseNumberSixFont);
                        choseNumberFourFont = TTF_OpenFont("arial.ttf", 60);
                        choseNumberFiveFont = TTF_OpenFont("arial.ttf", 60);
                        choseNumberSixFont = TTF_OpenFont("arial.ttf", 80);
                        break;
                    default: break;
                }

                // Option d'animation des indices.
                SDL_Point clickPoint1 = {event.button.x, event.button.y};

                SDL_Rect yesIndiceRect = {400, 350, 93, 68};
                SDL_Rect nonIndiceRect = {700, 350, 109, 68};
                if (isInside(clickPoint1, yesIndiceRect)) {
                    indiceOrNon = true;
                } else if (isInside(clickPoint1, nonIndiceRect)) {
                    indiceOrNon = false;
                }
                if (indiceOrNon) {
                    TTF_CloseFont(yesIndiceFont);
                    TTF_CloseFont(nonIndiceFont);
                    yesIndiceFont = TTF_OpenFont("arial.ttf", 72);
                    nonIndiceFont = TTF_OpenFont("arial.ttf", 60);
                } else {
                    TTF_CloseFont(yesIndiceFont);
                    TTF_CloseFont(nonIndiceFont);
                    yesIndiceFont = TTF_OpenFont("arial.ttf", 60);
                    nonIndiceFont = TTF_OpenFont("arial.ttf", 72);
                }

                // Option d'animation du son.
                SDL_Point clickPoint2 = {event.button.x, event.button.y};

                SDL_Rect yesSoundRect = {400, 550, 93, 68};
                SDL_Rect nonSoundRect = {700, 550, 109, 68};
                if (isInside(clickPoint2, yesSoundRect)) {
                    SoundOrNon = true;
                    Mix_ResumeMusic();
                } else if (isInside(clickPoint2, nonSoundRect)) {
                    SoundOrNon = false;
                    Mix_PauseMusic();
                }
                if (SoundOrNon) {
                    TTF_CloseFont(yesSoundFont);
                    TTF_CloseFont(nonSoundFont);
                    yesSoundFont = TTF_OpenFont("arial.ttf", 72);
                    nonSoundFont = TTF_OpenFont("arial.ttf", 60);
                } else {
                    TTF_CloseFont(yesSoundFont);
                    TTF_CloseFont(nonSoundFont);
                    yesSoundFont = TTF_OpenFont("arial.ttf", 60);
                    nonSoundFont = TTF_OpenFont("arial.ttf", 72);
                }
            } else if (event.type == SDL_MOUSEMOTION && gameState == jouerPage && indiceOrNon) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                SDL_Point mousePoint = {x, y};
                SDL_Rect indiceIconRect = {196, 712, 51, 51};

                if (isInside(mousePoint, indiceIconRect))
                    hoverIndice = true;
                else
                    hoverIndice = false;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN && gameState == jouerPage && indiceOrNon && hoverIndice)
                cliqueSurIndice = true;

            if (event.type == SDL_TEXTINPUT && gameState == jouerPage && isalpha(event.text.text[0])) {
                entry = event.text.text[0];
                entry = tolower(entry);
                found = false;

                for (i = 0; i < strlen(word); i++) {
                    if (entry == word[i]) {
                        guessedLetters[i] = true;
                        found = true;
                    }
                }
                if (!found) {
                    tries--;
                    if (tries != 0)
                        Mix_PlayChannel(-1, wrongGuessSFX, 0);
                }
            }
            if (gameState == LOST || gameState == WIN) {
                cliqueSurIndice = false;
                hoverIndice = false;

                for (i = 0; i < 8; ++i) {
                    guessedLetters[i] = false;
                }
                if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_SPACE) {
                    gameState = menuPage;
                }
            }
        }

        SDL_RenderClear(renderer);
        switch (gameState) {
            case premierePage:
                SDL_RenderCopy(renderer, premierePageImageTexture, NULL, NULL);
                font = TTF_OpenFont("./src/bold.ttf", 48);
                if (showText) {
                    SDL_Color textColor = {0, 0, 0};
                    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Cliquer pour commencer", textColor);
                    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                    SDL_Rect textRect = {500, 680, textSurface->w, textSurface->h};
                    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                    SDL_FreeSurface(textSurface);
                    SDL_DestroyTexture(textTexture);
                }
                break;
            case reglementPage:
                SDL_RenderCopy(renderer, reglementPageImageTexture, NULL, NULL);
                font = TTF_OpenFont("./src/bold.ttf", 30);

                SDL_Color reglementTextColor = {0, 0, 0, 220};
                SDL_Surface* reglementTextSurface = TTF_RenderText_Solid(font, "Cliquer sur (Entrer,espace) pour revenir au menu", reglementTextColor);
                SDL_Texture* reglementTextTexture = SDL_CreateTextureFromSurface(renderer, reglementTextSurface);
                SDL_Rect reglementTextRect = {270, 746, reglementTextSurface->w, reglementTextSurface->h};
                SDL_RenderCopy(renderer, reglementTextTexture, NULL, &reglementTextRect);
                SDL_FreeSurface(reglementTextSurface);
                SDL_DestroyTexture(reglementTextTexture);
                break;
            case menuPage:


                    SDL_RenderCopy(renderer, menuPageImageTexture, NULL, NULL);

                SDL_Rect leaderBoardRect = {78, 49, 200, 200};
                SDL_RenderCopy(renderer, leaderBoardTexture, NULL, &leaderBoardRect);

                    SDL_Rect manWithoutEyesRect = {967, 199, 158, 562};
                    SDL_RenderCopy(renderer, manWithoutEyesTexture, NULL, &manWithoutEyesRect);

                    if (backgroundeyes) {
                        SDL_RenderCopy(renderer, manWithEyesTexture, NULL, &manWithoutEyesRect);
                    }

                    SDL_Color jouerColor = {0, 0, 0};
                    SDL_Surface *jouerSurface = TTF_RenderText_Solid(menuTextJouerFont, "Jouer", jouerColor);
                    SDL_Texture *jouerTexture = SDL_CreateTextureFromSurface(renderer, jouerSurface);
                    SDL_Rect jouerRect = {400, 200, jouerSurface->w, jouerSurface->h};
                    SDL_RenderCopy(renderer, jouerTexture, NULL, &jouerRect);
                    SDL_FreeSurface(jouerSurface);
                    SDL_DestroyTexture(jouerTexture);

                    SDL_Color reglementColor = {0, 0, 0};
                    SDL_Surface *reglementSurface = TTF_RenderText_Solid(menuTextReglementFont, "Reglements",
                                                                         reglementColor);
                    SDL_Texture *reglementTexture = SDL_CreateTextureFromSurface(renderer, reglementSurface);
                    SDL_Rect reglementRect = {400, 350, reglementSurface->w, reglementSurface->h};
                    SDL_RenderCopy(renderer, reglementTexture, NULL, &reglementRect);
                    SDL_FreeSurface(reglementSurface);
                    SDL_DestroyTexture(reglementTexture);

                    SDL_Color optionColor = {0, 0, 0};
                    SDL_Surface *optionSurface = TTF_RenderText_Solid(menuTextOptionFont, "Options", optionColor);
                    SDL_Texture *optionTexture = SDL_CreateTextureFromSurface(renderer, optionSurface);
                    SDL_Rect optionRect = {400, 500, optionSurface->w, optionSurface->h};
                    SDL_RenderCopy(renderer, optionTexture, NULL, &optionRect);
                    SDL_FreeSurface(optionSurface);
                    SDL_DestroyTexture(optionTexture);


                SDL_Color scoreColor = {0, 0, 0}; // Black color for text
                SDL_Rect textPosition = {140, 95, 0, 0}; // Starting position for the text

                for (i = 0; i < 5; ++i) {
                    char displayText[50];
                    snprintf(displayText, sizeof(displayText), "%s  %d", player[i].username,
                             player[i].score);
                    SDL_Surface *textSurface = TTF_RenderText_Solid(fontleaderBoard, displayText, scoreColor);
                    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

                    textPosition.w = textSurface->w;
                    textPosition.h = textSurface->h;
                    SDL_RenderCopy(renderer, textTexture, NULL, &textPosition);

                    // Move the position for the next text down
                    textPosition.y += textSurface->h + 15;

                    SDL_FreeSurface(textSurface);
                    SDL_DestroyTexture(textTexture);


                }



                break;
            case optionPage:
                tries = 7;
                cliqueSurIndice = false;


               if(showpopup){
                   SDL_RenderCopy(renderer, backgroundplainImageTexture, NULL, NULL);

                   SDL_Rect popupRect = { 1200 / 4, 800 / 4, 1200 / 2, 800 / 2 };

                   // Set the drawing color with transparency (alpha value)
                   SDL_SetRenderDrawColor(renderer, 200, 200, 200, 0); // Alpha value set to 150 for transparency
                   SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); // Enable blending mode
                   SDL_RenderFillRect(renderer, &popupRect);

                   // Render the prompt
                   TTF_Font *font = TTF_OpenFont("./src/bold.ttf", 20);
                   if (showpopup) {
                       SDL_Color textColor = { 0, 0, 0, 255 }; // Full opacity for text
                       SDL_Surface *promptSurface = TTF_RenderText_Solid(font, "Entrez votre nom:", textColor);
                       SDL_Texture *promptTexture = SDL_CreateTextureFromSurface(renderer, promptSurface);

                       SDL_Rect promptRect = { popupRect.x + 10, popupRect.y + 10, promptSurface->w, promptSurface->h };
                       SDL_RenderCopy(renderer, promptTexture, NULL, &promptRect);

                       SDL_FreeSurface(promptSurface);
                       SDL_DestroyTexture(promptTexture);
                   }

                   // Render the input text character by character
                   font = TTF_OpenFont("./src/bold.ttf", 24);
                   if (showpopup) {
                       SDL_Color textColor = { 0, 0, 0, 255 }; // Full opacity for text
                       int x = popupRect.x + 10;
                       int y = popupRect.y + 50;
                       for ( i = 0; i < strlen(inputText); ++i) {
                           char entryText[2] = {inputText[i], '\0'};
                           SDL_Surface *entrySurface = TTF_RenderText_Solid(font, entryText, textColor);
                           SDL_Texture *entryTexture = SDL_CreateTextureFromSurface(renderer, entrySurface);
                           SDL_Rect entryRect = { x, y, entrySurface->w, entrySurface->h };
                           SDL_RenderCopy(renderer, entryTexture, NULL, &entryRect);
                           x += entrySurface->w;
                           SDL_FreeSurface(entrySurface);
                           SDL_DestroyTexture(entryTexture);
                       }
                       TTF_CloseFont(font);
                   }
               }
               else{
                   SDL_RenderCopy(renderer, backgroundplainImageTexture, NULL, NULL);

                SDL_Color backgroundplainColor = {0, 0, 0};
                SDL_Surface* backgroundplainSurface = TTF_RenderText_Solid(optionOptionsFont, "Nombre de lettre", backgroundplainColor);
                SDL_Texture* backgroundplainTexture = SDL_CreateTextureFromSurface(renderer, backgroundplainSurface);
                SDL_Rect backgroundplainRect = {100, 150, backgroundplainSurface->w, backgroundplainSurface->h};
                SDL_RenderCopy(renderer, backgroundplainTexture, NULL, &backgroundplainRect);
                SDL_FreeSurface(backgroundplainSurface);
                SDL_DestroyTexture(backgroundplainTexture);

                backgroundplainSurface = TTF_RenderText_Solid(choseNumberFourFont, "4", backgroundplainColor);
                backgroundplainTexture = SDL_CreateTextureFromSurface(renderer, backgroundplainSurface);
                SDL_Rect Number1Rect = {600, 150, backgroundplainSurface->w, backgroundplainSurface->h};
                SDL_RenderCopy(renderer, backgroundplainTexture, NULL, &Number1Rect);
                SDL_FreeSurface(backgroundplainSurface);
                SDL_DestroyTexture(backgroundplainTexture);

                backgroundplainSurface = TTF_RenderText_Solid(choseNumberFiveFont, "5", backgroundplainColor);
                backgroundplainTexture = SDL_CreateTextureFromSurface(renderer, backgroundplainSurface);
                SDL_Rect Number2Rect = {750, 150, backgroundplainSurface->w, backgroundplainSurface->h};
                SDL_RenderCopy(renderer, backgroundplainTexture, NULL, &Number2Rect);
                SDL_FreeSurface(backgroundplainSurface);
                SDL_DestroyTexture(backgroundplainTexture);

                backgroundplainSurface = TTF_RenderText_Solid(choseNumberSixFont, "6", backgroundplainColor);
                backgroundplainTexture = SDL_CreateTextureFromSurface(renderer, backgroundplainSurface);
                SDL_Rect Number3Rect = {900, 150, backgroundplainSurface->w, backgroundplainSurface->h};
                SDL_RenderCopy(renderer, backgroundplainTexture, NULL, &Number3Rect);
                SDL_FreeSurface(backgroundplainSurface);
                SDL_DestroyTexture(backgroundplainTexture);

                backgroundplainSurface = TTF_RenderText_Solid(optionOptionsFont, "Indice", backgroundplainColor);
                backgroundplainTexture = SDL_CreateTextureFromSurface(renderer, backgroundplainSurface);
                SDL_Rect backgroundplain2Rect = {100, 350, backgroundplainSurface->w, backgroundplainSurface->h};
                SDL_RenderCopy(renderer, backgroundplainTexture, NULL, &backgroundplain2Rect);
                SDL_FreeSurface(backgroundplainSurface);
                SDL_DestroyTexture(backgroundplainTexture);

                backgroundplainSurface = TTF_RenderText_Solid(yesIndiceFont, "Oui", backgroundplainColor);
                backgroundplainTexture = SDL_CreateTextureFromSurface(renderer, backgroundplainSurface);
                SDL_Rect yesIndiceRect = {400, 350, backgroundplainSurface->w, backgroundplainSurface->h};
                SDL_RenderCopy(renderer, backgroundplainTexture, NULL, &yesIndiceRect);
                SDL_FreeSurface(backgroundplainSurface);
                SDL_DestroyTexture(backgroundplainTexture);

                backgroundplainSurface = TTF_RenderText_Solid(nonIndiceFont, "Non", backgroundplainColor);
                backgroundplainTexture = SDL_CreateTextureFromSurface(renderer, backgroundplainSurface);
                SDL_Rect nonIndiceRect = {700, 350, backgroundplainSurface->w, backgroundplainSurface->h};
                SDL_RenderCopy(renderer, backgroundplainTexture, NULL, &nonIndiceRect);
                SDL_FreeSurface(backgroundplainSurface);
                SDL_DestroyTexture(backgroundplainTexture);

                backgroundplainSurface = TTF_RenderText_Solid(optionOptionsFont, "Sound", backgroundplainColor);
                backgroundplainTexture = SDL_CreateTextureFromSurface(renderer, backgroundplainSurface);
                SDL_Rect soundRect = {100, 550, backgroundplainSurface->w, backgroundplainSurface->h};
                SDL_RenderCopy(renderer, backgroundplainTexture, NULL, &soundRect);
                SDL_FreeSurface(backgroundplainSurface);
                SDL_DestroyTexture(backgroundplainTexture);

                backgroundplainSurface = TTF_RenderText_Solid(yesSoundFont, "Oui", backgroundplainColor);
                backgroundplainTexture = SDL_CreateTextureFromSurface(renderer, backgroundplainSurface);
                SDL_Rect yesSoundRect = {400, 550, backgroundplainSurface->w, backgroundplainSurface->h};
                SDL_RenderCopy(renderer, backgroundplainTexture, NULL, &yesSoundRect);
                SDL_FreeSurface(backgroundplainSurface);
                SDL_DestroyTexture(backgroundplainTexture);

                backgroundplainSurface = TTF_RenderText_Solid(nonSoundFont, "Non", backgroundplainColor);
                backgroundplainTexture = SDL_CreateTextureFromSurface(renderer, backgroundplainSurface);
                SDL_Rect nonSoundRect = {700, 550, backgroundplainSurface->w, backgroundplainSurface->h};
                SDL_RenderCopy(renderer, backgroundplainTexture, NULL, &nonSoundRect);
                SDL_FreeSurface(backgroundplainSurface);
                SDL_DestroyTexture(backgroundplainTexture);
                }

                for (i = 0; i < 10; ++i) {
                    guessedLetters[i] = false;
                }
                break;
            case jouerPage:
                SDL_RenderCopy(renderer, gameImageTexture, NULL, NULL);

                if (indiceOrNon) {
                    SDL_Rect yesIndiceIconRect = {196, 712, 51, 51};
                    if (hoverIndice)
                        SDL_RenderCopy(renderer, yesIndiceGreenIconTexture, NULL, &yesIndiceIconRect);
                    else
                        SDL_RenderCopy(renderer, yesIndiceIconTexture, NULL, &yesIndiceIconRect);

                    if (cliqueSurIndice) {
                        SDL_RenderCopy(renderer, yesIndiceGreenIconTexture, NULL, &yesIndiceIconRect);
                        SDL_Color textColor = {0, 0, 0};
                        SDL_Surface* entry1Surface = TTF_RenderText_Solid(indiceFont, wordIndice, textColor);
                        SDL_Texture* entry2Texture = SDL_CreateTextureFromSurface(renderer, entry1Surface);
                        SDL_Rect entry2Rect = {265, 716, entry1Surface->w, entry1Surface->h};
                        SDL_RenderCopy(renderer, entry2Texture, NULL, &entry2Rect);
                        SDL_FreeSurface(entry1Surface);
                        SDL_DestroyTexture(entry2Texture);
                    }
                }
                SDL_Color lifesColor = {1, 0, 4};
                char lifesText[20];
                snprintf(lifesText, 50, "%d", tries);
                SDL_Surface* lifesSurface = TTF_RenderText_Solid(font4, lifesText, lifesColor);
                SDL_Texture* lifesTexture = SDL_CreateTextureFromSurface(renderer, lifesSurface);
                SDL_Rect lifesRect = {900, 176, lifesSurface->w, lifesSurface->h};
                SDL_RenderCopy(renderer, lifesTexture, NULL, &lifesRect);
                SDL_FreeSurface(lifesSurface);
                SDL_DestroyTexture(lifesTexture);

                if (entry != '\0' && isalpha(entry)) {
                    SDL_Color textColor = {0, 0, 0};
                    char entryText[2] = {entry, '\0'};
                    SDL_Surface* entrySurface = TTF_RenderText_Solid(font4, entryText, textColor);
                    SDL_Texture* entryTexture = SDL_CreateTextureFromSurface(renderer, entrySurface);
                    SDL_Rect entryRect = {850, 410, entrySurface->w, entrySurface->h};
                    SDL_RenderCopy(renderer, entryTexture, NULL, &entryRect);
                    SDL_FreeSurface(entrySurface);
                    SDL_DestroyTexture(entryTexture);
                }

                if (tries <= 6) {
                    SDL_Rect hangmanRect0 = {1044, 51, Hangman1W, Hangman1H};
                    SDL_RenderCopy(renderer, hangmanTextures[0], NULL, &hangmanRect0);
                }
                if (tries <= 5) {
                    SDL_Rect hangmanRect1 = {1014, 51 + Hangman1H - 6, Hangman2W, Hangman2H};
                    SDL_RenderCopy(renderer, hangmanTextures[1], NULL, &hangmanRect1);
                }
                if (tries <= 4) {
                    SDL_Rect hangmanRect2 = {1050, 51 + Hangman1H + Hangman2H - 10, Hangman3W, Hangman3H};
                    SDL_RenderCopy(renderer, hangmanTextures[2], NULL, &hangmanRect2);
                }
                if (tries <= 3) {
                    SDL_Rect hangmanRect3 = {1014, 51 + Hangman1H + Hangman2H + 32, Hangman4_5W, Hangman4_5H};
                    SDL_RenderCopy(renderer, hangmanTextures[3], NULL, &hangmanRect3);
                }
                if (tries <= 2) {
                    SDL_Rect hangmanRect4 = {1068, 51 + Hangman1H + Hangman2H + 32, Hangman4_5W, Hangman4_5H};
                    SDL_RenderCopy(renderer, hangmanTextures[4], NULL, &hangmanRect4);
                }
                if (tries <= 1) {
                    SDL_Rect hangmanRect5 = {1004, 51 + Hangman1H + Hangman2H + Hangman3H - 32, Hangman6_7W, Hangman6_7H};
                    SDL_RenderCopy(renderer, hangmanTextures[5], NULL, &hangmanRect5);
                }
                if (tries == 0) {
                    SDL_Rect hangmanRect6 = {1068, 51 + Hangman1H + Hangman2H + Hangman3H - 32, Hangman6_7W, Hangman6_7H};
                    SDL_RenderCopy(renderer, hangmanTextures[6], NULL, &hangmanRect6);
                    SDL_RenderPresent(renderer);
                    tries = 7;
                    gameState = LOST;

                    hoverIndice = false;
                    cliqueSurIndice = false;

                    Mix_PlayChannelTimed(-1, loseSFX, 0, 2600);
                    entry = ' ';
                }

                for (int j = 0; j < strlen(word); j++) {
                    SDL_Rect doorDestRect = {200 + (j * 140), 264, 107, 132};
                    SDL_RenderCopy(renderer, doorimagetexture, NULL, &doorDestRect);
                    if (guessedLetters[j]) {
                        SDL_Color textColor = {0, 0, 0};
                        char letter[2] = {word[j], '\0'};
                        SDL_Surface* letterSurface = TTF_RenderText_Solid(font4, letter, textColor);
                        SDL_Texture* letterTexture = SDL_CreateTextureFromSurface(renderer, letterSurface);
                        SDL_Rect letterRect = {doorDestRect.x + (doorDestRect.w / 2) - (letterSurface->w / 2), doorDestRect.y + (doorDestRect.h / 2) - (letterSurface->h / 2) + 10, letterSurface->w, letterSurface->h};
                        SDL_RenderCopy(renderer, letterTexture, NULL, &letterRect);
                        SDL_FreeSurface(letterSurface);
                        SDL_DestroyTexture(letterTexture);
                    }
                }
                SDL_RenderPresent(renderer);
                break;
            case WIN:
                SDL_RenderCopy(renderer, CongratsTexture, NULL, NULL);
                cliqueSurIndice = false;
                hoverIndice = false;
                font = TTF_OpenFont("./src/bold.ttf", 60);
                if (showText2) {
                    SDL_Color textColor = {1, 50, 32};
                    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Merci!!! Je suis LIIBRE.", textColor);
                    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                    SDL_Rect textRect = {250, 220, textSurface->w, textSurface->h};
                    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                    SDL_FreeSurface(textSurface);
                    SDL_DestroyTexture(textTexture);
                }
                SDL_Color winTextColor = {0, 0, 0};
                SDL_Surface* winTextSurface = TTF_RenderText_Solid(font4, "-Bravo, votre score est: ", winTextColor);
                SDL_Texture* winTextTexture = SDL_CreateTextureFromSurface(renderer, winTextSurface);
                SDL_Rect winTextRect = {180, 400, winTextSurface->w, winTextSurface->h};
                SDL_RenderCopy(renderer, winTextTexture, NULL, &winTextRect);
                SDL_FreeSurface(winTextSurface);
                SDL_DestroyTexture(winTextTexture);

                char scoreText[20];
                snprintf(scoreText, 50, "%d", score);
                SDL_Surface* scoreSurface = TTF_RenderText_Solid(font4, scoreText, winTextColor);
                SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
                SDL_Rect scoreRect = {180, 520, scoreSurface->w, scoreSurface->h};
                SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
                SDL_FreeSurface(scoreSurface);
                SDL_DestroyTexture(scoreTexture);

                SDL_RenderPresent(renderer);

                backgroundeyes = false;
                menuTextJouerFont = TTF_OpenFont("./src/FeENsc2Bold.ttf", 48);
                menuTextReglementFont = TTF_OpenFont("./src/FeENsc2Bold.ttf", 48);
                menuTextOptionFont = TTF_OpenFont("./src/FeENsc2Bold.ttf", 48);
                break;
            case LOST:
                SDL_RenderCopy(renderer, lostTexture, NULL, NULL);
                cliqueSurIndice = false;
                hoverIndice = false;
                font = TTF_OpenFont("./src/bold.ttf", 60);

                if (showText2) {
                    SDL_Color textLostColor = {50, 2, 32};
                    SDL_Surface* textLostSurface = TTF_RenderText_Solid(font, "MOOORT :(", textLostColor);
                    SDL_Texture* textLostTexture = SDL_CreateTextureFromSurface(renderer, textLostSurface);
                    SDL_Rect textLostRect = {250, 220, textLostSurface->w, textLostSurface->h};
                    SDL_RenderCopy(renderer, textLostTexture, NULL, &textLostRect);
                    SDL_FreeSurface(textLostSurface);
                    SDL_DestroyTexture(textLostTexture);
                }
                SDL_Color lostTextColor = {0, 0, 0};
                SDL_Surface* lostTextSurface = TTF_RenderText_Solid(font4, "-vous etes nul...", lostTextColor);
                SDL_Texture* lostTextTexture = SDL_CreateTextureFromSurface(renderer, lostTextSurface);
                SDL_Rect lostTextRect = {180, 400, lostTextSurface->w, lostTextSurface->h};
                SDL_RenderCopy(renderer, lostTextTexture, NULL, &lostTextRect);
                SDL_FreeSurface(lostTextSurface);
                SDL_DestroyTexture(lostTextTexture);

                SDL_RenderPresent(renderer);

                backgroundeyes = false;
                menuTextJouerFont = TTF_OpenFont("./src/FeENsc2Bold.ttf", 48);
                menuTextReglementFont = TTF_OpenFont("./src/FeENsc2Bold.ttf", 48);
                menuTextOptionFont = TTF_OpenFont("./src/FeENsc2Bold.ttf", 48);
                break;
            default:
                break;
        }

        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastToggleTime >= 500) {
            showText = !showText;
            lastToggleTime = currentTime;
        }

        allGuessed = true;
        for (i = 0; i < strlen(word); i++) {
            if (!guessedLetters[i]) {
                allGuessed = false;
                break;
            }
        }
        if (allGuessed) {
            usleep(200000);
            score = (100 * numberchoice * tries);
            if(!cliqueSurIndice){
                score*=1.5;
            }
            tries = 7;
            strcpy(player[5].username,inputText);
            player[5].score=score;

            for (int j = 5; j > 0; j--) {
                if (player[j].score > player[j - 1].score) {

                    int tempScore = player[j].score;
                    player[j].score = player[j - 1].score;
                    player[j - 1].score = tempScore;


                    char tempUsername[20];
                    strcpy(tempUsername, player[j].username);
                    strcpy(player[j].username, player[j - 1].username);
                    strcpy(player[j - 1].username, tempUsername);
                }
            }

            gameState = WIN;
            if (indiceOrNon) {
                hoverIndice = false;
                cliqueSurIndice = false;
            }
            entry = ' ';
            Mix_PlayChannelTimed(-1, winSFX, 0, 2600);
        }


        Uint32 currentTime2 = SDL_GetTicks();
        if (currentTime2 - lastToggleTime2 >= 500) {
            showText2 = !showText2;
            lastToggleTime2 = currentTime2;
        }

        SDL_RenderPresent(renderer);
    }

    bgMusic = NULL;
    loseSFX = NULL;
    winSFX = NULL;
    wrongGuessSFX = NULL;

    Mix_FreeMusic(bgMusic);
    Mix_FreeChunk(loseSFX);
    Mix_FreeChunk(winSFX);
    Mix_FreeChunk(wrongGuessSFX);
    Mix_CloseAudio();

    free(word);
    free(wordIndice);

    TTF_CloseFont(font);
    TTF_CloseFont(optionOptionsFont);
    TTF_CloseFont(font1);
    TTF_CloseFont(font3);
    TTF_CloseFont(font4);
    TTF_CloseFont(indiceFont);
    TTF_CloseFont(choseNumberFourFont);
    TTF_CloseFont(choseNumberFiveFont);
    TTF_CloseFont(choseNumberSixFont);
    TTF_CloseFont(yesIndiceFont);
    TTF_CloseFont(nonIndiceFont);
    TTF_CloseFont(yesSoundFont);
    TTF_CloseFont(nonSoundFont);
    TTF_CloseFont(menuTextJouerFont);
    TTF_CloseFont(menuTextReglementFont);
    TTF_CloseFont(menuTextOptionFont);

    SDL_DestroyTexture(premierePageImageTexture);
    SDL_DestroyTexture(menuPageImageTexture);
    SDL_DestroyTexture(gameImageTexture);
    SDL_DestroyTexture(backgroundplainImageTexture);
    SDL_DestroyTexture(reglementPageImageTexture);
    SDL_DestroyTexture(manWithoutEyesTexture);
    SDL_DestroyTexture(manWithEyesTexture);
    SDL_DestroyTexture(doorimagetexture);
    SDL_DestroyTexture(yesIndiceIconTexture);
    SDL_DestroyTexture(yesIndiceGreenIconTexture);
    SDL_DestroyTexture(CongratsTexture);
    SDL_DestroyTexture(lostTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
