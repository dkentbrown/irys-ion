#include "irys_ion/player_foundation.hpp"

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using namespace irys_ion;

namespace {

int failures = 0;

void check(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        ++failures;
    }
}

void near(double actual, double expected, double tolerance, const std::string& message) {
    check(std::abs(actual - expected) <= tolerance,
          message + " (actual=" + std::to_string(actual) +
              ", expected=" + std::to_string(expected) + ")");
}

struct Fixture {
    FoundationTuning tuning;
    RoomDefinition room;
    PlayerController controller;
    PlayerState player;
    double dt;

    Fixture()
        : tuning(loadTuning(std::filesystem::path(IRYS_ION_SOURCE_DIR) /
                            "config/player_foundation.json").tuning),
          room(makeFoundationRoom()), controller(tuning), dt(1.0 / tuning.movement.fixedHz) {
        controller.reset(player, room.spawn);
        for (int i = 0; i < 3; ++i) controller.step(player, {}, room, dt);
    }

    StepDiagnostics step(InputFrame input = {}) { return controller.step(player, input, room, dt); }
};

void testTuningValidation() {
    const auto tuningPath = std::filesystem::path(IRYS_ION_SOURCE_DIR) /
                            "config/player_foundation.json";
    const auto valid = loadTuning(tuningPath);
    check(valid.ok, "committed tuning loads and validates");
    auto invalid = valid.tuning;
    invalid.movement.maxRunSpeed = -1.0;
    invalid.movement.coyoteTime = 2.0;
    invalid.camera.deadZoneWidth = 1000.0;
    const auto errors = validateTuning(invalid);
    check(errors.size() >= 3, "invalid values produce explicit validation failures");
    const auto missing = loadTuning("/definitely/not/a/config.json");
    check(!missing.ok && !missing.error.empty(), "missing tuning produces a useful error");

    std::ifstream source(tuningPath);
    const std::string committed((std::istreambuf_iterator<char>(source)),
                                std::istreambuf_iterator<char>());
    std::string fractional = committed;
    const std::string integerText = "\"max_catch_up_steps\": 5";
    const auto valuePosition = fractional.find(integerText);
    check(valuePosition != std::string::npos, "test fixture locates max_catch_up_steps");
    if (valuePosition != std::string::npos) {
        fractional.replace(valuePosition, integerText.size(),
                           "\"max_catch_up_steps\": 5.5");
        const auto invalidPath = std::filesystem::temp_directory_path() /
                                 "irys_ion_fractional_tuning.json";
        {
            std::ofstream output(invalidPath);
            output << fractional;
        }
        const auto fractionalResult = loadTuning(invalidPath);
        std::filesystem::remove(invalidPath);
        check(!fractionalResult.ok &&
                  fractionalResult.error.find("must be an integer") != std::string::npos,
              "fractional fixed-step count is rejected instead of truncated");
    }
}

void testFixedTimestepAndInputEdges() {
    FixedStepClock clock(60.0, 5);
    int updates = 0;
    int edges = 0;
    InputFrame input;
    input.jumpPressed = true;
    const int steps = clock.advance(3.2 / 60.0, input, [&](const InputFrame& frame, double dt) {
        near(dt, 1.0 / 60.0, 1e-12, "fixed timestep is exact");
        ++updates;
        if (frame.jumpPressed) ++edges;
    });
    check(steps == 3 && updates == 3, "catch-up advances exactly three fixed updates");
    check(edges == 1, "fresh input edge is consumed once across catch-up updates");
    near(clock.interpolationAlpha(), 0.2, 1e-9, "clock preserves interpolation remainder");

    FixedStepClock highRefreshClock(60.0, 5);
    int deferredEdges = 0;
    InputFrame pressWithoutStep;
    pressWithoutStep.jumpPressed = true;
    check(highRefreshClock.advance(0.25 / 60.0, pressWithoutStep,
                                   [&](const InputFrame&, double) {}) == 0,
          "sub-step render frame performs no premature simulation");
    highRefreshClock.advance(0.75 / 60.0, {}, [&](const InputFrame& frame, double) {
        if (frame.jumpPressed) ++deferredEdges;
    });
    check(deferredEdges == 1,
          "input edge survives a render frame with no fixed simulation step");
}

void testHorizontalMovement() {
    Fixture f;
    const double initial = f.player.velocity.x;
    f.step({.rightHeld = true});
    check(f.player.velocity.x > initial, "right input accelerates immediately");
    for (int i = 0; i < 120; ++i) f.step({.rightHeld = true});
    near(f.player.velocity.x, f.tuning.movement.maxRunSpeed, 0.01, "run speed is capped");

    const double running = f.player.velocity.x;
    f.step();
    check(f.player.velocity.x < running, "release begins decisive deceleration");
    for (int i = 0; i < 30; ++i) f.step();
    near(f.player.velocity.x, 0.0, 0.01, "deceleration settles without drift");

    for (int i = 0; i < 30; ++i) f.step({.rightHeld = true});
    f.step({.leftHeld = true});
    check(f.player.facingRight == false, "facing responds immediately on reversal");
    const double afterOne = f.player.velocity.x;
    for (int i = 0; i < 12; ++i) f.step({.leftHeld = true});
    check(afterOne > f.player.velocity.x && f.player.velocity.x < 0.0,
          "reversal crosses zero promptly without accidental delay");
}

double jumpApex(bool releaseEarly) {
    Fixture f;
    const double startY = f.player.position.y;
    f.step({.jumpHeld = true, .jumpPressed = true});
    double minimumY = f.player.position.y;
    for (int i = 0; i < 90; ++i) {
        InputFrame input;
        input.jumpHeld = !releaseEarly || i < 4;
        input.jumpReleased = releaseEarly && i == 4;
        f.step(input);
        minimumY = std::min(minimumY, f.player.position.y);
        if (f.player.grounded && i > 5) break;
    }
    return startY - minimumY;
}

void testVariableJump() {
    const double shortHeight = jumpApex(true);
    const double fullHeight = jumpApex(false);
    check(shortHeight > 35.0, "short jump still has useful height");
    check(fullHeight > shortHeight + 30.0, "held jump produces a materially higher arc");
}

void testCoyoteTime() {
    Fixture f;
    f.player.position = {444.0, 498.0};
    f.player.previousPosition = f.player.position;
    f.player.grounded = true;
    f.player.groundedOnOneWay = true;
    f.player.velocity.x = 245.0;
    for (int i = 0; i < 12 && f.player.grounded; ++i) f.step({.rightHeld = true});
    check(!f.player.grounded && f.player.coyoteRemaining > 0.0, "leaving an edge starts coyote time");
    const auto result = f.step({.rightHeld = true, .jumpHeld = true, .jumpPressed = true});
    check(result.jumped && f.player.velocity.y < 0.0, "jump succeeds during coyote window");
}

void testJumpBuffering() {
    Fixture f;
    f.player.position = {100.0, 500.0};
    f.player.previousPosition = f.player.position;
    f.player.velocity.y = 520.0;
    f.player.grounded = false;
    bool jumped = false;
    for (int i = 0; i < 20; ++i) {
        InputFrame input;
        input.jumpHeld = true;
        input.jumpPressed = i == 5;
        const auto result = f.step(input);
        jumped = jumped || result.jumped;
        if (jumped) break;
    }
    check(jumped && f.player.velocity.y < 0.0, "pre-landing jump press fires on landing");
}

void testOneWayAndDropThrough() {
    Fixture f;
    f.player.position = {350.0, 450.0};
    f.player.previousPosition = f.player.position;
    f.player.velocity.y = 280.0;
    f.player.grounded = false;
    for (int i = 0; i < 30 && !f.player.grounded; ++i) f.step();
    near(f.player.position.y, 568.0 - f.tuning.movement.playerHeight, 0.01,
         "falling player lands on one-way surface");
    check(f.player.groundedOnOneWay, "grounded surface remembers one-way kind");
    f.step({.downHeld = true});
    check(!f.player.grounded && f.player.dropThroughRemaining > 0.0,
          "down input starts drop-through window");
    for (int i = 0; i < 8; ++i) f.step({.downHeld = true});
    check(f.player.position.y + f.tuning.movement.playerHeight > 568.0,
          "drop-through passes below platform top");

    Fixture upward;
    upward.player.position = {350.0, 570.0};
    upward.player.previousPosition = upward.player.position;
    upward.player.velocity.y = -560.0;
    upward.player.grounded = false;
    for (int i = 0; i < 16; ++i) upward.step({.jumpHeld = true});
    check(upward.player.position.y < 568.0, "rising player passes through one-way underside");
}

void testGroundStabilityAndBounds() {
    Fixture f;
    const double groundY = f.player.position.y;
    for (int i = 0; i < 600; ++i) {
        f.step();
        check(f.player.grounded, "standing grounded state remains stable");
    }
    near(f.player.position.y, groundY, 1e-9, "standing position has no vertical drift");

    f.player.position = {34.0, 570.0};
    f.player.velocity.x = -700.0;
    for (int i = 0; i < 30; ++i) f.step({.leftHeld = true});
    check(f.player.position.x >= 32.0 - 0.01, "left room wall remains authoritative");
    f.player.position = {1850.0, 570.0};
    f.player.velocity.x = 700.0;
    for (int i = 0; i < 30; ++i) f.step({.rightHeld = true});
    check(f.player.position.x + f.tuning.movement.playerWidth <= 1888.01,
          "right room wall remains authoritative");
}

void testCameraClamping() {
    Fixture f;
    CameraController controller(f.tuning);
    CameraState camera;
    controller.reset(camera, {-500.0, -500.0}, f.room.bounds);
    near(camera.position.x, 320.0, 0.01, "camera clamps to left room boundary");
    near(camera.position.y, 180.0, 0.01, "camera clamps to top room boundary");
    const Vec2 far = controller.clampPosition({5000.0, 5000.0}, f.room.bounds);
    near(far.x, 1600.0, 0.01, "camera clamps to right room boundary");
    near(far.y, 540.0, 0.01, "camera clamps to bottom room boundary");

    const Vec2 stable = camera.position;
    f.player.position = {305.0, 145.0};
    f.player.velocity = {};
    for (int i = 0; i < 240; ++i) controller.step(camera, f.player, {}, f.room.bounds, f.dt);
    near(camera.position.x, stable.x, 0.01, "camera remains still inside dead zone");
    near(camera.position.y, stable.y, 0.01, "vertical camera remains still inside dead zone");
}

std::vector<Vec2> deterministicTrace() {
    Fixture f;
    std::vector<Vec2> trace;
    for (int i = 0; i < 360; ++i) {
        InputFrame input;
        input.rightHeld = i < 150;
        input.leftHeld = i >= 190 && i < 300;
        input.jumpPressed = i == 50 || i == 175;
        input.jumpHeld = (i >= 50 && i < 71) || (i >= 175 && i < 181);
        input.jumpReleased = i == 71 || i == 181;
        f.step(input);
        trace.push_back(f.player.position);
    }
    return trace;
}

void testDeterminism() {
    const auto first = deterministicTrace();
    const auto second = deterministicTrace();
    check(first.size() == second.size(), "deterministic traces have equal length");
    for (std::size_t i = 0; i < first.size(); ++i) {
        if (first[i].x != second[i].x || first[i].y != second[i].y) {
            check(false, "identical movement input produces bit-stable repeated positions");
            return;
        }
    }
}

} // namespace

int main() {
    testTuningValidation();
    testFixedTimestepAndInputEdges();
    testHorizontalMovement();
    testVariableJump();
    testCoyoteTime();
    testJumpBuffering();
    testOneWayAndDropThrough();
    testGroundStabilityAndBounds();
    testCameraClamping();
    testDeterminism();

    if (failures != 0) {
        std::cerr << failures << " test assertion(s) failed\n";
        return EXIT_FAILURE;
    }
    std::cout << "All player foundation tests passed\n";
    return EXIT_SUCCESS;
}
