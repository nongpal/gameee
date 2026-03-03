#include "mmapalloc.h"
#include "raylib.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define GAMEEE "gameee"
#define GAMEEE_DEFAULT_FPS 60
#define GAMEEE_MAX_CONTEXT_SWITCH 3

typedef struct {
    Rectangle bounds;
    Color bgColor;
    const char *text;
    Color textColor;
    int textSize;
} Button;

bool IsButtonPressed(Button *btn) {
    if (CheckCollisionPointRec(GetMousePosition(), btn->bounds) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return true;
    }
    return false;
}

void DrawButton(Button *btn) {
    // Uncomment the code below to enable center text alignment
    // and change X position parameter of the text in DrawText to textX
    // int textWidth = MeasureText(btn->text, btn->textSize);
    // int textX = btn->bounds.x + btn->bounds.width*0.5f - textWidth*0.5f;
    int textY = btn->bounds.y + btn->bounds.height*0.5f - btn->textSize*0.5f;

    DrawRectangleRec(btn->bounds, btn->bgColor);
    DrawText(btn->text, btn->bounds.x + 20.0f, textY, btn->textSize, btn->textColor);
}

enum GameeeContextFlags { MainWindows, MenuWindows, OptionWindows, None };

struct GameeeContextMetadata {
    bool windowsLifetime;
    enum GameeeContextFlags windowPrev;
};

struct GameeeContextAssets {
    Vector2 vec;
};

struct GameeeContextSwitching {
    enum GameeeContextFlags (*switchWindows[GAMEEE_MAX_CONTEXT_SWITCH])(
        struct GameeeContextMetadata *, struct GameeeContextAssets *);
};

// Main State

enum GameeeContextFlags MainWin(struct GameeeContextMetadata *metadata,
                                struct GameeeContextAssets *shared) {
#ifdef _WIN_32
#include <windows.h>
    // menggunakan API Nvidia untuk memakai GPU
    __declspec(dllexport) DWORD NvOptimusEnablement = 1;
    // menggunakan API AMD untuk memakai GPU
    __declspec(dllexport) int AmdPowerXpressRequestHighPerfomance = 1;

#endif /* ifdef __WIN_32  #include <windows.h> __declspec(dllexport) */

    Button btnResume = {
        .bounds = (Rectangle){ 100.0f, 200.0f, 200.0f, 50.0f},
        .bgColor = DARKGRAY,
        .text = "RESUME",
        .textColor = WHITE,
        .textSize = 30,
    };
    Button btnOption = {
        .bounds = (Rectangle){ 100.0f, 270.0f, 200.0f, 50.0f},
        .bgColor = DARKGRAY,
        .text = "OPTION",
        .textColor = WHITE,
        .textSize = 30,
    };
    Button btnReturn = {
        .bounds = (Rectangle){ 100.0f, 340.0f, 200.0f, 50.0f},
        .bgColor = DARKGRAY,
        .text = "RETURN",
        .textColor = WHITE,
        .textSize = 30,
    };

    Vector2 *vec = &shared[MainWindows].vec;
    // event handler
    if (IsKeyPressed(KEY_A)) {
        return MenuWindows;
    }
    if (IsKeyDown(KEY_SPACE)) {
        vec->y += 10;
    }

    if (IsButtonPressed(&btnResume)) return MainWindows;
    if (IsButtonPressed(&btnOption)) {
        metadata->windowPrev = MainWindows;
        return OptionWindows;
    }
    if (IsButtonPressed(&btnReturn)) return MenuWindows;

    ClearBackground(BLACK);
    DrawText("MAIN WINDOWS", 100, 100, 50, WHITE);
    DrawRectangle(50, 100, (int)vec->x, (int)vec->y, LIME);
    DrawButton(&btnResume);
    DrawButton(&btnOption);
    DrawButton(&btnReturn);

    return MainWindows;
}

// Menu State

enum GameeeContextFlags MenuWin(struct GameeeContextMetadata *metadata,
                                struct GameeeContextAssets *shared) {
    Button btnNewGame = {
        .bounds = (Rectangle){ 100.0f, 200.0f, 200.0f, 50.0f},
        .bgColor = DARKGRAY,
        .text = "NEW GAME",
        .textColor = WHITE,
        .textSize = 30,
    };
    Button btnContinue = {
        .bounds = (Rectangle){ 100.0f, 270.0f, 200.0f, 50.0f},
        .bgColor = DARKGRAY,
        .text = "CONTINUE",
        .textColor = WHITE,
        .textSize = 30,
    };
    Button btnOption = {
        .bounds = (Rectangle){ 100.0f, 340.0f, 200.0f, 50.0f},
        .bgColor = DARKGRAY,
        .text = "OPTION",
        .textColor = WHITE,
        .textSize = 30,
    };
    Button btnExit = {
        .bounds = (Rectangle){ 100.0f, 410.0f, 200.0f, 50.0f},
        .bgColor = DARKGRAY,
        .text = "EXIT",
        .textColor = WHITE,
        .textSize = 30,
    };

    Vector2 *vec = &shared[MenuWindows].vec;
    if (IsKeyPressed(KEY_A)) {
        return MainWindows;
    }
    if (IsKeyDown(KEY_SPACE)) {
        vec->y += 10;
    }
    if (IsButtonPressed(&btnNewGame)) return MainWindows;
    if (IsButtonPressed(&btnContinue)) return MainWindows;
    if (IsButtonPressed(&btnOption)) {
        metadata->windowPrev = MenuWindows;
        return OptionWindows;
    }
    if (IsButtonPressed(&btnExit)) CloseWindow();

    ClearBackground(BLACK);
    DrawText("MENU WINDOWS", 100, 100, 50, WHITE);
    DrawRectangle(50, 100, (int)vec->x, (int)vec->y, SKYBLUE);
    DrawButton(&btnNewGame);
    DrawButton(&btnContinue);
    DrawButton(&btnOption);
    DrawButton(&btnExit);

    return MenuWindows;
}

// Option State

enum GameeeContextFlags OptionWin(struct GameeeContextMetadata *metadata,
                                struct GameeeContextAssets *shared) {
    Button btnReturn = {
        .bounds = (Rectangle){ 100.0f, 200.0f, 200.0f, 50.0f},
        .bgColor = DARKGRAY,
        .text = "RETURN",
        .textColor = WHITE,
        .textSize = 30,
    };

    if (IsKeyPressed(KEY_A)) {
        return MainWindows;
    }
    if (IsButtonPressed(&btnReturn)) {
        if (metadata->windowPrev == MainWindows) return MainWindows;
        if (metadata->windowPrev == MenuWindows) return MenuWindows;
    }

    ClearBackground(BLACK);
    DrawText("OPTION WINDOWS", 100, 100, 50, WHITE);
    DrawButton(&btnReturn);

    return OptionWindows;
}


int main(void) {
    const int screenWidth = 1920, screenHeight = 1200, minScreenWidth = 810,
              minScreenHeight = 600;

    char *memory = (char *)mmapalloc(1024);
    printf("INFO : initialize first arena\n");
    char *second_memory = (char *)mmapalloc(32);
    printf("PTR : %p\n", second_memory);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | // enable resizable window
                   FLAG_VSYNC_HINT |       // enable vsync
                   FLAG_MSAA_4X_HINT);     // enable anti-alias

    InitWindow(screenWidth, screenHeight, GAMEEE);
    SetWindowMinSize(minScreenWidth, minScreenHeight);

    SetTargetFPS(GAMEEE_DEFAULT_FPS);

    struct GameeeContextMetadata mdata; // Metadata shared
    struct GameeeContextAssets *context_assets = mmapalloc(
        sizeof(struct GameeeContextAssets) * GAMEEE_MAX_CONTEXT_SWITCH);
    if (!context_assets) {
        fprintf(stderr, GAMEEE "failed to allocate struct\n");
        return EXIT_FAILURE;
    }
    mdata.windowsLifetime = true;
    mdata.windowPrev = None;

    context_assets[MainWindows].vec = (Vector2){10, 20};
    context_assets[MenuWindows].vec = (Vector2){10, 20};

    struct GameeeContextSwitching switchCtx;
    /*
     * fungsi struct switch context,menyimpan function to ptr
     */

    switchCtx.switchWindows[MainWindows] = MainWin;
    switchCtx.switchWindows[MenuWindows] = MenuWin;
    switchCtx.switchWindows[OptionWindows] = OptionWin;

    enum GameeeContextFlags flagsCtx = MenuWindows;

    while (!WindowShouldClose()) {
        BeginDrawing();
        switch (switchCtx.switchWindows[flagsCtx](&mdata, context_assets)) {
        case MainWindows:
            flagsCtx = MainWindows;
            break;
        case MenuWindows:
            flagsCtx = MenuWindows;
            break;
        case OptionWindows:
            flagsCtx = OptionWindows;
            break;
        case None:
            // fprintf(stderr,"ERROR : failed to change state,state not
            // found\n");
            break;
        default:
            break;
        }
        EndDrawing();
    }

    CloseWindow();
    fprintf(stdout, "INFO : Block active %d\n", mmapalloc_destroy());
    return 0;
}
