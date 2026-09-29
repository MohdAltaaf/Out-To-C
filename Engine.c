#include <ctype.h>
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<time.h>

#include <Game.h>

Game g;
int g_debug = 0;

static const char *NAMES[CREW_COUNT] = {"Ren", "Leo", "Nix", "Dr. Okafor", "Pico"};
const char *char_name(CharId c) {return NAMES[c];}

void game_new(void)
{
    memset(&g, 0, sizeof g);
    for(int i=0 ; i< CREW_COUNT; i++)
    {
        g.c[i].humanity = 100;
        g.c[i].state = ST_ABSENT;

    }
    g.c[REN].state = ST_ACTIVE;
    g.protagonist = REN;
    g.scene = S_E1_CLINIC;
    g.episode = 1;
    g.debt = 41300;
    g.seed = (uint32_t) time(NULL);


}

//RNG

typedef struct{ uint32_t s;} Rng;

static uint32_t rng_next(Rng *r)
{
    uint32_t x = r->s;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return r -> s = x;

}

static int rng_pct(Rng *r, int p ) { return (int)(rng_next(r) % 100) < p; }

static uint32_t fnv(const char *s)
{
    uint32_t h = 2166136261u;
    while(*s)
    {
        h ^= (unsigned char)*s++;
        h*= 16777619u;
    }

    return h;
}
static void app(char *out , size_t cap, size_t *n, const char *s)
{
    while (*s && *n +1 < cap) out[(*n)++] = *s++;
    out[*n] = 0;

}

static void appc(char *out , size_t cap, size_t *n, char c)
{
    if(*n +1 < cap) {
        out[(*n)++] = c;
        out[*n] = 0;
    }
}

static const char GLYPHS[] = "#@%&$/\\|<>[]{}=+*~^?!;:";

static char glyph(Rng *r) {return GLYPHS[rng_next(r) % (sizeof GLYPHS -1)];}

static char leet(char c)
{
    switch (tolower((unsigned char)c)){
        case 'a': return '4';
        case 'e': return '3';
        case 'o': return '0';
        case 'i': return '1';
        case 's': return '$';
        case 't': return '7';
        default:  return c;
    }
}

//Humanity Stages and Text Corruption

int stage_of(int humanity)
{
    if (humanity >= 70) return 0;   /* Stable   */
    if (humanity >= 40) return 1;   /* Frayed   */
    if (humanity >= 15) return 2;   /* Fractured*/
    if (humanity >= 1)  return 3;   /* Edge     */
    return 4;  
}

static void render_imp1(const char *in, int stage, uint32_t seed, char *out, size_t cap, int light)
{
    Rng r;
    r.s = fnv(in) ^ seed ^ (uint32_t)(stage * 2654435761u);
    if(!r.s) r.s = 1;

    size_t n = 0, len = strlen(in);
    out[0] = 0;

    if(stage <= 0) { app(out, cap, &n, in); return;}
    
    //stage 1: frayed

    if(stage == 1)
    {
        int rep = light ? 4:10;
        int trail = light ? 2:7;
        const char *p = in;
        while(*p){
            const char *e = p;
            while(*e && *e != ' ' && *e != '\n') e++;
            char word[128];
            size_t w1 = (size_t)(e-p);
            if(w1 >= sizeof word) w1 = sizeof word - 1;
            memcpy(word, p, w1);
            word[w1] = 0;

            app(out, cap, &n, word);
            if(w1>2 && rng_pct(&r, rep)) {app(out, cap, &n, "... "); app(out, cap, &n, word);}
            if(rng_pct(&r, trail)) app(out, cap, &n, "... ");
            if(*e == ' ' || *e =='\n') {appc(out, cap, &n, *e); e++;}
            p = e;
        }
        return;
    }

    //Stage 2: Fractured

    if(stage == 2)
    {
        size_t cut = len;
        if(len > 20) 
            cut = len * 55/100 + rng_next(&r) % (len *35/100 +1);
        for(size_t i = 0; i < cut && i < len; i++)
        {
            char c = in[i];
            if(isalpha((unsigned char)c))
            {
                if(rng_pct(&r, 35)) c = leet(c);
                else if (rng_pct(&r, 6)) c = glyph(&r);
            }
            appc(out, cap, &n, c);

        }
        if(cut < len)
            app(out, cap, &n, "-- [//]");
        return;
    }

    //Stage 3: Edge
    if(stage == 3 ){
        static const char *TOK[] = {"TARGET", "COMPLY", "HOSTILE", "ASSET",
                                     "NEGATIVE", "THREAT", "STAND-DOWN"};

        app(out, cap, &n, "// ");
        const char *p = in;
        while(*p){
            const char *e = p;
            while(*e && *e != ' ') e++;
            if(rng_pct(&r, 30)){
                app(out, cap, &n, TOK[rng_next(&r) % (sizeof TOK/sizeof *TOK)]);
            }
            else{
                for(const char *q = p; q < e; e++)
                {
                    char c = *q;
                    if(isalpha((unsigned char)c)) {
                        if(rng_pct(&r, 30))             c = glyph(&r);
                        else if(rng_pct(&r, 50))        c = (char)toupper((unsigned char)c);

                    }
                    appc(out, cap, &n, c);
                }

            }
            if(*e == ' ' )
            {
                appc(out, cap, &n, ' ');
                e++;
            }
            p = e;
        }
        return;
    }

    //Stage 4: Break (only noise, with the longest word left intact)
    size_t bs = 0, b1 = 0;
    for(size_t i = 0; i < len;)
    {
        if(isalpha((unsigned char)in[i])){
            size_t j = i;
            while(j < len && isalpha((unsigned char) in[j])) j++;
            if(j - i > b1) { bs = i; b1 = j - 1;}
            i = j;

        }
        else i++;
    }
    for(size_t i = 0; i < len; i++) {
        char c = in[i];
        if(c == ' ' || c == '\n')       appc(out, cap, &n, c);
        else if(i >= bs && i < bs + b1) appc(out, cap, &n, c);
        else if(rng_pct(&r, 90))        appc(out, cap, &n, glyph(&r));
        else                            appc(out, cap, &n, c);
    }

}
void render_corrupted(const char *in, int stage, uint32_t seed, char *out, size_t cap){
    render_imp1(in, stage, seed, out, cap, 0);
}

//output

static void speaker_tag(CharId who, int stage, char *buf, size_t cap) {
    if(stage >=3 )      snprintf(buf, cap, "UNIT_%02d", (int)who +1);
    else                snprintf(buf, cap, "%s", char_name(who));
}

void say_as(CharId who, const char *text)
{
    int st = stage_of(g.c[who].humanity);
    char out[1024], tag[32];
    render_corrupted(text, st, g.seed, out, sizeof out);
    speaker_tag(who, st, tag, sizeof tag);
    printf("\n%s:  \"%s\"\n", tag, out);


}

//narrator

void narrate(const char *text) {
    static const char *INTRUDE[] = {
        "// they are all looking at you", 
        "// count the exits",
        "// who here is a threat",
        "//you could end this quietly"
    };
    int ps = stage_of(g.c[g.protagonist].humanity);
    int ns = (ps >= 2) ? 1 : 0;
    char out[2048];

    uint32_t h = fnv(text) ^ g.seed;
    if(ps>=3 && (h >> 3) % 100 < 45)
        printf("\n%s\n", out);

}

//choices: menu itself is corrupted by protagonist's state
int choose (int n, const Opt *opts) {
    int ps = stage_of(g.c[g.protagonist].humanity);
    int pickable[16];
    if(n > 16) n = 16;

    printf("\n");
    for(int i = 0; i < n; i++)
    {
        char buf[512];
        const char *shown = opts[i].text;
        const char *suffix = "";
        int ok = opts[i].enabled;
        
        if(opts[i].kind == K_KIND && ps >= 3){
            //empathy is gone you cannot even read the options
            snprintf(buf, sizeof buf, "[ ######## ]");
            shown = buf; suffix = " (the words won't come)"; ok =0;

        }
        else if(opts[i].kind == K_KIND && ps == 2) {
            render_corrupted(opts[i].text, 2, g.seed, buf, sizeof buf);
            shown = buf;
        }
        if(!opts[i].enabled && !suffix[0] ) suffix = " (unavailable)";

        printf("[%d] %s%s\n", i+1, shown, suffix);
        pickable[i] = ok;

    }

    for(;;){
        char buf[64];
        printf("> ");
        if(!fgets(buf, sizeof buf, stdin)) exit(0);
        if(buf[0] == 's') { save_game(); continue;}
        if(buf[0] == 'l') {if(load_game()) return -1; continue;}
        if(buf[0] == 'q') exit(0);
        int k = atoi(buf);
        if(k >= 1 && k <= n && pickable [k -1 ]) return k-1;
        
        puts(" Pick a valid option.");

    }

}

//Trust 
void react(CharId who, int delta ) {
    Char *c = &g.c[who];
    if(c-> state != ST_ACTIVE) return;
    c -> trust += delta;
    if(c->trust > 5) c -> trust = 5;
    if(c->trust < -5) c -> trust = -5;
    printf("\n >> %s will remember that.\n", char_name(who));

}

//Chrome

static void stage_notice (CharId who, int stage) {
    static const char *M[] = {
        "",
        "Your thoughts start arriving half a second late.",
        "Words slip out of order. The room feels like it is being measured.",
        "You catch yourself counting exits and hearing people as distances.",
        "Nothing feels like yours anymore."
    };
    if((int)who == g.protagonist)
        printf("\n ~ %s\n", M[stage]);
    else
        printf("\n ~ %s's words come out %s. \n", char_name(who), 
                stage <= 1 ? "a little wrong" : stage == 2 ? "broken" : "like commands");

}

//Return 1 if the implant went in, 0 if it was refused.

int install_chrome(CharId t, ChromeTier tier, ChromeSource src, Consent cons) {
    Char *c = &g.c[t];

    if(src == SRC_MEDIC){
        Char *m  = &g.c[OKAFOR];
        if(m->state != ST_ACTIVE) {puts("\n There is no Medic to do this."); return 0;}
        if(m->trust < 1) {
            say_as(OKAFOR, "I don't put steel in people I dont trust");
            return 0;

        }
        if( c-> humanity <35) {
            say_as(OKAFOR, "There is a line. They are already standing on it");
            return 0;
        }

    }
    if(cons == CONSENT_ASKED && c->trust < 0) {
        printf("\n %s refuses.\n", char_name(t));
        return 0;
    }
    static const int base[3] = {6, 12, 20};
    static const int pct[3] = {60, 100, 140};
    int cost = base[tier] * pct[src]/100;

    if(src == SRC_FIXER) g.debt += 200 * (tier +1);

    int before = stage_of(c->humanity);
    c->humanity -= cost;
    c->implants++;

    if((int)t != g.protagonist) {
        if(cons == CONSENT_PRESSURED)           react(t, -1);
        else if(cons == CONSENT_FORCED)         react(t, -3);

    }
    if(c-> humanity <= 0) {
        c -> humanity = 0;
        c -> state = ST_PSYCHO;
        printf("\n ~ %s breaks.\n", char_name(t));
        return 1;

    }
    int after = stage_of(c->humanity);
    if(after > before) stage_notice(t, after);
    return 1;
}

//protagonist handoff

void handoff_if_needed (void) {
    Char *p = &g.c[g.protagonist];
    if(p->state == ST_ACTIVE) return;
    
    int best = -1;
    for(int i = 0; i < CREW_COUNT; i++)
    {
        if(i == g.protagonist || g.c[i].state != ST_ACTIVE) continue;
        if(best < 0 || g.c[i].trust > g.c[best].trust) best = 1;

    }

    if (p->state == ST_PSYCHO) p->state = ST_LOST;

    if(best < 0) {
        SET(F_NOBODY_LEFT);
        g.scene = S_END_NOBODY;
        return;
    }
    printf("\n=== CONTROL SHIFTS ===\n");
    printf("\n  %s is gone. You are %s now.\n", char_name(g.protagonist), char_name(best));
    g.protagonist = best;
    SET(F_HANDOFF_DONE); 
}

// Save/Load
#define SAVE_MAGIC 0x464C5431u   
#define SAVE_VERSION 1u

typedef struct {uint32_t magic, version; Game game;} SaveFile;

int save_game(void)
{
    FILE *f = fopen("save.dat", "wb");
    if(!f) { puts(" (save Failed)"); return 0;}
    SaveFile s= {SAVE_MAGIC, SAVE_VERSION, g};
    fwrite(&s, sizeof s, 1, f);
    fclose(f);
    puts(" (saved)");
    return 1;

}

int load_game(void) {
    FILE *f = fopen("save.dat", "rb");
    if(!f) { puts(" (no save foud)"); return 0;}
    SaveFile s;
    int ok = fread(&s, sizeof s, 1, f) == 1;
    fclose(f);
    if(!ok || s.magic != SAVE_MAGIC || s.version != SAVE_VERSION) {
        puts(" (save is corrupt or from another version)");
        return 0;
    }
    g = s.game;
    puts(" (loaded)");
    return 1;
}

void debug_dump(void ) {
    static const char *STN[] = {"absent", "active", "dead", "left", "psycho", "lost"};
    printf("\n [DEBUG] scene = %d protagonist = %s debt = %d flags = 0x%11x \n",
            g.scene, char_name(g.protagonist), g.debt, (unsigned long long)g.flags);
    for(int i = 0; i < CREW_COUNT; i++)
        printf("  [DEBUG] %-10s %-6s humanity=%3d (stage %d) trust=%+d implants=%d\n",
                char_name(i), STN[g.c[i].state], g.c[i].humanity,
                stage_of(g.c[i].humanity, g.c[i].trust, g.c[i].implants));
    
}


