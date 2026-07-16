#pragma once

#include <cstddef>
#include <filesystem>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace irys_ion {

struct Vec2 {
    double x{};
    double y{};
};

struct Rect {
    double x{};
    double y{};
    double width{};
    double height{};
};

enum class SurfaceKind { Solid, OneWay };

struct Surface {
    Rect bounds;
    SurfaceKind kind{SurfaceKind::Solid};
};

struct Marker {
    std::string name;
    Vec2 position;
};

struct RoomDefinition {
    Rect bounds;
    Vec2 spawn;
    std::vector<Surface> surfaces;
    std::vector<Marker> markers;
};

struct MovementTuning {
    double fixedHz{60.0};
    double playerWidth{30.0};
    double playerHeight{70.0};
    double groundAcceleration{1900.0};
    double airAcceleration{980.0};
    double maxRunSpeed{245.0};
    double groundDeceleration{2350.0};
    double airDeceleration{330.0};
    double reversalAcceleration{2850.0};
    double jumpSpeed{560.0};
    double gravity{1760.0};
    double fallGravityMultiplier{1.22};
    double jumpCutGravityMultiplier{3.15};
    double maxFallSpeed{760.0};
    double coyoteTime{0.085};
    double jumpBufferTime{0.095};
    double dropThroughTime{0.18};
    double groundProbe{1.25};
    double hardLandingSpeed{590.0};
};

struct CameraTuning {
    double viewportWidth{640.0};
    double viewportHeight{360.0};
    double deadZoneWidth{112.0};
    double deadZoneHeight{58.0};
    double lookAheadDistance{92.0};
    double lookAheadResponse{6.5};
    double followResponse{9.5};
    double verticalFollowResponse{7.0};
    double directionHoldTime{0.11};
};

struct FoundationTuning {
    MovementTuning movement;
    CameraTuning camera;
    int maxCatchUpSteps{5};
    double slowMotionScale{0.25};
};

struct TuningLoadResult {
    bool ok{};
    FoundationTuning tuning;
    std::string error;
};

TuningLoadResult loadTuning(const std::filesystem::path& path);
std::vector<std::string> validateTuning(const FoundationTuning& tuning);
RoomDefinition makeFoundationRoom();

struct InputFrame {
    bool leftHeld{};
    bool rightHeld{};
    bool jumpHeld{};
    bool downHeld{};
    bool jumpPressed{};
    bool jumpReleased{};
};

enum class LocomotionState {
    Idle,
    Accelerating,
    Running,
    Braking,
    Turning,
    JumpLaunch,
    Rising,
    Apex,
    Falling,
    Landing,
    HardLanding
};

std::string_view locomotionStateName(LocomotionState state);

struct PlayerState {
    Vec2 position;
    Vec2 previousPosition;
    Vec2 velocity;
    bool grounded{};
    bool groundedOnOneWay{};
    bool facingRight{true};
    double coyoteRemaining{};
    double jumpBufferRemaining{};
    double dropThroughRemaining{};
    double stateTime{};
    double lastLandingSpeed{};
    LocomotionState locomotion{LocomotionState::Idle};
};

struct StepDiagnostics {
    bool jumped{};
    bool landed{};
    bool hitWall{};
    bool hitCeiling{};
    bool consumedJumpEdge{};
};

class PlayerController {
public:
    explicit PlayerController(FoundationTuning tuning);

    void reset(PlayerState& player, Vec2 position) const;
    StepDiagnostics step(PlayerState& player, const InputFrame& input,
                         const RoomDefinition& room, double dt) const;
    Rect collisionBounds(const PlayerState& player) const;
    const FoundationTuning& tuning() const { return tuning_; }

private:
    FoundationTuning tuning_;
};

struct CameraState {
    Vec2 position;
    Vec2 previousPosition;
    Vec2 target;
    double lookAhead{};
    int intendedDirection{1};
    double directionHoldRemaining{};
};

class CameraController {
public:
    explicit CameraController(FoundationTuning tuning);

    void reset(CameraState& camera, Vec2 focus, Rect roomBounds) const;
    void step(CameraState& camera, const PlayerState& player,
              const InputFrame& input, Rect roomBounds, double dt) const;
    Vec2 clampPosition(Vec2 position, Rect roomBounds) const;
    Rect deadZone(const CameraState& camera) const;

private:
    CameraTuning tuning_;
    MovementTuning movement_;
};

class FixedStepClock {
public:
    FixedStepClock(double fixedHz, int maxCatchUpSteps);

    int advance(double realSeconds, InputFrame input,
                const std::function<void(const InputFrame&, double)>& update);
    double interpolationAlpha() const;
    double fixedDelta() const { return fixedDelta_; }
    void reset();

private:
    double fixedDelta_{};
    double accumulator_{};
    int maxCatchUpSteps_{};
    bool pendingJumpPressed_{};
    bool pendingJumpReleased_{};
};

double moveToward(double value, double target, double maximumDelta);
bool intersects(Rect a, Rect b);

} // namespace irys_ion
