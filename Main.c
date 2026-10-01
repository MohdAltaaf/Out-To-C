/*
 *
 *   ./fault_line                  play
 *   ./fault_line --debug          print hidden humanity/trust after each scene
 *   ./fault_line --humanity N     start Ren at humanity N (test corruption stages)
 *   ./fault_line --demo           print the corruption ladder and exit
 *   ./fault_line --fast           no typewriter effect
 *   ./fault_line --nocolor        no colors
 *
 * 'S' to save, 'l' to load and 'q' to quit at any prompt.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>

 #include "game.h"

 static void corruption_demo(void) {
    static const char *LINES[] = {
        "I'm fine. Really. We should keep moving before they find us.",
        "Don't worry about me. I have the door, you get Leo out of here"
    };

    static const int LEVELS[] ={ 100, 65, 30, 10, 0};
    static const char *LABELS[] = {"Stable", "Frayed", "Fractured", "Edge", "Break"};
    char out[1024];

    game_new();
    for (size_t l = 0; l < sizeof LINES / sizeof *LINES; l++) {
        printf("\nSOURCE: %s\n", LINES[l]);
        for (size_t i = 0; i < sizeof LEVELS / sizeof *LEVELS; i++) {
            int st = stage_of(LEVELS[i]);
            render_corrupted(LINES[l], st, g.seed, out, sizeof out);
            printf("  humanity %3d  %-9s %s\n", LEVELS[i], LABELS[st], out);
        }
    }

 }

 int main(int argc, char **argv) {
    int humanity = -1;
    int fast = 0, nocolor = 0;

    for(int i = 1; i < argc; i++)
    {
        if(!strcmp(argv[i], "--debug"))             g_debug = 1;
        else if(!strcmp(argv[i], "--demo"))         {corruption_demo(); return 0;}
        else if(!strcmp(argv[i], "--fast"))         fast = 1;
        else if(!strcmp(argv[i], "--nocolor"))      nocolor = 1;
        else if(!strcmp(argv[i], "--humanity") && i +1 < argc)  humanity = atoi(argv[++i]);
    }
    ui_init(fast, nocolor);

    ui_print(COL_TITLE, "NIGHT CITY: FAULT LINE\n");
    ui_print(COL_NOTE, "(type 's' to save, 'l' to load, 'q' to quit at any prompt)\n");
    ui_print(COL_NOTE, "(press Enter while text is typing to skip it)\n\n");

    char b[16];
    ui_print(COL_OPT, "[1] New game     [2] Continue\n");
    ui_print(COL_NUM, ">");
    game_new();
    if(fgets(b, sizeof b, stdin) && b[0] == '2') {
        if(!load_game()) game_new();
    }
    else if(humanity >= 0) {
        g.c[REN].humanity = humanity;
    }


    while(g.scene != S_QUIT) {
        int next = story_run(g.scene);
        g.scene = next;
        handoff_if_needed();
        if(g_debug) debug_dump();
    }
    return 0;
 }