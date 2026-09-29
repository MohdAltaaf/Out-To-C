//Episode 1 only
#include <stdio.h>

#include "game.h"

#define ASK(o) choose((int)(sizeof(o) / sizeof((o)[0])), (o))
#define CHECK(c) do { if((c) < 0) return g.scene;} while (0)

static void show_debt(void) {
    printf("\n [DEBT: %d eb owed to Petrochem]\n", g.debt);
}

static int scene_clinic(void) {
    narrate("Arroyo, Santo Domingo. Twelve Hours into a shift in a clinic with no"
            "windows and no licence.\n\n"
            "On Your Bench: a gangoon's cracked optic, half a lens of flickering blue."
            "In the corner of your vision the Petrochem counter shows the same number "
            "it showed yesterday. You know it by heart.");
    
    show_debt();
    
    narrate("Dex, the owner of the eye, leans onto the counter and doesn't look at anything "
            "for longer than a second."
            "\"Just make it work. It doesn't have to be pretty\"");
    
    Opt o[] ={
        {"Fix it properly. It costs you an extra hour and the good solder", K_KIND, 1},
        {"Tell him the parts cost double. He won't check (probably)", K_COLD, 1},
        {"Fix it and quietly pocket a spare lens from the bin", K_COLD, 1}
    };
    int c = ASK(o); CHECK(c);

    if(c == 0)
    {
        SET(F_FIXED_DEX_RIGHT);
        narrate("The blue steadies into a clean white. Dex looks at you. \"...Thanks\"");
    }
    else if (c==1)
    {
        SET(F_CHEATED_DEX);
        g.debt -= 240;
        narrate("He pays without a word. The number in your debt drops, barely.."
                "You don't feel better.");
        show_debt();
    }
    else
    {
        SET(F_HAS_LENS);
        narrate("The optic now works, you also now have a lens that you stole.");
    }
    return S_E1_MERC;
}
static int scene_merc(void)
{
    narrate("2:07 a.m. The front door doesn't open. It Breaks.\n \n"
            "A woman in torn armor falls across the floor, trailing blood and a sound "
            "like a radio between stations.\n Marlo, Who owns the clinic and every excuse for it,"
            "is shouting from the back. A tag on her shoulder plate reads"
            "MILITECH ARMORED TRANSPORT. She has three minutes, maybe four before cyberpsychosis kicks in.");
    Opt o[] ={
        {"Get her on the table and try to stablize her", K_KIND, 1},
        {"Lock the door and wake Mario. You don't get paid enough for this.", K_NEUTRAL, 1},
        {"Check her implants before she goes. That chrome is worth a year of debt.", K_COLD, 1}
    };
    int c = ASK(o); CHECK(o);

    if(c == 0)
    {
        SET(F_TRIED_SAVING_MERC);
        narrate("Your hands know this part even if your training doesn't. Her pulse is ... a rumor."
                "\n She grips your collar.\"Epoch Drive. Not MILITECH's. Don't let them--\" ");
        
    }
    else if(c == 1)
    {
        SET(F_WOKE_MARLO);
        narrate("Marlo comes out of the back, sees the tag, and goes gray. "
                "\n\"Whatever she brought, we did NOT see it.\""
                "\n he is already dragging a shelf against his door");
        
    }
    else
    {
        SET(F_SCAVENGED_MERC);
        narrate("You work a subdermal weave out of her forearm while her eyes track you."
                "\n She doesn't fight it. That is the part you'll remember.");
    }
    narrate("Then her deck spits white light. A dead-man protocol, older than the woman wearing it"
            ", reaches across the room and pushes something cold into the \"Diagnostic deck at your temple.\n\n"
            "A file bar crawls across your vision . EPOCH DRIVE// REMOTE TRIGGER//"
            "CLASS: NEUROLOGICAL. Then the woman's heart stops."
            "Above you, through four floors of concrete, you hear it.../n"
            "MILITECH is coming");
    return S_E1_FIRE;

}


static int scene_fire(void)
{
    if(HAS(F_WOKE_MARLO))
        narrate("Militech does not knock doors. They will make the room White burning hot before asking anything"
                "\n By the time you reach the back stairs. You see Marlo holding the door for you"
                "You'll never know if he meant to.");
    
    else
        narrate("Militech clean-up doesn't knock. They would incinerate you before asking anything"
                "\n By the time you reach the back stairs. Marlo had already fleed to his safety."
                "Valid considering you didn't wake him up.");
    
    narrate("In the service tunnel a kid with a stolen hi-vis jacket steps out of the dark,"
            "a bag over her shoulder and a price in her eyes.");
    
    g.c[PICO].state = ST_ACTIVE;

    say_as(PICO, "I see you must've pulled something big to get Militech on your back"
                 "\nI'm PICO, do you wanna get chromed up? I feel you're going to need it.");
    
    Opt o[] ={
        {"Buy the Subdermal Weave from PICO. (Her price goes into your debt. )",K_NEUTRAL, 1 },
        {"Install the Weave you pulled off the merc.", K_COLD, HAS(F_SCAVENGED_MERC) != 0},
        {"Use the skimmed lens to blind the drone overhead and slip past.", K_NEUTRAL, HAS(F_HAS_LENS) != 0},
        {"No Chrome. I'll Run.", K_KIND, 1}
    };
    int c = ASK(o); CHECK(o);

    if(c == 0)
    {
        if(install_chrome(REN, TIER_LIGHT, SRC_FIXER, CONSENT_SELF))
        {
            SET(F_REN_CHROMED);
            react(PICO, +1);
            narrate("The weave goes in cold and fast. Your skin tightens over a new, quiet weight. It works. You'd have paid more."
                    "\n Using the Weave you dodge past the MILITECH drones with ease and escape with PICO");
        }
    }
    else if(c == 1)
    {
        if(install_chrome(REN, TIER_LIGHT, SRC_SCAVENGE, CONSENT_SELF))
        {
            SET(F_REN_CHROMED);
            narrate("PICO watches you slot the dead woman's weave into your own arm and says nothing. That says plenty."
                    "\n Using the Weave you dodge past the MILITECH drones with ease and escape with PICO");

        }
    }
    else if (c == 2)
    {
        narrate("The Lens flares and teh drone's sensor whites out. For eleven seconds, eleven is enough. You both escape the drones.");
    }
    else
    {
        SET(F_INJURED);
        narrate("You Run. A piece of ceiling gets your leg on the way out. It will keep talking to you for days. PICO helps you walk.");
    }
    return S_E1_LEO;
}

static int scene_leo(void)
{
    narrate("Megabuilding H6, forty floors up, 5:40 a.m. Leo answers the door wearing formals ready to go to his job");
    g.c[LEO].state = ST_ACTIVE;
    g.c[LEO].trust = 1;
    say_as(LEO, "Ren!? What happened to you??");

    Opt o[] = {
        {"Tell him everything. The merc, the epoch shard, militech", K_KIND, 1 },
        {"Lie. A gas leak at the clinic. You guys just need a place to crash at", K_COLD, 1},
        {"Tell him the clinic got burnt down and you have to leave Arroyo tonight", K_NEUTRAL, 1}
    };
    int c = ASK(o); CHECK(o);

    if(c == 0)
    {
        SET(F_TOLD_LEO);
        react(LEO, +2);
        say_as(LEO, "Okay. OKAY. *deep breathes* yeah we gotta get out of here and fast. Both of you come with me the to the roof.");
    }
    else if (c == 1)
    {
        SET(F_HID_FROM_LEO);
        react(LEO, -2);
        narrate("A news feed is already scrolling across Leo's optic: MILITECH CLEAN-UP, Arroyo. His eyes flick to it and back to you.");
        say_as(LEO, "Right. A gas leak huh.");
        narrate("PICO smirks at you as she sees your lie getting caught. Leo seems dissapointed by your lie.");
    }
    else
    {
        SET(F_PARTIAL_TO_LED);
        say_as(LEO, "There's a LOT you're not telling me. I'm trusting you both here you gotta tell me everything later. Let's move rn. Come on both of you");
        say_as(PICO, "Oh god im hella tired.");
        say_as(LEO, "Then walk tired kid. Come on now.")
    }
    return S_E1_ROOF;
}


static int scene_roof(void)
{
    narrate("The three of you cross the rooftops from H6 to H7. Below Arroyo burns "
            "in patches, like something being crossed off a list."
            "Then Leo stops mid-step, one had at his left eye. His company issued lens just turned amber.");
    say_as(LEO, "That's wierd. It has neer done this.");
    narrate("Overhead you see a Gunmetal drone peel off the skyline and turn towards you...");
    say_as(PICO, "That's Arasaka, not MILITECH. They're not even trying to hide it now."
            "\n I can fit your brother with a weave before it gets here. Quick there's no time");
    

    Opt o[]= {
        {"Ask Leo if he wants it.", K_KIND, 1},
        {"Tell him he doesn't have a choice", K_COLD, 1},
        {"No Chrome. Cover him and run", K_NEUTRAL, 1}
    };
    int c = ASK(o); CHECK(o);

    //Continue TMRW LIne- 194 
}
