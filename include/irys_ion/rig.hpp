#pragma once

#include "irys_ion/player_foundation.hpp"

#include <cstddef>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace irys_ion {

struct RigPart {
    std::string name;
    std::filesystem::path texturePath;
    std::string parent;
    std::size_t parentIndex{};
    Vec2 pivot;
    Vec2 attachment;
    Vec2 offset;
    Vec2 scale{1.0, 1.0};
    int renderOrder{};
    int textureWidth{};
    int textureHeight{};
};

struct RigDefinition {
    std::filesystem::path directory;
    std::vector<RigPart> parts;

    const RigPart* findPart(std::string_view name) const;
    std::size_t partIndex(std::string_view name) const;
};

struct RigLoadResult {
    bool ok{};
    RigDefinition rig;
    std::string error;
};

struct RigKeyframe {
    double time{};
    Vec2 translation;
    double rotation{};
    Vec2 scale{1.0, 1.0};
};

struct RigTrack {
    std::string part;
    std::size_t partIndex{};
    std::vector<RigKeyframe> keyframes;
};

struct RigClip {
    std::string name;
    double duration{};
    bool loop{};
    std::vector<RigTrack> tracks;
};

struct RigAnimations {
    std::vector<RigClip> clips;

    const RigClip* findClip(std::string_view name) const;
};

struct AnimationLoadResult {
    bool ok{};
    RigAnimations animations;
    std::string error;
};

struct RigTransform {
    Vec2 position;
    double rotation{};
    Vec2 scale{1.0, 1.0};
};

struct EvaluatedRigPart {
    RigTransform transform;
};

struct RigPose {
    std::vector<EvaluatedRigPart> parts;
    bool facingRight{true};

    const EvaluatedRigPart& part(const RigDefinition& rig, std::string_view name) const;
};

RigLoadResult loadRig(const std::filesystem::path& path);
AnimationLoadResult loadRigAnimations(const std::filesystem::path& path,
                                      const RigDefinition& rig);
RigPose evaluateRig(const RigDefinition& rig, const RigAnimations& animations,
                    std::string_view clipName, double time, bool facingRight);
Vec2 transformPoint(const RigTransform& transform, Vec2 localPoint);
std::string_view rigClipName(LocomotionState state);

} // namespace irys_ion
