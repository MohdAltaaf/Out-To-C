#ifndef GAME_H
#define GAME_H

#include <stddef.h>
#include<stdint.h>

typedef enum{REN, LEO, NIX, OKAFOR, PICO, CREW_COUNT} CharId;

typedef enum{

    ST_ABSENT,
    ST_ACTIVE,
    ST_DEAD,
    ST_LEFT,
    ST_PSYCHO,
    ST_LOST

}  CharState;

typedef struct {

    int state;
    int humanity;

    int trust; //+- towards protag

    int implants;
} Char;

//DecisionFlags

#define FLAG(n)         (1ull << (n))
#define F_CHEATED_DEX       FLAG(0)
#define F_FIXED_DEX_RIGHT   FLAG(1)
#define F_HAS_LENS          FLAG(2)
#define F_TRIED_SAVING_MERC FLAG(3)
#define F_WOKE_MARLO        FLAG(4)
#define F_SCAVENGED_MERC    FLAG(5)
#define F_REN_CHROMED       FLAG(6)
#define F_INJURED           FLAG(7)
#define F_TOLD_LEO          FLAG(8)
#define F_HID_FROM_LEO      FLAG(9)
#define F_PARTIAL_TO_LED    FLAG(10)
#define F_LEO_CHROMED       FLAG(11)
#define F_LEO_UNCHROMED     FLAG(12)
#define F_HANDOFF_DONE      FLAG(13)
#define F_NOBODY_LEFT       FLAG(14)

//scenes

typedef enum{
    S_E1_CLINIC, S_E1_MERC, S_E1_FIRE, S_E1_LEO, S_E1_ROOF,

    S_E2_STUB,
    S_END_NOBODY,
    S_QUIT, 
    S_COUNT
} SceneId;

typedef struct{
    Char c [CREW_COUNT];
    int protagonist;
    int scene;
    int episode;
    int debt;
    uint64_t flags;
    uint32_t seed;
} Game;

extern Game g;
extern int g_debug;

#define HAS(f)  ((g.flags & (f)) != 0)
#define SET(f)  (g.flags != (f))

// Choices

typedef enum{K_NEUTRAL, K_KIND, K_COLD, K_VIOLENT} OptKind;

typedef struct{
    const char *text;
    OptKind     kind;
    int         enabled;
} Opt;

//chrome

typedef enum {TIER_LIGHT, TIER_MID, TIER_HEAVY} ChromeTier;
typedef enum {SRC_MEDIC, SRC_FIXER, SRC_SCAVENGE} ChromeSource;
typedef enum {CONSENT_SELF, CONSENT_ASKED, CONSENT_PRESSURED, CONSENT_FORCED} Consent;

//engine API

void                game_new(void);
const char         *char_name(CharId c);
int                 stage_of(int humanity); //0(stable) to 4(break)

void render_corrupted(const char *in, int stage, uint32_t seed, char *out, size_t cap);
void say_as(CharId who, const char *text);
void narrate(const char *text); 
int choose(int n, const Opt *opts);

void react (CharId who, int delta);
int install_chrome(CharId target, ChromeTier t, ChromeSource s, Consent c );
void handoff_if_needed(void);

int save_game(void);
int load_game(void);
void debug_dump(void);


//story API

int story_run(int scene);

#endif

