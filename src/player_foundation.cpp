#include "irys_ion/player_foundation.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <limits>
#include <map>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace irys_ion {
namespace {

struct JsonValue {
    enum class Type { Object, Number } type{Type::Object};
    std::map<std::string, JsonValue> object;
    double number{};
};

class JsonParser {
public:
    explicit JsonParser(std::string text) : text_(std::move(text)) {}

    JsonValue parse() {
        skipWhitespace();
        JsonValue value = parseObject();
        skipWhitespace();
        if (cursor_ != text_.size()) {
            fail("unexpected trailing content");
        }
        return value;
    }

private:
    JsonValue parseObject() {
        expect('{');
        JsonValue value;
        value.type = JsonValue::Type::Object;
        skipWhitespace();
        if (consume('}')) return value;

        while (true) {
            skipWhitespace();
            const std::string key = parseString();
            skipWhitespace();
            expect(':');
            skipWhitespace();
            value.object.emplace(key, peek() == '{' ? parseObject() : parseNumber());
            skipWhitespace();
            if (consume('}')) break;
            expect(',');
        }
        return value;
    }

    JsonValue parseNumber() {
        const std::size_t start = cursor_;
        if (peek() == '-') ++cursor_;
        while (std::isdigit(static_cast<unsigned char>(peek()))) ++cursor_;
        if (peek() == '.') {
            ++cursor_;
            while (std::isdigit(static_cast<unsigned char>(peek()))) ++cursor_;
        }
        if (peek() == 'e' || peek() == 'E') {
            ++cursor_;
            if (peek() == '+' || peek() == '-') ++cursor_;
            while (std::isdigit(static_cast<unsigned char>(peek()))) ++cursor_;
        }
        if (start == cursor_) fail("expected a number or object");
        JsonValue value;
        value.type = JsonValue::Type::Number;
        try {
            value.number = std::stod(text_.substr(start, cursor_ - start));
        } catch (const std::exception&) {
            fail("invalid number");
        }
        return value;
    }

    std::string parseString() {
        expect('"');
        std::string result;
        while (cursor_ < text_.size() && text_[cursor_] != '"') {
            const char c = text_[cursor_++];
            if (c == '\\') {
                if (cursor_ >= text_.size()) fail("unterminated escape");
                const char escaped = text_[cursor_++];
                if (escaped == '"' || escaped == '\\' || escaped == '/') {
                    result.push_back(escaped);
                } else {
                    fail("unsupported string escape");
                }
            } else {
                result.push_back(c);
            }
        }
        expect('"');
        return result;
    }

    char peek() const { return cursor_ < text_.size() ? text_[cursor_] : '\0'; }
    bool consume(char c) {
        if (peek() != c) return false;
        ++cursor_;
        return true;
    }
    void expect(char c) {
        if (!consume(c)) fail(std::string("expected '") + c + "'");
    }
    void skipWhitespace() {
        while (cursor_ < text_.size() &&
               std::isspace(static_cast<unsigned char>(text_[cursor_]))) {
            ++cursor_;
        }
    }
    [[noreturn]] void fail(const std::string& message) const {
        throw std::runtime_error(message + " at byte " + std::to_string(cursor_));
    }

    std::string text_;
    std::size_t cursor_{};
};

double numberAt(const JsonValue& root, std::string_view section, std::string_view key) {
    const auto sectionIt = root.object.find(std::string(section));
    if (sectionIt == root.object.end() || sectionIt->second.type != JsonValue::Type::Object) {
        throw std::runtime_error("missing object '" + std::string(section) + "'");
    }
    const auto keyIt = sectionIt->second.object.find(std::string(key));
    if (keyIt == sectionIt->second.object.end() || keyIt->second.type != JsonValue::Type::Number) {
        throw std::runtime_error("missing numeric key '" + std::string(section) + "." +
                                 std::string(key) + "'");
    }
    return keyIt->second.number;
}

bool horizontalOverlap(Rect a, Rect b) {
    constexpr double epsilon = 0.001;
    return a.x + a.width > b.x + epsilon && a.x < b.x + b.width - epsilon;
}

bool verticalOverlap(Rect a, Rect b) {
    constexpr double epsilon = 0.001;
    return a.y + a.height > b.y + epsilon && a.y < b.y + b.height - epsilon;
}

void setLocomotion(PlayerState& player, LocomotionState state) {
    if (player.locomotion != state) {
        player.locomotion = state;
        player.stateTime = 0.0;
    }
}

double expResponse(double current, double target, double response, double dt) {
    const double blend = 1.0 - std::exp(-response * dt);
    return current + (target - current) * blend;
}

} // namespace

TuningLoadResult loadTuning(const std::filesystem::path& path) {
    TuningLoadResult result;
    std::ifstream stream(path);
    if (!stream) {
        result.error = "Cannot open tuning file: " + path.string();
        return result;
    }

    try {
        std::ostringstream buffer;
        buffer << stream.rdbuf();
        const JsonValue root = JsonParser(buffer.str()).parse();
        auto& m = result.tuning.movement;
        m.fixedHz = numberAt(root, "movement", "fixed_hz");
        m.playerWidth = numberAt(root, "movement", "player_width");
        m.playerHeight = numberAt(root, "movement", "player_height");
        m.groundAcceleration = numberAt(root, "movement", "ground_acceleration");
        m.airAcceleration = numberAt(root, "movement", "air_acceleration");
        m.maxRunSpeed = numberAt(root, "movement", "max_run_speed");
        m.groundDeceleration = numberAt(root, "movement", "ground_deceleration");
        m.airDeceleration = numberAt(root, "movement", "air_deceleration");
        m.reversalAcceleration = numberAt(root, "movement", "reversal_acceleration");
        m.jumpSpeed = numberAt(root, "movement", "jump_speed");
        m.gravity = numberAt(root, "movement", "gravity");
        m.fallGravityMultiplier = numberAt(root, "movement", "fall_gravity_multiplier");
        m.jumpCutGravityMultiplier = numberAt(root, "movement", "jump_cut_gravity_multiplier");
        m.maxFallSpeed = numberAt(root, "movement", "max_fall_speed");
        m.coyoteTime = numberAt(root, "movement", "coyote_time");
        m.jumpBufferTime = numberAt(root, "movement", "jump_buffer_time");
        m.dropThroughTime = numberAt(root, "movement", "drop_through_time");
        m.groundProbe = numberAt(root, "movement", "ground_probe");
        m.hardLandingSpeed = numberAt(root, "movement", "hard_landing_speed");

        auto& c = result.tuning.camera;
        c.viewportWidth = numberAt(root, "camera", "viewport_width");
        c.viewportHeight = numberAt(root, "camera", "viewport_height");
        c.deadZoneWidth = numberAt(root, "camera", "dead_zone_width");
        c.deadZoneHeight = numberAt(root, "camera", "dead_zone_height");
        c.lookAheadDistance = numberAt(root, "camera", "look_ahead_distance");
        c.lookAheadResponse = numberAt(root, "camera", "look_ahead_response");
        c.followResponse = numberAt(root, "camera", "follow_response");
        c.verticalFollowResponse = numberAt(root, "camera", "vertical_follow_response");
        c.directionHoldTime = numberAt(root, "camera", "direction_hold_time");
        const double maxCatchUpSteps = numberAt(root, "runtime", "max_catch_up_steps");
        if (!std::isfinite(maxCatchUpSteps) || std::floor(maxCatchUpSteps) != maxCatchUpSteps) {
            throw std::runtime_error("runtime.max_catch_up_steps must be an integer");
        }
        result.tuning.maxCatchUpSteps = static_cast<int>(maxCatchUpSteps);
        result.tuning.slowMotionScale = numberAt(root, "runtime", "slow_motion_scale");

        const auto errors = validateTuning(result.tuning);
        if (!errors.empty()) {
            std::ostringstream message;
            message << "Invalid tuning in " << path << ':';
            for (const auto& error : errors) message << "\n - " << error;
            result.error = message.str();
            return result;
        }
        result.ok = true;
    } catch (const std::exception& error) {
        result.error = "Failed to parse " + path.string() + ": " + error.what();
    }
    return result;
}

std::vector<std::string> validateTuning(const FoundationTuning& t) {
    std::vector<std::string> errors;
    const auto positive = [&](double value, std::string_view name) {
        if (!std::isfinite(value) || value <= 0.0) errors.emplace_back(std::string(name) + " must be finite and > 0");
    };
    const auto range = [&](double value, double low, double high, std::string_view name) {
        if (!std::isfinite(value) || value < low || value > high) {
            errors.emplace_back(std::string(name) + " must be in [" + std::to_string(low) + ", " + std::to_string(high) + "]");
        }
    };
    const auto& m = t.movement;
    positive(m.fixedHz, "movement.fixed_hz");
    if (m.fixedHz != 60.0) errors.emplace_back("movement.fixed_hz must equal 60 for this quality gate");
    positive(m.playerWidth, "movement.player_width");
    positive(m.playerHeight, "movement.player_height");
    positive(m.groundAcceleration, "movement.ground_acceleration");
    positive(m.airAcceleration, "movement.air_acceleration");
    positive(m.maxRunSpeed, "movement.max_run_speed");
    positive(m.groundDeceleration, "movement.ground_deceleration");
    positive(m.airDeceleration, "movement.air_deceleration");
    positive(m.reversalAcceleration, "movement.reversal_acceleration");
    positive(m.jumpSpeed, "movement.jump_speed");
    positive(m.gravity, "movement.gravity");
    range(m.fallGravityMultiplier, 1.0, 4.0, "movement.fall_gravity_multiplier");
    range(m.jumpCutGravityMultiplier, 1.0, 6.0, "movement.jump_cut_gravity_multiplier");
    positive(m.maxFallSpeed, "movement.max_fall_speed");
    range(m.coyoteTime, 0.0, 0.25, "movement.coyote_time");
    range(m.jumpBufferTime, 0.0, 0.25, "movement.jump_buffer_time");
    range(m.dropThroughTime, 0.05, 0.5, "movement.drop_through_time");
    range(m.groundProbe, 0.1, 4.0, "movement.ground_probe");
    positive(m.hardLandingSpeed, "movement.hard_landing_speed");
    if (m.reversalAcceleration < m.groundAcceleration) errors.emplace_back("movement.reversal_acceleration must be >= ground_acceleration");
    if (m.maxFallSpeed < m.jumpSpeed) errors.emplace_back("movement.max_fall_speed must be >= jump_speed");

    const auto& c = t.camera;
    positive(c.viewportWidth, "camera.viewport_width");
    positive(c.viewportHeight, "camera.viewport_height");
    range(c.deadZoneWidth, 0.0, c.viewportWidth * 0.75, "camera.dead_zone_width");
    range(c.deadZoneHeight, 0.0, c.viewportHeight * 0.75, "camera.dead_zone_height");
    range(c.lookAheadDistance, 0.0, c.viewportWidth * 0.4, "camera.look_ahead_distance");
    positive(c.lookAheadResponse, "camera.look_ahead_response");
    positive(c.followResponse, "camera.follow_response");
    positive(c.verticalFollowResponse, "camera.vertical_follow_response");
    range(c.directionHoldTime, 0.0, 0.5, "camera.direction_hold_time");
    if (t.maxCatchUpSteps < 1 || t.maxCatchUpSteps > 12) errors.emplace_back("runtime.max_catch_up_steps must be an integer in [1, 12]");
    range(t.slowMotionScale, 0.05, 1.0, "runtime.slow_motion_scale");
    return errors;
}

RoomDefinition makeFoundationRoom() {
    RoomDefinition room;
    room.bounds = {0.0, 0.0, 1920.0, 720.0};
    room.spawn = {112.0, 570.0};
    room.surfaces = {
        {{0.0, 640.0, 1920.0, 80.0}, SurfaceKind::Solid},
        {{0.0, 0.0, 32.0, 720.0}, SurfaceKind::Solid},
        {{1888.0, 0.0, 32.0, 720.0}, SurfaceKind::Solid},
        {{300.0, 568.0, 172.0, 16.0}, SurfaceKind::OneWay},
        {{520.0, 510.0, 176.0, 16.0}, SurfaceKind::OneWay},
        {{748.0, 454.0, 142.0, 16.0}, SurfaceKind::OneWay},
        {{940.0, 514.0, 190.0, 16.0}, SurfaceKind::OneWay},
        {{1184.0, 438.0, 180.0, 16.0}, SurfaceKind::OneWay},
        {{1428.0, 354.0, 164.0, 16.0}, SurfaceKind::OneWay},
        {{1644.0, 430.0, 156.0, 16.0}, SurfaceKind::OneWay},
        // Major backdrop catwalks also collide so every readable surface is dependable.
        {{64.0, 454.0, 128.0, 16.0}, SurfaceKind::OneWay},
        {{544.0, 338.0, 344.0, 16.0}, SurfaceKind::OneWay},
        {{806.0, 510.0, 330.0, 16.0}, SurfaceKind::OneWay},
        {{896.0, 258.0, 588.0, 16.0}, SurfaceKind::OneWay},
        {{1538.0, 412.0, 254.0, 16.0}, SurfaceKind::OneWay},
        {{1780.0, 552.0, 108.0, 88.0}, SurfaceKind::Solid},
    };
    room.markers = {
        {"START", {112.0, 570.0}},
        {"SPRINT", {610.0, 570.0}},
        {"ASCENT", {820.0, 374.0}},
        {"DROP", {1270.0, 358.0}},
        {"APEX", {1510.0, 274.0}},
        {"BOUNDARY", {1828.0, 482.0}},
    };
    return room;
}

std::string_view locomotionStateName(LocomotionState state) {
    switch (state) {
    case LocomotionState::Idle: return "IDLE";
    case LocomotionState::Accelerating: return "ACCEL";
    case LocomotionState::Running: return "RUN";
    case LocomotionState::Braking: return "BRAKE";
    case LocomotionState::Turning: return "TURN";
    case LocomotionState::JumpLaunch: return "LAUNCH";
    case LocomotionState::Rising: return "RISE";
    case LocomotionState::Apex: return "APEX";
    case LocomotionState::Falling: return "FALL";
    case LocomotionState::Landing: return "LAND";
    case LocomotionState::HardLanding: return "HARD LAND";
    }
    return "UNKNOWN";
}

double moveToward(double value, double target, double maximumDelta) {
    if (value < target) return std::min(value + maximumDelta, target);
    return std::max(value - maximumDelta, target);
}

bool intersects(Rect a, Rect b) {
    return a.x < b.x + b.width && a.x + a.width > b.x &&
           a.y < b.y + b.height && a.y + a.height > b.y;
}

PlayerController::PlayerController(FoundationTuning tuning) : tuning_(std::move(tuning)) {}

void PlayerController::reset(PlayerState& player, Vec2 position) const {
    player = {};
    player.position = position;
    player.previousPosition = position;
    player.facingRight = true;
}

Rect PlayerController::collisionBounds(const PlayerState& player) const {
    return {player.position.x, player.position.y,
            tuning_.movement.playerWidth, tuning_.movement.playerHeight};
}

StepDiagnostics PlayerController::step(PlayerState& player, const InputFrame& input,
                                       const RoomDefinition& room, double dt) const {
    StepDiagnostics diagnostics;
    const auto& t = tuning_.movement;
    player.previousPosition = player.position;
    player.stateTime += dt;
    player.coyoteRemaining = std::max(0.0, player.coyoteRemaining - dt);
    player.jumpBufferRemaining = std::max(0.0, player.jumpBufferRemaining - dt);
    player.dropThroughRemaining = std::max(0.0, player.dropThroughRemaining - dt);

    if (input.jumpPressed) {
        player.jumpBufferRemaining = t.jumpBufferTime;
        diagnostics.consumedJumpEdge = true;
    }

    if (input.downHeld && player.groundedOnOneWay) {
        player.dropThroughRemaining = t.dropThroughTime;
        player.grounded = false;
        player.groundedOnOneWay = false;
        player.position.y += 2.0;
    }

    const int horizontal = static_cast<int>(input.rightHeld) - static_cast<int>(input.leftHeld);
    const bool reversing = horizontal != 0 && player.velocity.x != 0.0 &&
                           std::copysign(1.0, player.velocity.x) != static_cast<double>(horizontal);
    if (horizontal != 0) {
        player.facingRight = horizontal > 0;
        const double acceleration = reversing ? t.reversalAcceleration
                                              : (player.grounded ? t.groundAcceleration : t.airAcceleration);
        player.velocity.x = moveToward(player.velocity.x, horizontal * t.maxRunSpeed, acceleration * dt);
    } else {
        const double deceleration = player.grounded ? t.groundDeceleration : t.airDeceleration;
        player.velocity.x = moveToward(player.velocity.x, 0.0, deceleration * dt);
    }

    if (player.grounded) player.coyoteRemaining = t.coyoteTime;
    const bool canJump = player.grounded || player.coyoteRemaining > 0.0;
    if (player.jumpBufferRemaining > 0.0 && canJump && player.dropThroughRemaining <= 0.0) {
        player.velocity.y = -t.jumpSpeed;
        player.grounded = false;
        player.groundedOnOneWay = false;
        player.coyoteRemaining = 0.0;
        player.jumpBufferRemaining = 0.0;
        diagnostics.jumped = true;
        setLocomotion(player, LocomotionState::JumpLaunch);
    }

    if (!player.grounded) {
        double gravityMultiplier = player.velocity.y > 0.0 ? t.fallGravityMultiplier : 1.0;
        if (!input.jumpHeld && player.velocity.y < 0.0) gravityMultiplier = t.jumpCutGravityMultiplier;
        player.velocity.y = std::min(player.velocity.y + t.gravity * gravityMultiplier * dt, t.maxFallSpeed);
    } else {
        player.velocity.y = std::max(0.0, player.velocity.y);
    }

    // Horizontal axis is resolved before vertical to prevent sticky platform edges.
    player.position.x += player.velocity.x * dt;
    Rect bounds = collisionBounds(player);
    for (const auto& surface : room.surfaces) {
        if (surface.kind != SurfaceKind::Solid || !intersects(bounds, surface.bounds)) continue;
        if (player.velocity.x > 0.0) player.position.x = surface.bounds.x - t.playerWidth;
        else if (player.velocity.x < 0.0) player.position.x = surface.bounds.x + surface.bounds.width;
        player.velocity.x = 0.0;
        diagnostics.hitWall = true;
        bounds = collisionBounds(player);
    }

    const double previousBottom = player.position.y + t.playerHeight;
    const double landingVelocity = player.velocity.y;
    player.position.y += player.velocity.y * dt;
    bounds = collisionBounds(player);
    bool landedOnOneWay = false;
    bool verticalCollision = false;
    for (const auto& surface : room.surfaces) {
        if (!horizontalOverlap(bounds, surface.bounds)) continue;
        if (surface.kind == SurfaceKind::OneWay) {
            const double newBottom = player.position.y + t.playerHeight;
            if (player.dropThroughRemaining <= 0.0 && player.velocity.y >= 0.0 &&
                previousBottom <= surface.bounds.y + t.groundProbe && newBottom >= surface.bounds.y) {
                player.position.y = surface.bounds.y - t.playerHeight;
                player.velocity.y = 0.0;
                verticalCollision = true;
                landedOnOneWay = true;
            }
            continue;
        }
        bounds = collisionBounds(player);
        if (!verticalOverlap(bounds, surface.bounds)) continue;
        if (player.velocity.y >= 0.0 && previousBottom <= surface.bounds.y + t.groundProbe) {
            player.position.y = surface.bounds.y - t.playerHeight;
            player.velocity.y = 0.0;
            verticalCollision = true;
        } else if (player.velocity.y < 0.0) {
            player.position.y = surface.bounds.y + surface.bounds.height;
            player.velocity.y = 0.0;
            diagnostics.hitCeiling = true;
        }
    }

    const bool wasGrounded = player.grounded;
    player.grounded = verticalCollision;
    player.groundedOnOneWay = verticalCollision && landedOnOneWay;

    if (!player.grounded && player.velocity.y >= 0.0 && player.dropThroughRemaining <= 0.0) {
        Rect probe = collisionBounds(player);
        probe.y += t.groundProbe;
        for (const auto& surface : room.surfaces) {
            const bool allowed = surface.kind == SurfaceKind::Solid || previousBottom <= surface.bounds.y + t.groundProbe;
            if (allowed && horizontalOverlap(probe, surface.bounds) && intersects(probe, surface.bounds)) {
                player.grounded = true;
                player.groundedOnOneWay = surface.kind == SurfaceKind::OneWay;
                player.position.y = surface.bounds.y - t.playerHeight;
                player.velocity.y = 0.0;
                break;
            }
        }
    }

    if (!wasGrounded && player.grounded) {
        diagnostics.landed = true;
        player.lastLandingSpeed = landingVelocity;
        setLocomotion(player, landingVelocity >= t.hardLandingSpeed ? LocomotionState::HardLanding
                                                                    : LocomotionState::Landing);
        if (player.jumpBufferRemaining > 0.0) {
            player.velocity.y = -t.jumpSpeed;
            player.grounded = false;
            player.groundedOnOneWay = false;
            player.jumpBufferRemaining = 0.0;
            diagnostics.jumped = true;
            setLocomotion(player, LocomotionState::JumpLaunch);
        }
    }

    if (!diagnostics.jumped && !diagnostics.landed) {
        if (!player.grounded) {
            if (player.velocity.y < -85.0) setLocomotion(player, LocomotionState::Rising);
            else if (std::abs(player.velocity.y) <= 85.0) setLocomotion(player, LocomotionState::Apex);
            else setLocomotion(player, LocomotionState::Falling);
        } else if (player.locomotion == LocomotionState::Landing || player.locomotion == LocomotionState::HardLanding) {
            const double hold = player.locomotion == LocomotionState::HardLanding ? 0.16 : 0.09;
            if (player.stateTime >= hold) setLocomotion(player, std::abs(player.velocity.x) > 1.0 ? LocomotionState::Accelerating : LocomotionState::Idle);
        } else if (horizontal != 0 && reversing) {
            setLocomotion(player, LocomotionState::Turning);
        } else if (horizontal == 0 && std::abs(player.velocity.x) > 18.0) {
            setLocomotion(player, LocomotionState::Braking);
        } else if (std::abs(player.velocity.x) < 8.0) {
            setLocomotion(player, LocomotionState::Idle);
        } else if (std::abs(player.velocity.x) < t.maxRunSpeed * 0.82) {
            setLocomotion(player, LocomotionState::Accelerating);
        } else {
            setLocomotion(player, LocomotionState::Running);
        }
    }
    return diagnostics;
}

CameraController::CameraController(FoundationTuning tuning)
    : tuning_(std::move(tuning.camera)), movement_(std::move(tuning.movement)) {}

Vec2 CameraController::clampPosition(Vec2 position, Rect roomBounds) const {
    const double halfWidth = tuning_.viewportWidth * 0.5;
    const double halfHeight = tuning_.viewportHeight * 0.5;
    const double minX = roomBounds.x + halfWidth;
    const double maxX = roomBounds.x + roomBounds.width - halfWidth;
    const double minY = roomBounds.y + halfHeight;
    const double maxY = roomBounds.y + roomBounds.height - halfHeight;
    position.x = roomBounds.width <= tuning_.viewportWidth ? roomBounds.x + roomBounds.width * 0.5
                                                           : std::clamp(position.x, minX, maxX);
    position.y = roomBounds.height <= tuning_.viewportHeight ? roomBounds.y + roomBounds.height * 0.5
                                                             : std::clamp(position.y, minY, maxY);
    return position;
}

void CameraController::reset(CameraState& camera, Vec2 focus, Rect roomBounds) const {
    camera = {};
    camera.intendedDirection = 1;
    camera.position = clampPosition(focus, roomBounds);
    camera.previousPosition = camera.position;
    camera.target = camera.position;
}

void CameraController::step(CameraState& camera, const PlayerState& player,
                            const InputFrame& input, Rect roomBounds, double dt) const {
    camera.previousPosition = camera.position;
    const int intention = static_cast<int>(input.rightHeld) - static_cast<int>(input.leftHeld);
    if (intention != 0 && intention != camera.intendedDirection) {
        camera.directionHoldRemaining += dt;
        if (camera.directionHoldRemaining >= tuning_.directionHoldTime) {
            camera.intendedDirection = intention;
            camera.directionHoldRemaining = 0.0;
        }
    } else {
        camera.directionHoldRemaining = 0.0;
    }
    const double speedRatio = std::clamp(std::abs(player.velocity.x) / movement_.maxRunSpeed, 0.0, 1.0);
    const double desiredLookAhead = camera.intendedDirection * tuning_.lookAheadDistance * speedRatio;
    camera.lookAhead = expResponse(camera.lookAhead, desiredLookAhead, tuning_.lookAheadResponse, dt);

    const Vec2 focus{player.position.x + movement_.playerWidth * 0.5,
                     player.position.y + movement_.playerHeight * 0.5};
    const double halfDeadX = tuning_.deadZoneWidth * 0.5;
    const double halfDeadY = tuning_.deadZoneHeight * 0.5;
    double targetX = camera.target.x;
    double targetY = camera.target.y;
    const double framedX = focus.x + camera.lookAhead;
    if (framedX < camera.position.x - halfDeadX) targetX = framedX + halfDeadX;
    if (framedX > camera.position.x + halfDeadX) targetX = framedX - halfDeadX;
    if (focus.y < camera.position.y - halfDeadY) targetY = focus.y + halfDeadY;
    if (focus.y > camera.position.y + halfDeadY) targetY = focus.y - halfDeadY;
    camera.target = clampPosition({targetX, targetY}, roomBounds);
    camera.position.x = expResponse(camera.position.x, camera.target.x, tuning_.followResponse, dt);
    camera.position.y = expResponse(camera.position.y, camera.target.y, tuning_.verticalFollowResponse, dt);
    camera.position = clampPosition(camera.position, roomBounds);
}

Rect CameraController::deadZone(const CameraState& camera) const {
    return {camera.position.x - tuning_.deadZoneWidth * 0.5,
            camera.position.y - tuning_.deadZoneHeight * 0.5,
            tuning_.deadZoneWidth, tuning_.deadZoneHeight};
}

FixedStepClock::FixedStepClock(double fixedHz, int maxCatchUpSteps)
    : fixedDelta_(1.0 / fixedHz), maxCatchUpSteps_(maxCatchUpSteps) {}

int FixedStepClock::advance(double realSeconds, InputFrame input,
                            const std::function<void(const InputFrame&, double)>& update) {
    pendingJumpPressed_ = pendingJumpPressed_ || input.jumpPressed;
    pendingJumpReleased_ = pendingJumpReleased_ || input.jumpReleased;
    accumulator_ += std::clamp(realSeconds, 0.0, fixedDelta_ * static_cast<double>(maxCatchUpSteps_));
    int steps = 0;
    while (accumulator_ >= fixedDelta_ && steps < maxCatchUpSteps_) {
        InputFrame fixedInput = input;
        fixedInput.jumpPressed = pendingJumpPressed_;
        fixedInput.jumpReleased = pendingJumpReleased_;
        update(fixedInput, fixedDelta_);
        pendingJumpPressed_ = false;
        pendingJumpReleased_ = false;
        accumulator_ -= fixedDelta_;
        ++steps;
    }
    if (steps == maxCatchUpSteps_ && accumulator_ >= fixedDelta_) {
        accumulator_ = std::fmod(accumulator_, fixedDelta_);
    }
    return steps;
}

double FixedStepClock::interpolationAlpha() const {
    return std::clamp(accumulator_ / fixedDelta_, 0.0, 1.0);
}

void FixedStepClock::reset() {
    accumulator_ = 0.0;
    pendingJumpPressed_ = false;
    pendingJumpReleased_ = false;
}

} // namespace irys_ion
