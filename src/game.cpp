#include "irys_ion/game.hpp"
#include "irys_ion/core.hpp"

#include <raylib.h>

#include <string>

namespace irys {

int runGame(int argc, char** argv) {
    bool validateOnly = false;
    for (int i = 1; i < argc; ++i) if (std::string(argv[i]) == "--validate") validateOnly = true;
    std::string warning;
    const auto tuning = Tuning::load(std::string(IRYSION_RESOURCE_DIR) + "/config/tuning.json", warning);
    if (validateOnly) return warning.empty() && tuning.simulationHz == 60 ? 0 : 2;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "Irys+Ion");
    SetTargetFPS(120);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground({7, 9, 16, 255});
        DrawText("IRYS+ION", 48, 44, 48, {80, 238, 255, 255});
        DrawText("Deterministic foundation active", 52, 108, 20, {255, 166, 130, 255});
        DrawText("Complete campaign runtime integration in progress", 52, 140, 18, {170, 180, 205, 255});
        if (!warning.empty()) DrawText(warning.c_str(), 52, 680, 16, {255, 100, 110, 255});
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

} // namespace irys
