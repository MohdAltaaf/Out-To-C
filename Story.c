#include <stdio.h>
#include <string.h>

#include "game.h"

#define ASK(o) choose((int)(sizeof(o) / sizeof((o)[0])), (o))
#define CHECK(c) do {if((c) < 0) return g.scene;} while (0)
#define ACTIVE(id) (g.c[id].state == ST_ACTIVE)

//helpers


static void show_debt(void){
    ui_print(COL_NOTE, "\n[DEBT: %d EB owed to Petrochem]\n", g.debt);

}
static void banner(const char *t){
    ui_print(COL_TITLE, "\n=== %s ===\n", t);
    
}

// will remember that.

static void marker(const char *who) {
    ui_print(COL_NOTE, "\n >> %s will remember that.\n", who);
}

//a speaker who isnt CharId..

static void say_npc(const char *name, const char *text, int stage) {
    char out[1024], line[1100];
    render_corrupted(text, stage, g.seed, out, sizeof out);
    snprintf(line, sizeof line, "\"%s\"", out);
    ui_print(COL_OPT, "\n%s:\n", name);
    ui_type(line, COL_OPT, 2, stage);
    ui_print("", "\n");
    ui_pause(250);
}


static void say_flicker(CharId who, const char *text, int stage)
{
    char out[1024], line[1100];
    render_corrupted(text, stage, g.seed, out, sizeof out);
    snprintf(line, sizeof line, "\"%s\"", out);
    ui_print(ui_char_color(who), "\n%s:", char_name(who));
    ui_type(line, ui_char_color(who), 2, stage);
    ui_print("", "\n");
    ui_pause(250);
    
}

static void set_trust(CharId who, int v) {
    if(v > 5) v = 5;
    if(v < -5) v = -5;
    g.c[who].trust = v;
}

//Episode 1: FLATLINE

static int scene_clinic(void) {
    banner("EPISODE 1: FLATLINE");
 
    narrate("Arroyo, Santo Domingo. Hour twelve of a shift in a clinic with no windows "
            "and no licence. The ceiling drips into a bucket Marlo calls \"the budget\".\n\n"
            "On your bench: a gangoon's cracked optic, half a lens of flickering blue. "
            "In the corner of your vision the Petrochem counter shows the same number "
            "it showed yesterday. You know it by heart.");
 
    show_debt();
 
    narrate("Dinner is a tube of synth-protein paste, eaten cold, one-handed. "
            "A message from your brother slides across the corner of your vision.");
 
    say_npc("LEO [msg]", "Found us a place in Westbrook. Two rooms. Real windows. "
                         "If I make team lead by spring the sublease is basically ours. "
                         "Please eat something that isn't paste.", 0);
 
    narrate("Leo keeps records for an Arasaka subcontractor a few districts up. "
            "The company gave him a badge, a ladder, and a lens, and he climbs like a man "
            "who can see the top. \"Company-issue,\" he says of the lens. \"Free.\" "
            "You have told him more than once that nothing in this city is free.\n\n"
            "Westbrook. Real windows. It's the only thing the two of you have ever agreed to want.");
 
    narrate("Marlo leans out of the back office. He owns the clinic and every excuse for it, "
            "and he runs the place on one rule.");
    say_npc("Marlo", "Everybody in Arroyo keeps a ledger. Be careful what you write in someone else's.", 0);
 
    narrate("Across the waiting room a factory hand named Tamsin sits with her forearm open on a tray, "
            "waiting on a recalibration. She has been talking to nobody for ten minutes.");
    say_npc("Tamsin", "I just need it to grip. I just need it to grip properly so I can go back to work.", 1);
    narrate("Marlo watches you notice. \"Third install,\" he says, quietly. "
            "\"The arm is fine. The arm is always fine. It's the person the chrome eats.\"");
 
    narrate("Dex, the owner of the eye on your bench, leans onto the counter and doesn't look at anything "
            "for longer than a second. \"Just make it work. It doesn't have to be pretty.\"");
 
    Opt o[] ={
        {"Fix it properly. It costs you an extra hour and the good solder", K_KIND, 1},
        {"Tell him the parts cost double. He won't check (probably)", K_COLD, 1},
        {"Fix it and quietly pocket a spare lens from the bin", K_COLD, 1}
    };
    int c = ASK(o); CHECK(c);
 
    if(c == 0)
    {
        SET(F_FIXED_DEX_RIGHT);
        narrate("The blue steadies into a clean white. Dex holds the eye up to the light and turns his head, "
                "like a man checking a stranger's face.\n"
                "\"...Thanks. Most people would've just made it work.\"\n\n"
                "He taps the counter twice. \"There's a doc under the drain district who'd like you. "
                "Fixes anyone, asks nothing. Tell her Dex sent you.\"");
        marker("Dex");
    }
    else if (c==1)
    {
        SET(F_CHEATED_DEX);
        g.debt -= 240;
        narrate("He pays without a word. The number in your debt drops, barely. "
                "You don't feel better.\n\n"
                "On his way out Dex looks at every face in the room, one second each. "
                "The way you'd memorise a room you might have to describe later.");
        show_debt();
        marker("Dex");
    }
    else
    {
        SET(F_HAS_LENS);
        narrate("The optic works. In your sleeve, a spare lens the size of a coin. "
                "A lens like that can whiteout a sensor for a few seconds. "
                "It isn't much. Some nights it's enough.\n\n"
                "Dex never looks at your hands. Marlo does.");
    }
    return S_E1_MERC;
}

static int scene_merc(void)
{
    narrate("2:07 a.m. The front door doesn't open. It BREAKS. \n\n"
            "A woman in torn armor falls across the floor, trailing blood and a sound"
        "like a radio between stations. Marlo is shouting from the back. The tag on her shoulder plate reads"
    "MILITECH ARMORED TRANSPORT. Her eyes have a shine you've seen before, always in the last few minutes of a person."
        "the chrome in her head finishes what started with her first implant.");

    Opt o[] = {
        {"Get her on the table and try to stablize her", K_KIND, 1},
        {"Lock the door and plan with Marlo how to deal with this", K_NEUTRAL, 1},
        {"Check her implants before she goes. She might have some good chrome.", K_COLD, 1}

    };
    int c = ASK(o); CHECK(c);

    if(c == 0) {
       SET(F_TRIED_SAVING_MERC);
        narrate("Your hands know this part even if your training doesn't. Her pulse is a rumour.\n"
                "She grips your collar. \"Epoch Drive. Not Militech's. Find Nix. Flooded levels, H4. "
                "She's the only one who can read it without--\"\n\n"
                "The name lodges in your head like a splinter. It's the kind of thing people "
                "in Arroyo pay for. You got it for free.");
    }
    else if(c == 1) {
        SET(F_WOKE_MARLO);
        narrate("Marlo says: "
                "\"Whatever she brought, we did NOT see it.\" "
                "He is already dragging a shelf against the door.\n\n"
                "Behind you the woman's lips are still moving. You don't lean in. "
                "Later you'll wish you had.");
    }
    else {
        SET(F_SCAVENGED_MERC);
        narrate("You work a subdermal weave out of her forearm while her eyes track you. "
                "Stamped along the housing: MILITECH // FIELD ISSUE. The metal is still warm, "
                "and it hums, very faintly, like something listening.\n"
                "She doesn't fight you. That is the part you'll remember.");
    }

    narrate("Then her deck spits white light. A dead-man protocol, older than the woman wearing it, "
            "reaches across the room and pushes something cold into the diagnostic deck at your temple.\n\n"
            "A file bar crawls across your vision. EPOCH DRIVE // REMOTE TRIGGER // "
            "CLASS: NEUROLOGICAL. Then the woman's heart stops. "
            "Above you, through four floors of concrete, you hear it...\n"
            "Militech is coming.");
    return S_E1_FIRE;
}

static int scene_fire(void)
{
    narrate("The clinic is on fire. The dead-man protocol has set off a chain reaction in the chemical storage."
            "The room is a furnace. The walls are melting. The floor is melting. "
            "You have to get out before the whole building collapses.");
    
    if(HAS(F_WOKE_MARLO))
        narrate("Militech clean-up doesn't knock. They will make a billion holes in you before asking where the deck is."
                "On the back service stairs you see Marlo holding the door for you."
                "You don't know if he meant to or not.");
    else
        narrate("Militech clean-up doesn't knock. They will make a billion holes in you before asking where the deck is."
                "By the time you reach the back service stairs, Marlo is already gone. Priorities huh");

    narrate("You descend the stairs and reach the sevice tunnel. A kid in a stolen hi-vis jacket steps out of the dark,"
                "A bag over her shoulder and a price behind her eyes.");
                g.c[PICO].state = ST_ACTIVE;
    say_as(PICO, "You're going to be on every wallscreen in Arroyo in about ten minutes. "
                    "Militech doesn't torch a whole block for something small.");
    say_as(PICO, "Name's Pico. I sell things people can't afford to need."
                    "Any type of chrome, any type of gun. You gotta take me with you. It'll be lit. ");
    narrate("Her rate card blinks across your deck. WEAVE, light. BLADES, heavy. KEREZNIKOV, heavy."
                    "Every line has a price column, a third column she doesn't show."
                    "You've seen Tasmin's hand. You've seen what the third column does.");

    if(HAS(F_SCAVENGED_MERC)) {
        narrate("You show her the Militech weave.");
        say_as(PICO, "THAT is a good weave. I have weaves too but they aren't as good. Although there is only one way to obtain the one you have... by Killing a merc....");
    }
    Opt o[ ] = {
        {"Buy the Subdermal Weave from PICO. (Her cost goes into your debt.)", K_NEUTRAL, 1},
        {"Install the weave you pulled off the mercenary", K_COLD, HAS(F_SCAVENGED_MERC) != 0},
        {"No chrome. We run.", K_KIND, 1}
    };
    int c = ASK(o); CHECK(c);
    if(c == 0)
    {
        if(install_chrome(REN, TIER_LIGHT, SRC_FIXER,CONSENT_SELF))
        {
            SET(F_REN_CHROMED);
            react(PICO, +1);
            narrate("The weave goes in cold and fast. Your skin tightens over a new, quiet weight. "
                    "It works. You'd have paid more\n");
            show_debt();
            narrate("You dodge past the Militech drones outside the tunnel with ease and get out with Pico.");
            say_flicker(REN, "It's fine. It's fine. I'm fine.", 1);
            narrate("Your own thoughts arrive half a second late and stumbles over themselves. "
                    "Then it's gone. Your deck shows your vitals are fine... But you feel that this implant cost you something.");
        }
    }
    else if( c == 1)
    {
        if(install_chrome(REN, TIER_LIGHT, SRC_SCAVENGE, CONSENT_SELF))
        {
            SET(F_REN_CHROMED);
            SET(F_REN_SCAV_WEAVE);
            narrate("Pico watches you slot a dead woman's weave in to your own arm and says nothing"
                    "That says plenty.\n"
                    "It's free and hums against your bones");
            narrate("You dodge past the Militech drones outside the tunnel with ease and get out with Pico.");
            say_flicker(REN, "It's fine. It's fine. I'm fine.", 1);
            narrate("Your own thoughts arrive half a second late and stumbles over themselves. "
                    "Then it's gone. Your deck shows your vitals are fine... But you feel that this implant cost you something."
                    "Scavenged chrome doesn't fit the way bought chrome does. It comes with someone else's habits.");
        }
    }
    else if(c == 2)
    {
        SET(F_LENS_SPENT);
        narrate("The lens flares and the drone's sensor whites out."
                "Eleven seconds. Eleven is enough"
                "You both slip away, and the lens is dead in your fingers.");
    }
    else{
        SET(F_INJURED);
        narrate("You run. A piece of ceiling gets your leg on the way out. It will keep talking to you for days."
                "Pico helps you walk the rest of the way ad lets you know that you had a choice.");
        
    }

return S_E1_LEO;

}

static int scene_leo(void)
{
    narrate("Megabuilding H6, floors up, 5:40 a.m. Leo answers the door in his work formals, "
            "the company lens glowing a faint teal at the corner of his eye. "
            "The one thing he brags about. ");
    g.c[LEO].state = ST_ACTIVE;
    g.c[LEO].trust = 1;
    say_as(LEO, "Ren!? What happened to you??");
    Opt o[] = {
        {"Tell him everything. The merc, the Epoch shard, Militech", K_KIND, 1},
        {"Lie. A gas leak at the clinic. You just need a place to crash", K_COLD, 1},
        {"Tell him the clinic burned down and you have to leave Arroyo tonight.(you owe him an explanation later)", K_NEUTRAL, 1}

    };
    int c = ASK(o); CHECK(c);

    if(c == 0 ) {
        SET(F_TOLD_LEO);
        react(LEO, +2);
        say_as(LEO, "Okay. OKAY.... *deep breath* Yeah. We have to get out of here, fast. Both of you, come with me to the roof");

    }
    else if( c == 1){
        SET(F_HID_FROM_LEO);
        narrate("A news feed is already scrolling across Leo's lens: MILITECH CLEAN-UP, ARROYO"
                "His eyes flick to it and back to you.");
        say_as(LEO, "Right. A gas leak.");
        react(LEO, -2);
        narrate("You feel bad about lying to Leo.");
        say_as(PICO, "*smirks* Lying to a guy with a corporate lens. Bold. "
                "Everyone's connected to something, Ren.");

    }
    else{
        SET(F_PARTIAL_TO_LED);
        say_as(LEO, "There's a LOT you're not telling me. I'm trusting you both here, you gotta tell me everything later. "
                    "Let's move. Come on, both of you.");
        say_as(PICO, "Oh god, I'm hella tired.");
        say_as(LEO, "Then walk tired, kid. Come on now.");
    }
    return S_E1_ROOF;
}


static int scene_roof(void)
{
    narrate("The three of you cross the rooftops from H6 to H7. Below Arroyo burns "
            "in Patches, like something being crossed off a list. "
            "Then Leo stops mid-step, one hand at his left eye. His company lens has turned amber.");
    say_as(LEO, "That's weird. It has never done this.");
    narrate("It isn't a glitch. Somewhere behind the lens a light that's never been amber is amber now"
            "The lens just pinged something, or someone."
            "Overhead, you see a Gunmetal drone cruise towards you from the horizon...");
    say_as(PICO, "That's Arasaka, not Militech. They're not even tryna hide it."
                "Your brother's lens just pinged out location"
                "I can defend myself and you can too, but can Leo?"
                "I can fit him with a weave before it gets here. Quick there's no time!");
    
    Opt o[] ={
        {"Ask Leo if he wants it.", K_KIND, 1},
        {"Tell him he doesn't have a choice", K_COLD, 1},
        {"No chrome. Cover him and run", K_NEUTRAL, 1}
    };
    int c = ASK(o); CHECK(c);

    if(c == 0){
        if(install_chrome(LEO, TIER_LIGHT, SRC_FIXER, CONSENT_ASKED))
        {
            SET(F_LEO_CHROMED);
            say_as(LEO, "Do it. FAST!");
            say_flicker(LEO, "Okay. It's okay. It feels heavy though.", 1);
            narrate("For a moment his voice comes out slightly wrong. Then it's Leo again.");
            show_debt();
        }
        else {
            SET(F_LEO_UNCHROMED);
            if(HAS(F_HID_FROM_LEO))
                say_as(LEO, "After the gas leak? No. I'm not putting anything in my body on your word.");

            else
                say_as(LEO, "I'm not turning into one of them for you.");
        }
    }
    else if (c == 1) {
        if(install_chrome(LEO, TIER_LIGHT, SRC_FIXER, CONSENT_PRESSURED))
        {
            SET(F_LEO_CHROMED);
            SET(F_LEO_PRESSURED);
            say_as(LEO, "You don't get to decide that for me.");
            say_flicker(LEO, "You don't get to. You don't get to decide.", 1);
            narrate("He doesn't look at you again until the drone is gone. ");
            show_debt();

        }
    }
    else{
        SET(F_LEO_UNCHROMED);
    }

    //drone shoots: chrome on ren or leo is what decides who get's hurt.
    
    
    // neither is chromed
    if(HAS(F_LEO_UNCHROMED) && !HAS(F_REN_CHROMED))
    {
        SET(F_LEO_FRAGILE);
        narrate("You shove Leo behind a vent housing, but you're not fast enough due to your leg injury. "
            "The drone rounds tear a strip out of his shoulder. \"I'm fine\" he says, "
            "while he is obviously not fine."
            "He will move slower now, that is if you make it out alive of here");

    }
    else if(HAS(F_LEO_UNCHROMED))
    {
        narrate("The weave in your arm reads the drone's aim before you do. You drag Leo and Pico down to cover. "
                "Leo stares at your arm but doesn't say anything");
        
    }
    else{
        narrate("You reach to push Leo out of harms way but he's already taken cover using his new weave."
        "You do the same, your weave reads the drone's aim before you do and dodges it and goes into cover.");
        
        say_flicker(REN, "Leo, you alright?", 1);

        narrate("You see Leo gasping heavily");
        say_flicker(LEO, "Yeah, never been better", 1);
    }
    narrate("Drone keeps firing. You make it into the vent just in time."
            "Then it stops, hovers and broadcasts to every wallscreen in the sector at once: \n\n"
                "BOUNTY: 4,000,000 EB\n"
                "SUBJECT: REN, diagnostic technician, Arroyo\n"
            
                "KNOWN ASSOCIATES: LEO, data archivist, Arasaka subcontractor\n"
                "                  PICO, impant merchant/fixer, Arroyo\n\n"
                "Leo's corporate ladder has just been kicked into hell. And every gang in Santo Domingo"
                    "is reading the bounty screen.");

    return S_QUIT;
}

//Episode 2: STREET VALUE


static int scene_e2_hub(void)
{
    banner("EPISODE 2: STREET VALUE");

    narrate("The vent drop you three levels, into a crawlspace beneath the Arroyo night market"
            "Pico's humble abode apparently. A mattress, three scavenged screens and a heater built from a car battery"
                "The screens loop the same broadcast. Your face. Leo's. Hers. 4.000.000 EB.");

    say_as(PICO, "Four million. That's worth more than this block is worth. Every crew in Santo Domingo will want us by breakfast. Bro I didn't even run away with the shard, It was you Ren."
            "why the #### am I on the bounty.");
    
    say_as(LEO, "You were the one eager to sell weaves, heres your payment lol we made you famous."
            "Please someone tell me we have have a plan. Please.");
    narrate("Truth being nobody has a plan. You have a file you can't open and a bounty you can't outrun."
            "You need a crew, someone to decrypt the Epoch Shard and possibly some other helping hand in fights.");
    narrate("Luckily for you Pico always knows a guy who knows a guy...");
    say_as(PICO, "Sooo this info will be 50 EB. :D");
    say_as(LEO, "OMG, PICO.");
    say_as(PICO, "Right right lol.\n"
            "A netrunner who left 6th Street the hard way. She can read your shard. "
            "And a doctor Trauma Team threw out, she's got a small bounty on her I think we might manage to get her in the big leagues as well."
            "Both of them owe us nothing so there's that, we are the ones in need. We better not let that show.");
    return S_E2_MAP;
}

static int scene_e2_map(void)
{
    if(HAS(F_VISITED_NIX) && HAS(F_VISITED_OKAFOR))
        return S_E2_AMBUSH;
    
    if(HAS(F_VISITED_NIX) || HAS(F_VISITED_OKAFOR))
        narrate("Every wallscreen you pass is reading your face to somebody. The block is getting hot."
                "One lead left and you're running out of streets to hide in.");

    else
        narrate("Two leads. You can only be in one place at a time, you feel the order you go in will matter.");

    char nixtxt[220], oktxt[220];
    int nixcost = HAS(F_TRIED_SAVING_MERC) ? 0 : 900;
    int okcost = HAS(F_FIXED_DEX_RIGHT) ?    0 : 400;

    if(nixcost)
            snprintf(nixtxt, sizeof nixtxt,
                        "The netrunner: flooded parking level under H4. (Pico's price: %d eb, onto your debt)", nixcost);
    else
        snprintf(nixtxt, sizeof nixtxt, 
                    "The netrunner: flooded parking level under H4. (No cost. The dying merc gave you the name.)");

    if(okcost)
            snprintf(oktxt, sizeof oktxt,
                    "The doctor: a disused metro station in the drain district. (Pico's price: %d eb, onto your debt)", okcost);

    else
            snprintf(oktxt, sizeof oktxt, 
                    "The doctor: a disused metro station in the drain district. (No cost. Dex told you about her.)");

    
    Opt o[] = {
        {nixtxt, K_NEUTRAL, !HAS(F_VISITED_NIX)},
        {oktxt, K_NEUTRAL, !HAS(F_VISITED_OKAFOR)}
    };
    int c = ASK(o); CHECK(c);

    if(c == 0)
    {
        if(nixcost) {g.debt += nixcost; show_debt(); }
        return S_E2_NIX;
    }
    if(okcost) { g.debt += okcost; show_debt();}
    return S_E2_OKAFOR;
    
}

//Nix- the audit 

static int scene_e2_nix(void)
{
    int nt = 0;
    int sold = 0;
    int remote = 0;
    int paranoid = 0;

    narrate("The lower parking levels under H4 flooded a decade ago and nobody bothered to say goodbye"
            "The water is ankle-deep and warm, the concrete sweats, and every third pillar has a scrap of Faraday mesh tacked to it. "
            "Your deck goes quiet when you pass the third one.\n\n"
            "Then, in the silence, it speaks.");
    say_as(NIX, "Don't touch the door. I'm already in your deck Ren. And your brother's lens."
            "Nice lens btw. It's also a leash put on him by Arasaka.");
    say_as(LEO, "....? She's in my LENS?");
    say_as(NIX, "I've read everything else in your deck too, that's some dangerous tech in that shard you got there. "
            "Now before i open the only room in this city that Arasaka can't see, "
            "you answer my questions. I'll know if you lie. I have your deck's logs open right here.");
    narrate("You feel a chill running down your spine...");
    say_as(NIX, "Question one. The dead woman. Your logs say you spent four minutes with her before she died. What did you do.");

    for(;;)
    {
        Opt o[] ={
            {"Tell her the truth.", K_NEUTRAL, 1},
            {"\"I couldn't interact with her. She was gone before i got to her.\"", K_COLD, 1},
            {"\"Prove you're not Arasaka first.\"", K_COLD, !paranoid}
        };
        int c = ASK(o); CHECK(c);

        if(c == 2)
        {
            paranoid = 1;
            nt += 1;
            {
                char pf[300];
                snprintf(pf, sizeof pf, "I see. I'm sure you know Arasaka doesn't know about THIS [DEBT: %d eb owed to Petrochem] AND %s. Now answer.",
                            g.debt, HAS(F_CHEATED_DEX) ? "you charged double to a man named Dex" : "that you run your clinic pretty honestly, appreciate that ngl");
                    say_as(NIX, pf);;
            }
            continue;
        }
        if(c == 0)
        {
             nt += 1;
            if(HAS(F_SCAVENGED_MERC))
                say_as(NIX, "You pulled her weave. Disgusting. Remember that it's militech property, they know you have it.");
            else if(HAS(F_TRIED_SAVING_MERC))
                say_as(NIX, "You tried to save her. She'd have flatlined either way. Good instinct. Useless outcome.");
            else
                say_as(NIX, "You locked the door and ran. Prudent. Cowardly. Same thing in Arroyo.");
            break;
        }

        if(HAS(F_SCAVENGED_MERC) || HAS(F_TRIED_SAVING_MERC))
        {
            SET(F_LIED_TO_NIX);
            say_as(NIX, "I have a log that says otherwise. Goodbye, Ren.");
            sold = 1;
        }
        else
        {
            say_as(NIX, "...True, actually. You did nothing. Fine. I'll take a coward who doesn't lie about it.");
        }
        break;
    }

    if(!sold)
    {
        narrate("The mesh over the door drops. Beyond it, a cage of copper and shelved servers "
                "and a girl in a coat too big for her, arms folded, watching you through three cameras.");
        say_as(NIX, "Question two. Everything on the shard is a kill code. It reaches specific cyberware. "
                    "Leo's Arasaka lens is on the list. One signal and his head goes red inside a minute, "
                    "and they can send it whenever they like. The lens comes out, or that door stays shut.");
        say_as(LEO, "It's a company lens. It's just a lens. I've had it for two years.");
        if(HAS(F_HID_FROM_LEO))
        {
            say_as(NIX, "Also, Leo: your sibling told you it was a gas leak. I have the logs. It wasn't.");
            say_as(LEO, "...I know. I saw the feed. That doesn't make it better.");
        }
 
        Opt o[] = {
            {"Ask Leo what he wants to do.", K_KIND, 1},
            {"Take it out. It isn't his call.", K_COLD, 1},
            {"\"His lens is clean. Your records are wrong.\"", K_COLD, 1}
        };
        int c = ASK(o); CHECK(c);
 
        if(c == 0)
        {
            if(g.c[LEO].trust >= 1)
            {
                if(HAS(F_TOLD_LEO))
                    say_as(LEO, "You told me the truth when it counted. Take it out.");
                else
                    say_as(LEO, "You kept things from me and I'm still standing here. Do it before I change my mind.");
                SET(F_LEO_OPTICS_OUT);
                nt += 2;
                react(LEO, +1);
                narrate("He hands you the lens. In the pale light it looks like nothing. Somewhere in a tower "
                        "his employee ID logs a silent mismatch, and the ladder he was climbing goes quiet under his feet.");
            }
            else
            {
                if(HAS(F_HID_FROM_LEO))
                    say_as(LEO, "No. I'm not giving up the only thing keeping me employed because you lied to me about a gas leak.");
                else
                    say_as(LEO, "No. I don't know you well enough right now to let you cut into me.");
                Opt s[] = {
                    {"Side with Leo. Find another way.", K_KIND, 1},
                    {"Do it anyway. Now.", K_VIOLENT, 1}
                };
                int d = ASK(s); CHECK(d);
                if(d == 0)
                {
                    remote = 1;
                    nt += 0;
                    react(LEO, +1);
                    say_as(NIX, "Then I help you from where I sit. You don't get in. That's the deal.");
                }
                else
                {
                    SET(F_LEO_OPTICS_OUT);
                    react(LEO, -3);
                    nt += 1;
                    narrate("It takes both of you and one very quiet second. Leo doesn't scream. "
                            "That's the part that hurts.");
                }
            }
        }
        else if(c == 1)
        {
            SET(F_LEO_OPTICS_OUT);
            react(LEO, -3);
            nt += 1;
            narrate("Pico holds his arms. You do the rest. Leo doesn't scream, which is worse.");
            say_as(LEO, "I'll remember this.");
        }
        else
        {
            SET(F_LIED_TO_NIX);
            say_as(NIX, "Leo Vance, data archivist, Arasaka contractor. Lease optic, series K-4. Registered. "
                        "Goodbye, Ren.");
            sold = 1;
        }
    }
 
    SET(F_VISITED_NIX);
 
    if(sold)
    {
        g.c[NIX].state = ST_LEFT;
        SET(F_NIX_SOLD);
        narrate("The lights in the parking level die. On your deck, a single line: "
                "PING SENT: 6TH STREET. SUBJECT: REN. LAST KNOWN: H4.\n\n"
                "Leo drags you toward the ramp. Behind you the water starts to ripple. "
                "Whatever place you go next is going to have company.");
    }
    else if(remote)
    {
        g.c[NIX].state = ST_LEFT;
        set_trust(NIX, nt);
        SET(F_NIX_REMOTE);
        narrate("The cage stays shut. On your deck a line of text settles in: I'LL READ IT FOR YOU. "
                "DON'T EXPECT ME TO SHOW MY FACE.");
    }
    else
    {
        g.c[NIX].state = ST_ACTIVE;
        set_trust(NIX, nt);
        SET(F_NIX_JOINED);
        narrate("The door unbolts. Nix is shorter than her reputation, with cropped hair the colour of a dead screen "
                "and three fingers on her left hand replaced with something that isn't a hand. "
                "Her stare has already read you.");
        say_as(NIX, "Nix. Don't hover. Sit down and let me look at what's in your head.");
    }
    return S_E2_MAP;

}

// Okafor

static int scene_e2_okafor(void)
{
    int score = 0;
    int patient = 0;   /* 0 helped, 1 shouted for Okafor, 2 let him die */
 
    narrate("The drain district sits below the water table and above nothing. The old metro station's sign "
            "is still legible: PLATFORM 3. Someone has painted a red cross over the mural of a smiling commuter.\n\n"
            "Inside, cots line the platform, a dozen people wait on them, and a woman moves between the curtains "
            "with a tablet and no visible hurry.");
 
    if(HAS(F_NIX_SOLD))
    {
        narrate("Two men in 6th Street colours lean on the far turnstile, pretending not to watch the entrance. "
                "You didn't bring them. You didn't exactly not bring them, either.");
        score -= 1;
    }
 
    say_as(OKAFOR, "Wait your turn. I don't ask names and I don't take sides. "
                   "If you're bleeding, tell my nurse. If you're not, sit.");
 
    if(HAS(F_INJURED))
        say_as(OKAFOR, "You're limping. Sit. No, not there, the other chair.");
 
    narrate("She disappears behind a curtain with a patient. There's a chair. You wait.");
 
    /* Beat A: a patient crashes. Ren can help, because Ren is a diagnostic tech. */
    narrate("Three cots down, a man starts to shake. Not a tremor. A rhythm. His monitor shrieks and the nurse "
            "runs to him and starts pulling at his chrome. You see what she's missing at a glance: "
            "somebody stacked two suppressant doses. The chrome isn't the problem. The cocktail is.");
    {
        Opt o[] = {
            {"Get up and tell the nurse: it's the suppressant stack. Pull the second dose.", K_KIND, 1},
            {"Shout for Okafor and stay out of it.", K_NEUTRAL, 1},
            {"Stay seated. Not your clinic. Not your patient.", K_COLD, 1}
        };
        int c = ASK(o); CHECK(c);
        if(c == 0)
        {
            SET(F_OK_HELPED_PATIENT);
            score += 2;
            narrate("The nurse hesitates, then does what you say. The shaking eases in ten seconds. "
                    "The man is breathing. Your hands are steady and you don't know why.");
        }
        else if(c == 1)
        {
            patient = 1;
            narrate("Okafor bursts through the curtain, curses, and saves him. Barely. He'll keep his life. "
                    "He won't keep his left hand.");
        }
        else
        {
            score -= 1;
            patient = 2;
            narrate("By the time Okafor arrives, the monitor has settled into a single flat tone. "
                    "The nurse pulls the sheet up. The room goes back to waiting. That's what frightens you.");
        }
    }
 
    /* Beat B: a frightened family. Leo is tested here, separately from Ren. */
    narrate("In the corner, a woman and two kids sit very still around a cot with a sheet pulled up. "
            "Nobody has told them anything for an hour. The younger one has stopped asking.");
    {
        Opt o[] = {
            {"Nod at Leo. He's better at this than you.", K_KIND, 1},
            {"Go over yourself.", K_KIND, 1},
            {"Keep Leo close. It isn't our business.", K_COLD, 1}
        };
        int c = ASK(o); CHECK(c);
        if(c == 0)
        {
            SET(F_OK_CALMED_FAMILY);
            score += 1;
            narrate("Leo crouches by the kids and starts by asking their names. He does the thing with his voice "
                    "that makes people stop bracing. It's a talent you have never had.");
        }
        else if(c == 1)
        {
            SET(F_OK_CALMED_FAMILY);
            score += 1;
            narrate("You're awkward at it. The woman answers anyway. Sometimes it's enough that someone crossed the room.");
        }
        else
        {
            if(g.c[LEO].trust >= 3)
            {
                SET(F_OK_CALMED_FAMILY);
                score += 1;
                narrate("Leo hesitates. Then he goes anyway, without asking you, and sits with the kids. "
                        "You watch your brother do something you told him not to. "
                        "It's the first time he has ever looked like he trusts himself more than you.");
            }
            else
            {
                score -= 1;
                narrate("Leo stays. He doesn't like it. He keeps looking at the corner, and you don't look at him at all.");
            }
        }
    }
 
    /* Beat C: a scavenger works a dead patient's implants. Nobody has noticed. Or nobody has decided to notice. */
    narrate("Against the far wall a man in a patched jacket bends over a covered cot, sheet peeled back at the wrist. "
            "He's working a subdermal weave loose with a butter knife and steady hands.");
    if(HAS(F_SCAVENGED_MERC))
        narrate("You know exactly what he's doing. You've done it.");
    {
        Opt o[] = {
            {"Tell him to stop. Quietly, and mean it.", K_KIND, 1},
            {"Break his wrist.", K_VIOLENT, 1},
            {"Offer to look the other way. For a cut.", K_COLD, 1},
            {"Say nothing.", K_NEUTRAL, 1}
        };
        int c = ASK(o); CHECK(c);
        if(c == 0)
        {
            SET(F_OK_STOPPED_SCAV);
            score += 1;
            narrate("He looks at you for a long moment, then wipes the knife and walks out. Nobody else in the room moves.");
        }
        else if(c == 1)
        {
            SET(F_OK_HURT_SCAV);
            score -= 1;
            narrate("The knife hits the floor and so does he. He leaves with his wrist wrong and his eyes on you.");
        }
        else if(c == 2)
        {
            SET(F_TOOK_SCAV_CUT);
            score -= 2;
            narrate("He grins and slides a small case across the floor. Inside, a scavenged implant. "
                    "It's yours if you want it. You don't ask what's in it.");
        }
        else
        {
            narrate("He finishes. It takes him four minutes. Nobody stops him.");
        }
    }
 
    /* The verdict. She only says it afterwards. */
    narrate("When Okafor comes out from behind the curtain she is wiping her hands on a rag. "
            "She has been in the doorway, you realise, for a while.");
    say_as(OKAFOR, "You two just told me everything I need.");
 
    if(HAS(F_OK_HELPED_PATIENT))
        say_as(OKAFOR, "You saw the stacked dose before my nurse did. That's not a thing you learn in a week.");
    else if(patient == 1)
        say_as(OKAFOR, "You called for me. It cost that man a hand, but he's breathing.");
    else
        say_as(OKAFOR, "A man died three cots from you and you never stood up.");
 
    if(HAS(F_OK_CALMED_FAMILY))
        say_as(OKAFOR, "And someone sat with a family I forgot for an hour. Thank you.");
    else
        say_as(OKAFOR, "Nobody told that family a thing. You noticed and didn't move.");
 
    if(HAS(F_OK_STOPPED_SCAV))
        say_as(OKAFOR, "You stopped that man without breaking anything. That's rarer than you think.");
    else if(HAS(F_OK_HURT_SCAV))
        say_as(OKAFOR, "You broke his wrist. I understand why. I'm asking you not to do it in my station.");
    else if(HAS(F_TOOK_SCAV_CUT))
        say_as(OKAFOR, "I watched you cut a deal over a dead man's arm. I know exactly what that is.");
    else
        say_as(OKAFOR, "You saw him and looked away. So did my nurse. So did I, for a while. It's the not-looking I hate.");
 
    if(HAS(F_REN_SCAV_WEAVE))
    {
        score -= 1;
        say_as(OKAFOR, "And that weave in your arm didn't come from anyone living. It's still humming. Can you feel it?");
    }
 
    SET(F_VISITED_OKAFOR);
 
    if(HAS(F_INJURED))
    {
        CLR(F_INJURED);
        narrate("Before she says anything else she kneels and does something to your leg that hurts for exactly one second. "
                "It stops talking to you.");
    }
 
    if(score >= 1)
    {
        g.c[OKAFOR].state = ST_ACTIVE;
        set_trust(OKAFOR, score > 3 ? 3 : score);
        SET(F_OK_JOINED);
        say_as(OKAFOR, "I'm not joining your war. I'm joining you. I'll patch anyone you bring me, "
                       "and if you need steel, I'm the gentlest hand you'll find. But there is a line. "
                       "I won't put chrome in someone who is already standing next to it. Ask, and I'll say no. "
                       "You'll know when.");
    }
    else
    {
        g.c[OKAFOR].state = ST_LEFT;
        SET(F_OK_DECLINED);
        say_as(OKAFOR, "I'll wrap what needs wrapping and send you back out. You told me something tonight. "
                       "I'm not sure I like it.");
        narrate("She isn't cruel about it. She just closes the curtain.");
    }
    return S_E2_MAP;
}

//The first ambush - possible death

static int scene_e2_ambush(void)
{
    char buf[900];
    const char *crew;
 
    narrate("The crew has been scattered across the block all day. By night everyone is back in Pico's crawlspace, "
            "because it's the only address nobody has sold yet.");
 
    if(HAS(F_NIX_SOLD))            crew = "6th Street runners, their optics glowing";
    else if(HAS(F_CHEATED_DEX))    crew = "Dex's crew";
    else                           crew = "bounty hunters with licences from three corporations and manners from none";
 
    if(HAS(F_FIXED_DEX_RIGHT))
        narrate("A message tears across your deck a second before the power dies.\n\n"
                "   DEX: Saw your face on the wall. Back exit. Now. We're square.");
 
    snprintf(buf, sizeof buf,
             "3:12 a.m. The night market's power dies all at once, the way a held breath does. "
             "Then the hatch above the crawlspace unseats.\n\n"
             "%s. Five of them, on ropes.", crew);
    narrate(buf);
 
    if(HAS(F_CHEATED_DEX))
        say_npc("Dex", "Nothing personal. You shorted me.", 0);
 
    if(HAS(F_LEO_FRAGILE))
        narrate("Leo tries to stand. His shoulder gives out. He's slower than he used to be. Everyone sees it.");
 
    Opt o[] = {
        {"Drag Leo out of the line of fire.", K_KIND, 1},
        {"Take the door yourself. Let the weave soak it.", K_VIOLENT, HAS(F_REN_CHROMED) != 0},
        {"Nix: seal the corridor and kill their cameras.", K_NEUTRAL, ACTIVE(NIX)},
        {"Go out the back, the way Dex told you.", K_NEUTRAL, HAS(F_FIXED_DEX_RIGHT) != 0},
        {"Throw the skimmed lens. Blind them.", K_NEUTRAL, (HAS(F_HAS_LENS) && !HAS(F_LENS_SPENT))},
        {"Shove Pico into the doorway and run.", K_COLD, ACTIVE(PICO)}
    };
    int c = ASK(o); CHECK(c);
 
    if(c == 0)
    {
        narrate("You pull Leo flat against the floor. The doorway spits fire. Pico is closer to it than either of you.");
        if(ACTIVE(OKAFOR))
        {
            SET(F_PICO_SPARED_BY_DR);
            react(OKAFOR, +1);
            narrate("Pico goes down hard. Okafor is on her knees before you can turn, hands already inside the wound. "
                    "\"Pressure. Here. Don't let go.\" When the shooting stops, Pico is still breathing. "
                    "She'll walk again. She'll remember who you pulled first.");
        }
        else
        {
            g.c[PICO].state = ST_DEAD;
            narrate("Pico goes down. You reach her, and you're a diagnostic tech, not a surgeon, and there's no surgeon here. "
                    "You realise that too late. She dies looking at the hi-vis jacket she stole, and her rate card "
                    "goes dark on your deck. No more fixer.");
        }
    }
    else if(c == 1)
    {
        SET(F_INJURED);
        narrate("You hit the doorway low and let the weave take what comes. Rounds bite into your arm and stay there. "
                "You're not a fighter. You're a wall. Two of them stumble over you and Pico finishes the job with a pipe. "
                "The others run.");
        if(HAS(F_REN_SCAV_WEAVE))
        {
            say_flicker(REN, "TARGET down. Next. Next. No, that's mine.", 2);
            narrate("A voice in your head that isn't yours counts exits. It goes quiet when the fight stops. "
                    "It doesn't go away.");
        }
        narrate("A round grazes your thigh. You feel it later.");
    }
    else if(c == 2)
    {
        react(NIX, +1);
        narrate("Every light on the level snaps to red, then to black. The hatch seals, and two of the runners "
                "are locked out with their own doors humming at them. Nix doesn't say anything. "
                "She just wipes her nose with the back of a metal hand.");
    }
    else if(c == 3)
    {
        SET(F_LOST_STASH);
        react(PICO, -1);
        narrate("You go out the back with everyone. Nobody gets hurt. Behind you Pico looks once at her mattress, "
                "her three screens, and the crate under the bed with everything she owns in it. Then she follows.");
        say_as(PICO, "That was all my stock, Ren.");
    }
    else if(c == 4)
    {
        SET(F_LENS_SPENT);
        narrate("The lens flares. Five sets of optics whiteout and the runners stumble into each other. "
                "You get everyone out, but one man on the ropes saw your face and didn't lose it.");
    }
    else
    {
        SET(F_SACRIFICED_PICO);
        g.c[PICO].state = ST_DEAD;
        react(LEO, -2);
        if(ACTIVE(OKAFOR)) react(OKAFOR, -2);
        narrate("Pico doesn't even scream. She just looks at you as the fire takes her, and then you're through "
                "the back exit with everyone else, running. Nobody says anything for three blocks.");
        say_as(LEO, "You... you used her.");
    }
 
    SET(F_AMBUSH_DONE);
 
    if(HAS(F_NIX_SOLD))
    {
        SET(F_CLINIC_HIT);
        narrate("On the run, a message from the drain district cuts through: the station is burning. "
                "6th Street followed the ping past the crawlspace, and past the people who tried to sit in it.");
        if(ACTIVE(OKAFOR))
        {
            react(OKAFOR, -1);
            say_as(OKAFOR, "I don't blame you. That's the worst part. I don't have anywhere to go.");
        }
    }
    return S_E2_DECRYPT;
}

//Shard is finally read

static int scene_e2_decrypt(void)
{
    narrate("Dawn is a rumour down here. You gather around the one screen nobody sold.");
 
    if(ACTIVE(NIX))
        narrate("Nix takes your deck with both hands and plugs her cage straight into it. "
                "The shard opens like a hand.");
    else if(HAS(F_NIX_REMOTE))
        narrate("Nix's voice on the deck, no face, no room, no promise. The shard opens under her hands anyway.");
    else
        narrate("Nix isn't coming. Leo pulls up a document reader he built for work and squints. "
                "\"I can do headers,\" he says. \"That's all I can do.\"");
 
    SET(F_SHARD_READ);
 
    narrate("A list scrolls. Not names: model numbers. The cyberware families the Epoch code can reach. "
            "One signal, one handshake, and whatever is inside the chrome stops asking permission.");
 
    if(ACTIVE(NIX) || HAS(F_NIX_REMOTE))
    {
        say_as(NIX, "Three families. Arasaka lease optics. Militech M-series field weave. "
                    "And one more I can't read yet. The header's burned.");
 
        if(HAS(F_REN_SCAV_WEAVE))
        {
            say_as(NIX, "M-series, Ren. That's the weave in your arm.");
            narrate("You remember the lettering on the housing. MILITECH // FIELD ISSUE. "
                    "You remember it humming, like it was waiting for someone to ask.");
        }
        else if(HAS(F_SCAVENGED_MERC))
        {
            say_as(NIX, "You pulled the merc's weave and didn't put it in yourself. Good. Don't put it in anyone.");
        }
        else if(HAS(F_REN_CHROMED))
        {
            say_as(NIX, "Pico's stock isn't on the list. Lucky. Buy from the living, not the dead.");
        }
        else
        {
            say_as(NIX, "No chrome in you at all? Then you're the one person here they can't just switch off.");
        }
 
        if(HAS(F_LEO_OPTICS_OUT))
            say_as(NIX, "Leo's lens is off. Keep it that way. It can't be fired if it isn't plugged in.");
        else
            say_as(NIX, "Leo's still on it. Every hour he wears that lens is an hour they can fire it.");
 
        narrate("Nix leans back, and for the first time she looks tired.");
        say_as(NIX, "Two ways to make this stop. Kill it, or lock it. Locking it takes a host, someone who holds the "
                    "shard inside their own head for the rest of their life. Or use it. Turn it round on whoever sent it. "
                    "But it isn't picky. Anyone we love with the wrong chrome in them goes too.");
    }
    else
    {
        say_as(LEO, "Arasaka lease optics. That's me. And something called M-series... does that mean anything to you?");
        if(HAS(F_REN_SCAV_WEAVE))
        {
            narrate("It does. You remember the lettering on the housing: MILITECH // FIELD ISSUE, M-4. "
                    "Your arm hums, faintly. It sounds almost like recognition.");
        }
        else if(HAS(F_SCAVENGED_MERC))
        {
            narrate("It does. The weave you pulled from the merc is stamped M-4. You didn't put it in your arm. "
                    "You're not sure you'd have known to worry if you had.");
        }
        else
        {
            narrate("It doesn't, yet. But somebody in Night City is wearing M-series right now and doesn't know it.");
        }
        narrate("Without Nix, that's all you get. One family Leo can't read. Another you can't name.");
    }
 
    if(!HAS(F_LEO_OPTICS_OUT))
    {
        narrate("Then Leo's lens flashes amber and a voice comes out of it, soft and professional.");
        say_npc("ARASAKA", "Leo. Come in. We can fix this. We can fix all of this.", 0);
        say_as(LEO, "...Ren.");
    }
    else
    {
        narrate("An unsigned message hits your deck from an unlisted number.");
        say_npc("UNLISTED", "Leo's ID has been suspended. He is welcome to come home. We'll be waiting.", 0);
    }
 
    g.debt += 1200;
    narrate("Your Petrochem counter ticks over: missed payment, collections penalty. "
            "Ren, you have been offline for nearly a day. They noticed.");
    show_debt();
 
    return S_E3_HIDEOUT;
}

// EPISODE 3: CHROME DEBT

static uint32_t bit(int c) { return 1u << (unsigned)c; }
 
static int implant_total(void)
{
    int n = 0;
    for(int i = 0; i < CREW_COUNT; i++)
        if(ACTIVE(i)) n += g.c[i].implants;
    return n;
}
//trust change that also works for nix when she is helping from a distance.

static void trust_shift(CharId who, int d)
{
    if(ACTIVE(who)) react(who, d);
    else if(g.c[who].state == ST_LEFT) set_trust(who, g.c[who].trust + d);
}

static void heal(CharId who, int amt)
{
    Char *c = &g.c[who];
    int before = stage_of(c->humanity);
    c->humanity += amt;
    if(c->humanity > 100) c->humanity = 100;
    if(stage_of(c->humanity) < before)
    {
        if((int)who == g.protagonist)
            ui_print(COL_NOTE, "\n ~ Your thoughts arrive on time again.\n");
        else
            ui_print(COL_NOTE, "\n ~ %s's words settle, a little.\n", char_name(who));
    }
}

static void lose_humanity(CharId who, int amt)
{
    static const char *M[] = {
        "", "Your thoughts start arriving half a second late.",
        "Words slip out of order. The room feels like it is being measured.",
        "You catch yourself counting exits and hearing people as distances.",
        "Nothing feels like yours anymore."
    };
    Char *c = &g.c[who];
    if(c->state != ST_ACTIVE) return;
    int before = stage_of(c->humanity);
    c->humanity -= amt;
    if(c->humanity <= 0)
    {
        c->humanity = 0;
        c->state = ST_PSYCHO;
        ui_print(COL_WARN, "\n ~ %s breaks.\n", char_name(who));
        return;
    }
    int after = stage_of(c->humanity);
    if(after > before)
    {
        if((int)who == g.protagonist) ui_print(COL_GLITCH, "\n ~ %s\n", M[after]);
        else ui_print(COL_GLITCH, "\n ~ %s's words come out %s.\n", char_name(who),
                      after <= 1 ? "a little wrong" : after == 2 ? "broken" : "like commands");
    }
}

//someone (not REN) has hit zero. 

static int psycho_scene(CharId who)
{
    char buf[400];
    snprintf(buf, sizeof buf,
             "%s stops mid-sentence. Their eyes go flat and far away, and they start counting the exits. "
             "Then they start counting you.", char_name(who));
    narrate(buf);
    say_as(who, "Stay where you are. Stay. Where. You are.");
 
    Opt o[] = {
        {"End it yourself, before they reach anyone.", K_VIOLENT, 1},
        {"Get everyone out. Leave them to it.", K_COLD, 1}
    };
    int c = ASK(o);
    if(c < 0) return -1;
 
    if(c == 0)
    {
        g.c[who].state = ST_DEAD;
        narrate("It's over faster than it should be. The worst part is that it's easy.");
        trust_shift(LEO, -1);
        trust_shift(OKAFOR, -1);
        return 1;
    }
 
    g.c[who].state = ST_LOST;
    static const int order[] = {PICO, NIX, OKAFOR, LEO};
    for(int k = 0; k < 4; k++)
    {
        int v = order[k];
        if(v == (int)who || !ACTIVE(v)) continue;
        g.c[v].state = ST_DEAD;
        snprintf(buf, sizeof buf,
                 "You get most of the crew out. Behind you, a sound you will not describe to anyone. "
                 "%s doesn't make it to the stairs.", char_name((CharId)v));
        narrate(buf);
        break;
    }
    return 0;
}

static int check_psychos(void)
{
    for(int i = 1; i < CREW_COUNT; i++)   /* Ren (0) is handled by the hand-off */
        if(g.c[i].state == ST_PSYCHO)
            if(psycho_scene((CharId)i) < 0) return -1;
    return 0;
}
 
/* Generic "pick someone from the living crew". 1 = picked, 0 = none, -1 = reload */
static int pick_crew(const char *none_text, CharId *out)
{
    char lbl[CREW_COUNT][32];
    int ids[CREW_COUNT];
    Opt t[CREW_COUNT + 1];
    int n = 0;
    for(int i = 0; i < CREW_COUNT; i++)
    {
        if(!ACTIVE(i)) continue;
        snprintf(lbl[n], sizeof lbl[n], "%s", i == REN ? "Yourself" : char_name((CharId)i));
        ids[n] = i;
        t[n].text = lbl[n]; t[n].kind = K_NEUTRAL; t[n].enabled = 1;
        n++;
    }
    t[n].text = none_text; t[n].kind = K_KIND; t[n].enabled = 1; n++;
    int c = choose(n, t);
    if(c < 0) return -1;
    if(c == n - 1) return 0;
    *out = (CharId)ids[c];
    return 1;
}

static CharId briefer(void)
{
    if(ACTIVE(PICO)) return PICO;
    if(ACTIVE(NIX)) return NIX;
    if(ACTIVE(OKAFOR)) return OKAFOR;
    return LEO;
}

static int scene_e3_hideout(void)
{
    banner("EPISODE 3: CHROME DEBT");
 
    const char *place;
    char buf[800];
    if(ACTIVE(NIX))                                place = "Nix's Faraday cage under H4";
    else if(ACTIVE(OKAFOR) && !HAS(F_CLINIC_HIT))  place = "Dr. Okafor's metro station";
    else                                           place = "a capsule hotel whose lock never quite catches";
 
    snprintf(buf, sizeof buf,
             "You move before dawn. After the ambush, the whole city has narrowed to one address: %s.\n\n"
             "The bounty on the wallscreens now reads 6,000,000 EB, and somebody has added a second line "
             "under Leo's name. Nobody in Santo Domingo will sell to you anymore. Every fixer in the sector "
             "has read the broadcast.", place);
    narrate(buf);
 
    if(g.c[PICO].state == ST_DEAD)
        narrate("Pico's rate card is dead on your deck. There is no fixer left to buy from.");
 
    say_as(briefer(), "We can't buy anything, so we take it. Petrochem Medical Depot 9: suppressant, surgical kit, "
                      "and a cold room of chrome they've repossessed off people like us.");
    narrate("Petrochem. The same Petrochem whose name sits on your debt. Taking from your creditor is the nicest "
            "idea anyone has had since Arroyo burned.");
 
    if(ACTIVE(OKAFOR))
        say_as(OKAFOR, "I have two days of suppressant left. After that, anyone carrying chrome starts fraying "
                       "faster than I can stitch them back together.");
    else
        narrate("Nobody here knows how much suppressant is left. Nobody is counting. That's the problem.");
 
    narrate("Before anything else there is a quiet hour, the only one you'll get. You can spend it on one person, or on sleep.");
 
    Opt o[] = {
        {"Sleep. Actually sleep.", K_NEUTRAL, 1},
        {"Sit with Leo. Say something real.", K_KIND, ACTIVE(LEO)},
        {"Sit with Dr. Okafor while she counts stock.", K_KIND, ACTIVE(OKAFOR)},
        {"Help Nix tune the relays.", K_NEUTRAL, ACTIVE(NIX)},
        {"Swap stories with Pico.", K_NEUTRAL, ACTIVE(PICO)}
    };
    int c = ASK(o); CHECK(c);
 
    if(c == 0)
    {
        heal(REN, 8);
        narrate("You dream about the Petrochem counter. In the dream it goes down by one, and nobody notices but you.");
    }
    else if(c == 1)
    {
        heal(LEO, 10);
        react(LEO, +1);
        say_as(LEO, "Do you remember the radio you fixed when I was nine? I think about it more than I should.");
        narrate("You talk until the lamp runs low. It's the longest you've been in a room with him without a screen between you.");
    }
    else if(c == 2)
    {
        heal(REN, 4);
        react(OKAFOR, +1);
        say_as(OKAFOR, "Suppressant, dosed right, gives people back a little of themselves. Stacked wrong, it kills. "
                       "You saw that. I'll need hands I trust on it.");
    }
    else if(c == 3)
    {
        heal(REN, 4);
        react(NIX, +1);
        say_as(NIX, "Someday somebody is going to have to decide who carries this thing. Not today.");
        narrate("She goes back to the relays. You realise she has just told you something she hasn't told anyone.");
    }
    else
    {
        heal(REN, 4);
        react(PICO, +1);
        say_as(PICO, "First thing I ever sold was a lens off a dead man. Got two eddies and a lecture.");
        narrate("She laughs at her own joke a little too long.");
    }
    return S_E3_BENCH;
}

static int chrome_bench(int max_installs)
{
    int done = 0;
    while(done < max_installs)
    {
        char prompt[90];
        CharId who;
        snprintf(prompt, sizeof prompt, "That's enough chrome for one night. (%d install%s left)",
                 max_installs - done, max_installs - done == 1 ? "" : "s");
        ui_print(COL_NOTE, "\n Who gets chrome?\n");
        int pc = pick_crew(prompt, &who);
        if(pc < 0) return -1;
        if(pc == 0) break;
 
        Opt tt[] = {
            {"Subdermal Weave. Light armor. Cheap on the mind.", K_NEUTRAL, 1},
            {"Kereznikov boosters. Faster than anyone should be.", K_NEUTRAL, 1},
            {"Mantis Blades. Nothing else does this job.", K_VIOLENT, 1},
            {"Never mind.", K_KIND, 1}
        };
        int tc = ASK(tt); if(tc < 0) return -1;
        if(tc == 3) continue;
        ChromeTier tier = (ChromeTier)tc;
 
        int merc_ok = HAS(F_SCAVENGED_MERC) && !HAS(F_REN_SCAV_WEAVE) && !HAS(F_MERC_WEAVE_USED);
        int cut_ok  = HAS(F_TOOK_SCAV_CUT) && !HAS(F_SCAV_CUT_USED);
        int scav_ok = (tier == TIER_LIGHT) ? (merc_ok || cut_ok) : cut_ok;
 
        Opt ss[] = {
            {"Dr. Okafor installs it. Gentlest on the mind, and she has to trust you.", K_KIND, ACTIVE(OKAFOR) && who != OKAFOR},
            {"Pico installs it. Cheap, and it goes on your debt.", K_NEUTRAL, ACTIVE(PICO) && who != PICO},
            {"Fit a scavenged implant. Free. Worst on the mind.", K_COLD, scav_ok},
            {"Back.", K_KIND, 1}
        };
        int sc = ASK(ss); if(sc < 0) return -1;
        if(sc == 3) continue;
        ChromeSource src = (ChromeSource)sc;
 
        Consent cons = CONSENT_SELF;
        if(who != REN)
        {
            Opt cc[] = {
                {"Ask them.", K_KIND, 1},
                {"Pressure them: \"We need this to survive.\"", K_COLD, 1},
                {"Strap them down and do it anyway.", K_VIOLENT, 1},
                {"Back.", K_KIND, 1}
            };
            int cq = ASK(cc); if(cq < 0) return -1;
            if(cq == 3) continue;
            cons = (Consent)(cq + 1);
        }
 
        int used_merc = (src == SRC_SCAVENGE && tier == TIER_LIGHT && merc_ok);
        if(install_chrome(who, tier, src, cons))
        {
            done++;
            if(who == REN) SET(F_REN_CHROMED);
            if(who == LEO) { SET(F_LEO_CHROMED); CLR(F_LEO_UNCHROMED); }
            if(src == SRC_SCAVENGE)
            {
                if(used_merc)
                {
                    SET(F_MERC_WEAVE_USED);
                    g.marked |= bit(who);
                    if(who == REN) SET(F_REN_SCAV_WEAVE);
                    narrate("The housing is stamped M-4. You feel better about it than you should.");
                }
                else SET(F_SCAV_CUT_USED);
            }
            if(src == SRC_FIXER) show_debt();
        }
        else if(cons == CONSENT_ASKED && g.c[who].trust < 0 &&
                (src != SRC_MEDIC || (g.c[OKAFOR].trust >= 1 && g.c[who].humanity >= 35)))
        {
            g.fragile |= bit(who);
            narrate("Refusing costs them. Whatever comes next, they'll walk into it unprotected, and everyone has seen it.");
        }
 
        if(g.c[REN].state != ST_ACTIVE) return 0;
        if(check_psychos() < 0) return -1;
    }
    return 0;
}

static int scene_e3_bench(void)
{
    narrate("Chrome is how this crew survives the next two days, and everyone in the room knows what it costs. "
            "Whatever goes in tonight goes in tonight. Three installs, at most. The night is short.");
 
    if((ACTIVE(NIX) || HAS(F_NIX_REMOTE)) && HAS(F_SCAVENGED_MERC) &&
       !HAS(F_REN_SCAV_WEAVE) && !HAS(F_MERC_WEAVE_USED))
        say_as(NIX, "If you're thinking about the merc's weave: don't. It's M-series. It's on the list. "
                    "Whoever wears it, the code can reach.");
 
    int r = chrome_bench(3);
    if(r < 0) return g.scene;
    return S_E3_PLAN;
}

// THe Heists: Three ways in and a bad fourth

static int pick_victim(void)
{
    static const int order[] = {LEO, PICO, NIX, OKAFOR};
    for(int k = 0; k < 4; k++)           /* the fragile go down first */
        if(ACTIVE(order[k]) && (g.fragile & bit(order[k]))) return order[k];
    for(int k = 0; k < 4; k++)
        if(ACTIVE(order[k]) && g.c[order[k]].implants == 0) return order[k];
    return -1;
}

static int heist_loud(void)
{
    int armed = implant_total();
    narrate("You go in through the loading dock with everything the crew is wearing. The guards at the door "
            "never finish their sentences.");
 
    for(int i = 0; i < CREW_COUNT; i++)
        if(ACTIVE(i) && g.c[i].implants > 0) lose_humanity((CharId)i, 3);
    if(g.c[REN].state != ST_ACTIVE) return 0;
    if(check_psychos() < 0) return -1;
 
    if(armed >= 3)
    {
        narrate("Chrome makes the corridor a solved problem. By the time the alarm has finished its first note, "
                "the cold room is open and nobody in the crew is bleeding. It feels good. That's the warning.");
        return 2;
    }
 
    int v = pick_victim();
    if(v < 0)
    {
        narrate("It's messier than it should be, but everyone is on their feet at the end.");
        return 2;
    }
 
    char nm[32], t0[160], t1[160], t2[160], t3[160], buf[300];
    snprintf(nm, sizeof nm, "%s", char_name((CharId)v));
    snprintf(buf, sizeof buf,
             "There weren't enough of you with steel in them. A burst catches %s across the chest in the cold room "
             "doorway, and they go down with a sound like a sack dropped on tile.", nm);
    narrate(buf);
    if(g.fragile & bit(v)) narrate("They refused the chrome. Everyone remembers that now.");
    if(!ACTIVE(OKAFOR)) narrate("There is nobody on this crew who can close a wound like that. Only steel can.");
 
    snprintf(t0, sizeof t0, "Slot a dead guard's implant into %s. No time to ask.", nm);
    snprintf(t1, sizeof t1, "Ask %s. Chrome or bleed, their call.", nm);
    snprintf(t2, sizeof t2, "Get Dr. Okafor working on %s. No chrome. It takes time.", nm);
    snprintf(t3, sizeof t3, "Leave %s. The vault is thirty seconds out.", nm);
    Opt o[] = {
        {t0, K_VIOLENT, 1},
        {t1, K_KIND, 1},
        {t2, K_KIND, ACTIVE(OKAFOR) && v != OKAFOR},
        {t3, K_COLD, 1}
    };
    int c = ASK(o); if(c < 0) return -1;
 
    int loot = 2;
    int asked_refused = 0;
    if(c == 0)
    {
        install_chrome((CharId)v, TIER_MID, SRC_SCAVENGE, CONSENT_FORCED);
        narrate("It goes in while they're unconscious. The bleeding stops. They'll remember who did it.");
    }
    else if(c == 1)
    {
        if(install_chrome((CharId)v, TIER_MID, SRC_SCAVENGE, CONSENT_ASKED))
            narrate("They nod. You do it while they're still holding your sleeve.");
        else
        {
            g.fragile |= bit(v);
            asked_refused = 1;
        }
    }
    if(c == 2 || asked_refused)
    {
        if(ACTIVE(OKAFOR) && v != OKAFOR)
        {
            react(OKAFOR, +1);
            loot = 1;
            narrate("Okafor works without a single implant. It takes four minutes you don't have, and half the cold room "
                    "is cleared out by the time she's finished. But the person on the floor is still themselves.");
        }
        else
        {
            g.c[v].state = ST_DEAD;
            narrate("They refuse, and there is nobody to save them without it. They die looking at the ceiling tiles.");
            trust_shift(LEO, -2); trust_shift(OKAFOR, -2); trust_shift(NIX, -1);
        }
    }
    else if(c == 3)
    {
        g.c[v].state = ST_DEAD;
        narrate("You leave them. You tell yourself it's arithmetic. Nobody says anything in the elevator.");
        trust_shift(LEO, -2); trust_shift(OKAFOR, -2); trust_shift(NIX, -1); trust_shift(PICO, -1);
    }
 
    if(g.c[REN].state != ST_ACTIVE) return loot;
    if(check_psychos() < 0) return -1;
    return loot;
}

static int heist_quiet(void)
{
    narrate("Nix works the building the way other people breathe. Doors unlatch a second before you reach them. "
            "Cameras look at the ceiling.");
    if(ACTIVE(NIX))
        say_as(NIX, "Cameras in eleven seconds. Nobody touches anything that blinks.");
    else
        say_as(NIX, "I'm in your ear, not in the room. Cameras in eleven seconds. Don't improvise.");
 
    narrate("Sublevel three. The cold room is open. So is the night nurse, asleep on a tablet.");
    Opt o[] = {
        {"Put her out quietly and move on.", K_COLD, 1},
        {"Lock her in the supply closet with water and a note. It costs four minutes.", K_KIND, 1}
    };
    int c = ASK(o); if(c < 0) return -1;
 
    if(c == 0)
    {
        trust_shift(NIX, +1);
        trust_shift(OKAFOR, -1);
        narrate("It's clean. It's also a stranger's unconscious body, and you step over her on the way to the vault.");
    }
    else
    {
        trust_shift(OKAFOR, +1);
        trust_shift(NIX, -1);
        narrate("Four minutes on the clock. Nix swears in your ear the whole time and holds the cameras anyway.");
    }
    return 2;
}

static int heist_inside(void)
{
    narrate("No guns, no hacks. A man in formals and a lens that says he belongs.");
    if(ACTIVE(NIX) || HAS(F_NIX_REMOTE))
        say_as(NIX, "Every time that lens talks to the building, it talks to Arasaka. You know that, right?");
    say_as(LEO, "Just act like you're supposed to be here. I do this every day.");
 
    Opt o[] = {
        {"Let Leo lead. He knows how these buildings breathe.", K_KIND, 1},
        {"Take point yourself. You don't want his lens talking to anyone.", K_NEUTRAL, 1}
    };
    int c = ASK(o); if(c < 0) return -1;
 
    SET(F_ARASAKA_PINGED);     /* either way, the lens has touched the tower */
    if(c == 0)
    {
        trust_shift(LEO, +1);
        narrate("Leo's lens chimes at the security desk and the gate opens as if it were his. He doesn't look at you. "
                "He doesn't have to. For once, he is the one who knows how this works.");
        narrate("In the corner of his eye, amber. Somewhere above you, a server logs his badge at a place he has no reason to be.");
        return 2;
    }
    SET(F_INJURED);
    narrate("The desk runs your face against the bounty feed before you reach the gate. Leo's lens chimes. "
            "You take a round through the turnstile and come out the other side of the vault with half of what you came for.");
    return 1;
}

static int heist_bluff(void)
{
    narrate("You walk in the front door like a man with an appointment. Hardship review, you tell the desk. "
            "Petrochem loves a debtor who wants to talk about paying.");
    narrate("For six minutes it works. Then the desk runs your face against the bounty feed.");
    SET(F_INJURED);
    SET(F_HEIST_BLUFF);
    narrate("You grab a cooler off the nearest rack and run. Something in the dark catches your leg. "
            "You make it out with a single case of suppressant, and a penalty notice rising in the corner of your vision.");
    return 1;
}

static int scene_e3_plan(void)
{
    narrate("Petrochem Medical Depot 9 sits under a forty-storey Petrochem tower. Cold storage on sublevel three. "
            "Two guards on the dock, a night nurse inside, cameras on every corner. The same company whose name is on your debt.");
 
    Opt o[] = {
        {"Go in loud. Whoever's chromed leads. (Somebody has to have steel in them.)", K_VIOLENT, implant_total() >= 1},
        {"Go in quiet. Nix runs the building. (Needs Nix.)", K_NEUTRAL, ACTIVE(NIX) || HAS(F_NIX_REMOTE)},
        {"Inside job. Walk in on Leo's Arasaka badge. (His lens has to still be in.)", K_NEUTRAL, ACTIVE(LEO) && !HAS(F_LEO_OPTICS_OUT)},
        {"Bluff the front desk. Alone. Nobody likes this plan.", K_COLD, 1}
    };
    int c = ASK(o); CHECK(c);
 
    int loot;
    if(c == 0)      loot = heist_loud();
    else if(c == 1) loot = heist_quiet();
    else if(c == 2) loot = heist_inside();
    else            loot = heist_bluff();
    if(loot < 0) return g.scene;
 
    if(loot >= 2) { SET(F_HEIST_FULL); SET(F_HAS_CHROME_CRATE); }
    else if(loot == 1) SET(F_HEIST_PARTIAL);
    return S_E3_AFTER;
}

//aftermath

static int therapy(int doses)
{
    int by_medic = ACTIVE(OKAFOR);
    int by_ren = HAS(F_OK_HELPED_PATIENT);
 
    if(!by_medic && !by_ren)
    {
        narrate("The suppressant sits in a cooler. Nobody here trusts their hands with it. "
                "Stacked wrong, it kills. You watched it do that to a man.");
        return 0;
    }
    if(!by_medic)
        narrate("No Dr. Okafor. But you saw the stacked dose before her nurse did. You know the numbers. "
                "You'll get about half of what she would.");
 
    int amt = by_medic ? 12 : 6;
    for(int d = 0; d < doses; d++)
    {
        CharId who;
        char prompt[60];
        snprintf(prompt, sizeof prompt, "Hold the rest back. (%d dose%s left)", doses - d, doses - d == 1 ? "" : "s");
        ui_print(COL_NOTE, "\n Who gets a dose of suppressant?\n");
        int pc = pick_crew(prompt, &who);
        if(pc < 0) return -1;
        if(pc == 0) break;
        heal(who, amt);
        if(who != REN) react(who, +1);
        narrate("Their hands stop shaking. They don't say thank you. They look at you like someone who's been let out of a room.");
    }
    return 0;
}

static int scene_e3_after(void)
{
    int loot = HAS(F_HEIST_FULL) ? 2 : (HAS(F_HEIST_PARTIAL) ? 1 : 0);
    char buf[300];
 
    narrate("Back at the hideout, everything you took is laid out on the floor under a single lamp. "
            "Nobody talks for a minute.");
 
    if(HAS(F_HAS_CHROME_CRATE))
        narrate("A second case, sealed: three repossessed implants in foam. Someone's last eviction notice.");
    if(HAS(F_ARASAKA_PINGED))
        narrate("Leo's lens hasn't gone quiet since the tower. Every few minutes it flickers amber and then goes dark again, "
                "like something checking a door.");
 
    /* --- the money, and the Fixer's quiet test --- */
    int claim = (loot == 2) ? 6000 : (loot == 1 && !HAS(F_HEIST_BLUFF) ? 1500 : 0);
    if(HAS(F_HEIST_BLUFF))
    {
        g.debt += 2000;
        narrate("The penalty notice resolves into a number. Petrochem would like their cooler back.");
        show_debt();
    }
    else if(claim > 0 && ACTIVE(PICO))
    {
        int skim = claim / 10;
        int before = g.debt;
        g.debt -= (claim - skim);
        snprintf(buf, sizeof buf, "I fenced the surplus. Petrochem's counter just dropped by %d. You're welcome.", claim);
        say_as(PICO, buf);
 
        Opt o[] = {
            {"Open the Petrochem counter and read it properly.", K_NEUTRAL, 1},
            {"Take her word for it and move on.", K_KIND, 1}
        };
        int c = ASK(o); CHECK(c);
        if(c == 0)
        {
            ui_print(COL_DEBT, "\n [DEBT BEFORE: %d eb]   [DEBT NOW: %d eb]\n", before, g.debt);
            narrate("The counter moved by less than she said. Not by much. Just enough.");
            say_as(PICO, "...Good eye. Most people never look. Keep looking.");
            g.debt -= skim;
            SET(F_PICO_CAUGHT);
            react(PICO, +1);
            show_debt();
        }
        else
        {
            SET(F_PICO_MARK);
            set_trust(PICO, g.c[PICO].trust - 1);
            show_debt();
        }
    }
    else if(claim > 0)
    {
        g.debt -= claim;
        narrate("Someone on the crew fences the surplus before dawn.");
        show_debt();
    }
 
    /* --- suppressant: recovery costs something, and it's rationed --- */
    int doses = loot == 2 ? 2 : (loot == 1 ? 1 : 0);
    if(doses > 0)
    {
        if(therapy(doses) < 0) return g.scene;
    }
 
    /* --- the hidden event: someone desperate chromes themselves behind your back --- */
    int leo_desperate = ACTIVE(LEO) && !HAS(F_LEO_SECRET_CHROME) &&
        ((g.fragile & bit(LEO)) || HAS(F_LEO_FRAGILE) || g.c[LEO].trust <= -2);
    if(leo_desperate)
    {
        SET(F_LEO_SECRET_CHROME);
        g.c[LEO].humanity -= 9;
        g.c[LEO].implants++;
    }
 
    /* --- debrief: the fraying is visible because the lines are rendered through real Humanity --- */
    narrate("Someone makes tea. Nobody drinks it. You go around the room because you have to.");
    say_as(REN, "We're alive. That's what matters. We're all alive.");
    if(ACTIVE(LEO))    say_as(LEO, "We did it. We actually did it. I keep replaying it in my head.");
    if(ACTIVE(PICO))   say_as(PICO, "Cleanest score of my life. Tell me somebody else is shaking.");
    if(ACTIVE(NIX))    say_as(NIX, "Logs wiped. Don't thank me. Pay me.");
    if(ACTIVE(OKAFOR)) say_as(OKAFOR, "Hands out. All of you. I'm counting who's still yours.");
 
    if(HAS(F_LEO_SECRET_CHROME))
        narrate("When Leo reaches for the cup, there's a thin clean line across the back of his wrist that wasn't there yesterday. "
                "He pulls his sleeve down before you can ask.");
 
    int worst_stage = 0;
    for(int i = 0; i < CREW_COUNT; i++)
        if(ACTIVE(i) && stage_of(g.c[i].humanity) > worst_stage) worst_stage = stage_of(g.c[i].humanity);
    if(worst_stage >= 1)
        narrate("Somewhere in the debrief a word goes wrong. You don't find out whose.");
 
    /* --- who will sell you out in Episode 4: whoever you've alienated most --- */
    int worst = -1;
    for(int i = 1; i < CREW_COUNT; i++)
    {
        if(!ACTIVE(i)) continue;
        if(worst < 0 || g.c[i].trust < g.c[worst].trust) worst = i;
    }
    g.betrayer = worst;
 
    narrate("You wake at 4:12 a.m. to a soft click. Someone is typing under a blanket, the screen dimmed to nothing. "
            "By the time you sit up, it's dark, and everyone is asleep.");
 
    return S_E4_STUB;
}

// Episode 4: STUB 

static int scene_e4_stub(void)
{
    ui_print(COL_TITLE, "\n=====================================\n"
                        "  END OF THE EPISODE 3 SLICE\n"
                        "  Next: Episode 4 - FAULT LINE\n"
                        "  To be continued..."
                        "=====================================\n");
    return S_QUIT;
}

static int scene_end_nobody(void){
    const char *line = "The shard broadcasts anyway, from a corrupted signal, in a dead city, to no one at all...";
    char buf[256];
    ui_print(COL_WARN, "\n === FAULT LINE === \n\n");
    for(int st = 1; st<=4; st++)
    {
        render_corrupted(line, st, g.seed, buf, sizeof buf);
        ui_type(buf, st >= 3 ? COL_GLITCH : COL_NARR, 0, st);
        ui_print("", "\n");
    }
    return S_QUIT;
}
 
int story_run(int scene) {
    /* Episodes 1-3 are written from Ren's point of view only. Hand-off variants
     * are Episode 3-4 work, so if the player is already someone else, stop
     * cleanly instead of running Ren's scenes with the wrong protagonist. */
    if (scene <= S_E3_AFTER && g.protagonist != REN) {
        puts("\n  [DEV NOTE] Hand-off happened inside Episodes 1-3. Variants for this "
             "scene aren't written yet.");
        return S_E4_STUB;
    }
    switch(scene) {
        case S_E1_CLINIC  :      return scene_clinic();
        case S_E1_MERC    :      return scene_merc();
        case S_E1_FIRE    :      return scene_fire();
        case S_E1_LEO     :      return scene_leo();
        case S_E1_ROOF    :      return scene_roof();
        case S_E2_HUB     :      return scene_e2_hub();
        case S_E2_MAP     :      return scene_e2_map();
        case S_E2_NIX     :      return scene_e2_nix();
        case S_E2_OKAFOR  :      return scene_e2_okafor();
        case S_E2_AMBUSH  :      return scene_e2_ambush();
        case S_E2_DECRYPT :      return scene_e2_decrypt();
        case S_E3_HIDEOUT :      return scene_e3_hideout();
        case S_E3_BENCH   :      return scene_e3_bench();
        case S_E3_PLAN    :      return scene_e3_plan();
        case S_E3_AFTER   :      return scene_e3_after();
        case S_E4_STUB    :      return scene_e4_stub();
        case S_END_NOBODY :      return scene_end_nobody();
        default           :      return S_QUIT;   
 
    }
 
}




