#include <iostream>
#include <raylib.h>
#include <string>

void fillScreen(Vector2 screen, RenderTexture2D& target)
{
    BeginTextureMode(target);
    ClearBackground(BLACK);

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

void spawnGlider(int x, int y, RenderTexture2D& target)
{
    BeginTextureMode(target);
    
    DrawPixel(x, y - 1, WHITE);
    DrawPixel(x + 1, y, WHITE);
    DrawRectangle(x - 1, y + 1, 3, 1, WHITE);
    
    EndTextureMode();
}

void makeVerticalLine(int width, int height, RenderTexture2D& target)
{
    BeginTextureMode(target);

        DrawRectangle(width, 50, 1, height-100, WHITE);

    EndTextureMode();
}

int main()
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Raylib GLSL GoL");
    
    RenderTexture2D currentGen = LoadRenderTexture(screenWidth, screenHeight);
    RenderTexture2D nextGen = LoadRenderTexture(screenWidth, screenHeight);

    auto* currentPointer = &currentGen;
    auto* nextPointer = &nextGen;

    Rectangle targetRect{ 0, 0, (float)currentGen.texture.width, (float)-currentGen.texture.height };

    Shader shader = LoadShader(0, "shaders/gol.frag");
    int u_resolution_loc = GetShaderLocation(shader, "u_resolution");
    int u_cur_loc = GetShaderLocation(shader, "u_cur");
    float s_resolution[2] = { (float)screenWidth, (float)screenHeight };
    
    SetShaderValue(shader, u_resolution_loc, s_resolution, SHADER_UNIFORM_VEC2);

    bool runSimulation = false;
    float simTimer = 0.f;
    float simUpdateTime = 0.007f; // 144fps

    fillScreen({ (float)screenWidth, (float)screenHeight }, nextGen);
    fillScreen({ (float)screenWidth, (float)screenHeight }, currentGen);

    SetShaderValueTexture(shader, u_cur_loc, currentGen.texture);

    while (!WindowShouldClose())
    {
        const float delta = GetFrameTime();

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            BeginTextureMode(*currentPointer);
                Vector2 mouse = GetMousePosition();
                DrawPixelV(mouse, WHITE);
            EndTextureMode();
        }

        if (IsKeyPressed(KEY_SPACE))
            runSimulation = !runSimulation;

        if (IsKeyPressed(KEY_C))
            fillScreen({screenWidth, screenHeight}, *currentPointer);

        if (IsKeyPressed(KEY_A))
            makeVerticalLine(GetMouseX(), screenHeight, *currentPointer);

        if (IsKeyPressed(KEY_B))
            spawnGlider(GetMouseX(), GetMouseY(), *currentPointer);

        simTimer += delta;
        if (runSimulation)
            if (simTimer > simUpdateTime)
            {
                BeginTextureMode(*nextPointer);
                    BeginShaderMode(shader);
                        DrawTextureRec(nextPointer->texture, targetRect, { 0.f,0.f }, WHITE);
                    EndShaderMode();
                EndTextureMode();
                
                auto* swap = currentPointer;
                currentPointer = nextPointer;
                nextPointer = swap;
                
                simTimer = 0.f;
                
                SetShaderValueTexture(shader, u_cur_loc, currentPointer->texture);
            }
        

        BeginDrawing();

            ClearBackground(RAYWHITE);
            DrawTextureRec(currentPointer->texture, targetRect, { 0.f,0.f }, WHITE);
            DrawFPS(10, 10);
            std::string infoString = "Cells: " + std::to_string(screenHeight * screenWidth);
            DrawText(infoString.c_str(),10,30,20,RED);

        EndDrawing();
    }

    UnloadRenderTexture(currentGen);
    UnloadRenderTexture(nextGen);
    UnloadShader(shader);

    CloseWindow();

    return 0;
}
