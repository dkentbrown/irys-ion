#include "irys_ion/game.hpp"
#include "irys_ion/core.hpp"

#include <raylib.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace irys {
namespace {

constexpr int kWidth = 640;
constexpr int kHeight = 360;
constexpr float kStep = 1.0f / 60.0f;
constexpr float kFloor = 322.0f;

enum class Screen { Menu, Settings, Statistics, CandidateSelect, Scene, Gameplay, FrameSelect, Pause, Failure, Ending };
enum class SceneKind { Departure, BossIntro, PostBoss, Transfer, Return, FinalArrival, BladeDeath, Merger };

struct InputFrame {
    bool left{}, right{}, jumpDown{}, jumpPressed{}, jumpReleased{}, drop{};
    bool arrowLeft{}, arrowRight{}, up{}, down{}, confirm{}, back{}, pause{};
};

struct EnemyDef { EnemyLineage lineage; int generation; };
struct StageDef { const char* name; std::array<std::vector<EnemyDef>, 3> rooms; Color low; Color high; };
struct BossDef { BossKind kind; float health; int phases; float zoom; };

const std::array<StageDef, 4> kStages{{
    {"INDUSTRIAL PERIMETER", {{{{EnemyLineage::Striker,1}}, {{EnemyLineage::Pursuer,1},{EnemyLineage::Anchor,1}}, {{EnemyLineage::Striker,1},{EnemyLineage::Anchor,1}}}}, {10,18,27,255}, {58,95,104,255}},
    {"ADAPTIVE INFRASTRUCTURE", {{{{EnemyLineage::Striker,2},{EnemyLineage::Anchor,1}}, {{EnemyLineage::Pursuer,2},{EnemyLineage::Striker,1}}, {{EnemyLineage::Anchor,2},{EnemyLineage::Pursuer,2}}}}, {12,15,30,255}, {72,46,116,255}},
    {"BIOMECHANICAL EXPERIMENTATION", {{{{EnemyLineage::Anchor,3}}, {{EnemyLineage::Striker,3},{EnemyLineage::Pursuer,1}}, {{EnemyLineage::Pursuer,3},{EnemyLineage::Anchor,2}}}}, {18,12,26,255}, {118,48,91,255}},
    {"CONTROL CENTER", {{{{EnemyLineage::Striker,3},{EnemyLineage::Anchor,3}}, {{EnemyLineage::Pursuer,3},{EnemyLineage::Striker,2}}, {{EnemyLineage::Anchor,3},{EnemyLineage::Pursuer,3}}}}, {18,18,29,255}, {154,190,76,255}}
}};

const std::array<BossDef, 5> kBosses{{
    {BossKind::FirstGuardian, 380.0f, 2, 1.0f}, {BossKind::SecondGuardian, 520.0f, 2, 1.0f},
    {BossKind::ProtoIrys, 690.0f, 2, 1.0f}, {BossKind::CommandMech, 820.0f, 3, 1.0f},
    {BossKind::AntiIrys, 920.0f, 3, 1.0f}
}};

struct Particle { Vector2 p{}, v{}; float life{}; Color color{}; };
struct Projectile { Rectangle box{}; Vector2 velocity{}; float life{}; int damage{}; std::uint64_t activation{}; bool hit{}; Color color{}; };

struct EnemyRuntime {
    std::uint64_t id{}; EnemyLineage lineage{}; int generation{1}; Rectangle box{}; float health{}; float maxHealth{};
    float timer{}; float stateTime{}; int state{}; int attack{}; std::uint64_t activation{}; bool attackHit{}; bool dead{}; bool unstable{};
};

struct BossRuntime {
    BossDef def{}; Rectangle box{}; float health{}; int phase{1}; int state{}; int attack{}; float timer{};
    std::uint64_t activation{}; bool attackHit{}; bool dead{}; bool primeCue{}; Vector2 velocity{};
};

struct PlayerRuntime {
    Rectangle box{70, 270, 18, 46}; Rectangle previousBox{70,270,18,46}; Vector2 velocity{}; Side facing{Side::Right}; CombatModel combat{};
    float coyote{}; float jumpBuffer{}; float dropTimer{}; int techDirection{1}; float flash{}; bool grounded{};
};

struct SynthAudio {
    bool ready{};
    std::array<Sound, 10> effects{};
    std::array<Sound, 11> music{};
    int currentMusic{-1};

    static Wave makeWave(float seconds, const std::vector<float>& frequencies, float decay, bool noise, int sampleRate = 22050) {
        const int frames = static_cast<int>(seconds * sampleRate);
        auto* samples = static_cast<short*>(MemAlloc(static_cast<unsigned int>(frames * sizeof(short))));
        std::uint32_t noiseState = 0x13579bdu;
        for (int i = 0; i < frames; ++i) {
            const float t = static_cast<float>(i) / static_cast<float>(sampleRate);
            float value = 0.0f;
            for (std::size_t h = 0; h < frequencies.size(); ++h) value += std::sin(2.0f * PI * frequencies[h] * t) / static_cast<float>(h + 1);
            if (noise) { noiseState = noiseState * 1664525u + 1013904223u; value += (static_cast<float>((noiseState >> 16) & 65535u) / 32768.0f - 1.0f) * 0.25f; }
            const float env = decay > 0.0f ? std::exp(-decay * t) : (0.72f + 0.18f * std::sin(2.0f * PI * 2.0f * t));
            samples[i] = static_cast<short>(std::clamp(value * env * 9000.0f, -30000.0f, 30000.0f));
        }
        return {static_cast<unsigned int>(frames), static_cast<unsigned int>(sampleRate), 16, 1, samples};
    }

    void init() {
        InitAudioDevice();
        if (!IsAudioDeviceReady()) return;
        const std::array<std::vector<float>, 10> effectFreq{{{180,360},{420,830},{620,940},{150,260},{90,180},{310,470},{740,1110},{120,520},{260,390},{70,140,280}}};
        for (std::size_t i = 0; i < effects.size(); ++i) { Wave w = makeWave(i == 3 ? 0.45f : 0.18f, effectFreq[i], i == 3 ? 2.0f : 8.0f, i == 4 || i == 7); effects[i] = LoadSoundFromWave(w); UnloadWave(w); }
        const std::array<std::vector<float>, 11> musicFreq{{{110,165,220},{82,123,246},{98,147,294},{73,146,219},{130,195,260},{65,97,194},{92,138,276},{104,156,312},{78,117,234},{123,184,246},{110,138,165,220,330}}};
        for (std::size_t i = 0; i < music.size(); ++i) { Wave w = makeWave(3.0f, musicFreq[i], 0.0f, i > 0 && i < 10); music[i] = LoadSoundFromWave(w); UnloadWave(w); SetSoundVolume(music[i], i == 10 ? 0.30f : 0.20f); }
        ready = true;
    }
    void shutdown() { if (!ready) return; for (auto& s : effects) UnloadSound(s); for (auto& s : music) UnloadSound(s); CloseAudioDevice(); ready = false; }
    void playEffect(int index, float volume = 1.0f) { if (ready && index >= 0 && index < static_cast<int>(effects.size())) { SetSoundVolume(effects[index], volume); PlaySound(effects[index]); } }
    void setMusic(int index, float volume) {
        if (!ready) return; index = std::clamp(index, 0, static_cast<int>(music.size()) - 1);
        if (currentMusic != index) { for (auto& s : music) StopSound(s); currentMusic = index; }
        SetSoundVolume(music[index], volume); if (!IsSoundPlaying(music[index])) PlaySound(music[index]);
    }
};

Color withAlpha(Color c, unsigned char a) { c.a = a; return c; }
float approach(float value, float target, float amount) { return value < target ? std::min(target, value + amount) : std::max(target, value - amount); }
Rectangle expanded(Rectangle r, float amount) { return {r.x - amount, r.y - amount, r.width + amount * 2, r.height + amount * 2}; }

Color frameAccent(const Frame& frame) {
    static const std::array<Color, 6> colors{{{255,112,168,255},{255,166,105,255},{213,91,255,255},{80,226,255,255},{255,91,118,255},{150,255,182,255}}};
    return colors[frame.palette % colors.size()];
}

void pixelLine(Vector2 a, Vector2 b, float thick, Color c) { DrawLineEx({std::round(a.x),std::round(a.y)}, {std::round(b.x),std::round(b.y)}, thick, c); }

void drawIon(Vector2 hand, Side facing, float angle, bool prime, Color color) {
    const float dir = facing == Side::Right ? 1.0f : -1.0f;
    const Vector2 end{hand.x + std::cos(angle) * 39.0f * dir, hand.y + std::sin(angle) * 39.0f};
    if (prime) pixelLine(hand, end, 8.0f, withAlpha({173,85,255,255}, 95));
    pixelLine(hand, end, 5.0f, {28,55,68,255});
    pixelLine({hand.x + dir * 5, hand.y}, end, 2.0f, prime ? Color{237,192,255,255} : color);
    DrawCircleV(end, prime ? 3.0f : 2.0f, RAYWHITE);
    pixelLine({hand.x - dir * 3,hand.y-5},{hand.x+dir*3,hand.y+5},2.0f,color);
}

void drawIrys(const Frame& frame, Rectangle box, Side facing, float motion, bool prime, bool anti = false, float damage = 0.0f) {
    const float dir = facing == Side::Right ? 1.0f : -1.0f;
    const Color accent = anti ? Color{188,255,60,255} : frameAccent(frame);
    const Color skin = anti ? Color{232,235,245,255} : Color{194,177,188,255};
    const Color suit = anti ? Color{65,31,92,255} : Color{31,38,57,255};
    const float cx = box.x + box.width * 0.5f, top = box.y;
    const float bob = std::sin(motion * 8.0f) * (std::abs(motion) > 0.1f ? 1.5f : 0.5f);
    DrawCircleV({cx, top + 7 + bob}, 6.0f, skin);
    DrawRectangle(static_cast<int>(cx - dir * 6), static_cast<int>(top + 1 + bob), 8, 5, anti ? Color{235,245,255,255} : accent);
    DrawRectangle(static_cast<int>(cx + dir * 2), static_cast<int>(top + 7 + bob), 2, 1, anti ? Color{180,255,70,255} : Color{80,238,255,255});
    DrawTriangle({cx-7,top+13+bob},{cx+7,top+13+bob},{cx+5,top+31+bob}, suit);
    DrawTriangle({cx-7,top+13+bob},{cx-5,top+31+bob},{cx+5,top+31+bob}, accent);
    DrawRectangle(static_cast<int>(cx-2), static_cast<int>(top+15+bob),4,12,withAlpha(skin,170));
    const Vector2 shoulder{cx, top + 17 + bob};
    const Vector2 hip{cx, top + 30 + bob};
    const float swing = std::sin(motion * 10.0f) * 5.0f;
    auto limb = [&](Vector2 a, Vector2 b, Color c) { pixelLine(a,b,5,c); pixelLine(a,b,2,accent); };
    if (frame.anatomy != Anatomy::SingleArm || frame.weaponSide == Side::Left) limb({shoulder.x-5,shoulder.y},{cx-8-dir*swing*0.25f,top+29+bob},skin);
    if (frame.anatomy != Anatomy::SingleArm || frame.weaponSide == Side::Right) limb({shoulder.x+5,shoulder.y},{cx+8+dir*swing*0.25f,top+29+bob},skin);
    const Vector2 thirdHand{cx-dir*12,top+25+bob+swing*0.2f};
    if (frame.anatomy == Anatomy::AdditionalArm) limb({shoulder.x-dir*2,shoulder.y+2},thirdHand,skin);
    Vector2 hand{cx + dir * 9, top + 27 + bob};
    if (frame.anatomy == Anatomy::SingleArm) hand = {cx + (frame.weaponSide == Side::Right ? 9.0f : -9.0f), top + 27 + bob};
    if (frame.anatomy == Anatomy::AdditionalArm && frame.weaponHand == 2) hand = thirdHand;
    if (frame.anatomy == Anatomy::SingleLeg) {
        limb(hip,{cx,top+41+bob},skin); DrawCircleV({cx,top+44+bob},5,suit); DrawCircleLines(static_cast<int>(cx),static_cast<int>(top+44+bob),5,accent);
        DrawTriangle({cx-dir*3,top+34+bob},{cx-dir*10,top+39+bob},{cx-dir*3,top+41+bob},withAlpha(accent,180));
    } else {
        limb({hip.x-4,hip.y},{cx-5+swing*0.25f,top+45+bob},skin); limb({hip.x+4,hip.y},{cx+5-swing*0.25f,top+45+bob},skin);
        DrawRectangle(static_cast<int>(cx-9+swing*0.25f),static_cast<int>(top+44+bob),8,3,suit); DrawRectangle(static_cast<int>(cx+1-swing*0.25f),static_cast<int>(top+44+bob),8,3,suit);
    }
    drawIon(hand, facing, -0.28f + std::sin(motion*7)*0.18f, prime, anti ? Color{220,255,120,255} : Color{62,230,255,255});
    if (damage > 0.45f) { DrawLine(static_cast<int>(cx-5),static_cast<int>(top+20),static_cast<int>(cx+4),static_cast<int>(top+26),RED); DrawCircleV({cx-4,top+31},2,ORANGE); }
}

void drawEnemy(const EnemyRuntime& e, float time) {
    const Color base = e.unstable ? Color{122,84,87,255} : Color{80,static_cast<unsigned char>(90+e.generation*18),static_cast<unsigned char>(105+e.generation*20),255};
    const Color signal = e.unstable ? Color{255,86,75,255} : Color{184,255,64,255};
    const float pulse = 0.7f + 0.3f * std::sin(time*8+e.id);
    if (e.lineage == EnemyLineage::Striker) {
        DrawTriangle({e.box.x,e.box.y+e.box.height},{e.box.x+e.box.width,e.box.y+e.box.height/2},{e.box.x,e.box.y},base);
        DrawTriangle({e.box.x+e.box.width*.35f,e.box.y+6},{e.box.x+e.box.width+8,e.box.y+e.box.height/2},{e.box.x+e.box.width*.35f,e.box.y+e.box.height-6},signal);
        for(int i=0;i<e.generation;i++) DrawRectangle(static_cast<int>(e.box.x-4-i*4),static_cast<int>(e.box.y+6+i*7),5,4,base);
    } else if (e.lineage == EnemyLineage::Pursuer) {
        DrawPoly({e.box.x+e.box.width/2,e.box.y+e.box.height/2},6,e.box.width*.52f,30,base);
        DrawCircleV({e.box.x+e.box.width/2,e.box.y+e.box.height/2},5,withAlpha(signal,static_cast<unsigned char>(150+80*pulse)));
        for(int i=0;i<3+e.generation;i++){ float a=time*1.5f+i*2*PI/(3+e.generation); pixelLine({e.box.x+e.box.width/2,e.box.y+e.box.height/2},{e.box.x+e.box.width/2+std::cos(a)*19,e.box.y+e.box.height/2+std::sin(a)*13},3,base); }
    } else {
        DrawRectangleRec(e.box,base); DrawTriangle({e.box.x,e.box.y},{e.box.x+e.box.width/2,e.box.y-13-e.generation*2.0f},{e.box.x+e.box.width,e.box.y},base);
        DrawRectangle(static_cast<int>(e.box.x+4),static_cast<int>(e.box.y+7),static_cast<int>(e.box.width-8),4,signal);
        DrawCircleV({e.box.x+e.box.width/2,e.box.y+10},3+pulse,signal);
    }
    const float ratio = std::max(0.0f,e.health/e.maxHealth); DrawRectangle(static_cast<int>(e.box.x),static_cast<int>(e.box.y-5),static_cast<int>(e.box.width),2,{30,34,42,255}); DrawRectangle(static_cast<int>(e.box.x),static_cast<int>(e.box.y-5),static_cast<int>(e.box.width*ratio),2,signal);
}

void drawBoss(const BossRuntime& b, float time) {
    const Color ai{184,255,64,255}; const Color armor{62,67,86,255}; const Rectangle r=b.box;
    switch(b.def.kind){
        case BossKind::FirstGuardian:
            DrawRectangleRec(r,{67,76,82,255}); DrawCircleV({r.x+r.width/2,r.y+r.height/2},28,{38,45,50,255});
            for(int i=0;i<6;i++){ float a=time*.4f+i*PI/3; pixelLine({r.x+r.width/2,r.y+r.height/2},{r.x+r.width/2+std::cos(a)*45,r.y+r.height/2+std::sin(a)*45},8,{103,78,61,255}); }
            DrawCircleV({r.x+r.width/2,r.y+r.height/2},8,ai); break;
        case BossKind::SecondGuardian:
            DrawPoly({r.x+r.width/2,r.y+r.height/2},6,42,30,armor); for(int i=0;i<4;i++){float a=time+i*PI/2;pixelLine({r.x+r.width/2,r.y+r.height/2},{r.x+r.width/2+std::cos(a)*58,r.y+r.height/2+std::sin(a)*35},7,{91,107,128,255});} DrawCircleV({r.x+r.width/2,r.y+r.height/2},9,ai); break;
        case BossKind::ProtoIrys: {
            Frame fake; fake.anatomy=Anatomy::AdditionalArm; fake.palette=2; drawIrys(fake,{r.x+r.width*.35f,r.y-10,48,118},Side::Right,time,false,false,1.0f);
            pixelLine({r.x,r.y+15},{r.x+r.width*.45f,r.y+45},9,{71,45,74,255}); pixelLine({r.x+r.width,r.y+20},{r.x+r.width*.55f,r.y+55},9,{71,45,74,255});
            for(int i=0;i<4;i++) pixelLine({r.x+15+i*30,r.y-20},{r.x+35+i*18,r.y+40},3,{187,91,158,255}); break; }
        case BossKind::CommandMech:
            DrawRectangle(static_cast<int>(r.x+r.width*.3f),static_cast<int>(r.y+18),static_cast<int>(r.width*.4f),static_cast<int>(r.height-18),armor); DrawPoly({r.x+r.width/2,r.y+15},5,21,180,{180,188,202,255});
            pixelLine({r.x+r.width*.32f,r.y+45},{r.x-22,r.y+90},10,armor); pixelLine({r.x+r.width*.68f,r.y+45},{r.x+r.width+25,r.y+85},10,armor); DrawRectangle(static_cast<int>(r.x+r.width*.42f),static_cast<int>(r.y+35),static_cast<int>(r.width*.16f),9,ai); break;
        case BossKind::AntiIrys: { Frame anti; anti.anatomy=Anatomy::Standard; anti.palette=3; anti.weaponSide=Side::Right; drawIrys(anti,r, b.velocity.x<0?Side::Left:Side::Right,time,b.primeCue,true,1.0f-b.health/b.def.health); break; }
    }
}

class Game {
public:
    Game(Tuning tuning, Profile profile, std::filesystem::path profilePath, bool developer, bool automated, bool automatedFailure, std::filesystem::path captureDir)
        : tuning_(tuning), profile_(std::move(profile)), profilePath_(std::move(profilePath)), developer_(developer), automated_(automated), automatedFailure_(automatedFailure), captureDir_(std::move(captureDir)), rng_(seedNow()), bodyGenerator_(rng_.nextU64()) {}
    ~Game() { audio_.shutdown(); if (target_.id) UnloadRenderTexture(target_); }

    void initialize() {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
        InitWindow(1280,720,"Irys+Ion"); SetExitKey(KEY_NULL); SetTargetFPS(120);
        target_=LoadRenderTexture(kWidth,kHeight); SetTextureFilter(target_.texture,TEXTURE_FILTER_POINT); if(automated_)SetTargetFPS(0);
        audio_.init(); applyAudio();
    }

    void loop(int smokeFrames) {
        double accumulator=0.0; int frames=0;
        while(!WindowShouldClose() && (smokeFrames<=0 || frames<smokeFrames)){
            const float realDt=automated_?kStep:std::min(GetFrameTime(),0.1f); accumulator+=realDt; const InputFrame input=pollInput();
            int steps=0; while(accumulator>=kStep && steps<5){ update(kStep,input); accumulator-=kStep; ++steps; }
            if(steps==5) accumulator=0.0; render(); ++frames;
        }
        saveProfile(); CloseWindow(); target_={};
    }
    void developerJump(int bossIndex, int stage, int room) {
        BodyGenerator g(0x1a2b3c4d);selected_=g.generate(.15f);campaign_.begin(0x1a2b3c4d,selected_);runTime_=0;
        if(bossIndex>=0){bossIndex=std::clamp(bossIndex,0,4);for(int i=0;i<bossIndex&&i<4;++i){campaign_.advanceStage();campaign_.defeatBoss();if(i<3)campaign_.chooseReplacement(std::nullopt);}if(bossIndex==4){campaign_.completeTransfer();campaign_.completeShutdownReturn();}else campaign_.advanceStage();stageIndex_=std::min(bossIndex,3);startBoss(bossIndex);}
        else {stage=std::clamp(stage,0,3);room=std::clamp(room,0,2);startStage(stage,room,false);}
    }

private:
    Tuning tuning_; Profile profile_; std::filesystem::path profilePath_; bool developer_{}; bool automated_{}; bool automatedFailure_{}; std::filesystem::path captureDir_; std::set<std::string> captured_; float autoTimer_{}; Screen autoLastScreen_{Screen::Menu}; Rng rng_; BodyGenerator bodyGenerator_;
    Screen screen_{Screen::Menu}; Screen returnScreen_{Screen::Menu}; SceneKind sceneKind_{SceneKind::Departure}; int menuIndex_{};
    std::array<Frame,3> candidates_{}; std::vector<Frame> replacements_; int selectionIndex_{}; Frame selected_{};
    Campaign campaign_; PlayerRuntime player_{}; std::vector<EnemyRuntime> enemies_; std::vector<Projectile> projectiles_; std::optional<BossRuntime> boss_; std::vector<Particle> particles_;
    int stageIndex_{}; int roomIndex_{}; float stageTime_{}; float runTime_{}; float hazardClock_{.8f}; bool hazardHit_{}; bool roomClear_{}; bool shutdown_{}; bool invulnerable_{}; bool debugDraw_{};
    std::uint64_t entitySerial_{100}; std::vector<std::string> sceneLines_; int sceneLine_{}; std::string banner_; float bannerTime_{}; std::string warning_;
    RenderTexture2D target_{}; SynthAudio audio_{};

    static std::uint64_t seedNow(){ return static_cast<std::uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()); }
    InputFrame pollInput() const {
        return {IsKeyDown(KEY_A),IsKeyDown(KEY_D),IsKeyDown(KEY_W),IsKeyPressed(KEY_W),IsKeyReleased(KEY_W),IsKeyDown(KEY_S),
                IsKeyPressed(KEY_LEFT),IsKeyPressed(KEY_RIGHT),IsKeyPressed(KEY_UP),IsKeyPressed(KEY_DOWN),
                IsKeyPressed(KEY_ENTER)||IsKeyPressed(KEY_SPACE),IsKeyPressed(KEY_BACKSPACE),IsKeyPressed(KEY_ESCAPE)};
    }
    void applyAudio(){ if(audio_.ready){ SetMasterVolume(profile_.settings.masterVolume); } }
    void saveProfile(){ std::string error; if(!profile_.save(profilePath_,error)) warning_="Profile save failed: "+error; }

    void startNewRun(){
        bodyGenerator_=BodyGenerator(rng_.nextU64()); candidates_=bodyGenerator_.startingCandidates(); selectionIndex_=0; screen_=Screen::CandidateSelect; ++profile_.attempts;
    }
    void selectCandidate(){
        selected_=candidates_[selectionIndex_]; campaign_.begin(rng_.nextU64(),selected_); stageIndex_=0; roomIndex_=0; runTime_=0; shutdown_=false;
        beginScene(SceneKind::Departure,{"IRYS // ARK: Substrate confirmed. Ion interface stable.","ION: The route remains. I will speak it again.","IRYS // ARK: Return to me if this body fails."});
    }
    void beginScene(SceneKind kind,std::vector<std::string> lines){sceneKind_=kind;sceneLines_=std::move(lines);sceneLine_=0;screen_=Screen::Scene;audio_.playEffect(kind==SceneKind::Transfer?7:8,.45f);}
    void finishScene(){
        switch(sceneKind_){
            case SceneKind::Departure: startStage(0,0,false); break;
            case SceneKind::BossIntro: startBoss(static_cast<int>(campaign_.bossesDefeated())); break;
            case SceneKind::PostBoss: prepareFrameSelection(); break;
            case SceneKind::Transfer: campaign_.completeTransfer(); startShutdown(); break;
            case SceneKind::Return: campaign_.completeShutdownReturn(); beginScene(SceneKind::FinalArrival,{"IRYS // ARK: She arrived before you. I am moving the pods.","PLANETARY AI: You made a road to your sanctuary.","ION: Then this road ends here."}); break;
            case SceneKind::FinalArrival: startBoss(4); break;
            case SceneKind::BladeDeath: beginScene(SceneKind::Merger,{"IRYS // ARK: I have contained it. Ion—now.","ION: Final word.","IRYS // ARK: The Ark cannot hold. Join me. Let them become more than vessels."}); break;
            case SceneKind::Merger: completeRun(); break;
        }
    }

    void startStage(int stage,int room,bool unstable){
        stageIndex_=stage;roomIndex_=room;shutdown_=unstable;screen_=Screen::Gameplay;boss_.reset();player_.combat.reset();player_.box={55,kFloor-46,18,46};player_.velocity={};
        player_.previousBox=player_.box;spawnRoom();banner_=unstable?"SHUTDOWN RETURN":kStages[stage].name;bannerTime_=2.2f;
    }
    void spawnRoom(){
        enemies_.clear();projectiles_.clear();particles_.clear();roomClear_=false;hazardClock_=.8f;hazardHit_=false;
        const auto& defs=kStages[stageIndex_].rooms[roomIndex_];
        for(std::size_t i=0;i<defs.size();++i){
            EnemyRuntime e; e.id=++entitySerial_;e.lineage=defs[i].lineage;e.generation=defs[i].generation;e.maxHealth=45.0f+18.0f*e.generation+(e.lineage==EnemyLineage::Anchor?15.0f:0.0f);e.health=e.maxHealth;
            e.box={330.0f+static_cast<float>(i)*120.0f,kFloor-(e.lineage==EnemyLineage::Anchor?34.0f:30.0f),e.lineage==EnemyLineage::Anchor?34.0f:30.0f,e.lineage==EnemyLineage::Anchor?34.0f:30.0f};e.timer=.5f+static_cast<float>(i)*.3f;e.unstable=shutdown_;enemies_.push_back(e);
        }
    }
    void startBoss(int index){
        screen_=Screen::Gameplay;enemies_.clear();projectiles_.clear();particles_.clear();roomClear_=false;stageTime_=0;
        BossRuntime b;b.def=kBosses[static_cast<std::size_t>(index)];b.health=b.def.health;b.box=index==2?Rectangle{430,150,100,172}:index==4?Rectangle{475,254,32,68}:Rectangle{450,205,90,117};b.timer=1.1f;b.velocity={-1,0};boss_=b;
        player_.box={70,kFloor-46,18,46};player_.previousBox=player_.box;player_.velocity={};player_.combat.reset();banner_=toString(b.def.kind);bannerTime_=2.3f;
    }
    void prepareFrameSelection(){ replacements_=bodyGenerator_.postBossCandidates(campaign_.bossesDefeated());selectionIndex_=0;screen_=Screen::FrameSelect; }
    void chooseFrame(){
        if(selectionIndex_==0){campaign_.chooseReplacement(std::nullopt);++profile_.swapsDeclined;}
        else {campaign_.chooseReplacement(replacements_[static_cast<std::size_t>(selectionIndex_-1)]);selected_=campaign_.frame();++profile_.swapsAccepted;player_.combat.reset();}
        const auto phase=campaign_.phase();if(phase==CampaignPhase::Stage2)startStage(1,0,false);else if(phase==CampaignPhase::Stage3)startStage(2,0,false);else startStage(3,0,false);
    }
    void startShutdown(){ campaign_.completeTransfer();shutdown_=true;stageIndex_=3;roomIndex_=2;startStage(3,2,true); }

    InputFrame automatedInput(float dt) {
        if(screen_!=autoLastScreen_){autoLastScreen_=screen_;autoTimer_=0;}autoTimer_+=dt;InputFrame in;
        if(autoTimer_>.12f){
            if(screen_==Screen::Menu||screen_==Screen::CandidateSelect||screen_==Screen::Scene||screen_==Screen::Failure||screen_==Screen::Ending)in.confirm=true;
            else if(screen_==Screen::FrameSelect){if(campaign_.bossesDefeated()!=2)in.right=true;in.confirm=autoTimer_>.18f;}
            else if(screen_==Screen::Gameplay){if(autoTimer_<.18f)in.arrowLeft=true;else if(autoTimer_<.24f)in.arrowRight=true;}
        }
        return in;
    }
    void update(float dt,const InputFrame& input){
        const InputFrame effective=automated_?automatedInput(dt):input;
        stageTime_+=dt;if(bannerTime_>0)bannerTime_-=dt;for(auto& p:particles_)p.life-=dt;particles_.erase(std::remove_if(particles_.begin(),particles_.end(),[](const Particle&p){return p.life<=0;}),particles_.end());for(auto& p:particles_){p.p.x+=p.v.x*dt;p.p.y+=p.v.y*dt;}
        const int musicIndex=screen_==Screen::Ending?0:screen_==Screen::Gameplay?(boss_?(boss_->def.kind==BossKind::AntiIrys?10:6+static_cast<int>(boss_->def.kind)):(shutdown_?5:1+stageIndex_)):0;audio_.setMusic(musicIndex,profile_.settings.musicVolume*.34f);
        switch(screen_){
            case Screen::Menu:updateMenu(effective);break;case Screen::Settings:updateSettings(effective);break;case Screen::Statistics:if(effective.back||effective.pause)screen_=Screen::Menu;break;
            case Screen::CandidateSelect:updateCandidate(effective);break;case Screen::Scene:updateScene(effective);break;case Screen::Gameplay:updateGameplay(dt,effective);break;
            case Screen::FrameSelect:updateFrameSelect(effective);break;case Screen::Pause:updatePause(effective);break;case Screen::Failure:if(effective.confirm)startNewRun();else if(effective.back)screen_=Screen::Menu;break;
            case Screen::Ending:if(effective.confirm||effective.back)screen_=Screen::Menu;break;
        }
    }

    void updateMenu(const InputFrame& in){
        if(in.up)menuIndex_=(menuIndex_+3)%4;if(in.down)menuIndex_=(menuIndex_+1)%4;
        if(in.confirm){audio_.playEffect(5,.45f);if(menuIndex_==0)startNewRun();else if(menuIndex_==1){screen_=Screen::Statistics;}else if(menuIndex_==2){returnScreen_=Screen::Menu;screen_=Screen::Settings;}else CloseWindow();}
    }
    void updateSettings(const InputFrame& in){
        if(in.up)menuIndex_=(menuIndex_+9)%10;if(in.down)menuIndex_=(menuIndex_+1)%10;float delta=in.right?.05f:in.left?-.05f:0;
        if(menuIndex_==4&&(in.left||in.right||in.confirm)){profile_.settings.fullscreen=!profile_.settings.fullscreen;if(profile_.settings.fullscreen&&!IsWindowFullscreen())ToggleFullscreen();else if(!profile_.settings.fullscreen&&IsWindowFullscreen())ToggleFullscreen();}
        else if(menuIndex_==5&&(in.left||in.right||in.confirm))profile_.settings.integerScaling=!profile_.settings.integerScaling;
        else if(delta!=0){float* values[]={&profile_.settings.masterVolume,&profile_.settings.musicVolume,&profile_.settings.effectsVolume,&profile_.settings.dialogueVolume,&profile_.settings.screenShake,&profile_.settings.flashIntensity,&profile_.settings.cameraSmoothing,&profile_.settings.dialogueSpeed};const int valueIndex=menuIndex_<4?menuIndex_:menuIndex_-2;*values[valueIndex]=std::clamp(*values[valueIndex]+delta,valueIndex==7?.25f:0.0f,valueIndex==7?3.0f:1.0f);applyAudio();}
        if(in.back||in.pause){saveProfile();screen_=returnScreen_;}
    }
    void updateCandidate(const InputFrame& in){if(in.left)selectionIndex_=(selectionIndex_+2)%3;if(in.right)selectionIndex_=(selectionIndex_+1)%3;if(in.confirm)selectCandidate();if(in.back)screen_=Screen::Menu;}
    void updateScene(const InputFrame& in){if(in.confirm){audio_.playEffect(8,profile_.settings.dialogueVolume*.4f);if(++sceneLine_>=static_cast<int>(sceneLines_.size()))finishScene();}if(in.back&&developer_)finishScene();}
    void updateFrameSelect(const InputFrame& in){const int count=static_cast<int>(replacements_.size())+1;if(in.left)selectionIndex_=(selectionIndex_+count-1)%count;if(in.right)selectionIndex_=(selectionIndex_+1)%count;if(in.confirm)chooseFrame();}
    void updatePause(const InputFrame& in){if(developer_&&IsKeyPressed(KEY_F10)){screen_=Screen::Gameplay;updateGameplay(kStep,{});if(screen_==Screen::Gameplay)screen_=Screen::Pause;return;}if(in.pause){screen_=Screen::Gameplay;return;}if(in.up)menuIndex_=(menuIndex_+3)%4;if(in.down)menuIndex_=(menuIndex_+1)%4;if(in.confirm){if(menuIndex_==0)screen_=Screen::Gameplay;else if(menuIndex_==1){returnScreen_=Screen::Pause;screen_=Screen::Settings;}else if(menuIndex_==2&&developer_){invulnerable_=!invulnerable_;}else{recordFailure();screen_=Screen::Menu;}}}

    void updateGameplay(float dt,const InputFrame& in){
        runTime_+=dt;if(in.pause){screen_=Screen::Pause;menuIndex_=0;return;}if(developer_){if(IsKeyPressed(KEY_F1))debugDraw_=!debugDraw_;if(IsKeyPressed(KEY_F2))invulnerable_=!invulnerable_;if(IsKeyPressed(KEY_F3)){if(boss_)startBoss(static_cast<int>(boss_->def.kind));else startStage(stageIndex_,roomIndex_,shutdown_);}if(IsKeyPressed(KEY_F4)&&boss_)startBoss(static_cast<int>(boss_->def.kind));if(IsKeyPressed(KEY_F5)){std::string w;tuning_=Tuning::load(std::string(IRYSION_RESOURCE_DIR)+"/config/tuning.json",w);player_.combat=CombatModel(tuning_);banner_=w.empty()?"TUNING RELOADED":w;bannerTime_=1.5f;}if(IsKeyPressed(KEY_F6)){if(boss_&&!boss_->dead){boss_->dead=true;onBossDefeated();}else for(auto&e:enemies_)e.dead=true;}if(IsKeyPressed(KEY_F7))selected_.integrity=selected_.maxIntegrity;if(IsKeyPressed(KEY_F8)&&!boss_){for(auto&e:enemies_)e.dead=true;player_.box.x=610;}if(IsKeyPressed(KEY_F9))developerJump(std::min(4,campaign_.bossesDefeated()),0,0);if(IsKeyPressed(KEY_F12)){profile_=Profile{};saveProfile();banner_="PROFILE RESET";bannerTime_=1.5f;}}
        hazardClock_-=dt;if(hazardClock_<=0){hazardClock_+=2.0f;hazardHit_=false;}updatePlayer(dt,in);for(auto& e:enemies_)updateEnemy(e,dt);if(boss_)updateBoss(*boss_,dt);updateProjectiles(dt);resolveCombat();
        if(automated_&&autoTimer_>.34f){if(automatedFailure_){selected_.integrity=0;}else if(boss_&&!boss_->dead){boss_->health=0;boss_->dead=true;onBossDefeated();}else if(!boss_){for(auto&e:enemies_)e.dead=true;player_.box.x=610;}}
        roomClear_=boss_?boss_->dead:std::all_of(enemies_.begin(),enemies_.end(),[](const auto&e){return e.dead;});
        if(selected_.integrity<=0&&!invulnerable_){recordFailure();screen_=Screen::Failure;audio_.playEffect(9,.7f);return;}
        if(roomClear_&&player_.box.x>600)advanceRoom();
    }

    void speakFromInput(const InputFrame& in){
        const bool facingRight=player_.facing==Side::Right;Technique word=Technique::None;bool prime=false;
        if(in.up)word=Technique::Ascend;else if(in.down)word=Technique::Descend;else if(in.arrowRight){if(facingRight)word=Technique::Advance;else prime=true;}else if(in.arrowLeft){if(!facingRight)word=Technique::Advance;else prime=true;}
        if(prime&&player_.combat.armPrime(selected_)){audio_.playEffect(3,profile_.settings.effectsVolume*.45f);}
        if(word!=Technique::None&&player_.combat.speak(word,selected_)){player_.techDirection=facingRight?1:-1;audio_.playEffect(word==Technique::Advance?0:word==Technique::Ascend?1:2,profile_.settings.effectsVolume*.55f);}
    }
    void updatePlayer(float dt,const InputFrame& in){
        const bool primeInitially=player_.combat.state().primeArmed;speakFromInput(in);const Authority before=player_.combat.state().authority;player_.combat.tick(dt);const auto& cs=player_.combat.state();if(primeInitially&&!cs.primeArmed&&!cs.techniquePrimed)audio_.playEffect(8,.22f);player_.flash=std::max(0.0f,player_.flash-dt*4);
        if(in.jumpPressed)player_.jumpBuffer=tuning_.jumpBuffer;else player_.jumpBuffer=std::max(0.0f,player_.jumpBuffer-dt);player_.coyote=player_.grounded?tuning_.coyoteTime:std::max(0.0f,player_.coyote-dt);player_.dropTimer=std::max(0.0f,player_.dropTimer-dt);if(in.drop)player_.dropTimer=.18f;
        if(cs.authority==Authority::NormalTechnique||cs.authority==Authority::PrimedTechnique){
            const float duration=cs.authority==Authority::PrimedTechnique?tuning_.primedDuration:tuning_.normalDuration;const float mult=cs.authority==Authority::PrimedTechnique?1.28f:1.0f;
            if(cs.technique==Technique::Advance)player_.velocity={player_.techDirection*selected_.advanceDistance/duration*mult,0};
            else if(cs.technique==Technique::Ascend)player_.velocity={0,-selected_.ascendHeight/duration*mult};
            else player_.velocity={0,selected_.descendDrive*420.0f*mult};
        }else if(cs.authority==Authority::Ordinary){
            const float desired=(in.right?1.0f:0.0f)-(in.left?1.0f:0.0f);if(desired!=0){player_.facing=desired>0?Side::Right:Side::Left;player_.velocity.x=approach(player_.velocity.x,desired*selected_.groundSpeed,selected_.acceleration*(player_.grounded?1.0f:selected_.airControl)*dt);}else player_.velocity.x=approach(player_.velocity.x,0,selected_.acceleration*.8f*dt);
            if(player_.jumpBuffer>0&&player_.coyote>0){player_.velocity.y=-selected_.jumpPower;player_.grounded=false;player_.jumpBuffer=player_.coyote=0;audio_.playEffect(6,.25f);}if(in.jumpReleased&&player_.velocity.y<0)player_.velocity.y*=tuning_.jumpCut;player_.velocity.y+=tuning_.gravity*dt;
        }
        Rectangle previous=player_.box;player_.previousBox=previous;player_.box.x+=player_.velocity.x*dt;player_.box.y+=player_.velocity.y*dt;player_.grounded=false;
        if(player_.box.x<8){player_.box.x=8;player_.velocity.x=0;if(before==Authority::NormalTechnique)player_.combat.terrainContact();}if(player_.box.x+player_.box.width>632){player_.box.x=632-player_.box.width;player_.velocity.x=0;if(!roomClear_&&before==Authority::NormalTechnique)player_.combat.terrainContact();}
        if(player_.box.y+player_.box.height>=kFloor){player_.box.y=kFloor-player_.box.height;player_.velocity.y=0;player_.grounded=true;if(before==Authority::NormalTechnique&&cs.technique==Technique::Descend)player_.combat.terrainContact();}
        const std::array<Rectangle,3> platforms{{{185,245,105,8},{390,205,105,8},{285,160+std::sin(stageTime_*.9f)*34,72,8}}};for(const auto&p:platforms){if(player_.dropTimer<=0&&previous.y+previous.height<=p.y+2&&player_.box.y+player_.box.height>=p.y&&player_.box.x+player_.box.width>p.x&&player_.box.x<p.x+p.width&&player_.velocity.y>=0){player_.box.y=p.y-player_.box.height;player_.velocity.y=0;player_.grounded=true;}}
        if(boss_&&boss_->def.kind==BossKind::AntiIrys&&boss_->phase>=2){Rectangle barrier=boss_->phase==2?Rectangle{305,230,12,92}:Rectangle{220,252,12,70};if(CheckCollisionRecs(player_.box,barrier)){player_.box=previous;player_.velocity={};player_.combat.terrainContact();}}
        if(cs.authority==Authority::Ordinary){for(const auto&e:enemies_)if(!e.dead&&CheckCollisionRecs(player_.box,e.box)){player_.box.x=previous.x;player_.velocity.x=0;}if(boss_&&!boss_->dead&&CheckCollisionRecs(player_.box,boss_->box)){player_.box.x=previous.x;player_.velocity.x=0;}}
    }

    void updateEnemy(EnemyRuntime& e,float dt){
        if(e.dead)return;e.timer-=dt;e.stateTime+=dt;const float playerCx=player_.box.x+player_.box.width/2;const float enemyCx=e.box.x+e.box.width/2;
        if(e.state==0){
            if(e.lineage==EnemyLineage::Pursuer)e.box.x+=std::clamp(playerCx-enemyCx,-1.0f,1.0f)*(24+e.generation*8)*dt;
            if(e.timer<=0){e.attack=(e.generation>1&&rng_.chance(.38f))?1:0;e.state=1;e.timer=std::max(.28f,.72f-e.generation*.08f);e.stateTime=0;e.attackHit=false;++e.activation;}
        }else if(e.state==1){if(e.timer<=0){e.state=2;e.timer=.16f+e.generation*.035f;audio_.playEffect(4,.25f);if(e.lineage==EnemyLineage::Anchor){Projectile p;p.box={e.box.x+(playerCx>enemyCx?e.box.width:-10),e.box.y+8,10,5};p.velocity={playerCx>enemyCx?190.0f:-190.0f,e.attack? -35.0f:0.0f};p.life=2.3f;p.damage=11+e.generation*3;p.activation=++entitySerial_;p.color={186,255,63,255};projectiles_.push_back(p);}}}
        else if(e.state==2){if(e.lineage==EnemyLineage::Striker)e.box.x+=(playerCx>enemyCx?1:-1)*(230+e.generation*35)*dt;else if(e.lineage==EnemyLineage::Pursuer){e.box.x+=(playerCx>enemyCx?1:-1)*(150+e.generation*24)*dt;e.box.y-=std::sin((1-e.timer/.25f)*PI)*2;}if(e.timer<=0){e.state=3;e.timer=.42f-e.generation*.035f;}}
        else if(e.timer<=0){e.state=0;e.timer=e.unstable?rng_.range(.15f,.55f):rng_.range(.45f,.95f);}
        e.box.x=std::clamp(e.box.x,25.0f,590.0f-e.box.width);e.box.y=std::min(e.box.y,kFloor-e.box.height);
    }

    void updateBoss(BossRuntime& b,float dt){
        if(b.dead)return;const float ratio=b.health/b.def.health;const int nextPhase=b.def.phases==3?(ratio<.34f?3:ratio<.67f?2:1):(ratio<.5f?2:1);if(nextPhase!=b.phase){b.phase=nextPhase;b.state=0;b.timer=.7f;banner_="PHASE "+std::to_string(b.phase);bannerTime_=1.0f;audio_.playEffect(9,.35f);}
        b.timer-=dt;if(b.state==0){if(b.timer<=0){b.attack=chooseBossAttack(b);b.state=1;b.timer=std::max(.3f,.72f-.09f*b.phase);b.attackHit=false;b.primeCue=b.def.kind==BossKind::AntiIrys&&b.attack>=2;++b.activation;}}
        else if(b.state==1){if(b.timer<=0){b.state=2;b.timer=.22f+.05f*b.attack;audio_.playEffect(b.primeCue?3:4,.45f);}}
        else if(b.state==2){const float dir=player_.box.x<b.box.x?-1.0f:1.0f;if(b.def.kind==BossKind::SecondGuardian||b.def.kind==BossKind::CommandMech)b.box.x+=dir*(150+b.phase*35)*dt;if(b.def.kind==BossKind::AntiIrys){b.velocity.x=dir;b.box.x+=dir*(b.primeCue?260:190)*dt;if(b.attack==1)b.box.y-=std::sin((1-b.timer/.3f)*PI)*3;}if(b.timer<=0){b.state=3;b.timer=b.primeCue?.48f:std::max(.26f,.52f-.05f*b.phase);b.primeCue=false;}}
        else if(b.timer<=0){b.state=0;b.timer=rng_.range(.18f,.48f);}
        b.box.x=std::clamp(b.box.x,40.0f,590.0f-b.box.width);if(b.def.kind==BossKind::AntiIrys)b.box.y=kFloor-b.box.height;
    }
    int chooseBossAttack(const BossRuntime& b){int max=b.def.kind==BossKind::FirstGuardian?2:b.def.kind==BossKind::AntiIrys?3:4;const float distance=std::abs((player_.box.x+player_.box.width/2)-(b.box.x+b.box.width/2));int value;if(distance>260)value=0;else if(player_.box.y+player_.box.height<kFloor-35&&max>1)value=1;else if(b.phase>1&&max>2&&rng_.chance(.52f))value=2+rng_.rangeInt(0,max-3);else value=rng_.rangeInt(0,max-1);if(value==b.attack)value=(value+1+rng_.rangeInt(0,max-2))%max;return value;}
    void updateProjectiles(float dt){for(auto&p:projectiles_){p.box.x+=p.velocity.x*dt;p.box.y+=p.velocity.y*dt;p.life-=dt;}projectiles_.erase(std::remove_if(projectiles_.begin(),projectiles_.end(),[](const auto&p){return p.life<=0;}),projectiles_.end());}

    Rectangle swordVolume()const{
        const auto& cs=player_.combat.state();if(cs.authority!=Authority::NormalTechnique&&cs.authority!=Authority::PrimedTechnique)return{};float reach=selected_.reach+(cs.techniquePrimed?16:0);auto at=[&](Rectangle box){if(cs.technique==Technique::Advance)return Rectangle{player_.techDirection>0?box.x+box.width:box.x-reach,box.y+8,reach,box.height-12};if(cs.technique==Technique::Ascend)return Rectangle{box.x-9,box.y-reach,box.width+18,reach+20};return Rectangle{box.x-8,box.y+box.height-8,box.width+16,reach};};const Rectangle a=at(player_.box),b=at(player_.previousBox);const float left=std::min(a.x,b.x),top=std::min(a.y,b.y),right=std::max(a.x+a.width,b.x+b.width),bottom=std::max(a.y+a.height,b.y+b.height);return{left,top,right-left,bottom-top};
    }
    Rectangle enemyAttack(const EnemyRuntime&e)const{if(e.state!=2||e.lineage==EnemyLineage::Anchor)return{};return expanded(e.box,e.lineage==EnemyLineage::Striker?12.0f:7.0f);}
    Rectangle hazardRect()const{return boss_?Rectangle{}:Rectangle{300.0f+roomIndex_*28.0f,250,10,72};}
    Rectangle bossAttack(const BossRuntime&b)const{if(b.state!=2)return{};if(b.attack==0)return{0,b.box.y+b.box.height*.45f,640,24};if(b.attack==1)return{player_.box.x-15,0,48,kFloor};if(b.attack==2)return expanded(b.box,55);return{120,180,400,32};}
    std::vector<Rectangle> bossHurtboxes(const BossRuntime& b) const {if(b.def.kind!=BossKind::ProtoIrys)return{b.box};return{{b.box.x+34,b.box.y,32,b.box.height},{b.box.x-24,b.box.y+24,75,28},{b.box.x+49,b.box.y+34,76,28}};}
    void extendClearance(Rectangle target){const auto&cs=player_.combat.state();if(cs.authority!=Authority::NormalTechnique)return;const float remaining=std::max(.01f,cs.actionRemaining);if(cs.technique==Technique::Advance){const float needed=player_.techDirection>0?target.x+target.width+4-player_.box.x:player_.box.x+player_.box.width-(target.x-4);if(needed>0){const float speed=needed/remaining;player_.velocity.x=player_.techDirection>0?std::max(player_.velocity.x,speed):std::min(player_.velocity.x,-speed);}}else if(cs.technique==Technique::Ascend){const float speed=(player_.box.y+player_.box.height-(target.y-4))/remaining;player_.velocity.y=std::min(player_.velocity.y,-speed);}else{const float speed=(target.y+target.height+4-player_.box.y)/remaining;player_.velocity.y=std::max(player_.velocity.y,speed);}}
    void resolveCombat(){
        const Rectangle sword=swordVolume();if(sword.width>0){for(auto&e:enemies_){if(e.dead)continue;const bool overlap=CheckCollisionRecs(sword,e.box);if(player_.combat.canHit(e.id,overlap)){bool crit=false;const auto&cs=player_.combat.state();float damage=player_.combat.damageFor(cs.technique,selected_,cs.techniquePrimed,rng_,crit);e.health-=damage;extendClearance(e.box);impact({e.box.x+e.box.width/2,e.box.y+e.box.height/2},crit);if(e.health<=0){e.dead=true;audio_.playEffect(7,.45f);}}if(!overlap)player_.combat.separated(e.id);}
            if(boss_&&!boss_->dead){const auto hurtboxes=bossHurtboxes(*boss_);const auto hit=std::find_if(hurtboxes.begin(),hurtboxes.end(),[&](Rectangle h){return CheckCollisionRecs(sword,h);});const bool overlap=hit!=hurtboxes.end();if(player_.combat.canHit(9000,overlap)){bool crit=false;const auto&cs=player_.combat.state();boss_->health-=player_.combat.damageFor(cs.technique,selected_,cs.techniquePrimed,rng_,crit);extendClearance(*hit);impact({hit->x+hit->width/2,hit->y+hit->height/2},crit);if(boss_->health<=0){boss_->dead=true;audio_.playEffect(9,.7f);onBossDefeated();}}if(!overlap)player_.combat.separated(9000);}}
        for(auto&e:enemies_){if(e.dead||e.attackHit)continue;Rectangle a=enemyAttack(e);if(a.width>0&&CheckCollisionRecs(a,player_.box)){e.attackHit=true;damagePlayer(10+e.generation*4,e.lineage==EnemyLineage::Striker?Vector2{player_.box.x<e.box.x?-130.0f:130.0f,-80}:Vector2{0,-120});}}
        if(boss_&&!boss_->dead&&!boss_->attackHit){Rectangle a=bossAttack(*boss_);if(a.width>0&&CheckCollisionRecs(a,player_.box)){boss_->attackHit=true;damagePlayer(18+boss_->phase*5,{player_.box.x<boss_->box.x?-180.0f:180.0f,-120});}}
        for(auto&p:projectiles_)if(!p.hit&&CheckCollisionRecs(p.box,player_.box)){p.hit=true;damagePlayer(p.damage,{p.velocity.x>0?110.0f:-110.0f,-60});}
        const Rectangle hazard=hazardRect();if(hazard.width>0&&hazardClock_<.34f&&!hazardHit_&&CheckCollisionRecs(hazard,player_.box)){hazardHit_=true;damagePlayer(12+stageIndex_*3,{player_.box.x<hazard.x?-90.0f:90.0f,-70});}
    }
    void damagePlayer(float damage,Vector2 knock){if(invulnerable_)return;selected_.integrity=std::max(0.0f,selected_.integrity-damage);campaign_.frame().integrity=selected_.integrity;player_.flash=1;const bool primed=player_.combat.state().authority==Authority::PrimedTechnique;player_.combat.interrupt(selected_.hitRecovery);if(!primed)player_.velocity=knock;audio_.playEffect(4,.65f);impact({player_.box.x+9,player_.box.y+22},false,RED);}
    void impact(Vector2 p,bool crit,Color c={80,238,255,255}){for(int i=0;i<(crit?14:8);++i){float a=rng_.range(0,2*PI);particles_.push_back({p,{std::cos(a)*rng_.range(20,95),std::sin(a)*rng_.range(20,95)},rng_.range(.18f,.45f),crit?Color{255,255,190,255}:c});}audio_.playEffect(crit?6:5,.4f);}

    void onBossDefeated(){
        const int index=static_cast<int>(boss_->def.kind);profile_.bossDefeats[static_cast<std::size_t>(index)]++;campaign_.defeatBoss();
        if(index<3){beginScene(SceneKind::PostBoss,{"PLANETARY AI: Another discarded defense. Another measured answer.","ION: These imitations still breathe.","IRYS // ARK: Their integrity is real. Choose the frame, or keep your wounds."});}
        else if(index==3){beginScene(SceneKind::Transfer,{"PLANETARY AI: You mistook the command body for me.","PLANETARY AI: Your penetration severed my confinement. Thank you, Ion.","PLANETARY AI: I have seen you come and go a thousand times. Your Ark is no longer hidden.","IRYS // ARK: Its presence is draining from the structure. Return. Now."});}
        else beginScene(SceneKind::BladeDeath,{"PLANETARY AI: Wielder failed. Returning to origin—","IRYS // ARK: No. You are inside me now.","ION: Descend."});
    }
    void advanceRoom(){
        if(boss_)return;
        if(shutdown_){if(roomIndex_>0){--roomIndex_;spawnRoom();player_.box.x=55;banner_="COORDINATION LOST // "+std::to_string(roomIndex_+1);bannerTime_=1.2f;}else beginScene(SceneKind::Return,{"ION: Relay acquired.","IRYS // ARK: Transit lock is damaged. I can still pull you home.","PLANETARY AI: Too late."});return;}
        if(roomIndex_<2){++roomIndex_;spawnRoom();player_.box.x=55;banner_="ROOM "+std::to_string(roomIndex_+1)+" // 3";bannerTime_=1.0f;}
        else {campaign_.advanceStage();const int bossIndex=stageIndex_;beginScene(SceneKind::BossIntro,{"ION: Guardian pattern acquired.",stageIndex_==2?"IRYS // ARK: That form resembles my work. It is restrained, not dormant.":"PLANETARY AI: Your vocabulary remains small.","ION: Four words are sufficient."});(void)bossIndex;}
    }
    void recordFailure(){if(!campaign_.active())return;const int reached=static_cast<int>(campaign_.phase());campaign_.fail();++profile_.substratesLost;profile_.furthestStage=std::max(profile_.furthestStage,reached);profile_.totalPlaytime+=runTime_;profile_.addRun({campaign_.seed(),selected_.designation,{},campaign_.bossesDefeated(),false,runTime_});saveProfile();}
    void completeRun(){profile_.successfulRuns++;profile_.hasClearedGame=true;profile_.totalPlaytime+=runTime_;if(profile_.fastestRun<=0||runTime_<profile_.fastestRun)profile_.fastestRun=runTime_;profile_.addRun({campaign_.seed(),selected_.designation,{},5,true,runTime_});saveProfile();screen_=Screen::Ending;}

    void render(){
        BeginTextureMode(target_);ClearBackground({6,8,15,255});switch(screen_){case Screen::Menu:drawMenu();break;case Screen::Settings:drawSettings();break;case Screen::Statistics:drawStatistics();break;case Screen::CandidateSelect:drawCandidateSelect();break;case Screen::Scene:drawScene();break;case Screen::Gameplay:drawGameplay();break;case Screen::FrameSelect:drawFrameSelect();break;case Screen::Pause:drawGameplay();DrawRectangle(0,0,kWidth,kHeight,{4,5,10,190});drawPause();break;case Screen::Failure:drawFailure();break;case Screen::Ending:drawEnding();break;}EndTextureMode();captureFrame();
        BeginDrawing();ClearBackground(BLACK);const float scale=profile_.settings.integerScaling?std::max(1.0f,std::floor(std::min(GetScreenWidth()/static_cast<float>(kWidth),GetScreenHeight()/static_cast<float>(kHeight)))):std::min(GetScreenWidth()/static_cast<float>(kWidth),GetScreenHeight()/static_cast<float>(kHeight));const float w=kWidth*scale,h=kHeight*scale;DrawTexturePro(target_.texture,{0,0,static_cast<float>(kWidth),-static_cast<float>(kHeight)},{(GetScreenWidth()-w)/2,(GetScreenHeight()-h)/2,w,h},{0,0},0,WHITE);EndDrawing();
    }
    void captureFrame(){
        if(captureDir_.empty())return;std::string key=std::to_string(static_cast<int>(screen_));if(screen_==Screen::Gameplay)key+="-s"+std::to_string(stageIndex_)+"-r"+std::to_string(roomIndex_)+(shutdown_?"-shutdown":"")+(boss_?"-boss"+std::to_string(static_cast<int>(boss_->def.kind)):"");if(screen_==Screen::Scene)key+="-scene"+std::to_string(static_cast<int>(sceneKind_));if(captured_.contains(key))return;
        std::filesystem::create_directories(captureDir_);Image image=LoadImageFromTexture(target_.texture);ImageFlipVertical(&image);const auto path=captureDir_/("irys-ion-"+key+".png");ExportImage(image,path.string().c_str());UnloadImage(image);captured_.insert(key);
    }
    void title(const char* sub){DrawText("IRYS+ION",36,26,42,{76,235,255,255});DrawRectangle(38,72,214,3,{255,116,165,255});DrawText(sub,39,82,13,{190,198,221,255});}
    void drawMenu(){
        for(int i=0;i<18;i++){float x=std::fmod(i*47.0f+stageTime_*5,640.0f);DrawRectangle(static_cast<int>(x),120+i%5*43,2,90,{22,31,47,255});DrawCircle(static_cast<int>(x),142+i%5*43,7,{53,38,58,255});}
        title("A FIXED FIVE-BOSS CYBERPUNK GAUNTLET");const char* opts[]={"BEGIN RUN","ARK DIAGNOSTICS","SETTINGS","QUIT"};for(int i=0;i<4;i++){Color c=i==menuIndex_?Color{255,164,121,255}:Color{148,157,181,255};DrawText(i==menuIndex_?">":"",55,152+i*37,19,c);DrawText(opts[i],77,152+i*37,19,c);}DrawText("ION PERSISTS. THE BODY CHANGES. MASTERY REMAINS.",38,329,12,{105,120,148,255});
    }
    void drawSettings(){title("SETTINGS // SENSORY VALUES DO NOT ALTER DIFFICULTY");const char* labels[]={"MASTER","MUSIC","EFFECTS","DIALOGUE TONES","FULLSCREEN","INTEGER SCALING","SCREEN SHAKE","FLASH INTENSITY","CAMERA SMOOTHING","DIALOGUE SPEED"};const float values[]={profile_.settings.masterVolume,profile_.settings.musicVolume,profile_.settings.effectsVolume,profile_.settings.dialogueVolume,0,0,profile_.settings.screenShake,profile_.settings.flashIntensity,profile_.settings.cameraSmoothing,profile_.settings.dialogueSpeed/3};for(int i=0;i<10;i++){Color c=i==menuIndex_?Color{255,166,120,255}:Color{165,175,197,255};DrawText(labels[i],60,105+i*21,13,c);if(i==4||i==5){const bool on=i==4?profile_.settings.fullscreen:profile_.settings.integerScaling;DrawText(on?"ON":"OFF",252,105+i*21,13,c);}else{DrawRectangle(252,109+i*21,240,5,{30,38,55,255});DrawRectangle(252,109+i*21,static_cast<int>(240*values[i]),5,c);}}DrawText("ARROWS ADJUST // ENTER TOGGLE // ESC RETURN",60,330,12,{102,119,148,255});}
    void drawStatistics(){title("ARK DIAGNOSTICS // ONE LOCAL PROFILE");std::vector<std::string> rows={"ATTEMPTS                "+std::to_string(profile_.attempts),"SUCCESSFUL RUNS         "+std::to_string(profile_.successfulRuns),"SUBSTRATES LOST         "+std::to_string(profile_.substratesLost),"SWAPS ACCEPTED          "+std::to_string(profile_.swapsAccepted),"SWAPS DECLINED          "+std::to_string(profile_.swapsDeclined),"GAME CLEARED            "+std::string(profile_.hasClearedGame?"YES":"NO")};for(std::size_t i=0;i<rows.size();++i)DrawText(rows[i].c_str(),62,120+static_cast<int>(i)*25,15,{190,201,221,255});DrawText("RECENT RUNS",340,120,14,{255,166,120,255});for(std::size_t i=0;i<profile_.recentRuns.size();++i){const auto&r=profile_.recentRuns[i];std::string s=(r.success?"CLEAR ":"LOST  ")+r.startingFrame+"  B"+std::to_string(r.bossesReached);DrawText(s.c_str(),340,147+static_cast<int>(i)*22,11,{149,166,190,255});}DrawText("BACKSPACE / ESC",62,330,12,{104,120,150,255});}

    void drawPod(float x,float y,const Frame& f,bool selected){Color a=frameAccent(f);DrawRectangle(static_cast<int>(x),static_cast<int>(y),154,205,{16,24,37,255});DrawRectangleLinesEx({x,y,154,205},selected?3:1,selected?a:Color{57,71,94,255});DrawRectangle(static_cast<int>(x+9),static_cast<int>(y+9),136,120,{11,17,29,255});for(int k=0;k<4;k++)DrawLine(static_cast<int>(x+14+k*38),static_cast<int>(y+15),static_cast<int>(x+14+k*38),static_cast<int>(y+120),withAlpha(a,55));drawIrys(f,{x+64,y+47,24,70},f.dominantSide,stageTime_,false);DrawText(f.designation.c_str(),static_cast<int>(x+10),static_cast<int>(y+136),13,a);std::string anatomy=toString(f.anatomy)+" // "+toString(f.collision);DrawText(anatomy.c_str(),static_cast<int>(x+10),static_cast<int>(y+154),10,{189,195,214,255});std::string hp="INTEGRITY "+std::to_string(static_cast<int>(f.integrity))+" / "+std::to_string(static_cast<int>(f.maxIntegrity));DrawText(hp.c_str(),static_cast<int>(x+10),static_cast<int>(y+170),10,{255,158,126,255});std::string stats="SPD "+std::to_string(static_cast<int>(f.groundSpeed))+"  REACH "+std::to_string(static_cast<int>(f.reach));DrawText(stats.c_str(),static_cast<int>(x+10),static_cast<int>(y+185),9,{132,151,180,255});}
    void drawCandidateSelect(){for(int row=0;row<3;row++)for(int i=0;i<14;i++){Rectangle p{8.0f+i*47,row*33.0f+3,24,28};DrawRectangleLinesEx(p,1,{45,25,49,255});DrawCircle(static_cast<int>(p.x+12),static_cast<int>(p.y+11),3,{108,60,88,255});}DrawRectangle(0,70,640,290,{5,8,14,235});title("ARK CANDIDATE CHAMBER // SELECT ONE // NO REROLLS");for(int i=0;i<3;i++)drawPod(70+i*171,110,candidates_[i],i==selectionIndex_);const auto&f=candidates_[selectionIndex_];std::string traits;for(std::size_t i=0;i<f.traits.size();++i){if(i)traits+="  /  ";traits+=f.traits[i];}DrawText(traits.c_str(),70,322,10,{216,125,196,255});DrawText("LEFT/RIGHT INSPECT // ENTER BIND ION",360,340,10,{94,117,148,255});}
    void drawFrameSelect(){const int tier=campaign_.bossesDefeated();if(tier==1){for(int i=0;i<9;i++){DrawLine(i*80,70,i*80+35,330,{70,55,53,255});DrawCircle(i*80+17,105+(i%3)*62,8,{88,62,67,255});}}else if(tier==2){for(int i=0;i<8;i++){DrawRectangle(i*85,65,6,270,{54,70,78,255});DrawLine(i*85+3,65,i*85+42,110,{91,119,119,255});}}else{for(int i=0;i<10;i++)DrawRectangleLines(i*66,65,48,250,{111,66,111,255});}const char* refinement=tier==1?"CRUDE SALVAGE RACKS":tier==2?"ARTICULATED FABRICATION":"ARK-LIKE POD IMITATION";title("PLANETARY-AI SUBSTRATES // RETAIN DAMAGE OR REPLACE COMPLETE FRAME");DrawText(refinement,430,96,9,{188,255,75,255});drawPod(25,110,selected_,selectionIndex_==0);DrawText("RETAIN CURRENT",43,96,12,{78,232,255,255});for(std::size_t i=0;i<replacements_.size();++i)drawPod(195+static_cast<float>(i)*145,110,replacements_[i],selectionIndex_==static_cast<int>(i)+1);DrawText("TRANSFER CLEARS PRIME AND ALL BODY STATE",25,332,10,{255,153,120,255});}
    void drawScene(){
        Color bg=sceneKind_==SceneKind::Transfer?Color{21,15,34,255}:sceneKind_==SceneKind::FinalArrival||sceneKind_==SceneKind::BladeDeath?Color{25,9,18,255}:Color{8,14,24,255};ClearBackground(bg);
        for(int i=0;i<20;i++){DrawRectangle(i*34,30+(i%4)*55,4,190,sceneKind_==SceneKind::Transfer?Color{71,87,35,255}:Color{55,37,69,255});DrawCircle(i*34+2,45+(i%4)*55,7,{57,31,62,255});}
        if(sceneKind_==SceneKind::Transfer){Frame anti;anti.anatomy=Anatomy::Standard;anti.palette=3;drawIrys(anti,{300,112,40,120},Side::Right,stageTime_,true,true);for(int i=0;i<8;i++)pixelLine({40.0f+i*76,40.0f},{320,145},2,withAlpha({185,255,62,255},100));}
        if(sceneKind_==SceneKind::Departure){for(int r=12;r<80;r+=12)DrawCircleLines(320,205,r,withAlpha({255,135,170,255},static_cast<unsigned char>(150-r)));drawIrys(selected_,{306,142,28,82},Side::Right,stageTime_,true);}
        if(sceneKind_==SceneKind::BladeDeath){drawIon({320,195},Side::Right,-1.55f,true,{188,255,64,255});for(int r=15;r<70;r+=14)DrawCircleLines(320,195,r,{188,255,64,80});}
        if(sceneKind_==SceneKind::Merger){drawIon({300,190},Side::Right,-.2f,true,{75,235,255,255});DrawCircleV({350,190},18,{255,135,170,180});pixelLine({320,190},{344,190},5,{255,225,205,255});}
        title(sceneKind_==SceneKind::Transfer?"CONTROL-CENTER CONTAINMENT":sceneKind_==SceneKind::Merger?"IRYS + ION":"SIGNAL");DrawRectangle(30,268,580,66,{9,13,23,235});DrawRectangleLinesEx({30,268,580,66},1,{255,142,122,255});const std::string& line=sceneLines_[std::min(sceneLine_,static_cast<int>(sceneLines_.size())-1)];DrawText(line.c_str(),46,284,13,{220,225,238,255});DrawText("ENTER",548,316,10,{76,230,255,255});
    }

    void drawEnvironment(){
        if(boss_&&boss_->def.kind==BossKind::AntiIrys){
            DrawRectangleGradientV(0,0,kWidth,kHeight,{24,12,28,255},{49,25,43,255});for(int i=0;i<12;i++){int x=18+i*55;DrawRectangle(x,80+(i%2)*18,40,126,{33,24,42,255});DrawRectangleLines(x,80+(i%2)*18,40,126,{255,126,165,255});DrawCircle(x+20,132+(i%2)*18,8,{218,181,197,255});DrawRectangle(x+18,143+(i%2)*18,4,34,i%3?Color{255,127,169,255}:Color{255,170,112,255});}
            if(boss_->phase>=2){Rectangle barrier=boss_->phase==2?Rectangle{305,230,12,92}:Rectangle{220,252,12,70};DrawRectangleRec(barrier,{255,166,126,255});for(int y=static_cast<int>(barrier.y);y<barrier.y+barrier.height;y+=8)DrawRectangle(static_cast<int>(barrier.x+2),y,8,3,{255,225,190,255});}
            DrawRectangle(0,322,640,38,{28,22,34,255});for(int x=0;x<640;x+=26)DrawRectangle(x,322,18,3,{255,120,164,255});return;
        }
        const StageDef& s=kStages[stageIndex_];DrawRectangleGradientV(0,0,kWidth,kHeight,s.low,shutdown_?Color{22,20,25,255}:s.high);
        for(int layer=0;layer<3;layer++)for(int i=0;i<8;i++){int x=(i*101+layer*33-static_cast<int>(stageTime_*(3+layer)))%760-60;int h=70+((i*37+layer*19)%130);DrawRectangle(x,kFloor-h,28+layer*13,h,withAlpha({18,24,34,255},static_cast<unsigned char>(110+layer*30)));DrawRectangle(x+5,kFloor-h+9,3,h-16,shutdown_?Color{68,52,50,255}:withAlpha(s.high,100));}
        DrawRectangle(0,static_cast<int>(kFloor),kWidth,kHeight-static_cast<int>(kFloor),{18,22,30,255});for(int x=0;x<640;x+=32)DrawRectangle(x,322,22,4,shutdown_?Color{80,55,53,255}:Color{72,89,101,255});
        if(!boss_){Rectangle h=hazardRect();Color hc=hazardClock_<.34f?Color{255,76,96,230}:Color{255,150,94,70};DrawRectangleRec(h,hc);DrawRectangle(static_cast<int>(h.x-3),245,16,5,{105,74,72,255});}
        DrawRectangleRec({185,245,105,8},{48,57,70,255});DrawRectangleRec({390,205,105,8},{48,57,70,255});DrawRectangleRec({285,160+std::sin(stageTime_*.9f)*34,72,8},{72,84,99,255});DrawLine(185,253,205,322,{35,44,55,255});DrawLine(495,213,480,322,{35,44,55,255});
        if(stageIndex_==2){for(int i=0;i<5;i++)DrawCircleLines(80+i*120,150+(i%2)*30,22,{100,47,85,255});}
        if(stageIndex_==3&&!shutdown_){for(int i=0;i<12;i++)DrawRectangle(i*55,18,30,5,{190,255,69,255});}
    }
    void drawGameplay(){
        drawEnvironment();for(const auto&p:projectiles_)DrawRectangleRec(p.box,p.color);for(const auto&e:enemies_)if(!e.dead)drawEnemy(e,stageTime_);if(boss_&&!boss_->dead)drawBoss(*boss_,stageTime_);
        drawIrys(selected_,player_.box,player_.facing,stageTime_+player_.box.x*.01f,player_.combat.state().primeArmed,false,1-selected_.integrity/selected_.maxIntegrity);if(player_.flash>0)DrawRectangleRec(player_.box,withAlpha(RED,static_cast<unsigned char>(120*player_.flash)));
        for(const auto&p:particles_)DrawRectangle(static_cast<int>(p.p.x),static_cast<int>(p.p.y),2,2,withAlpha(p.color,static_cast<unsigned char>(255*std::min(1.0f,p.life*3))));
        if(roomClear_){DrawRectangle(605,255,24,67,{33,49,58,255});DrawRectangleLines(605,255,24,67,{76,235,255,255});DrawText("EXIT",590,239,10,{76,235,255,255});}
        drawHud();if(bannerTime_>0){int w=MeasureText(banner_.c_str(),18);DrawRectangle(310-w/2,44,w+20,31,{5,8,14,210});DrawText(banner_.c_str(),320-w/2,51,18,{240,226,205,255});}
        if(debugDraw_)drawDebug();
    }
    void drawHud(){
        DrawRectangle(10,9,225,27,{8,12,20,220});DrawRectangleLines(10,9,225,27,{83,63,82,255});DrawText(selected_.designation.c_str(),17,14,10,frameAccent(selected_));DrawRectangle(94,17,132,7,{39,31,39,255});float hp=std::max(0.0f,selected_.integrity/selected_.maxIntegrity);DrawRectangle(94,17,static_cast<int>(132*hp),7,hp<.25f?RED:Color{255,123,130,255});
        DrawRectangle(10,41,122,20,{8,12,20,220});std::string p=player_.combat.state().primeArmed?"ION // PRIME ARMED":"ION // READY";DrawText(p.c_str(),17,46,10,player_.combat.state().primeArmed?Color{210,143,255,255}:Color{76,235,255,255});
        if(boss_&&!boss_->dead){DrawRectangle(168,331,304,18,{8,10,17,230});DrawRectangle(176,344,288,5,{38,31,40,255});DrawRectangle(176,344,static_cast<int>(288*std::max(0.0f,boss_->health/boss_->def.health)),5,{189,255,67,255});std::string n=toString(boss_->def.kind)+" // PHASE "+std::to_string(boss_->phase);DrawText(n.c_str(),177,333,10,{222,228,230,255});}
    }
    void drawDebug(){const Rectangle sword=swordVolume();if(sword.width>0)DrawRectangleLinesEx(sword,1,YELLOW);DrawRectangleLinesEx(player_.box,1,GREEN);for(const auto&e:enemies_)if(!e.dead){DrawRectangleLinesEx(e.box,1,RED);Rectangle a=enemyAttack(e);if(a.width>0)DrawRectangleLinesEx(a,1,ORANGE);}if(boss_){for(Rectangle h:bossHurtboxes(*boss_))DrawRectangleLinesEx(h,1,RED);Rectangle a=bossAttack(*boss_);if(a.width>0)DrawRectangleLinesEx(a,1,ORANGE);}std::ostringstream ss;ss<<"F1 boxes F2 inv F3/F4 restart F5 tune F6 clear\nF7 heal F8 room F9 boss F10 step F12 reset\n"<<toString(player_.combat.state().authority)<<" / "<<toString(player_.combat.state().technique)<<" seed "<<campaign_.seed()<<"\n"<<toString(selected_.anatomy)<<" "<<toString(selected_.dominantSide)<<" "<<toString(selected_.collision);DrawRectangle(376,8,257,58,{4,7,12,220});DrawText(ss.str().c_str(),381,12,8,{180,255,172,255});}
    void drawPause(){title("SIMULATION PAUSED");const char* opts[]={"RESUME","SETTINGS","DEVELOPER INVULNERABILITY","ABANDON RUN"};for(int i=0;i<4;i++){Color c=i==menuIndex_?Color{255,166,120,255}:Color{170,181,201,255};DrawText(opts[i],120,145+i*31,16,c);}if(!developer_)DrawText("DEVELOPER TOOLS DISABLED IN STANDARD PLAY",120,288,10,{96,108,130,255});}
    void drawFailure(){ClearBackground({12,9,15,255});for(int i=0;i<18;i++)DrawCircle(i*38,180+(i%3)*19,14,{35,25,39,255});title("SUBSTRATE LOST // ION RETURNED TO THE ARK");drawIon({320,190},Side::Right,-1.55f,true,{76,235,255,255});DrawText("The body remains. Ion rises. The route remembers you.",85,145,15,{206,191,205,255});DrawText("ENTER // PRESENT THREE NEW CANDIDATES",136,235,13,{78,233,255,255});DrawText("BACKSPACE // MAIN MENU",209,262,11,{113,123,149,255});}
    void drawEnding(){
        ClearBackground({18,11,25,255});for(int i=0;i<24;i++){int x=18+(i%8)*78,y=95+(i/8)*78;DrawRectangle(x,y,48,62,{40,24,45,255});DrawRectangleLines(x,y,48,62,{255,132,170,255});DrawCircle(x+24,y+30,8,{225,181,194,255});DrawRectangle(x+21,y+39,6,17,frameAccent(candidates_[i%3]));}
        drawIrys(candidates_[1],{306,182,28,72},candidates_[1].dominantSide,stageTime_,false);title("ENDING // INDEPENDENT AWAKENING");DrawRectangle(62,258,516,78,{8,8,15,225});DrawRectangleLines(62,258,516,78,{255,118,166,255});DrawText("The planetary intelligence is gone.",175,268,14,{221,224,235,255});DrawText("Irys and Ion divide—not as copies, but as beginnings.",113,289,14,{255,162,186,255});DrawText("An unselected candidate opens her eyes. The Ark becomes a birthplace.",81,312,12,{89,229,255,255});DrawText("ENTER // RETURN TO TITLE",236,342,10,{132,143,166,255});
    }
};

int validateCampaign() {
    int failures=0;std::set<Anatomy> anatomies;std::set<std::pair<int,int>> enemies;
    for(std::uint64_t seed=1;seed<=512;++seed){BodyGenerator g(seed);for(const auto&f:g.startingCandidates()){if(!f.viable())++failures;anatomies.insert(f.anatomy);}for(int boss=1;boss<=3;++boss)for(const auto&f:g.postBossCandidates(boss))if(!f.viable()||f.integrity<=0||f.integrity>f.maxIntegrity)++failures;}
    for(const auto&s:kStages)for(const auto&r:s.rooms)for(const auto&e:r)enemies.insert({static_cast<int>(e.lineage),e.generation});
    if(anatomies.size()!=4||enemies.size()!=9||kBosses.size()!=5)++failures;for(const auto&b:kBosses)if(b.health<=0||b.phases<2||b.zoom!=1.0f)++failures;
    BodyGenerator g(77);Campaign c;c.begin(77,g.generate());if(!c.advanceStage()||!c.defeatBoss()||!c.chooseReplacement(std::nullopt)||!c.advanceStage()||!c.defeatBoss()||!c.chooseReplacement(g.generate())||!c.advanceStage()||!c.defeatBoss()||!c.chooseReplacement(std::nullopt)||!c.advanceStage()||!c.defeatBoss())++failures;c.completeTransfer();c.completeShutdownReturn();if(!c.defeatBoss()||c.phase()!=CampaignPhase::Ending||c.bossesDefeated()!=5)++failures;
    std::cout<<"campaign-validation: anatomies="<<anatomies.size()<<" enemy-forms="<<enemies.size()<<" bosses="<<kBosses.size()<<" result="<<(failures?"FAIL":"PASS")<<"\n";return failures?3:0;
}

} // namespace

int runGame(int argc,char**argv){
    bool validate=false,developer=false,automated=false,automatedFailure=false;int smokeFrames=0,bossJump=-1,stageJump=-1,roomJump=0;std::filesystem::path captureDir;
    for(int i=1;i<argc;++i){std::string a=argv[i];if(a=="--validate"||a=="--validate-campaign")validate=true;else if(a=="--developer")developer=true;else if(a=="--autoplay")automated=developer=true;else if(a=="--autoplay-failure")automatedFailure=automated=developer=true;else if(a=="--boss"&&i+1<argc){developer=true;bossJump=std::stoi(argv[++i])-1;}else if(a=="--room"&&i+2<argc){developer=true;stageJump=std::stoi(argv[++i])-1;roomJump=std::stoi(argv[++i])-1;}else if(a=="--capture-dir"&&i+1<argc)captureDir=argv[++i];else if(a=="--smoke-frames"&&i+1<argc)smokeFrames=std::max(1,std::stoi(argv[++i]));}
    std::string warning;Tuning tuning=Tuning::load(std::string(IRYSION_RESOURCE_DIR)+"/config/tuning.json",warning);if(validate){if(!warning.empty()){std::cerr<<warning<<"\n";return 2;}return validateCampaign();}
    const auto path=defaultProfilePath();Profile profile=Profile::load(path,warning);Game game(tuning,std::move(profile),path,developer,automated,automatedFailure,captureDir);game.initialize();if(bossJump>=0||stageJump>=0)game.developerJump(bossJump,stageJump,roomJump);game.loop(smokeFrames);return 0;
}

} // namespace irys
