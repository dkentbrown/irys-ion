#include "irys_ion/player_foundation.hpp"
#include "irys_ion/rig.hpp"

#include <raylib.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

using namespace irys_ion;

namespace {

constexpr int kCanvasWidth = 640;
constexpr int kCanvasHeight = 360;

struct TextureSet {
    std::vector<Texture2D> rigParts;
    std::vector<std::size_t> rigDrawOrder;
    Texture2D background{};
    Texture2D foreground{};
    Texture2D tiles{};
};

bool textureReady(Texture2D texture) { return texture.id != 0 && texture.width > 0 && texture.height > 0; }

void unloadTextures(TextureSet& textures) {
    for (Texture2D texture : textures.rigParts) {
        if (textureReady(texture)) UnloadTexture(texture);
    }
    if (textureReady(textures.background)) UnloadTexture(textures.background);
    if (textureReady(textures.foreground)) UnloadTexture(textures.foreground);
    if (textureReady(textures.tiles)) UnloadTexture(textures.tiles);
}

std::optional<TextureSet> loadTextures(const std::filesystem::path& root,
                                       const RigDefinition& rig, std::string& error) {
    const auto assets = root / "assets/foundation";
    TextureSet textures;
    for (const RigPart& part : rig.parts) {
        Texture2D texture = LoadTexture((rig.directory / part.texturePath).string().c_str());
        if (!textureReady(texture) || texture.width != part.textureWidth ||
            texture.height != part.textureHeight) {
            if (textureReady(texture)) UnloadTexture(texture);
            error = "Rig texture failed to load or changed dimensions: " + part.texturePath.string();
            unloadTextures(textures);
            return std::nullopt;
        }
        SetTextureFilter(texture, TEXTURE_FILTER_POINT);
        textures.rigParts.push_back(texture);
    }
    textures.rigDrawOrder.resize(rig.parts.size());
    for (std::size_t i = 0; i < rig.parts.size(); ++i) textures.rigDrawOrder[i] = i;
    std::stable_sort(textures.rigDrawOrder.begin(), textures.rigDrawOrder.end(),
        [&](std::size_t a, std::size_t b) {
            return rig.parts[a].renderOrder < rig.parts[b].renderOrder;
        });
    textures.background = LoadTexture((assets / "foundation_background.png").string().c_str());
    textures.foreground = LoadTexture((assets / "foundation_foreground.png").string().c_str());
    textures.tiles = LoadTexture((assets / "industrial_tiles.png").string().c_str());
    if (!textureReady(textures.background) || !textureReady(textures.foreground) ||
        !textureReady(textures.tiles)) {
        error = "A required committed PNG could not be loaded from " + assets.string();
        unloadTextures(textures);
        return std::nullopt;
    }
    if (textures.background.width != 1920 || textures.background.height != 720 ||
        textures.foreground.width != 1920 || textures.foreground.height != 720) {
        error = "Foundation PNG dimensions do not match committed metadata";
        unloadTextures(textures);
        return std::nullopt;
    }
    SetTextureFilter(textures.background, TEXTURE_FILTER_POINT);
    SetTextureFilter(textures.foreground, TEXTURE_FILTER_POINT);
    SetTextureFilter(textures.tiles, TEXTURE_FILTER_POINT);
    return textures;
}

struct RenderContext {
    const TextureSet& textures;
    const RigDefinition& rig;
    const RigAnimations& animations;
    const RoomDefinition& room;
    const PlayerController& playerController;
    const CameraController& cameraController;
    const FoundationTuning& tuning;
};

Vector2 toScreen(Vec2 world, Vec2 cameraOrigin) {
    return {static_cast<float>(std::round(world.x - cameraOrigin.x)),
            static_cast<float>(std::round(world.y - cameraOrigin.y))};
}

Vec2 rigWorldPosition(const RigPose& pose, std::size_t index, Vec2 anchor) {
    return {anchor.x + pose.parts[index].transform.position.x,
            anchor.y + pose.parts[index].transform.position.y};
}

void drawRig(const RenderContext& context, const RigPose& pose,
             Vec2 anchor, Vec2 cameraOrigin) {
    for (std::size_t index : context.textures.rigDrawOrder) {
        const RigPart& part = context.rig.parts[index];
        const RigTransform& transform = pose.parts[index].transform;
        const Texture2D texture = context.textures.rigParts[index];
        const Vector2 joint = toScreen(rigWorldPosition(pose, index, anchor), cameraOrigin);
        Rectangle source{0.0F, 0.0F, static_cast<float>(texture.width), static_cast<float>(texture.height)};
        double pivotX = part.pivot.x;
        if (!pose.facingRight) {
            source.x = static_cast<float>(texture.width);
            source.width = -source.width;
            pivotX = static_cast<double>(texture.width) - pivotX;
        }
        DrawTexturePro(texture, source,
            {joint.x, joint.y,
             static_cast<float>(texture.width * transform.scale.x),
             static_cast<float>(texture.height * transform.scale.y)},
            {static_cast<float>(pivotX * transform.scale.x),
             static_cast<float>(part.pivot.y * transform.scale.y)},
            static_cast<float>(transform.rotation), WHITE);
    }
}

void drawRigJoints(const RenderContext& context, const RigPose& pose,
                   Vec2 anchor, Vec2 cameraOrigin) {
    for (std::size_t index = 0; index < context.rig.parts.size(); ++index) {
        const RigPart& part = context.rig.parts[index];
        if (part.parent.empty()) continue;
        const std::size_t parent = context.rig.partIndex(part.parent);
        const Vector2 a = toScreen(rigWorldPosition(pose, parent, anchor), cameraOrigin);
        const Vector2 b = toScreen(rigWorldPosition(pose, index, anchor), cameraOrigin);
        DrawLineV(a, b, Color{64, 184, 197, 115});
    }
    const std::array<std::pair<std::string_view, Color>, 15> joints{{
        {"root", Color{255, 255, 255, 255}}, {"pelvis", Color{255, 193, 77, 255}},
        {"thigh_front", Color{255, 193, 77, 255}}, {"shin_front", Color{60, 238, 255, 255}},
        {"foot_front", Color{126, 242, 174, 255}}, {"thigh_rear", Color{255, 193, 77, 255}},
        {"shin_rear", Color{60, 238, 255, 255}}, {"foot_rear", Color{126, 242, 174, 255}},
        {"upper_arm_front", Color{255, 118, 173, 255}}, {"forearm_front", Color{203, 130, 255, 255}},
        {"upper_arm_rear", Color{255, 118, 173, 255}}, {"forearm_rear", Color{203, 130, 255, 255}},
        {"hand_front", Color{255, 255, 255, 255}}, {"ion_grip", Color{54, 225, 255, 255}},
        {"ion_blade", Color{184, 249, 255, 255}},
    }};
    for (const auto& [name, color] : joints) {
        const std::size_t index = context.rig.partIndex(name);
        const Vector2 point = toScreen(rigWorldPosition(pose, index, anchor), cameraOrigin);
        DrawCircleLines(static_cast<int>(point.x), static_cast<int>(point.y), 3.0F, color);
        DrawCircleV(point, 1.0F, color);
    }
}

void drawDebug(const RenderContext& context, const PlayerState& player,
               const CameraState& camera, Vec2 cameraOrigin, int markerIndex,
               std::string_view status, double alpha, const RigPose& pose,
               Vec2 playerAnchor) {
    const auto screenRect = [&](Rect world) {
        return Rectangle{static_cast<float>(std::round(world.x - cameraOrigin.x)),
                         static_cast<float>(std::round(world.y - cameraOrigin.y)),
                         static_cast<float>(world.width), static_cast<float>(world.height)};
    };
    for (const auto& surface : context.room.surfaces) {
        const Color color = surface.kind == SurfaceKind::Solid ? Color{255, 88, 88, 190}
                                                               : Color{255, 208, 72, 210};
        const Rectangle rect = screenRect(surface.bounds);
        if (surface.kind == SurfaceKind::Solid) DrawRectangleLinesEx(rect, 1.0F, color);
        else DrawLine(static_cast<int>(rect.x), static_cast<int>(rect.y),
                      static_cast<int>(rect.x + rect.width), static_cast<int>(rect.y), color);
    }
    DrawRectangleLinesEx(screenRect(context.room.bounds), 1.0F, Color{224, 74, 143, 220});
    DrawRectangleLinesEx(screenRect(context.playerController.collisionBounds(player)), 1.0F,
                         Color{32, 237, 238, 255});
    DrawRectangleLinesEx(screenRect(context.cameraController.deadZone(camera)), 1.0F,
                         Color{110, 221, 155, 180});
    const Vector2 target = toScreen(camera.target, cameraOrigin);
    DrawLine(static_cast<int>(target.x) - 5, static_cast<int>(target.y),
             static_cast<int>(target.x) + 5, static_cast<int>(target.y), Color{255, 255, 255, 220});
    DrawLine(static_cast<int>(target.x), static_cast<int>(target.y) - 5,
             static_cast<int>(target.x), static_cast<int>(target.y) + 5, Color{255, 255, 255, 220});

    for (std::size_t i = 0; i < context.room.markers.size(); ++i) {
        const Vector2 marker = toScreen(context.room.markers[i].position, cameraOrigin);
        const Color color = static_cast<int>(i) == markerIndex ? Color{255, 219, 95, 255}
                                                               : Color{255, 255, 255, 120};
        DrawLine(static_cast<int>(marker.x), static_cast<int>(marker.y) - 8,
                 static_cast<int>(marker.x), static_cast<int>(marker.y) + 4, color);
        DrawText(context.room.markers[i].name.c_str(), static_cast<int>(marker.x) + 4,
                 static_cast<int>(marker.y) - 7, 6, color);
    }

    drawRigJoints(context, pose, playerAnchor, cameraOrigin);

    DrawRectangle(8, 8, 236, 83, Color{7, 10, 13, 225});
    DrawRectangleLines(8, 8, 236, 83, Color{50, 215, 218, 170});
    const std::string line1 = "STATE  " + std::string(locomotionStateName(player.locomotion));
    const std::string line2 = "VEL    " + std::to_string(static_cast<int>(std::round(player.velocity.x))) +
                              "  " + std::to_string(static_cast<int>(std::round(player.velocity.y)));
    const std::string line3 = "GROUND " + std::string(player.grounded ? "YES" : "NO") +
                              "  ONE-WAY " + (player.groundedOnOneWay ? "YES" : "NO");
    const std::string line4 = "COYOTE " + std::to_string(player.coyoteRemaining).substr(0, 5) +
                              "  BUFFER " + std::to_string(player.jumpBufferRemaining).substr(0, 5);
    const std::string line5 = "CAM    " + std::to_string(static_cast<int>(camera.position.x)) +
                              "  " + std::to_string(static_cast<int>(camera.position.y)) +
                              "  ALPHA " + std::to_string(alpha).substr(0, 4);
    DrawText(line1.c_str(), 14, 14, 8, RAYWHITE);
    DrawText(line2.c_str(), 14, 27, 8, Color{95, 236, 239, 255});
    DrawText(line3.c_str(), 14, 40, 8, Color{252, 193, 88, 255});
    DrawText(line4.c_str(), 14, 53, 8, Color{235, 112, 169, 255});
    DrawText(line5.c_str(), 14, 66, 8, Color{157, 225, 180, 255});
    if (!status.empty()) DrawText(std::string(status).c_str(), 14, 79, 7, Color{255, 116, 116, 255});
}

void renderScene(RenderTexture2D canvas, const RenderContext& context,
                 const PlayerState& player, const CameraState& camera,
                 double alpha, bool debug, int markerIndex, std::string_view status) {
    const Vec2 interpolatedCamera{
        camera.previousPosition.x + (camera.position.x - camera.previousPosition.x) * alpha,
        camera.previousPosition.y + (camera.position.y - camera.previousPosition.y) * alpha};
    const Vec2 cameraOrigin{std::floor(interpolatedCamera.x - kCanvasWidth * 0.5),
                            std::floor(interpolatedCamera.y - kCanvasHeight * 0.5)};
    const Vec2 interpolatedPlayer{
        player.previousPosition.x + (player.position.x - player.previousPosition.x) * alpha,
        player.previousPosition.y + (player.position.y - player.previousPosition.y) * alpha};

    BeginTextureMode(canvas);
    ClearBackground(Color{4, 7, 9, 255});
    const Rectangle worldSource{static_cast<float>(cameraOrigin.x), static_cast<float>(cameraOrigin.y),
                                static_cast<float>(kCanvasWidth), static_cast<float>(kCanvasHeight)};
    DrawTextureRec(context.textures.background, worldSource, {0.0F, 0.0F}, WHITE);

    const Vec2 playerAnchor{interpolatedPlayer.x + context.tuning.movement.playerWidth * 0.5,
                            interpolatedPlayer.y + context.tuning.movement.playerHeight};
    const double renderStateTime = std::max(0.0, player.stateTime -
        (1.0 - alpha) / context.tuning.movement.fixedHz);
    const RigPose pose = evaluateRig(context.rig, context.animations,
                                     rigClipName(player.locomotion), renderStateTime,
                                     player.facingRight);
    drawRig(context, pose, playerAnchor, cameraOrigin);
    DrawTextureRec(context.textures.foreground, worldSource, {0.0F, 0.0F}, WHITE);

    if (debug) drawDebug(context, player, camera, cameraOrigin, markerIndex, status, alpha,
                         pose, playerAnchor);
    EndTextureMode();
}

void presentCanvas(RenderTexture2D canvas) {
    const int windowWidth = GetScreenWidth();
    const int windowHeight = GetScreenHeight();
    const int integerScale = std::max(1, std::min(windowWidth / kCanvasWidth, windowHeight / kCanvasHeight));
    const int drawWidth = kCanvasWidth * integerScale;
    const int drawHeight = kCanvasHeight * integerScale;
    const int offsetX = (windowWidth - drawWidth) / 2;
    const int offsetY = (windowHeight - drawHeight) / 2;
    BeginDrawing();
    ClearBackground(Color{2, 3, 4, 255});
    DrawTexturePro(canvas.texture,
                   {0.0F, 0.0F, static_cast<float>(kCanvasWidth), -static_cast<float>(kCanvasHeight)},
                   {static_cast<float>(offsetX), static_cast<float>(offsetY),
                    static_cast<float>(drawWidth), static_cast<float>(drawHeight)},
                   {0.0F, 0.0F}, 0.0F, WHITE);
    EndDrawing();
}

bool exportCanvas(RenderTexture2D canvas, const std::filesystem::path& output) {
    std::filesystem::create_directories(output.parent_path());
    Image image = LoadImageFromTexture(canvas.texture);
    ImageFlipVertical(&image);
    const bool ok = ExportImage(image, output.string().c_str());
    UnloadImage(image);
    return ok;
}

struct CaptureCase {
    const char* filename;
    LocomotionState state;
    Vec2 position;
    Vec2 velocity;
    double stateTime;
    bool facingRight;
    bool debug;
};

bool captureValidationSet(RenderTexture2D canvas, const RenderContext& context,
                          const std::filesystem::path& directory) {
    const std::array<CaptureCase, 18> cases{{
        {"01_idle.png", LocomotionState::Idle, {112, 570}, {0, 0}, 0.4, true, false},
        {"02_acceleration.png", LocomotionState::Accelerating, {500, 570}, {110, 0}, 0.24, true, false},
        {"03_run_contact.png", LocomotionState::Running, {640, 570}, {245, 0}, 0.0, true, false},
        {"04_run_recovery.png", LocomotionState::Running, {700, 570}, {245, 0}, 0.16, true, false},
        {"05_run_opposite.png", LocomotionState::Running, {760, 570}, {245, 0}, 0.32, true, false},
        {"06_run_recovery_opposite.png", LocomotionState::Running, {820, 570}, {245, 0}, 0.48, true, false},
        {"07_braking.png", LocomotionState::Braking, {940, 570}, {110, 0}, 0.2, true, false},
        {"08_turn_start.png", LocomotionState::Turning, {1040, 570}, {-70, 0}, 0.0, false, false},
        {"09_turn_midpoint.png", LocomotionState::Turning, {1100, 570}, {-70, 0}, 0.24, false, false},
        {"10_turn_completion.png", LocomotionState::Turning, {1160, 570}, {-70, 0}, 0.48, false, false},
        {"11_jump_launch.png", LocomotionState::JumpLaunch, {820, 374}, {80, -560}, 0.15, true, false},
        {"12_rising.png", LocomotionState::Rising, {1020, 385}, {150, -310}, 0.25, true, false},
        {"13_apex.png", LocomotionState::Apex, {1240, 330}, {120, 0}, 0.2, true, false},
        {"14_falling.png", LocomotionState::Falling, {1320, 320}, {100, 330}, 0.25, true, false},
        {"15_soft_landing.png", LocomotionState::Landing, {1510, 284}, {65, 0}, 0.16, true, false},
        {"16_hard_landing.png", LocomotionState::HardLanding, {1510, 284}, {0, 0}, 0.23, true, false},
        {"17_facing_left.png", LocomotionState::Idle, {1210, 368}, {0, 0}, 0.4, false, false},
        {"18_debug_joint_overlay.png", LocomotionState::Falling, {1240, 340}, {130, 250}, 0.25, true, true},
    }};
    bool ok = true;
    for (const auto& capture : cases) {
        PlayerState player;
        player.position = capture.position;
        player.previousPosition = capture.position;
        player.velocity = capture.velocity;
        player.grounded = capture.state == LocomotionState::Idle ||
                          capture.state == LocomotionState::Accelerating ||
                          capture.state == LocomotionState::Running ||
                          capture.state == LocomotionState::Braking ||
                          capture.state == LocomotionState::Turning ||
                          capture.state == LocomotionState::Landing ||
                          capture.state == LocomotionState::HardLanding;
        player.facingRight = capture.facingRight;
        player.locomotion = capture.state;
        player.stateTime = capture.stateTime;
        CameraState camera;
        context.cameraController.reset(camera,
            {capture.position.x + context.tuning.movement.playerWidth * 0.5,
             capture.position.y + context.tuning.movement.playerHeight * 0.5}, context.room.bounds);
        camera.lookAhead = capture.velocity.x > 0 ? 70.0 : capture.velocity.x < 0 ? -70.0 : 0.0;
        camera.position.x += camera.lookAhead;
        camera.position = context.cameraController.clampPosition(camera.position, context.room.bounds);
        camera.previousPosition = camera.position;
        camera.target = camera.position;
        renderScene(canvas, context, player, camera, 1.0, capture.debug, 3, "CAPTURE VALIDATION");
        ok = exportCanvas(canvas, directory / capture.filename) && ok;
    }
    return ok;
}

std::filesystem::path sourceRoot() { return std::filesystem::path(IRYS_ION_SOURCE_DIR); }

} // namespace

int main(int argc, char** argv) {
    std::optional<std::filesystem::path> captureDirectory;
    for (int i = 1; i < argc; ++i) {
        const std::string_view argument(argv[i]);
        if (argument == "--capture-dir" && i + 1 < argc) captureDirectory = argv[++i];
        else if (argument == "--help") {
            std::cout << "Usage: irys_ion_foundation [--capture-dir PATH]\n";
            return 0;
        } else {
            std::cerr << "Unknown or incomplete argument: " << argument << '\n';
            return 2;
        }
    }

    const auto configPath = sourceRoot() / "config/player_foundation.json";
    auto loadResult = loadTuning(configPath);
    if (!loadResult.ok) {
        std::cerr << loadResult.error << '\n';
        return 1;
    }
    const auto rigDirectory = sourceRoot() / "assets/foundation/irys_rig";
    const RigLoadResult rigLoad = loadRig(rigDirectory / "rig.json");
    if (!rigLoad.ok) {
        std::cerr << rigLoad.error << '\n';
        return 1;
    }
    const AnimationLoadResult animationLoad =
        loadRigAnimations(rigDirectory / "animations.json", rigLoad.rig);
    if (!animationLoad.ok) {
        std::cerr << animationLoad.error << '\n';
        return 1;
    }

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "Irys+Ion - Player Foundation");
    SetWindowMinSize(kCanvasWidth, kCanvasHeight);
    SetTargetFPS(120);
    std::string assetError;
    auto textures = loadTextures(sourceRoot(), rigLoad.rig, assetError);
    if (!textures) {
        std::cerr << assetError << '\n';
        CloseWindow();
        return 1;
    }
    RenderTexture2D canvas = LoadRenderTexture(kCanvasWidth, kCanvasHeight);
    SetTextureFilter(canvas.texture, TEXTURE_FILTER_POINT);

    FoundationTuning tuning = loadResult.tuning;
    RoomDefinition room = makeFoundationRoom();
    auto playerController = std::make_unique<PlayerController>(tuning);
    auto cameraController = std::make_unique<CameraController>(tuning);
    auto clock = std::make_unique<FixedStepClock>(tuning.movement.fixedHz, tuning.maxCatchUpSteps);
    PlayerState player;
    CameraState camera;
    playerController->reset(player, room.spawn);
    cameraController->reset(camera, {room.spawn.x + tuning.movement.playerWidth * 0.5,
                                     room.spawn.y + tuning.movement.playerHeight * 0.5}, room.bounds);
    RenderContext context{*textures, rigLoad.rig, animationLoad.animations,
                          room, *playerController, *cameraController, tuning};

    if (captureDirectory) {
        const bool captured = captureValidationSet(canvas, context, *captureDirectory);
        UnloadRenderTexture(canvas);
        unloadTextures(*textures);
        CloseWindow();
        if (!captured) {
            std::cerr << "One or more validation captures failed\n";
            return 1;
        }
        std::cout << "Validation captures written to " << captureDirectory->string() << '\n';
        return 0;
    }

    bool developerMode = false;
    bool paused = false;
    bool slowMotion = false;
    int markerIndex = 0;
    std::string status;
    double statusRemaining = 0.0;

    while (!WindowShouldClose()) {
        InputFrame input;
        input.leftHeld = IsKeyDown(KEY_A);
        input.rightHeld = IsKeyDown(KEY_D);
        input.jumpHeld = IsKeyDown(KEY_W);
        input.downHeld = IsKeyDown(KEY_S);
        input.jumpPressed = IsKeyPressed(KEY_W);
        input.jumpReleased = IsKeyReleased(KEY_W);

        if (IsKeyPressed(KEY_F1) || IsKeyPressed(KEY_GRAVE)) {
            developerMode = !developerMode;
            status = developerMode ? "DEVELOPER OVERLAY ENABLED" : "DEVELOPER OVERLAY DISABLED";
            statusRemaining = 2.0;
        }
        bool singleStep = false;
        if (developerMode) {
            if (IsKeyPressed(KEY_F5) || IsKeyPressed(KEY_R) || IsKeyPressed(KEY_FIVE)) {
                const auto reloaded = loadTuning(configPath);
                if (reloaded.ok) {
                    tuning = reloaded.tuning;
                    playerController = std::make_unique<PlayerController>(tuning);
                    cameraController = std::make_unique<CameraController>(tuning);
                    clock = std::make_unique<FixedStepClock>(tuning.movement.fixedHz, tuning.maxCatchUpSteps);
                    status = "TUNING RELOADED";
                } else {
                    status = reloaded.error;
                }
                statusRemaining = 4.0;
            }
            if (IsKeyPressed(KEY_F6) || IsKeyPressed(KEY_HOME) || IsKeyPressed(KEY_SIX)) {
                playerController->reset(player, room.spawn);
                cameraController->reset(camera,
                    {room.spawn.x + tuning.movement.playerWidth * 0.5,
                     room.spawn.y + tuning.movement.playerHeight * 0.5}, room.bounds);
                clock->reset();
                status = "ROOM RESTARTED";
                statusRemaining = 2.0;
            }
            if (IsKeyPressed(KEY_F7) || IsKeyPressed(KEY_ONE)) {
                slowMotion = !slowMotion;
                status = slowMotion ? "SLOW MOTION 25%" : "SLOW MOTION DISABLED";
                statusRemaining = 2.0;
            }
            if (IsKeyPressed(KEY_F8) || IsKeyPressed(KEY_TWO)) {
                paused = !paused;
                status = paused ? "SIMULATION PAUSED" : "SIMULATION RESUMED";
                statusRemaining = 2.0;
            }
            singleStep = paused && (IsKeyPressed(KEY_F9) || IsKeyPressed(KEY_THREE));
            if (IsKeyPressed(KEY_LEFT_BRACKET) || IsKeyPressed(KEY_RIGHT_BRACKET)) {
                const int direction = IsKeyPressed(KEY_RIGHT_BRACKET) ? 1 : -1;
                markerIndex = (markerIndex + direction + static_cast<int>(room.markers.size())) %
                              static_cast<int>(room.markers.size());
                playerController->reset(player, room.markers[static_cast<std::size_t>(markerIndex)].position);
                const Vec2 markerPosition = room.markers[static_cast<std::size_t>(markerIndex)].position;
                cameraController->reset(camera,
                    {markerPosition.x + tuning.movement.playerWidth * 0.5,
                     markerPosition.y + tuning.movement.playerHeight * 0.5}, room.bounds);
                clock->reset();
                status = "TELEPORT: " + room.markers[static_cast<std::size_t>(markerIndex)].name;
                statusRemaining = 2.0;
            }
            if (IsKeyPressed(KEY_F10) || IsKeyPressed(KEY_FOUR)) {
                const auto output = std::filesystem::path("/tmp/irys-player-foundation") / "manual_capture.png";
                if (exportCanvas(canvas, output)) status = "CAPTURED " + output.string();
                else status = "SCREENSHOT CAPTURE FAILED";
                statusRemaining = 3.0;
            }
        }

        const auto update = [&](const InputFrame& fixedInput, double dt) {
            playerController->step(player, fixedInput, room, dt);
            cameraController->step(camera, player, fixedInput, room.bounds, dt);
        };
        if (singleStep) update(input, clock->fixedDelta());
        else if (!paused) {
            const double timeScale = slowMotion ? tuning.slowMotionScale : 1.0;
            clock->advance(GetFrameTime() * timeScale, input, update);
        }
        statusRemaining = std::max(0.0, statusRemaining - GetFrameTime());
        if (statusRemaining <= 0.0) status.clear();

        RenderContext frameContext{*textures, rigLoad.rig, animationLoad.animations,
                                   room, *playerController, *cameraController, tuning};
        renderScene(canvas, frameContext, player, camera, clock->interpolationAlpha(), developerMode,
                    markerIndex, status);
        presentCanvas(canvas);
    }

    UnloadRenderTexture(canvas);
    unloadTextures(*textures);
    CloseWindow();
    return 0;
}
