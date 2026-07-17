#include "irys_ion/player_foundation.hpp"
#include "irys_ion/rig.hpp"

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
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

struct RigFixture {
    std::filesystem::path directory;
    RigDefinition rig;
    RigAnimations animations;

    RigFixture() {
        directory = std::filesystem::path(IRYS_ION_SOURCE_DIR) / "assets/foundation/irys_rig";
        const RigLoadResult rigResult = loadRig(directory / "rig.json");
        check(rigResult.ok, "committed rig loads and validates: " + rigResult.error);
        rig = rigResult.rig;
        const AnimationLoadResult animationResult =
            loadRigAnimations(directory / "animations.json", rig);
        check(animationResult.ok, "committed animation data loads and validates: " + animationResult.error);
        animations = animationResult.animations;
    }
};

std::string readFile(const std::filesystem::path& path) {
    std::ifstream input(path);
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

bool replaceFirst(std::string& text, const std::string& from, const std::string& to) {
    const std::size_t position = text.find(from);
    if (position == std::string::npos) return false;
    text.replace(position, from.size(), to);
    return true;
}

struct TemporaryRigFile {
    std::filesystem::path path;
    TemporaryRigFile(const std::filesystem::path& directory, std::string_view name,
                     const std::string& contents)
        : path(directory / name) {
        std::ofstream output(path);
        output << contents;
    }
    ~TemporaryRigFile() { std::filesystem::remove(path); }
};

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

void testRigMetadataValidation() {
    RigFixture fixture;
    check(fixture.rig.parts.size() >= 29, "rig exposes every required layered body part");
    check(fixture.rig.findPart("ion_grip") != nullptr &&
              fixture.rig.findPart("ion_grip")->parent == "hand_front",
          "Ion grip is explicitly parented to the front hand");

    const std::string committed = readFile(fixture.directory / "rig.json");
    const auto rejectsMutation = [&](std::string text, const std::string& from,
                                     const std::string& to, std::string_view filename,
                                     const std::string& message) {
        check(replaceFirst(text, from, to), "rig validation fixture locates mutation for " + message);
        TemporaryRigFile invalid(fixture.directory, filename, text);
        const RigLoadResult result = loadRig(invalid.path);
        check(!result.ok && !result.error.empty(), message);
    };
    rejectsMutation(committed, "\"parent\": \"root\"", "\"parent\": \"missing_parent\"",
                    ".missing_parent_test.json", "missing rig parent is rejected");
    rejectsMutation(committed, "\"parent\": \"\"", "\"parent\": \"pelvis\"",
                    ".cycle_test.json", "rig parent cycle is rejected");
    rejectsMutation(committed, "\"name\": \"head\"", "\"name\": \"head_missing\"",
                    ".required_test.json", "missing required rig part is rejected");
    rejectsMutation(committed, "\"name\": \"pelvis\"", "\"name\": \"root\"",
                    ".duplicate_test.json", "duplicate rig part name is rejected");
    rejectsMutation(committed, "\"texture\": \"root.png\"", "\"texture\": \"absent.png\"",
                    ".texture_test.json", "missing rig texture is rejected");
    rejectsMutation(committed, "\"pivot\": [\n        0,\n        0\n      ]",
                    "\"pivot\": [\n        999,\n        999\n      ]",
                    ".pivot_test.json", "out-of-range rig pivot is rejected");
    rejectsMutation(committed, "\"parent\": \"hand_front\"", "\"parent\": \"hand_rear\"",
                    ".ion_parent_test.json", "Ion outside the intended hand hierarchy is rejected");
}

void testAnimationValidationAndDeterminism() {
    RigFixture fixture;
    check(fixture.animations.clips.size() == 11, "all eleven authored clips are present");
    std::string invalidText = readFile(fixture.directory / "animations.json");
    check(replaceFirst(invalidText, "\"duration\": 1.6", "\"duration\": 0"),
          "animation validation fixture locates duration");
    TemporaryRigFile invalid(fixture.directory, ".animation_test.json", invalidText);
    const AnimationLoadResult invalidResult = loadRigAnimations(invalid.path, fixture.rig);
    check(!invalidResult.ok && !invalidResult.error.empty(), "invalid clip duration is rejected");

    const RigPose first = evaluateRig(fixture.rig, fixture.animations, "run", 0.237, true);
    const RigPose second = evaluateRig(fixture.rig, fixture.animations, "run", 0.237, true);
    check(first.parts.size() == second.parts.size(), "deterministic rig poses have equal part counts");
    for (std::size_t i = 0; i < first.parts.size(); ++i) {
        const RigTransform& a = first.parts[i].transform;
        const RigTransform& b = second.parts[i].transform;
        if (a.position.x != b.position.x || a.position.y != b.position.y ||
            a.rotation != b.rotation || a.scale.x != b.scale.x || a.scale.y != b.scale.y) {
            check(false, "identical keyframe evaluation is bit-stable");
            break;
        }
    }
}

double localRotation(const RigFixture& fixture, const RigPose& pose,
                     std::string_view child, std::string_view parent) {
    return pose.part(fixture.rig, child).transform.rotation -
           pose.part(fixture.rig, parent).transform.rotation;
}

void testAuthoredArticulation() {
    RigFixture fixture;
    const RigPose contact = evaluateRig(fixture.rig, fixture.animations, "run", 0.0, true);
    const RigPose recovery = evaluateRig(fixture.rig, fixture.animations, "run", 0.16, true);
    const RigPose opposite = evaluateRig(fixture.rig, fixture.animations, "run", 0.32, true);
    const RigPose oppositeRecovery = evaluateRig(fixture.rig, fixture.animations, "run", 0.48, true);
    const double contactKnee = localRotation(fixture, contact, "shin_rear", "thigh_rear");
    const double recoveryKnee = localRotation(fixture, recovery, "shin_rear", "thigh_rear");
    check(std::abs(recoveryKnee - contactKnee) > 25.0 && recoveryKnee > 60.0,
          "run evaluates visible independent knee flexion");
    check(std::abs(contact.part(fixture.rig, "pelvis").transform.position.y -
                   recovery.part(fixture.rig, "pelvis").transform.position.y) > 2.0,
          "run evaluates authored hip rise and fall");
    check(std::abs(localRotation(fixture, oppositeRecovery, "foot_front", "shin_front")) > 20.0,
          "run evaluates independent ankle articulation");
    check(std::abs(localRotation(fixture, contact, "forearm_front", "upper_arm_front") -
                   localRotation(fixture, opposite, "forearm_front", "upper_arm_front")) > 20.0,
          "run evaluates visible elbow articulation");

    const RigPose idle = evaluateRig(fixture.rig, fixture.animations, "idle", 0.0, true);
    const RigPose soft = evaluateRig(fixture.rig, fixture.animations, "soft_landing", 0.16, true);
    const RigPose hard = evaluateRig(fixture.rig, fixture.animations, "hard_landing", 0.23, true);
    check(localRotation(fixture, soft, "shin_front", "thigh_front") > 65.0,
          "soft landing compresses the front knee");
    check(localRotation(fixture, hard, "shin_front", "thigh_front") >
              localRotation(fixture, soft, "shin_front", "thigh_front") + 15.0,
          "hard landing compresses more deeply than soft landing");
    check(hard.part(fixture.rig, "pelvis").transform.position.y >
              idle.part(fixture.rig, "pelvis").transform.position.y + 10.0,
          "hard landing lowers the hips to absorb impact");
}

void testAirPoseSeparationAndIonAttachment() {
    RigFixture fixture;
    const RigPose rising = evaluateRig(fixture.rig, fixture.animations, "rising", 0.25, true);
    const RigPose apex = evaluateRig(fixture.rig, fixture.animations, "apex", 0.2, true);
    const RigPose falling = evaluateRig(fixture.rig, fixture.animations, "falling", 0.25, true);
    const double riseKnee = localRotation(fixture, rising, "shin_front", "thigh_front");
    const double apexKnee = localRotation(fixture, apex, "shin_front", "thigh_front");
    const double fallTorso = falling.part(fixture.rig, "torso_upper").transform.rotation;
    check(std::abs(riseKnee - apexKnee) > 30.0, "rising and apex silhouettes use distinct knee poses");
    check(std::abs(fallTorso - rising.part(fixture.rig, "torso_upper").transform.rotation) > 15.0,
          "rising and falling use distinct torso orientation");
    check(std::abs(localRotation(fixture, apex, "shin_rear", "thigh_rear") -
                   localRotation(fixture, falling, "shin_rear", "thigh_rear")) > 10.0,
          "apex and falling leg gathers are distinct");

    for (const RigClip& clip : fixture.animations.clips) {
        for (double phase : {0.0, clip.duration * 0.5, clip.duration}) {
            const RigPose pose = evaluateRig(fixture.rig, fixture.animations, clip.name, phase, true);
            const RigTransform& hand = pose.part(fixture.rig, "hand_front").transform;
            const Vec2 expectedGrip = transformPoint(hand, {0, 6});
            const Vec2 actualGrip = pose.part(fixture.rig, "ion_grip").transform.position;
            near(actualGrip.x, expectedGrip.x, 1e-9, "Ion grip remains attached to the hand in " + clip.name);
            near(actualGrip.y, expectedGrip.y, 1e-9, "Ion grip vertical attachment remains stable in " + clip.name);
            const RigTransform& grip = pose.part(fixture.rig, "ion_grip").transform;
            const Vec2 expectedBlade = transformPoint(grip, {7, 0});
            const Vec2 actualBlade = pose.part(fixture.rig, "ion_blade").transform.position;
            near(actualBlade.x, expectedBlade.x, 1e-9, "Ion blade remains attached to its grip in " + clip.name);
            near(actualBlade.y, expectedBlade.y, 1e-9, "Ion blade attachment height remains stable in " + clip.name);
        }
    }
}

void testMirroringAndAnimationTransitions() {
    RigFixture fixture;
    const RigPose right = evaluateRig(fixture.rig, fixture.animations, "turn", 0.24, true);
    const RigPose left = evaluateRig(fixture.rig, fixture.animations, "turn", 0.24, false);
    for (std::size_t i = 0; i < right.parts.size(); ++i) {
        near(left.parts[i].transform.position.x, -right.parts[i].transform.position.x, 1e-9,
             "mirrored pose reflects joint x position");
        near(left.parts[i].transform.position.y, right.parts[i].transform.position.y, 1e-9,
             "mirrored pose preserves joint y position");
        near(left.parts[i].transform.rotation, -right.parts[i].transform.rotation, 1e-9,
             "mirrored pose reflects joint rotation");
    }

    Fixture movement;
    for (int i = 0; i < 8; ++i) movement.step();
    movement.step({.rightHeld = true});
    check(movement.player.locomotion == LocomotionState::Accelerating,
          "idle transitions to authored acceleration state");
    for (int i = 0; i < 30; ++i) movement.step({.rightHeld = true});
    check(movement.player.locomotion == LocomotionState::Running,
          "acceleration transitions to authored run state");
    movement.step();
    check(movement.player.locomotion == LocomotionState::Braking,
          "run transitions to authored brake state on release");
    movement.step({.leftHeld = true});
    check(movement.player.locomotion == LocomotionState::Turning && !movement.player.facingRight,
          "reversal selects authored turn state and new facing");

    Fixture jump;
    for (int i = 0; i < 8; ++i) jump.step();
    jump.step({.jumpHeld = true, .jumpPressed = true});
    check(jump.player.locomotion == LocomotionState::JumpLaunch,
          "jump edge selects authored launch state");
    bool sawRising = false;
    bool sawApex = false;
    bool sawFalling = false;
    bool sawLanding = false;
    for (int i = 0; i < 180; ++i) {
        jump.step({.jumpHeld = i < 24});
        sawRising = sawRising || jump.player.locomotion == LocomotionState::Rising;
        sawApex = sawApex || jump.player.locomotion == LocomotionState::Apex;
        sawFalling = sawFalling || jump.player.locomotion == LocomotionState::Falling;
        sawLanding = sawLanding || jump.player.locomotion == LocomotionState::Landing ||
                                      jump.player.locomotion == LocomotionState::HardLanding;
        if (sawLanding) break;
    }
    check(sawRising && sawApex && sawFalling && sawLanding,
          "jump traverses authored rising, apex, falling, and landing states");
}

void testRigFixedStepCatchUpAndCollisionIsolation() {
    RigFixture fixture;
    double catchUpTime = 0.0;
    FixedStepClock clock(60.0, 5);
    const int steps = clock.advance(4.0 / 60.0, {}, [&](const InputFrame&, double dt) {
        catchUpTime += dt;
    });
    check(steps == 4, "rig catch-up fixture advances four fixed simulation ticks");
    const RigPose catchUp = evaluateRig(fixture.rig, fixture.animations, "run", catchUpTime, true);
    const RigPose direct = evaluateRig(fixture.rig, fixture.animations, "run", 4.0 / 60.0, true);
    for (std::size_t i = 0; i < catchUp.parts.size(); ++i) {
        near(catchUp.parts[i].transform.position.x, direct.parts[i].transform.position.x, 1e-12,
             "fixed-step catch-up gives consistent rig x transforms");
        near(catchUp.parts[i].transform.position.y, direct.parts[i].transform.position.y, 1e-12,
             "fixed-step catch-up gives consistent rig y transforms");
        near(catchUp.parts[i].transform.rotation, direct.parts[i].transform.rotation, 1e-12,
             "fixed-step catch-up gives consistent rig rotations");
    }

    Fixture movement;
    const Rect before = movement.controller.collisionBounds(movement.player);
    (void)evaluateRig(fixture.rig, fixture.animations, "hard_landing", 0.23, false);
    const Rect after = movement.controller.collisionBounds(movement.player);
    near(after.x, before.x, 0.0, "rig evaluation does not alter collision x");
    near(after.y, before.y, 0.0, "rig evaluation does not alter collision y");
    near(after.width, before.width, 0.0, "rig evaluation does not alter collision width");
    near(after.height, before.height, 0.0, "rig evaluation does not alter collision height");
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
    testRigMetadataValidation();
    testAnimationValidationAndDeterminism();
    testAuthoredArticulation();
    testAirPoseSeparationAndIonAttachment();
    testMirroringAndAnimationTransitions();
    testRigFixedStepCatchUpAndCollisionIsolation();

    if (failures != 0) {
        std::cerr << failures << " test assertion(s) failed\n";
        return EXIT_FAILURE;
    }
    std::cout << "All player foundation tests passed\n";
    return EXIT_SUCCESS;
}
