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

        DrawRectangle(width, 0, 1, height, WHITE);

    EndTextureMode();
}

void makeHorizontalLine(int width, int height, RenderTexture2D& target)
{
    BeginTextureMode(target);

    DrawRectangle(0, height, width, 1, WHITE);

    EndTextureMode();
}

int main()
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    const int canvasWidth = screenWidth;
    const int canvasHeight = screenHeight;

    InitWindow(screenWidth, screenHeight, "Raylib GLSL GoL");
    
    RenderTexture2D currentGen =    LoadRenderTexture(canvasWidth, canvasHeight);
    RenderTexture2D nextGen =       LoadRenderTexture(canvasWidth, canvasHeight);
    RenderTexture2D target =        LoadRenderTexture(canvasWidth, canvasHeight);
    
    auto* currentPointer = &currentGen;
    auto* nextPointer = &nextGen;

    Rectangle shaderRect{ 0, 0, (float)currentGen.texture.width, (float)-currentGen.texture.height };

    Shader shader = LoadShader(0, "shaders/gol.frag");
    int u_resolution_loc = GetShaderLocation(shader, "u_resolution");
    int u_cur_loc = GetShaderLocation(shader, "u_cur");
    float s_resolution[2] = { (float)target.texture.width, (float)target.texture.height };
    
    SetShaderValue(shader, u_resolution_loc, s_resolution, SHADER_UNIFORM_VEC2);

    bool runSimulation = false;
    float simTimer = 0.f;
    float simUpdateTime = 0.007f; // 144fps

    fillScreen({ (float)screenWidth, (float)screenHeight }, nextGen);
    fillScreen({ (float)screenWidth, (float)screenHeight }, currentGen);
    //fillScreenRand({ (float)screenWidth, (float)screenHeight }, currentGen);

    SetShaderValueTexture(shader, u_cur_loc, nextGen.texture);

    Camera2D camera{ {target.texture.width / 2.f, target.texture.height / 2.f}, {target.texture.width / 2.f, target.texture.height / 2.f}, 0.f,1.f };

    std::string infoString = "Cells: " + std::to_string(target.texture.width * target.texture.height);

    BeginTextureMode(target);
    EndTextureMode();
    SetShaderValueTexture(shader, u_cur_loc, currentPointer->texture);
    BeginDrawing();
    DrawText(infoString.c_str(), 10, 30, 20, RED);
    EndDrawing();

    Vector2 oldMousePos{};

    while (!WindowShouldClose())
    {
        const float delta = GetFrameTime();
        Vector2 mouse = GetMousePosition();

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            BeginTextureMode(*currentPointer);
            Vector2 worldMouse = GetScreenToWorld2D(mouse, camera);
            DrawPixelV(worldMouse, WHITE);
            EndTextureMode();
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            std::cout << "Current " << currentPointer->id << " Next " << nextPointer->id << std::endl;

        if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON))
        {
            camera.target.x += oldMousePos.x - mouse.x;
            camera.target.y += oldMousePos.y - mouse.y;
        }

        camera.zoom += ((float)GetMouseWheelMove() * 0.05f);

        if (IsKeyPressed(KEY_SPACE))
            runSimulation = !runSimulation;

        if (IsKeyPressed(KEY_C))
            fillScreen({ (float)currentGen.texture.width, (float)currentGen.texture.height }, *currentPointer);

        if (IsKeyPressed(KEY_D))
            makeHorizontalLine(screenWidth, GetMouseY(), *currentPointer);

        if (IsKeyPressed(KEY_A))
            makeVerticalLine(GetMouseX(), screenHeight, *currentPointer);

        if (IsKeyPressed(KEY_B))
            spawnGlider(GetMouseX(), GetMouseY(), *currentPointer);

        BeginTextureMode(target);
        ClearBackground(RAYWHITE);
        DrawTexture(currentPointer->texture, 0,0, WHITE);

        EndTextureMode();

        simTimer += delta;
        if (runSimulation)
            if (simTimer > simUpdateTime)
            {
                BeginTextureMode(*nextPointer);
                    BeginShaderMode(shader);
                    DrawTextureRec(nextPointer->texture, shaderRect, {0.f,0.f}, WHITE);
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

            BeginMode2D(camera);
            DrawTextureRec(target.texture, { 0.f,0.f,(float)target.texture.width,(float)target.texture.height }, { 0.f,0.f }, WHITE);
            EndMode2D();
            DrawFPS(10, 10);
            DrawText(infoString.c_str(),10,30,20,RED);

        EndDrawing();

        oldMousePos = mouse;
    }

    UnloadRenderTexture(currentGen);
    UnloadRenderTexture(nextGen);
    UnloadShader(shader);

    CloseWindow();

    return 0;
}
