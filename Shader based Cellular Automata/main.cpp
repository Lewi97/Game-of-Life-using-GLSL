#include <iostream>
#include <raylib.h>
#include <string>

void fillScreen(Vector2 screen, RenderTexture2D& target)
{
    BeginTextureMode(target);

    for (int x = 0; x < screen.x; x++)
        for (int y = 0; y < screen.y; y++)
            DrawPixel(x, y, BLACK);

    EndTextureMode();
}

void fillScreenRand(Vector2 screen, RenderTexture2D& target)
{
    BeginTextureMode(target);

    for (int x = 0; x < screen.x; x++)
        for (int y = 0; y < screen.y; y++)
            if (GetRandomValue(1, 2) & 1)
                DrawPixel(x, y, BLACK);
            else
                DrawPixel(x, y, WHITE);

    EndTextureMode();
}

void swapPointers(void* p1, void* p2)
{
    void* swap = p1;
    p1 = p2;
    p2 = swap;
}

void makeVerticalLine(int width, int height, RenderTexture2D& target)
{
    BeginTextureMode(target);

        DrawRectangle(width, 0, 1, height, WHITE);

    EndTextureMode();
}

int main()
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Raylib GLSL GoL");
    
    RenderTexture2D currentGen = LoadRenderTexture(screenWidth, screenHeight);
    RenderTexture2D nextGen = LoadRenderTexture(screenWidth, screenHeight);

    Rectangle targetRect{ 0, 0, (float)currentGen.texture.width, (float)-currentGen.texture.height };

    Shader shader = LoadShader(0, "shaders/gol.frag");
    int u_resolution_loc = GetShaderLocation(shader, "u_resolution");
    int u_cur_loc = GetShaderLocation(shader, "u_cur");

    float s_resolution[2] = { (float)screenWidth, (float)screenHeight };
    
    SetShaderValueTexture(shader, u_cur_loc, currentGen.texture);
    SetShaderValue(shader, u_resolution_loc, s_resolution, SHADER_UNIFORM_VEC2);

    bool runSimulation = false;
    float simTimer = 0.f;
    float simUpdateTime = 0.016f;

    fillScreen({ (float)screenWidth, (float)screenHeight }, nextGen);
    fillScreenRand({ (float)screenWidth, (float)screenHeight }, nextGen);
    
    currentGen.texture = nextGen.texture;

    while (!WindowShouldClose())
    {
        const float delta = GetFrameTime();

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            BeginTextureMode(nextGen);
                Vector2 mouse = GetMousePosition();
                DrawPixelV(mouse, WHITE);
            EndTextureMode();
        }

        if (IsKeyPressed(KEY_SPACE))
            runSimulation = !runSimulation;

        if (IsKeyPressed(KEY_C))
            fillScreen({screenWidth, screenHeight}, nextGen);

        SetShaderValueTexture(shader, u_cur_loc, currentGen.texture);

        if (IsKeyPressed(KEY_A))
            makeVerticalLine(GetMouseX(), screenHeight, nextGen);

        if (runSimulation)
        {
            simTimer += delta;
            if (simTimer > simUpdateTime)
            {
                swapPointers(&currentGen, &nextGen);
                BeginTextureMode(nextGen);
                    BeginShaderMode(shader);
                        DrawTextureRec(nextGen.texture, targetRect, { 0.f,0.f }, WHITE);
                    EndShaderMode();
                EndTextureMode();
                simTimer = 0.f;
            }
        }

        BeginDrawing();

            ClearBackground(RAYWHITE);
            DrawTextureRec(nextGen.texture, targetRect, { 0.f,0.f }, WHITE);
            DrawFPS(10, 10);
            std::string infoString = "Cells: " + std::to_string(screenHeight * screenWidth);
            DrawText(infoString.c_str(),10,30,20,RED);

        EndDrawing();

    }

    CloseWindow();

    return 0;
}
