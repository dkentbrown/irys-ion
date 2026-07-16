#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

namespace irys {

enum class Anatomy { Standard, SingleArm, SingleLeg, AdditionalArm };
enum class Archetype { Balanced, Lightweight, Dense, Compact, Elongated, Reinforced, Unstable, Precision };
enum class Side { Left, Right };
enum class CollisionProfile { Small, Standard, Large };
enum class Technique { None, Advance, Ascend, Descend };
enum class Authority { Ordinary, NormalTechnique, PrimedTechnique, HitReaction, Defeated };
enum class EnemyLineage { Striker, Pursuer, Anchor };
enum class BossKind { FirstGuardian, SecondGuardian, ProtoIrys, CommandMech, AntiIrys };
enum class CampaignPhase {
    Ark, Stage1, Boss1, Selection1, Stage2, Boss2, Selection2, Stage3, Boss3,
    Selection3, ControlCenter, Boss4, Transfer, ShutdownReturn, FinalDuel, Ending
};

std::string toString(Anatomy value);
std::string toString(Archetype value);
std::string toString(Side value);
std::string toString(CollisionProfile value);
std::string toString(Technique value);
std::string toString(Authority value);
std::string toString(EnemyLineage value);
std::string toString(BossKind value);
std::string toString(CampaignPhase value);

class Rng {
public:
    explicit Rng(std::uint64_t seed = 1);
    std::uint64_t nextU64();
    float unit();
    float range(float low, float high);
    int rangeInt(int low, int highInclusive);
    bool chance(float probability);
    std::uint64_t state() const { return state_; }
private:
    std::uint64_t state_;
};

struct Frame {
    std::uint64_t id{};
    std::string designation;
    Anatomy anatomy{Anatomy::Standard};
    Archetype archetype{Archetype::Balanced};
    Side dominantSide{Side::Right};
    Side weaponSide{Side::Right};
    CollisionProfile collision{CollisionProfile::Standard};
    std::vector<std::string> traits;
    float maxIntegrity{100.0f};
    float integrity{100.0f};
    float groundSpeed{150.0f};
    float acceleration{900.0f};
    float jumpPower{350.0f};
    float airControl{0.7f};
    float hitRecovery{0.24f};
    float reach{42.0f};
    float strikeForce{1.0f};
    float critChance{0.06f};
    float critMultiplier{1.5f};
    float advanceDistance{120.0f};
    float ascendHeight{110.0f};
    float descendDrive{1.0f};
    float primeHold{1.5f};
    float stature{1.0f};
    float mass{1.0f};
    std::uint32_t palette{};
    bool viable() const;
};

class BodyGenerator {
public:
    explicit BodyGenerator(std::uint64_t seed);
    Frame generate(float qualityBias = 0.0f);
    std::array<Frame, 3> startingCandidates();
    std::vector<Frame> postBossCandidates(int defeatedBoss);
private:
    Rng rng_;
    std::uint64_t serial_{};
};

struct Tuning {
    int schemaVersion{1};
    int simulationHz{60};
    float gravity{980.0f};
    float jumpBuffer{0.10f};
    float coyoteTime{0.09f};
    float jumpCut{0.48f};
    float normalDuration{0.22f};
    float primedDuration{0.52f};
    float primedRecovery{0.18f};
    float advanceDamage{18.0f};
    float ascendDamage{16.0f};
    float descendDamage{22.0f};
    float primedMultiplier{1.65f};
    float cameraSmoothing{8.0f};
    float cameraShake{1.0f};
    float masterVolume{0.85f};
    float musicVolume{0.62f};
    float effectsVolume{0.88f};
    float dialogueVolume{0.72f};
    bool validate(std::string& error) const;
    static Tuning load(const std::filesystem::path& path, std::string& warning);
};

struct CombatState {
    Authority authority{Authority::Ordinary};
    Technique technique{Technique::None};
    bool primeArmed{};
    bool techniquePrimed{};
    float primeRemaining{};
    float actionRemaining{};
    std::uint64_t activation{};
    std::unordered_set<std::uint64_t> hitTargets;
    std::unordered_set<std::uint64_t> overlappingTargets;
};

class CombatModel {
public:
    explicit CombatModel(Tuning tuning = {});
    void reset();
    bool armPrime(const Frame& frame);
    bool speak(Technique technique, const Frame& frame);
    void tick(float dt);
    void interrupt(float recovery);
    void terrainContact();
    bool canHit(std::uint64_t targetId, bool overlapping);
    void separated(std::uint64_t targetId);
    float damageFor(Technique technique, const Frame& frame, bool primed, Rng& rng, bool& critical) const;
    const CombatState& state() const { return state_; }
private:
    Tuning tuning_;
    CombatState state_;
};

struct RunRecord {
    std::uint64_t seed{};
    std::string startingFrame;
    std::vector<std::string> swapDecisions;
    int bossesReached{};
    bool success{};
    double duration{};
};

struct Settings {
    float masterVolume{0.85f};
    float musicVolume{0.62f};
    float effectsVolume{0.88f};
    float dialogueVolume{0.72f};
    bool fullscreen{};
    bool integerScaling{true};
    float screenShake{1.0f};
    float flashIntensity{1.0f};
    float cameraSmoothing{1.0f};
    float dialogueSpeed{1.0f};
};

struct Profile {
    int schemaVersion{1};
    Settings settings;
    std::uint64_t attempts{};
    std::uint64_t successfulRuns{};
    std::uint64_t substratesLost{};
    std::uint64_t swapsAccepted{};
    std::uint64_t swapsDeclined{};
    std::array<std::uint64_t, 5> bossDefeats{};
    int furthestStage{};
    double totalPlaytime{};
    double fastestRun{};
    bool hasClearedGame{};
    std::vector<RunRecord> recentRuns;
    void addRun(const RunRecord& record);
    bool save(const std::filesystem::path& path, std::string& error) const;
    static Profile load(const std::filesystem::path& path, std::string& warning);
};

class Campaign {
public:
    void begin(std::uint64_t seed, Frame selected);
    bool advanceStage();
    bool defeatBoss();
    bool chooseReplacement(std::optional<Frame> replacement);
    void completeTransfer();
    void completeShutdownReturn();
    bool fail();
    CampaignPhase phase() const { return phase_; }
    int bossesDefeated() const { return bossesDefeated_; }
    const Frame& frame() const { return frame_; }
    Frame& frame() { return frame_; }
    std::uint64_t seed() const { return seed_; }
    bool active() const { return active_; }
private:
    CampaignPhase phase_{CampaignPhase::Ark};
    int bossesDefeated_{};
    std::uint64_t seed_{};
    Frame frame_{};
    bool active_{};
};

std::filesystem::path defaultProfilePath();

} // namespace irys
