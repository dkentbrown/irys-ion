#include "irys_ion/core.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>

namespace {
int failures = 0;
#define CHECK(condition) do { if (!(condition)) { std::cerr << "FAIL " << __FILE__ << ':' << __LINE__ << " " #condition "\n"; ++failures; } } while (false)
}

int main() {
    using namespace irys;
    Rng a(42), b(42);
    for (int i = 0; i < 100; ++i) CHECK(a.nextU64() == b.nextU64());

    std::set<Anatomy> anatomies;
    for (std::uint64_t seed = 1; seed <= 300; ++seed) {
        BodyGenerator generator(seed);
        for (const auto& frame : generator.startingCandidates()) {
            CHECK(frame.viable()); anatomies.insert(frame.anatomy);
            CHECK(frame.integrity == frame.maxIntegrity);
            CHECK(frame.traits.size() >= 2);
            CHECK(frame.weaponHand >= 0 && frame.weaponHand <= (frame.anatomy == Anatomy::AdditionalArm ? 2 : 1));
        }
    }
    CHECK(anatomies.size() == 4);

    BodyGenerator generator(99);
    auto frame = generator.generate();
    CombatModel combat;
    CHECK(combat.armPrime(frame));
    CHECK(!combat.armPrime(frame));
    CHECK(combat.speak(Technique::Advance, frame));
    CHECK(combat.state().authority == Authority::PrimedTechnique);
    CHECK(!combat.speak(Technique::Ascend, frame));
    combat.interrupt(frame.hitRecovery);
    CHECK(combat.state().authority == Authority::PrimedTechnique);
    for (int i = 0; i < 80; ++i) combat.tick(1.0f / 60.0f);
    CHECK(combat.state().authority == Authority::Ordinary);
    CHECK(combat.speak(Technique::Ascend, frame));
    CHECK(combat.speak(Technique::Descend, frame));
    CHECK(combat.state().technique == Technique::Descend);
    CHECK(combat.canHit(7, true));
    CHECK(!combat.canHit(7, true));
    combat.speak(Technique::Advance, frame);
    CHECK(!combat.canHit(7, true));
    combat.separated(7);
    CHECK(combat.canHit(7, true));
    combat.interrupt(frame.hitRecovery);
    CHECK(combat.state().authority == Authority::HitReaction);
    CHECK(!combat.speak(Technique::Advance, frame));

    Campaign campaign;
    campaign.begin(123, frame);
    CHECK(campaign.phase() == CampaignPhase::Stage1);
    CHECK(campaign.advanceStage()); CHECK(campaign.phase() == CampaignPhase::Boss1); CHECK(campaign.defeatBoss());
    CHECK(campaign.phase() == CampaignPhase::Selection1); CHECK(campaign.chooseReplacement(std::nullopt));
    CHECK(campaign.phase() == CampaignPhase::Stage2); CHECK(campaign.advanceStage()); CHECK(campaign.defeatBoss());
    auto replacement = generator.generate(); replacement.integrity *= 0.7f;
    CHECK(campaign.chooseReplacement(replacement)); CHECK(campaign.frame().id == replacement.id);
    CHECK(campaign.advanceStage()); CHECK(campaign.defeatBoss()); CHECK(campaign.chooseReplacement(std::nullopt));
    CHECK(campaign.phase() == CampaignPhase::ControlCenter); CHECK(campaign.advanceStage()); CHECK(campaign.defeatBoss());
    CHECK(campaign.phase() == CampaignPhase::Transfer); campaign.completeTransfer(); CHECK(campaign.phase() == CampaignPhase::ShutdownReturn);
    campaign.completeShutdownReturn(); CHECK(campaign.phase() == CampaignPhase::FinalDuel); CHECK(campaign.defeatBoss());
    CHECK(campaign.phase() == CampaignPhase::Ending); CHECK(campaign.bossesDefeated() == 5); CHECK(!campaign.active());

    const char* source = std::getenv("IRYSION_SOURCE_DIR");
    CHECK(source != nullptr);
    if (source) {
        std::string warning;
        const auto tuning = Tuning::load(std::filesystem::path(source) / "config" / "tuning.json", warning);
        CHECK(warning.empty()); CHECK(tuning.simulationHz == 60);
        auto invalid = std::filesystem::temp_directory_path() / "irys-ion-invalid-tuning.json";
        { std::ofstream out(invalid); out << "{\"schemaVersion\":1,\"simulationHz\":30}"; }
        const auto fallback = Tuning::load(invalid, warning);
        CHECK(!warning.empty()); CHECK(fallback.simulationHz == 60);
        std::filesystem::remove(invalid);
    }

    const auto profilePath = std::filesystem::temp_directory_path() / "irys-ion-tests" / "profile.json";
    Profile profile; profile.attempts = 3; profile.bossDefeats[2] = 2; profile.settings.musicVolume = 0.33f;
    profile.addRun({77, "IRYS-TEST", {"retain", "swap"}, 4, false, 120.0});
    std::string error; CHECK(profile.save(profilePath, error));
    std::string warning; auto loaded = Profile::load(profilePath, warning);
    CHECK(warning.empty()); CHECK(loaded.attempts == 3); CHECK(loaded.bossDefeats[2] == 2);
    CHECK(loaded.recentRuns.size() == 1); CHECK(loaded.settings.musicVolume > 0.32f && loaded.settings.musicVolume < 0.34f);
    { std::ofstream out(profilePath); out << "corrupt"; }
    loaded = Profile::load(profilePath, warning); CHECK(!warning.empty()); CHECK(loaded.attempts == 3);
    std::filesystem::remove_all(profilePath.parent_path());

    if (failures == 0) std::cout << "All deterministic core validations passed\n";
    return failures == 0 ? 0 : 1;
}
