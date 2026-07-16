#include "irys_ion/core.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <limits>
#include <nlohmann/json.hpp>
#include <sstream>
#include <system_error>

using json = nlohmann::json;

namespace irys {
namespace {

template <typename E, std::size_t N>
std::string enumName(E value, const std::array<const char*, N>& names) {
    const auto index = static_cast<std::size_t>(value);
    return index < names.size() ? names[index] : "Unknown";
}

float clamp(float value, float low, float high) { return std::max(low, std::min(high, value)); }

template <typename T>
T getOr(const json& value, const char* key, T fallback) {
    try {
        return value.contains(key) ? value.at(key).get<T>() : fallback;
    } catch (...) {
        return fallback;
    }
}

json settingsJson(const Settings& s) {
    return {{"masterVolume", s.masterVolume}, {"musicVolume", s.musicVolume},
            {"effectsVolume", s.effectsVolume}, {"dialogueVolume", s.dialogueVolume},
            {"fullscreen", s.fullscreen}, {"integerScaling", s.integerScaling},
            {"screenShake", s.screenShake}, {"flashIntensity", s.flashIntensity},
            {"cameraSmoothing", s.cameraSmoothing}, {"dialogueSpeed", s.dialogueSpeed}};
}

Settings parseSettings(const json& j) {
    Settings s;
    s.masterVolume = clamp(getOr(j, "masterVolume", s.masterVolume), 0.0f, 1.0f);
    s.musicVolume = clamp(getOr(j, "musicVolume", s.musicVolume), 0.0f, 1.0f);
    s.effectsVolume = clamp(getOr(j, "effectsVolume", s.effectsVolume), 0.0f, 1.0f);
    s.dialogueVolume = clamp(getOr(j, "dialogueVolume", s.dialogueVolume), 0.0f, 1.0f);
    s.fullscreen = getOr(j, "fullscreen", false);
    s.integerScaling = getOr(j, "integerScaling", true);
    s.screenShake = clamp(getOr(j, "screenShake", 1.0f), 0.0f, 1.0f);
    s.flashIntensity = clamp(getOr(j, "flashIntensity", 1.0f), 0.0f, 1.0f);
    s.cameraSmoothing = clamp(getOr(j, "cameraSmoothing", 1.0f), 0.0f, 1.0f);
    s.dialogueSpeed = clamp(getOr(j, "dialogueSpeed", 1.0f), 0.25f, 3.0f);
    return s;
}

json runJson(const RunRecord& r) {
    return {{"seed", r.seed}, {"startingFrame", r.startingFrame},
            {"swapDecisions", r.swapDecisions}, {"bossesReached", r.bossesReached},
            {"success", r.success}, {"duration", r.duration}};
}

RunRecord parseRun(const json& j) {
    RunRecord r;
    r.seed = getOr<std::uint64_t>(j, "seed", 0);
    r.startingFrame = getOr<std::string>(j, "startingFrame", "Unknown");
    if (j.contains("swapDecisions") && j["swapDecisions"].is_array()) {
        for (const auto& item : j["swapDecisions"]) if (item.is_string()) r.swapDecisions.push_back(item.get<std::string>());
    }
    r.bossesReached = std::clamp(getOr(j, "bossesReached", 0), 0, 5);
    r.success = getOr(j, "success", false);
    r.duration = std::max(0.0, getOr(j, "duration", 0.0));
    return r;
}

} // namespace

std::string toString(Anatomy v) { return enumName(v, std::array{"Standard", "SingleArm", "SingleLeg", "AdditionalArm"}); }
std::string toString(Archetype v) { return enumName(v, std::array{"Balanced", "Lightweight", "Dense", "Compact", "Elongated", "Reinforced", "Unstable", "Precision"}); }
std::string toString(Side v) { return enumName(v, std::array{"Left", "Right"}); }
std::string toString(CollisionProfile v) { return enumName(v, std::array{"Small", "Standard", "Large"}); }
std::string toString(Technique v) { return enumName(v, std::array{"None", "Advance", "Ascend", "Descend"}); }
std::string toString(Authority v) { return enumName(v, std::array{"Ordinary", "Normal Technique", "Primed Technique", "Hit Reaction", "Defeated"}); }
std::string toString(EnemyLineage v) { return enumName(v, std::array{"Striker", "Pursuer", "Anchor"}); }
std::string toString(BossKind v) { return enumName(v, std::array{"First Guardian", "Second Guardian", "Restrained Proto-Irys", "Command Mech", "Anti-Irys"}); }
std::string toString(CampaignPhase v) { return enumName(v, std::array{"Ark", "Industrial Perimeter", "First Guardian", "Substrate Decision I", "Adaptive Infrastructure", "Second Guardian", "Substrate Decision II", "Experimentation Zone", "Restrained Proto-Irys", "Substrate Decision III", "Control Center", "Command Mech", "Anti-Irys Transfer", "Shutdown Return", "Final Ark Duel", "Ending"}); }

Rng::Rng(std::uint64_t seed) : state_(seed ? seed : 0x9e3779b97f4a7c15ULL) {}
std::uint64_t Rng::nextU64() {
    std::uint64_t x = state_;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    state_ = x;
    return x * 2685821657736338717ULL;
}
float Rng::unit() { return static_cast<float>((nextU64() >> 40) * (1.0 / 16777216.0)); }
float Rng::range(float low, float high) { return low + (high - low) * unit(); }
int Rng::rangeInt(int low, int highInclusive) {
    if (highInclusive <= low) return low;
    return low + static_cast<int>(nextU64() % static_cast<std::uint64_t>(highInclusive - low + 1));
}
bool Rng::chance(float probability) { return unit() < clamp(probability, 0.0f, 1.0f); }

bool Frame::viable() const {
    return maxIntegrity >= 60.0f && integrity > 0.0f && integrity <= maxIntegrity && groundSpeed >= 105.0f &&
           acceleration >= 520.0f && jumpPower >= 285.0f && airControl >= 0.45f && hitRecovery <= 0.42f &&
           reach >= 30.0f && strikeForce >= 0.68f && critChance >= 0.02f && critChance <= 0.22f &&
           critMultiplier >= 1.35f && advanceDistance >= 86.0f && ascendHeight >= 82.0f &&
           descendDrive >= 0.72f && primeHold >= 1.0f;
}

BodyGenerator::BodyGenerator(std::uint64_t seed) : rng_(seed) {}

Frame BodyGenerator::generate(float qualityBias) {
    Frame f;
    f.id = ++serial_;
    f.designation = "IRYS-" + std::to_string(1000 + (rng_.nextU64() % 9000));
    f.anatomy = static_cast<Anatomy>(rng_.rangeInt(0, 3));
    f.archetype = static_cast<Archetype>(rng_.rangeInt(0, 7));
    f.dominantSide = rng_.chance(0.5f) ? Side::Left : Side::Right;
    f.weaponSide = f.dominantSide;
    f.weaponHand = f.weaponSide == Side::Left ? 0 : 1;
    f.palette = static_cast<std::uint32_t>(rng_.rangeInt(0, 5));
    const float quality = clamp(rng_.range(-0.32f, 0.38f) + qualityBias + (rng_.chance(0.035f) ? 0.36f : 0.0f), -0.42f, 0.68f);
    f.stature = rng_.range(0.86f, 1.16f);
    f.mass = rng_.range(0.78f, 1.24f);
    float speed = 1.0f, durable = 1.0f, precision = 1.0f, reach = 1.0f, recovery = 1.0f;
    switch (f.archetype) {
        case Archetype::Lightweight: speed = 1.14f; durable = 0.82f; f.mass *= 0.86f; f.traits.push_back("Lightweight Frame"); break;
        case Archetype::Dense: speed = 0.89f; durable = 1.24f; f.mass *= 1.16f; recovery = 1.12f; f.traits.push_back("Dense Structure"); break;
        case Archetype::Compact: speed = 1.05f; recovery = 0.78f; f.stature *= 0.91f; f.traits.push_back("Compact Balance"); break;
        case Archetype::Elongated: reach = 1.24f; recovery = 1.13f; f.stature *= 1.10f; f.traits.push_back("Elongated Reach"); break;
        case Archetype::Reinforced: durable = 1.32f; speed = 0.91f; f.traits.push_back("Reinforced Torso"); break;
        case Archetype::Unstable: speed = 1.08f; recovery = 1.35f; f.traits.push_back("Unstable Joints"); break;
        case Archetype::Precision: precision = 1.55f; durable = 0.93f; f.traits.push_back("Precision Construction"); break;
        case Archetype::Balanced: f.traits.push_back("Balanced Chassis"); break;
    }
    switch (f.anatomy) {
        case Anatomy::SingleArm: f.weaponSide = rng_.chance(0.5f) ? Side::Left : Side::Right; f.weaponHand = f.weaponSide == Side::Left ? 0 : 1; f.traits.push_back("SingleArm Specialization"); if(f.weaponSide!=f.dominantSide){f.traits.push_back("Non-dominant Interface");recovery*=1.05f;} reach *= 0.96f; break;
        case Anatomy::SingleLeg: f.traits.push_back("SingleLeg Wheel-Foot"); speed *= 0.94f; recovery *= 1.06f; break;
        case Anatomy::AdditionalArm: f.weaponHand=rng_.rangeInt(0,2); f.weaponSide=f.weaponHand==0?Side::Left:Side::Right; f.traits.push_back("AdditionalArm Stabilizer"); f.traits.push_back("Automatic Hand Transfer"); reach *= 1.08f; recovery *= 0.94f; break;
        case Anatomy::Standard: f.traits.push_back("Standard Rig"); break;
    }
    const float q = 1.0f + quality * 0.32f;
    f.maxIntegrity = clamp(rng_.range(82.0f, 132.0f) * durable * q, 60.0f, 190.0f);
    f.integrity = f.maxIntegrity;
    f.groundSpeed = clamp(rng_.range(132.0f, 168.0f) * speed * q, 105.0f, 218.0f);
    f.acceleration = clamp(rng_.range(720.0f, 1030.0f) * speed * q, 520.0f, 1420.0f);
    f.jumpPower = clamp(rng_.range(320.0f, 390.0f) * (0.96f + 0.06f * speed) * q, 285.0f, 480.0f);
    f.airControl = clamp(rng_.range(0.58f, 0.84f) * speed * q, 0.45f, 1.08f);
    f.hitRecovery = clamp(rng_.range(0.19f, 0.30f) * recovery / q, 0.14f, 0.42f);
    f.reach = clamp(rng_.range(37.0f, 47.0f) * reach * f.stature * q, 30.0f, 68.0f);
    f.strikeForce = clamp(rng_.range(0.84f, 1.18f) * (0.8f + f.mass * 0.2f) * q, 0.68f, 1.65f);
    f.critChance = clamp(rng_.range(0.03f, 0.105f) * precision * q, 0.02f, 0.22f);
    f.critMultiplier = clamp(rng_.range(1.42f, 1.68f) * (0.96f + quality * 0.12f), 1.35f, 1.88f);
    f.advanceDistance = clamp(rng_.range(104.0f, 136.0f) * speed * q, 86.0f, 178.0f);
    f.ascendHeight = clamp(rng_.range(96.0f, 126.0f) * q, 82.0f, 158.0f);
    f.descendDrive = clamp(rng_.range(0.88f, 1.18f) * (0.88f + f.mass * 0.12f) * q, 0.72f, 1.52f);
    f.primeHold = clamp(rng_.range(1.22f, 1.82f) * q, 1.0f, 2.35f);
    if (f.stature < 0.93f && f.mass < 0.96f) f.collision = CollisionProfile::Small;
    else if (f.stature > 1.08f || f.mass > 1.12f) f.collision = CollisionProfile::Large;
    if (quality > 0.45f) f.traits.push_back("High Synchronization");
    if (quality < -0.25f) f.traits.push_back("Asymmetric Frame");
    if (!f.viable()) {
        f.maxIntegrity = std::max(f.maxIntegrity, 60.0f);
        f.integrity = f.maxIntegrity;
        f.groundSpeed = std::max(f.groundSpeed, 105.0f);
        f.acceleration = std::max(f.acceleration, 520.0f);
        f.jumpPower = std::max(f.jumpPower, 285.0f);
        f.airControl = std::max(f.airControl, 0.45f);
        f.hitRecovery = std::min(f.hitRecovery, 0.42f);
        f.reach = std::max(f.reach, 30.0f);
        f.strikeForce = std::max(f.strikeForce, 0.68f);
        f.advanceDistance = std::max(f.advanceDistance, 86.0f);
        f.ascendHeight = std::max(f.ascendHeight, 82.0f);
        f.descendDrive = std::max(f.descendDrive, 0.72f);
        f.primeHold = std::max(f.primeHold, 1.0f);
    }
    return f;
}

std::array<Frame, 3> BodyGenerator::startingCandidates() {
    std::array<Frame, 3> result{generate(), generate(), generate()};
    const bool sameAnatomy = result[0].anatomy == result[1].anatomy && result[1].anatomy == result[2].anatomy;
    const bool nearStats = std::abs(result[0].groundSpeed - result[1].groundSpeed) < 5.0f &&
                           std::abs(result[1].groundSpeed - result[2].groundSpeed) < 5.0f;
    if (sameAnatomy && nearStats) {
        for (int attempt = 0; attempt < 8; ++attempt) {
            result[2] = generate();
            if (result[2].anatomy != result[0].anatomy || std::abs(result[2].groundSpeed - result[0].groundSpeed) > 8.0f) break;
        }
    }
    return result;
}

std::vector<Frame> BodyGenerator::postBossCandidates(int defeatedBoss) {
    const int count = rng_.chance(0.5f) ? 2 : 3;
    const float bias = clamp(0.035f * static_cast<float>(defeatedBoss), 0.0f, 0.16f);
    std::vector<Frame> frames;
    for (int i = 0; i < count; ++i) {
        Frame f = generate(bias);
        f.integrity = f.maxIntegrity * rng_.range(0.55f + 0.04f * defeatedBoss, 0.96f);
        frames.push_back(std::move(f));
    }
    return frames;
}

bool Tuning::validate(std::string& error) const {
    if (schemaVersion != 1) error = "unsupported tuning schema";
    else if (simulationHz != 60) error = "simulationHz must be 60";
    else if (gravity < 100.0f || gravity > 3000.0f) error = "gravity out of range";
    else if (normalDuration < 0.08f || normalDuration > 0.6f) error = "normalDuration out of range";
    else if (primedDuration <= normalDuration || primedDuration > 1.5f) error = "primedDuration out of range";
    else if (advanceDamage <= 0 || ascendDamage <= 0 || descendDamage <= 0) error = "Technique damage must be positive";
    else if (masterVolume < 0 || masterVolume > 1 || musicVolume < 0 || musicVolume > 1 || effectsVolume < 0 || effectsVolume > 1 || dialogueVolume < 0 || dialogueVolume > 1) error = "audio volume out of range";
    else return true;
    return false;
}

Tuning Tuning::load(const std::filesystem::path& path, std::string& warning) {
    Tuning out;
    try {
        std::ifstream stream(path);
        if (!stream) { warning = "Could not open " + path.string() + "; using compiled defaults."; return out; }
        json j; stream >> j;
        out.schemaVersion = getOr(j, "schemaVersion", 1);
        out.simulationHz = getOr(j, "simulationHz", 60);
        if (j.contains("movement")) {
            const auto& m = j["movement"];
            out.gravity = getOr(m, "gravity", out.gravity); out.jumpBuffer = getOr(m, "jumpBuffer", out.jumpBuffer);
            out.coyoteTime = getOr(m, "coyoteTime", out.coyoteTime); out.jumpCut = getOr(m, "jumpCut", out.jumpCut);
        }
        if (j.contains("techniques")) {
            const auto& t = j["techniques"];
            out.normalDuration = getOr(t, "normalDuration", out.normalDuration); out.primedDuration = getOr(t, "primedDuration", out.primedDuration);
            out.primedRecovery = getOr(t, "primedRecovery", out.primedRecovery); out.advanceDamage = getOr(t, "advanceDamage", out.advanceDamage);
            out.ascendDamage = getOr(t, "ascendDamage", out.ascendDamage); out.descendDamage = getOr(t, "descendDamage", out.descendDamage);
            out.primedMultiplier = getOr(t, "primedMultiplier", out.primedMultiplier);
        }
        if (j.contains("camera")) { out.cameraSmoothing = getOr(j["camera"], "smoothing", out.cameraSmoothing); out.cameraShake = getOr(j["camera"], "shake", out.cameraShake); }
        if (j.contains("audio")) {
            out.masterVolume = getOr(j["audio"], "master", out.masterVolume); out.musicVolume = getOr(j["audio"], "music", out.musicVolume);
            out.effectsVolume = getOr(j["audio"], "effects", out.effectsVolume); out.dialogueVolume = getOr(j["audio"], "dialogue", out.dialogueVolume);
        }
        std::string error;
        if (!out.validate(error)) { warning = "Invalid tuning: " + error + "; using compiled defaults."; return Tuning{}; }
    } catch (const std::exception& e) { warning = std::string("Malformed tuning: ") + e.what() + "; using compiled defaults."; return Tuning{}; }
    return out;
}

CombatModel::CombatModel(Tuning tuning) : tuning_(tuning) {}
void CombatModel::reset() { state_ = {}; }
bool CombatModel::armPrime(const Frame& frame) {
    if (state_.primeArmed || state_.authority == Authority::PrimedTechnique || state_.authority == Authority::HitReaction || state_.authority == Authority::Defeated) return false;
    state_.primeArmed = true; state_.primeRemaining = frame.primeHold; return true;
}
bool CombatModel::speak(Technique technique, const Frame&) {
    if (technique == Technique::None || state_.authority == Authority::PrimedTechnique || state_.authority == Authority::HitReaction || state_.authority == Authority::Defeated) return false;
    state_.technique = technique; state_.techniquePrimed = state_.primeArmed;
    state_.primeArmed = false; state_.primeRemaining = 0.0f;
    state_.authority = state_.techniquePrimed ? Authority::PrimedTechnique : Authority::NormalTechnique;
    state_.actionRemaining = state_.techniquePrimed ? tuning_.primedDuration + tuning_.primedRecovery : tuning_.normalDuration;
    ++state_.activation; state_.hitTargets.clear();
    return true;
}
void CombatModel::tick(float dt) {
    if (state_.primeArmed) { state_.primeRemaining -= dt; if (state_.primeRemaining <= 0.0f) { state_.primeArmed = false; state_.primeRemaining = 0.0f; } }
    if (state_.authority == Authority::NormalTechnique || state_.authority == Authority::PrimedTechnique || state_.authority == Authority::HitReaction) {
        state_.actionRemaining -= dt;
        if (state_.actionRemaining <= 0.0f) { state_.authority = Authority::Ordinary; state_.technique = Technique::None; state_.techniquePrimed = false; state_.actionRemaining = 0.0f; state_.hitTargets.clear(); }
    }
}
void CombatModel::interrupt(float recovery) {
    if (state_.authority == Authority::Defeated) return;
    if (state_.authority == Authority::PrimedTechnique) return;
    state_.authority = Authority::HitReaction; state_.technique = Technique::None; state_.techniquePrimed = false;
    state_.primeArmed = false; state_.primeRemaining = 0.0f; state_.actionRemaining = std::max(0.05f, recovery); state_.hitTargets.clear();
}
void CombatModel::terrainContact() {
    if (state_.authority == Authority::NormalTechnique) {
        state_.authority = Authority::Ordinary; state_.technique = Technique::None; state_.techniquePrimed = false;
        state_.actionRemaining = 0.0f; state_.hitTargets.clear();
    }
}
bool CombatModel::canHit(std::uint64_t targetId, bool overlapping) {
    if (state_.authority != Authority::NormalTechnique && state_.authority != Authority::PrimedTechnique) return false;
    if (!overlapping) { state_.overlappingTargets.erase(targetId); return false; }
    if (state_.overlappingTargets.contains(targetId) || state_.hitTargets.contains(targetId)) return false;
    state_.overlappingTargets.insert(targetId); state_.hitTargets.insert(targetId); return true;
}
void CombatModel::separated(std::uint64_t targetId) { state_.overlappingTargets.erase(targetId); }
float CombatModel::damageFor(Technique technique, const Frame& frame, bool primed, Rng& rng, bool& critical) const {
    float base = technique == Technique::Advance ? tuning_.advanceDamage : technique == Technique::Ascend ? tuning_.ascendDamage : tuning_.descendDamage;
    float damage = base * frame.strikeForce * (primed ? tuning_.primedMultiplier : 1.0f);
    critical = rng.chance(frame.critChance); if (critical) damage *= frame.critMultiplier; return damage;
}

void Profile::addRun(const RunRecord& record) {
    recentRuns.insert(recentRuns.begin(), record); if (recentRuns.size() > 5) recentRuns.resize(5);
}
bool Profile::save(const std::filesystem::path& path, std::string& error) const {
    try {
        std::filesystem::create_directories(path.parent_path());
        json j{{"schemaVersion", schemaVersion}, {"settings", settingsJson(settings)}, {"attempts", attempts},
               {"successfulRuns", successfulRuns}, {"substratesLost", substratesLost}, {"swapsAccepted", swapsAccepted},
               {"swapsDeclined", swapsDeclined}, {"bossDefeats", bossDefeats}, {"furthestStage", furthestStage},
               {"totalPlaytime", totalPlaytime}, {"fastestRun", fastestRun}, {"hasClearedGame", hasClearedGame}};
        j["recentRuns"] = json::array(); for (const auto& run : recentRuns) j["recentRuns"].push_back(runJson(run));
        const auto temp = path.string() + ".tmp"; const auto backup = path.string() + ".bak";
        { std::ofstream out(temp, std::ios::trunc); if (!out) throw std::runtime_error("cannot open temp profile"); out << std::setw(2) << j << '\n'; out.flush(); if (!out) throw std::runtime_error("cannot write temp profile"); }
        std::error_code ec; if (std::filesystem::exists(path)) { std::filesystem::copy_file(path, backup, std::filesystem::copy_options::overwrite_existing, ec); ec.clear(); }
        std::filesystem::rename(temp, path, ec); if (ec) { std::filesystem::remove(path, ec); ec.clear(); std::filesystem::rename(temp, path, ec); }
        if (ec) throw std::runtime_error(ec.message());
        if (!std::filesystem::exists(backup)) { ec.clear(); std::filesystem::copy_file(path, backup, std::filesystem::copy_options::overwrite_existing, ec); }
        return true;
    } catch (const std::exception& e) { error = e.what(); return false; }
}
Profile Profile::load(const std::filesystem::path& path, std::string& warning) {
    auto parse = [](const std::filesystem::path& source) -> Profile {
        std::ifstream in(source); if (!in) throw std::runtime_error("profile not found"); json j; in >> j;
        if (getOr(j, "schemaVersion", 0) != 1) throw std::runtime_error("unsupported profile schema");
        Profile p; p.settings = j.contains("settings") ? parseSettings(j["settings"]) : Settings{};
        p.attempts = getOr<std::uint64_t>(j, "attempts", 0); p.successfulRuns = getOr<std::uint64_t>(j, "successfulRuns", 0);
        p.substratesLost = getOr<std::uint64_t>(j, "substratesLost", 0); p.swapsAccepted = getOr<std::uint64_t>(j, "swapsAccepted", 0);
        p.swapsDeclined = getOr<std::uint64_t>(j, "swapsDeclined", 0); p.furthestStage = std::clamp(getOr(j, "furthestStage", 0), 0, 16);
        p.totalPlaytime = std::max(0.0, getOr(j, "totalPlaytime", 0.0)); p.fastestRun = std::max(0.0, getOr(j, "fastestRun", 0.0));
        p.hasClearedGame = getOr(j, "hasClearedGame", false);
        if (j.contains("bossDefeats") && j["bossDefeats"].is_array()) for (std::size_t i = 0; i < std::min<std::size_t>(5, j["bossDefeats"].size()); ++i) p.bossDefeats[i] = j["bossDefeats"][i].get<std::uint64_t>();
        if (j.contains("recentRuns") && j["recentRuns"].is_array()) for (const auto& r : j["recentRuns"]) if (p.recentRuns.size() < 5) p.recentRuns.push_back(parseRun(r));
        return p;
    };
    if (!std::filesystem::exists(path)) return {};
    try { return parse(path); } catch (const std::exception& first) {
        try { Profile p = parse(path.string() + ".bak"); warning = std::string("Recovered profile backup after: ") + first.what(); return p; }
        catch (...) { warning = std::string("Profile reset after corruption: ") + first.what(); return {}; }
    }
}

void Campaign::begin(std::uint64_t seed, Frame selected) { seed_ = seed; frame_ = std::move(selected); bossesDefeated_ = 0; phase_ = CampaignPhase::Stage1; active_ = true; }
bool Campaign::advanceStage() {
    if (!active_) return false;
    switch (phase_) {
        case CampaignPhase::Stage1: phase_ = CampaignPhase::Boss1; return true;
        case CampaignPhase::Stage2: phase_ = CampaignPhase::Boss2; return true;
        case CampaignPhase::Stage3: phase_ = CampaignPhase::Boss3; return true;
        case CampaignPhase::ControlCenter: phase_ = CampaignPhase::Boss4; return true;
        default: return false;
    }
}
bool Campaign::defeatBoss() {
    if (!active_) return false;
    switch (phase_) {
        case CampaignPhase::Boss1: ++bossesDefeated_; phase_ = CampaignPhase::Selection1; return true;
        case CampaignPhase::Boss2: ++bossesDefeated_; phase_ = CampaignPhase::Selection2; return true;
        case CampaignPhase::Boss3: ++bossesDefeated_; phase_ = CampaignPhase::Selection3; return true;
        case CampaignPhase::Boss4: ++bossesDefeated_; phase_ = CampaignPhase::Transfer; return true;
        case CampaignPhase::FinalDuel: ++bossesDefeated_; phase_ = CampaignPhase::Ending; active_ = false; return true;
        default: return false;
    }
}
bool Campaign::chooseReplacement(std::optional<Frame> replacement) {
    CampaignPhase next;
    if (phase_ == CampaignPhase::Selection1) next = CampaignPhase::Stage2;
    else if (phase_ == CampaignPhase::Selection2) next = CampaignPhase::Stage3;
    else if (phase_ == CampaignPhase::Selection3) next = CampaignPhase::ControlCenter;
    else return false;
    if (replacement) frame_ = std::move(*replacement); phase_ = next; return true;
}
void Campaign::completeTransfer() { if (phase_ == CampaignPhase::Transfer) phase_ = CampaignPhase::ShutdownReturn; }
void Campaign::completeShutdownReturn() { if (phase_ == CampaignPhase::ShutdownReturn) phase_ = CampaignPhase::FinalDuel; }
bool Campaign::fail() { if (!active_) return false; active_ = false; phase_ = CampaignPhase::Ark; return true; }

std::filesystem::path defaultProfilePath() {
#if defined(_WIN32)
    if (const char* appData = std::getenv("APPDATA")) return std::filesystem::path(appData) / "IrysIon" / "profile.json";
#else
    if (const char* home = std::getenv("HOME")) return std::filesystem::path(home) / ".local" / "share" / "irys-ion" / "profile.json";
#endif
    return std::filesystem::temp_directory_path() / "irys-ion" / "profile.json";
}

} // namespace irys
