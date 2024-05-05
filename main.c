#include<stdio.h>
#include<SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include<time.h>

//Constante
#define FALSE 0
#define TRUE 1
#define WINDOWWIDTH 1200
#define WINDOWHEIGHT 800
#define Hangman1W 61
#define Hangman1H 155
#define Hangman2W 128
#define Hangman2H 139
#define Hangman3W 51
#define Hangman3H 210
#define Hangman4_5W 72
#define Hangman4_5H 110
#define Hangman6_7W 80
#define Hangman6_7H 111

//VARIABLES Globales
int gameIsRunning = FALSE;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Surface* imageSurface = NULL;
SDL_Texture* imageTexture = NULL;
TTF_Font* font = NULL;
int tries=0;
SDL_Texture* hangmanTextures[7];
SDL_Texture* wordTexture = NULL;

int initializeSDL(void) {
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        //stderr standard error output
        fprintf(stderr,"Error initializing SDL.\n");
        return FALSE;
    }
    window = SDL_CreateWindow("Jeu pendu", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOWWIDTH, WINDOWHEIGHT,SDL_WINDOW_SHOWN);
    if(!window) {
        fprintf(stderr,"Error creating SDL Window.\n");
        return FALSE;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) {
        fprintf(stderr,"Error creating SDL Renderer.\n");
        return FALSE;
    }
    // SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    // SDL_RenderClear(renderer);
    IMG_Init(IMG_INIT_PNG); // Initialize SDL_image for PNG support
    imageSurface = IMG_Load("C:\\Users\\Mez Work\\CLionProjects\\SdlPractice\\cmake-build-debug\\src\\gameBAckground2.png");
    if (!imageSurface) {
        fprintf(stderr, "Error loading image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return FALSE;
    }
    // Create a texture from the image
    imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);
    if (!imageTexture) {
        fprintf(stderr, "Error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return FALSE;
    }
    // Render the texture to the window
    SDL_RenderCopy(renderer, imageTexture, NULL, NULL);
    SDL_RenderPresent(renderer);

    TTF_Init();
    font = TTF_OpenFont("./arial-font/G_ari_bd.TTF", 64);
    if (!font) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return FALSE;
    }
    SDL_Surface* surface;
    for (int i = 0; i < 7; i++) {
        char hangmanImagePath[50];
        sprintf(hangmanImagePath, "./src/hangman%d.png", i + 1);
        surface = IMG_Load(hangmanImagePath);
        if (!surface) {
            fprintf(stderr, "Error loading image: %s\n", IMG_GetError());
            return FALSE;
        }
        hangmanTextures[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!hangmanTextures[i]) {
            fprintf(stderr, "Error creating texture: %s\n", SDL_GetError());
            return FALSE;
        }
    }
    return TRUE;
}


void update() {

}
void footerMenu() {
    // Render menu options
    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, "Score", textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    float x = ((WINDOWWIDTH/3)-(surface->w))/2;
    SDL_Rect footerRect1 = {x, 700, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &footerRect1);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    surface = TTF_RenderText_Solid(font, "Option", textColor);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    x = ((WINDOWWIDTH/3)-(surface->w))/2 + (WINDOWWIDTH/3);
    SDL_Rect footerRect2 = {x, 700, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &footerRect2);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    surface = TTF_RenderText_Solid(font, "Quitter", textColor);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    x = ((WINDOWWIDTH/3)-(surface->w))/2 + (2*(WINDOWWIDTH/3));
    SDL_Rect footerRect3 = {x, 700, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &footerRect3);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    SDL_RenderPresent(renderer);
}
void destroyWindow() {
    IMG_Quit();
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
void drawHangman(int tries) {
    // Draw the corresponding hangman part based on the number of tries
    if(tries>=1) {
        //LA CORDE
        SDL_Rect hangmanRect0 = {1044, 51, Hangman1W, Hangman1H};
        SDL_RenderCopy(renderer, hangmanTextures[0], NULL, &hangmanRect0);
    }
    if(tries>=2) {
        //HEAD
        SDL_Rect hangmanRect1 = {1014, 51+ Hangman1H - 6, Hangman2W, Hangman2H};
        SDL_RenderCopy(renderer, hangmanTextures[1], NULL, &hangmanRect1);
    }
    if(tries>=3) {
        //BODY
        SDL_Rect hangmanRect2 = {1050, 51 +Hangman1H+Hangman2H-10 , Hangman3W, Hangman3H};
        SDL_RenderCopy(renderer, hangmanTextures[2], NULL, &hangmanRect2);
    }
    if(tries>=4) {
        //LEFT HAND
        SDL_Rect hangmanRect3 = {1014, 51 +Hangman1H+Hangman2H+ 32, Hangman4_5W, Hangman4_5H};
        SDL_RenderCopy(renderer, hangmanTextures[3], NULL, &hangmanRect3);
    }
    if(tries>=5) {
        //RIGHT hand
        SDL_Rect hangmanRect4 = {1068, 51 +Hangman1H+Hangman2H+ 32, Hangman4_5W, Hangman4_5H};
        SDL_RenderCopy(renderer, hangmanTextures[4], NULL, &hangmanRect4);
    }
    if(tries>=6) {
        //LEFT LEG
        SDL_Rect hangmanRect5 = {1004, 51 +Hangman1H+Hangman2H+ Hangman3H-32, Hangman6_7W, Hangman6_7H};
        SDL_RenderCopy(renderer, hangmanTextures[5], NULL, &hangmanRect5);
    }
    if(tries>=7) {
        SDL_Rect hangmanRect6 = {1068, 51 +Hangman1H+Hangman2H+ Hangman3H-32, Hangman6_7W, Hangman6_7H};
        SDL_RenderCopy(renderer, hangmanTextures[6], NULL, &hangmanRect6);
    }
    SDL_RenderPresent(renderer);
}
int getRendomWord(char *word) {
    srand(time(NULL));
    FILE *file = fopen("C:/Users/Mez Work/CLionProjects/SdlPractice/src/capitals.txt", "r");
    if(file == NULL){
        printf("Error opening the words file");
        return FALSE;
    }
     int aleo = rand() % 100 + 1;
    int i=1;
    while (fgets(word, 80, file) != NULL && i<= aleo) {
        // Remove the newline character if it exists
        size_t len = strlen(word);
        if (len > 0 && word[len - 1] == '\n') {
            word[len - 1] = '\0';
        }
        i++;
    }
    puts(word);
    fclose(file);
    return TRUE;
}
void hideMyWord(char* word, char*hiddenWord) {
    for(int i = 0; i < strlen(word) ; i++) {
        if(word[i] == 32 || word[i] == '-')
            hiddenWord[i] = ' ';
        else {
            hiddenWord[i] = '-';
        }
    }
    hiddenWord[strlen(word)]='\0';
}
int renderHiddenWord(const char *hiddenWord) {
    // Render the hidden word to the screen
    SDL_Color textColor = {0, 0, 0, 255}; //Couleur noir
    SDL_Surface* wordSurface = TTF_RenderText_Solid(font, hiddenWord, textColor);
    if (!wordSurface) {
        fprintf(stderr, "Error rendering text: %s\n", TTF_GetError());
        return FALSE;
    }
    wordTexture = SDL_CreateTextureFromSurface(renderer, wordSurface);
    SDL_FreeSurface(wordSurface);
    if (!wordTexture) {
        fprintf(stderr, "Error creating texture: %s\n", SDL_GetError());
        return FALSE;
    }
    //position ou on veux ecrire le text.
    int textWidth, textHeight;
    SDL_QueryTexture(wordTexture, NULL, NULL, &textWidth, &textHeight);
    int x = 204;
    int y = 296; // Adjust the vertical position as needed

    SDL_Rect wordRect = {x, y, textWidth, textHeight};
    SDL_RenderCopy(renderer, wordTexture, NULL, &wordRect);
    SDL_DestroyTexture(wordTexture);
    SDL_RenderPresent(renderer);
    return TRUE;
}
int main(int argc, char *argv[]) {
    char *word = malloc(80 * sizeof(char));
    char *hiddenWord = malloc(80 * sizeof(char));
    char enteredCharacter = '\0';
    char inputText[2];
    int tenta = 0;

    gameIsRunning = initializeSDL();
    //Function called once just to setup my game.
    // setup();
    /*drawHangman(1);
    drawHangman(2);
    drawHangman(3);
    drawHangman(4);
    drawHangman(5);
    drawHangman(6);
    drawHangman(7);*/
    getRendomWord(word);
    hideMyWord(word,hiddenWord);
    renderHiddenWord(hiddenWord);
    while (gameIsRunning) {
        SDL_Event event;
        SDL_PollEvent(&event);

        switch(event.type){
            case SDL_QUIT:
                gameIsRunning = FALSE;
            break;
            case SDL_KEYDOWN:
                if(event.key.keysym.sym == SDLK_ESCAPE)
                    gameIsRunning = FALSE;
                else
                    if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z) {
                        //CURRENT CLEARING SOLUTION FOR LETTER
                        // Define a rectangle representing the area to clear
                        SDL_Rect clearRect = {204, 484, 60, 80}; // Adjust the position and size as needed
                        // Clear the specified area
                        SDL_SetRenderDrawColor(renderer, 232, 164, 52, 255); // Set color to bg color fdik lblassa
                        SDL_RenderFillRect(renderer, &clearRect);

                        enteredCharacter = (char)(event.key.keysym.sym);
                        inputText[0] = enteredCharacter;
                        inputText[1] = '\0';

                        int change = 0;
                        for(int i = 0; i < strlen(hiddenWord) ; i++) {
                            if(word[i] == inputText[0]) {
                                hiddenWord[i]= inputText[0];
                                change=1;
                            }
                        }
                        if(!change)
                            tenta++;
                        //CURRENT CLEARING SOLUTION TO CLEAR LAST HIDDEN WORD
                        // Define a rectangle representing the area to clear
                        SDL_Rect clearLastHiddenWord = {204, 296, 472, 80}; // Adjust the position and size as needed
                        // Clear the specified area
                        SDL_SetRenderDrawColor(renderer, 253, 188, 20, 255); // Set color to bg color fdik lblassa
                        SDL_RenderFillRect(renderer, &clearLastHiddenWord);
                        renderHiddenWord(hiddenWord);
                        drawHangman(tenta);
                        if(tenta==7) {
                            tenta=0;
                            exit(0);
                        }
                }
                if (enteredCharacter != '\0') {
                    // Render the entered character on the screen
                    SDL_Color textColor = {0, 0, 0, 255};
                    SDL_Surface* charSurface = TTF_RenderText_Solid(font, inputText, textColor);
                    SDL_Texture* charTexture = SDL_CreateTextureFromSurface(renderer, charSurface);

                    // Adjust the position and size as needed
                    int x = 204;
                    int y = 484; // Adjust the vertical position as needed
                    SDL_Rect charRect = {x, y, charSurface->w, charSurface->h};
                    SDL_RenderCopy(renderer, charTexture, NULL, &charRect);

                    SDL_FreeSurface(charSurface);
                    SDL_DestroyTexture(charTexture);

                    // Reset entered character
                    enteredCharacter = '\0';
                }
            break;

        }
        footerMenu();
        update();

    }
    for (int i = 0; i < 7; i++) {
        SDL_DestroyTexture(hangmanTextures[i]);
    }
    free(word);
    free(hiddenWord);
    destroyWindow();
    return FALSE;
}
