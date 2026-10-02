#ifndef GAME_H
#define GAME_H

#include <stddef.h>
#include<stdint.h>

typedef enum{REN, LEO, NIX, OKAFOR, PICO, CREW_COUNT} CharId;

typedef enum{

    ST_ABSENT,
    ST_ACTIVE,
    ST_DEAD,
    ST_LEFT,      /* alive, but not in the squad (declined, remote-only, sold you out) */
    ST_PSYCHO,
    ST_LOST

}  CharState;

typedef struct {

    int state;
    int humanity;

    int trust; //+- towards protag

    int implants;
} Char;

//DecisionFlags (uint64_t: keep n below 64)

#define FLAG(n)         (1ull << (n))

/* ---- Episode 1 ---- */
#define F_CHEATED_DEX       FLAG(0)
#define F_FIXED_DEX_RIGHT   FLAG(1)
#define F_HAS_LENS          FLAG(2)
#define F_TRIED_SAVING_MERC FLAG(3)
#define F_WOKE_MARLO        FLAG(4)
#define F_SCAVENGED_MERC    FLAG(5)   /* pulled the weave out of the merc */
#define F_REN_CHROMED       FLAG(6)
#define F_INJURED           FLAG(7)
#define F_TOLD_LEO          FLAG(8)
#define F_HID_FROM_LEO      FLAG(9)
#define F_PARTIAL_TO_LED    FLAG(10)
#define F_LEO_CHROMED       FLAG(11)
#define F_LEO_UNCHROMED     FLAG(12)
#define F_HANDOFF_DONE      FLAG(13)
#define F_NOBODY_LEFT       FLAG(14)
#define F_REN_SCAV_WEAVE    FLAG(15)  /* Ren is wearing the MILITECH weave */
#define F_LENS_SPENT        FLAG(16)
#define F_LEO_FRAGILE       FLAG(17)  /* got hit because he had no chrome and Ren couldn't cover him */
#define F_LEO_PRESSURED     FLAG(18)

/* ---- Episode 2 ---- */
#define F_NIX_JOINED        FLAG(19)
#define F_NIX_REMOTE        FLAG(20)  /* helps from a distance, never joins */
#define F_NIX_SOLD          FLAG(21)  /* Nix sold your location to 6th Street */
#define F_LEO_OPTICS_OUT    FLAG(22)
#define F_VISITED_NIX       FLAG(23)
#define F_VISITED_OKAFOR    FLAG(24)
#define F_OK_JOINED         FLAG(25)
#define F_OK_DECLINED       FLAG(26)
#define F_OK_HELPED_PATIENT FLAG(27)
#define F_OK_STOPPED_SCAV   FLAG(28)
#define F_OK_CALMED_FAMILY  FLAG(29)
#define F_TOOK_SCAV_CUT     FLAG(30)  /* Ren is carrying a scavenged implant (E3 hook) */
#define F_OK_HURT_SCAV      FLAG(31)
#define F_LIED_TO_NIX       FLAG(32)
#define F_LEO_HURT          FLAG(33)
#define F_PICO_SPARED_BY_DR FLAG(34)
#define F_SACRIFICED_PICO   FLAG(35)
#define F_LOST_STASH        FLAG(36)
#define F_AMBUSH_DONE       FLAG(37)
#define F_CLINIC_HIT        FLAG(38)  /* 6th Street hit Okafor's station */
#define F_SHARD_READ        FLAG(39)  /* the shard has been decrypted (fully or partly) */

/* ---- Episode 3 ---- */
#define F_SCAV_CUT_USED     FLAG(40)
#define F_MERC_WEAVE_USED   FLAG(41)
#define F_HEIST_FULL        FLAG(42)
#define F_HEIST_PARTIAL     FLAG(43)
#define F_HEIST_BLUFF       FLAG(44)
#define F_ARASAKA_PINGED    FLAG(45)  /* Leo's lens talked to the tower: Arasaka knows where he is */
#define F_PICO_CAUGHT       FLAG(46)  /* you noticed Pico skimming */
#define F_PICO_MARK         FLAG(47)  /* you didn't. She noticed you didn't */
#define F_LEO_SECRET_CHROME FLAG(48)  /* Leo chromed himself behind your back */
#define F_HAS_CHROME_CRATE  FLAG(49)

//scenes  (NOTE: numbers changed, old save.dat files are invalid: bump SAVE_VERSION in Engine.c)

typedef enum{
    S_E1_CLINIC, S_E1_MERC, S_E1_FIRE, S_E1_LEO, S_E1_ROOF,

    S_E2_HUB, S_E2_MAP, S_E2_NIX, S_E2_OKAFOR, S_E2_AMBUSH, S_E2_DECRYPT,

    S_E3_HIDEOUT, S_E3_BENCH, S_E3_PLAN, S_E3_AFTER,

    S_E4_STUB,
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
    uint32_t marked;    /* bit per CharId: wearing M-series chrome the Epoch code can reach */
    uint32_t fragile;   /* bit per CharId: refused chrome, will be the first one hurt */
    int      betrayer;  /* CharId who sells you out in Ep4, or -1. Set at the end of Ep3 */
} Game;

extern Game g;
extern int g_debug;

#define HAS(f)  ((g.flags & (f)) != 0)
#define SET(f)  (g.flags |= (f))
#define CLR(f)  (g.flags &= ~(f))

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

//ui API (ui.c)

void ui_init(int fast, int nocolor);
void ui_print(const char *color, const char *fmt, ...);
void ui_type(const char *text, const char *color, int indent, int stage);
void ui_wrap(const char *text, const char *color, int indent);
void ui_pause(int ms);
void ui_flush_input(void);   /* throw away keys typed while text was printing */
const char *ui_char_color(CharId who);

//colors

#define COL_NARR    "\x1b[37m"
#define COL_NOTE    "\x1b[90m"
#define COL_GLITCH  "\x1b[91m"
#define COL_WARN    "\x1b[1;91m"
#define COL_DEBT    "\x1b[33m"
#define COL_NUM     "\x1b[96m"
#define COL_OPT     "\x1b[97m"
#define COL_LOCK    "\x1b[90m"
#define COL_TITLE   "\x1b[1;96m"

#endif