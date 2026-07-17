#include "irys_ion/rig.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <functional>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace irys_ion {
namespace {

constexpr std::size_t kNoParent = std::numeric_limits<std::size_t>::max();
constexpr double kPi = 3.14159265358979323846;

struct JsonValue {
    enum class Type { Object, Array, String, Number, Boolean } type{Type::Object};
    std::map<std::string, JsonValue> object;
    std::vector<JsonValue> array;
    std::string string;
    double number{};
    bool boolean{};
};

class JsonParser {
public:
    explicit JsonParser(std::string text) : text_(std::move(text)) {}

    JsonValue parse() {
        skipWhitespace();
        JsonValue value = parseValue();
        skipWhitespace();
        if (cursor_ != text_.size()) fail("unexpected trailing content");
        return value;
    }

private:
    JsonValue parseValue() {
        switch (peek()) {
        case '{': return parseObject();
        case '[': return parseArray();
        case '"': {
            JsonValue value;
            value.type = JsonValue::Type::String;
            value.string = parseString();
            return value;
        }
        case 't': return parseBoolean("true", true);
        case 'f': return parseBoolean("false", false);
        default: return parseNumber();
        }
    }

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
            if (!value.object.emplace(key, parseValue()).second) {
                fail("duplicate object key '" + key + "'");
            }
            skipWhitespace();
            if (consume('}')) break;
            expect(',');
        }
        return value;
    }

    JsonValue parseArray() {
        expect('[');
        JsonValue value;
        value.type = JsonValue::Type::Array;
        skipWhitespace();
        if (consume(']')) return value;
        while (true) {
            skipWhitespace();
            value.array.push_back(parseValue());
            skipWhitespace();
            if (consume(']')) break;
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
        if (start == cursor_) fail("expected JSON value");
        JsonValue value;
        value.type = JsonValue::Type::Number;
        try {
            value.number = std::stod(text_.substr(start, cursor_ - start));
        } catch (const std::exception&) {
            fail("invalid number");
        }
        if (!std::isfinite(value.number)) fail("number must be finite");
        return value;
    }

    JsonValue parseBoolean(std::string_view spelling, bool boolean) {
        if (text_.compare(cursor_, spelling.size(), spelling) != 0) fail("invalid boolean");
        cursor_ += spelling.size();
        JsonValue value;
        value.type = JsonValue::Type::Boolean;
        value.boolean = boolean;
        return value;
    }

    std::string parseString() {
        expect('"');
        std::string result;
        while (cursor_ < text_.size() && text_[cursor_] != '"') {
            const char character = text_[cursor_++];
            if (character == '\\') {
                if (cursor_ >= text_.size()) fail("unterminated escape");
                const char escaped = text_[cursor_++];
                switch (escaped) {
                case '"': case '\\': case '/': result.push_back(escaped); break;
                case 'b': result.push_back('\b'); break;
                case 'f': result.push_back('\f'); break;
                case 'n': result.push_back('\n'); break;
                case 'r': result.push_back('\r'); break;
                case 't': result.push_back('\t'); break;
                default: fail("unsupported string escape");
                }
            } else {
                result.push_back(character);
            }
        }
        expect('"');
        return result;
    }

    char peek() const { return cursor_ < text_.size() ? text_[cursor_] : '\0'; }
    bool consume(char character) {
        if (peek() != character) return false;
        ++cursor_;
        return true;
    }
    void expect(char character) {
        if (!consume(character)) fail(std::string("expected '") + character + "'");
    }
    void skipWhitespace() {
        while (cursor_ < text_.size() &&
               std::isspace(static_cast<unsigned char>(text_[cursor_]))) ++cursor_;
    }
    [[noreturn]] void fail(const std::string& message) const {
        throw std::runtime_error(message + " at byte " + std::to_string(cursor_));
    }

    std::string text_;
    std::size_t cursor_{};
};

const JsonValue& field(const JsonValue& object, std::string_view key) {
    if (object.type != JsonValue::Type::Object) throw std::runtime_error("expected object");
    const auto it = object.object.find(std::string(key));
    if (it == object.object.end()) throw std::runtime_error("missing key '" + std::string(key) + "'");
    return it->second;
}

const JsonValue* optionalField(const JsonValue& object, std::string_view key) {
    if (object.type != JsonValue::Type::Object) throw std::runtime_error("expected object");
    const auto it = object.object.find(std::string(key));
    return it == object.object.end() ? nullptr : &it->second;
}

std::string stringValue(const JsonValue& value, std::string_view label) {
    if (value.type != JsonValue::Type::String) {
        throw std::runtime_error(std::string(label) + " must be a string");
    }
    return value.string;
}

double numberValue(const JsonValue& value, std::string_view label) {
    if (value.type != JsonValue::Type::Number || !std::isfinite(value.number)) {
        throw std::runtime_error(std::string(label) + " must be a finite number");
    }
    return value.number;
}

bool boolValue(const JsonValue& value, std::string_view label) {
    if (value.type != JsonValue::Type::Boolean) {
        throw std::runtime_error(std::string(label) + " must be a boolean");
    }
    return value.boolean;
}

Vec2 vec2Value(const JsonValue& value, std::string_view label) {
    if (value.type != JsonValue::Type::Array || value.array.size() != 2) {
        throw std::runtime_error(std::string(label) + " must contain exactly two numbers");
    }
    return {numberValue(value.array[0], label), numberValue(value.array[1], label)};
}

JsonValue parseFile(const std::filesystem::path& path) {
    std::ifstream stream(path);
    if (!stream) throw std::runtime_error("cannot open " + path.string());
    std::ostringstream buffer;
    buffer << stream.rdbuf();
    return JsonParser(buffer.str()).parse();
}

std::pair<int, int> pngDimensions(const std::filesystem::path& path) {
    std::ifstream stream(path, std::ios::binary);
    std::array<unsigned char, 24> header{};
    stream.read(reinterpret_cast<char*>(header.data()), static_cast<std::streamsize>(header.size()));
    const std::array<unsigned char, 8> signature{137, 80, 78, 71, 13, 10, 26, 10};
    if (stream.gcount() != static_cast<std::streamsize>(header.size()) ||
        !std::equal(signature.begin(), signature.end(), header.begin()) ||
        std::string(reinterpret_cast<const char*>(header.data() + 12), 4) != "IHDR") {
        throw std::runtime_error("invalid PNG texture " + path.string());
    }
    const auto read32 = [&](std::size_t offset) {
        return static_cast<std::uint32_t>(header[offset]) << 24U |
               static_cast<std::uint32_t>(header[offset + 1]) << 16U |
               static_cast<std::uint32_t>(header[offset + 2]) << 8U |
               static_cast<std::uint32_t>(header[offset + 3]);
    };
    const std::uint32_t width = read32(16);
    const std::uint32_t height = read32(20);
    if (width == 0 || height == 0 || width > 4096 || height > 4096) {
        throw std::runtime_error("out-of-range PNG dimensions in " + path.string());
    }
    return {static_cast<int>(width), static_cast<int>(height)};
}

bool safeRelativePngPath(const std::filesystem::path& path) {
    if (path.empty() || path.is_absolute() || path.extension() != ".png") return false;
    for (const auto& component : path) {
        if (component == "..") return false;
    }
    return true;
}

RigKeyframe sampleTrack(const RigTrack& track, double time, double duration, bool loop) {
    double sampleTime = time;
    if (loop) {
        sampleTime = std::fmod(std::max(0.0, time), duration);
        if (sampleTime < 0.0) sampleTime += duration;
    } else {
        sampleTime = std::clamp(time, 0.0, duration);
    }
    const auto& keys = track.keyframes;
    const auto upper = std::upper_bound(keys.begin(), keys.end(), sampleTime,
        [](double value, const RigKeyframe& key) { return value < key.time; });
    if (upper == keys.begin()) return keys.front();
    if (upper == keys.end()) return keys.back();
    const RigKeyframe& b = *upper;
    const RigKeyframe& a = *(upper - 1);
    const double amount = (sampleTime - a.time) / (b.time - a.time);
    return {
        sampleTime,
        {a.translation.x + (b.translation.x - a.translation.x) * amount,
         a.translation.y + (b.translation.y - a.translation.y) * amount},
        a.rotation + (b.rotation - a.rotation) * amount,
        {a.scale.x + (b.scale.x - a.scale.x) * amount,
         a.scale.y + (b.scale.y - a.scale.y) * amount},
    };
}

} // namespace

const RigPart* RigDefinition::findPart(std::string_view name) const {
    const auto it = std::find_if(parts.begin(), parts.end(),
        [&](const RigPart& part) { return part.name == name; });
    return it == parts.end() ? nullptr : &*it;
}

std::size_t RigDefinition::partIndex(std::string_view name) const {
    const RigPart* found = findPart(name);
    if (!found) throw std::runtime_error("unknown rig part '" + std::string(name) + "'");
    return static_cast<std::size_t>(found - parts.data());
}

const RigClip* RigAnimations::findClip(std::string_view name) const {
    const auto it = std::find_if(clips.begin(), clips.end(),
        [&](const RigClip& clip) { return clip.name == name; });
    return it == clips.end() ? nullptr : &*it;
}

const EvaluatedRigPart& RigPose::part(const RigDefinition& rig, std::string_view name) const {
    return parts.at(rig.partIndex(name));
}

RigLoadResult loadRig(const std::filesystem::path& path) {
    RigLoadResult result;
    try {
        const JsonValue root = parseFile(path);
        const JsonValue& parts = field(root, "parts");
        if (parts.type != JsonValue::Type::Array || parts.array.empty()) {
            throw std::runtime_error("parts must be a non-empty array");
        }
        result.rig.directory = path.parent_path();
        std::set<std::string> names;
        for (const JsonValue& value : parts.array) {
            RigPart part;
            part.name = stringValue(field(value, "name"), "part.name");
            if (part.name.empty() || !names.insert(part.name).second) {
                throw std::runtime_error("duplicate or empty part name '" + part.name + "'");
            }
            part.texturePath = stringValue(field(value, "texture"), "part.texture");
            part.parent = stringValue(field(value, "parent"), "part.parent");
            part.pivot = vec2Value(field(value, "pivot"), "part.pivot");
            part.attachment = vec2Value(field(value, "attachment"), "part.attachment");
            const double renderOrder = numberValue(field(value, "render_order"), "part.render_order");
            if (std::floor(renderOrder) != renderOrder ||
                renderOrder < static_cast<double>(std::numeric_limits<int>::min()) ||
                renderOrder > static_cast<double>(std::numeric_limits<int>::max())) {
                throw std::runtime_error("part.render_order must be an in-range integer");
            }
            part.renderOrder = static_cast<int>(renderOrder);
            if (const JsonValue* offset = optionalField(value, "offset")) {
                part.offset = vec2Value(*offset, "part.offset");
            }
            if (const JsonValue* scale = optionalField(value, "scale")) {
                part.scale = vec2Value(*scale, "part.scale");
            }
            if (!safeRelativePngPath(part.texturePath)) {
                throw std::runtime_error("invalid texture path for part '" + part.name + "'");
            }
            const auto [width, height] = pngDimensions(result.rig.directory / part.texturePath);
            part.textureWidth = width;
            part.textureHeight = height;
            if (part.pivot.x < 0.0 || part.pivot.y < 0.0 ||
                part.pivot.x > static_cast<double>(width) ||
                part.pivot.y > static_cast<double>(height)) {
                throw std::runtime_error("out-of-range pivot for part '" + part.name + "'");
            }
            if (part.scale.x <= 0.0 || part.scale.y <= 0.0) {
                throw std::runtime_error("part scale must be positive for '" + part.name + "'");
            }
            result.rig.parts.push_back(std::move(part));
        }

        for (RigPart& part : result.rig.parts) {
            if (part.parent.empty()) {
                if (part.name != "root") throw std::runtime_error("only root may omit a parent");
                part.parentIndex = kNoParent;
            } else {
                const RigPart* parent = result.rig.findPart(part.parent);
                if (!parent) throw std::runtime_error("missing parent '" + part.parent + "' for '" + part.name + "'");
                part.parentIndex = static_cast<std::size_t>(parent - result.rig.parts.data());
            }
        }
        if (!result.rig.findPart("root")) throw std::runtime_error("missing required part 'root'");

        std::vector<int> visit(result.rig.parts.size());
        std::function<void(std::size_t)> visitPart = [&](std::size_t index) {
            if (visit[index] == 1) throw std::runtime_error("parent cycle includes '" + result.rig.parts[index].name + "'");
            if (visit[index] == 2) return;
            visit[index] = 1;
            if (result.rig.parts[index].parentIndex != kNoParent) visitPart(result.rig.parts[index].parentIndex);
            visit[index] = 2;
        };
        for (std::size_t i = 0; i < result.rig.parts.size(); ++i) visitPart(i);

        constexpr std::array required{
            "root", "pelvis", "torso_lower", "torso_upper", "neck", "head",
            "hair_rear", "hair_front", "upper_arm_front", "forearm_front", "hand_front",
            "upper_arm_rear", "forearm_rear", "hand_rear", "thigh_front", "shin_front",
            "foot_front", "thigh_rear", "shin_rear", "foot_rear", "shoulder_front",
            "shoulder_rear", "knee_front", "knee_rear", "ankle_front", "ankle_rear",
            "ion_grip", "ion_blade", "ion_channel"
        };
        for (std::string_view name : required) {
            if (!result.rig.findPart(name)) throw std::runtime_error("missing required part '" + std::string(name) + "'");
        }
        if (result.rig.findPart("ion_grip")->parent != "hand_front" ||
            result.rig.findPart("ion_blade")->parent != "ion_grip" ||
            result.rig.findPart("ion_channel")->parent != "ion_blade") {
            throw std::runtime_error("Ion must attach through hand_front -> ion_grip -> ion_blade -> ion_channel");
        }
        result.ok = true;
    } catch (const std::exception& error) {
        result.error = "Failed to load rig " + path.string() + ": " + error.what();
    }
    return result;
}

AnimationLoadResult loadRigAnimations(const std::filesystem::path& path,
                                      const RigDefinition& rig) {
    AnimationLoadResult result;
    try {
        const JsonValue root = parseFile(path);
        const JsonValue& clips = field(root, "clips");
        if (clips.type != JsonValue::Type::Array || clips.array.empty()) {
            throw std::runtime_error("clips must be a non-empty array");
        }
        std::set<std::string> clipNames;
        for (const JsonValue& clipValue : clips.array) {
            RigClip clip;
            clip.name = stringValue(field(clipValue, "name"), "clip.name");
            if (clip.name.empty() || !clipNames.insert(clip.name).second) {
                throw std::runtime_error("duplicate or empty clip name '" + clip.name + "'");
            }
            clip.duration = numberValue(field(clipValue, "duration"), "clip.duration");
            clip.loop = boolValue(field(clipValue, "loop"), "clip.loop");
            if (clip.duration <= 0.0) throw std::runtime_error("clip duration must be positive for '" + clip.name + "'");
            const JsonValue& tracks = field(clipValue, "tracks");
            if (tracks.type != JsonValue::Type::Array || tracks.array.empty()) {
                throw std::runtime_error("clip '" + clip.name + "' has no tracks");
            }
            std::set<std::string> trackedParts;
            for (const JsonValue& trackValue : tracks.array) {
                RigTrack track;
                track.part = stringValue(field(trackValue, "part"), "track.part");
                if (!trackedParts.insert(track.part).second) {
                    throw std::runtime_error("duplicate track for '" + track.part + "' in clip '" + clip.name + "'");
                }
                track.partIndex = rig.partIndex(track.part);
                const JsonValue& keyframes = field(trackValue, "keyframes");
                if (keyframes.type != JsonValue::Type::Array || keyframes.array.size() < 2) {
                    throw std::runtime_error("track '" + track.part + "' needs at least two keyframes");
                }
                double previousTime = -1.0;
                for (const JsonValue& keyValue : keyframes.array) {
                    RigKeyframe key;
                    key.time = numberValue(field(keyValue, "time"), "keyframe.time");
                    key.translation = vec2Value(field(keyValue, "translation"), "keyframe.translation");
                    key.rotation = numberValue(field(keyValue, "rotation"), "keyframe.rotation");
                    key.scale = vec2Value(field(keyValue, "scale"), "keyframe.scale");
                    if (key.time <= previousTime || key.time < 0.0 || key.time > clip.duration ||
                        key.scale.x <= 0.0 || key.scale.y <= 0.0) {
                        throw std::runtime_error("invalid keyframe ordering or value in '" + clip.name + "/" + track.part + "'");
                    }
                    previousTime = key.time;
                    track.keyframes.push_back(key);
                }
                if (track.keyframes.front().time != 0.0 ||
                    std::abs(track.keyframes.back().time - clip.duration) > 1e-9) {
                    throw std::runtime_error("track '" + track.part + "' must span its clip duration");
                }
                clip.tracks.push_back(std::move(track));
            }
            if (!trackedParts.contains("root") || !trackedParts.contains("pelvis") ||
                !trackedParts.contains("torso_upper") || !trackedParts.contains("head") ||
                !trackedParts.contains("thigh_front") || !trackedParts.contains("shin_front") ||
                !trackedParts.contains("foot_front") || !trackedParts.contains("thigh_rear") ||
                !trackedParts.contains("shin_rear") || !trackedParts.contains("foot_rear") ||
                !trackedParts.contains("upper_arm_front") || !trackedParts.contains("forearm_front") ||
                !trackedParts.contains("upper_arm_rear") || !trackedParts.contains("forearm_rear") ||
                !trackedParts.contains("ion_grip")) {
                throw std::runtime_error("clip '" + clip.name + "' omits required articulated tracks");
            }
            result.animations.clips.push_back(std::move(clip));
        }
        constexpr std::array requiredClips{
            "idle", "acceleration", "run", "brake", "turn", "jump_launch",
            "rising", "apex", "falling", "soft_landing", "hard_landing"
        };
        for (std::string_view name : requiredClips) {
            if (!result.animations.findClip(name)) {
                throw std::runtime_error("missing required clip '" + std::string(name) + "'");
            }
        }
        result.ok = true;
    } catch (const std::exception& error) {
        result.error = "Failed to load animations " + path.string() + ": " + error.what();
    }
    return result;
}

Vec2 transformPoint(const RigTransform& transform, Vec2 localPoint) {
    const double radians = transform.rotation * kPi / 180.0;
    const double x = localPoint.x * transform.scale.x;
    const double y = localPoint.y * transform.scale.y;
    return {transform.position.x + x * std::cos(radians) - y * std::sin(radians),
            transform.position.y + x * std::sin(radians) + y * std::cos(radians)};
}

RigPose evaluateRig(const RigDefinition& rig, const RigAnimations& animations,
                    std::string_view clipName, double time, bool facingRight) {
    const RigClip* clip = animations.findClip(clipName);
    if (!clip) throw std::runtime_error("unknown animation clip '" + std::string(clipName) + "'");
    std::vector<RigKeyframe> animation(rig.parts.size());
    for (const RigTrack& track : clip->tracks) {
        animation[track.partIndex] = sampleTrack(track, time, clip->duration, clip->loop);
    }

    RigPose pose;
    pose.parts.resize(rig.parts.size());
    pose.facingRight = facingRight;
    std::vector<bool> evaluated(rig.parts.size());
    const double facing = facingRight ? 1.0 : -1.0;
    std::function<void(std::size_t)> evaluatePart = [&](std::size_t index) {
        if (evaluated[index]) return;
        const RigPart& part = rig.parts[index];
        const RigKeyframe& animated = animation[index];
        Vec2 localPosition{part.attachment.x + part.offset.x + animated.translation.x,
                           part.attachment.y + part.offset.y + animated.translation.y};
        localPosition.x *= facing;
        const double localRotation = animated.rotation * facing;
        const Vec2 localScale{part.scale.x * animated.scale.x,
                              part.scale.y * animated.scale.y};
        RigTransform world;
        if (part.parentIndex == kNoParent) {
            world = {localPosition, localRotation, localScale};
        } else {
            evaluatePart(part.parentIndex);
            const RigTransform& parent = pose.parts[part.parentIndex].transform;
            world.position = transformPoint(parent, localPosition);
            world.rotation = parent.rotation + localRotation;
            world.scale = {parent.scale.x * localScale.x, parent.scale.y * localScale.y};
        }
        pose.parts[index].transform = world;
        evaluated[index] = true;
    };
    for (std::size_t i = 0; i < rig.parts.size(); ++i) evaluatePart(i);
    return pose;
}

std::string_view rigClipName(LocomotionState state) {
    switch (state) {
    case LocomotionState::Idle: return "idle";
    case LocomotionState::Accelerating: return "acceleration";
    case LocomotionState::Running: return "run";
    case LocomotionState::Braking: return "brake";
    case LocomotionState::Turning: return "turn";
    case LocomotionState::JumpLaunch: return "jump_launch";
    case LocomotionState::Rising: return "rising";
    case LocomotionState::Apex: return "apex";
    case LocomotionState::Falling: return "falling";
    case LocomotionState::Landing: return "soft_landing";
    case LocomotionState::HardLanding: return "hard_landing";
    }
    return "idle";
}

} // namespace irys_ion
