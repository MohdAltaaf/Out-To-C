#define _POSIX_C_SOURCE 200809L

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include <io.h>
#else
#include <sys/select.h>
#include <unistd.h>
#endif

#include "game.h"

#define UI_WIDTH 72

#ifdef _WIN32
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

static int g_color = 0;     //ansi colors on/off
static int g_typing = 0;    //typewriter on/off
static int g_can_skip = 0;  //only check the keyboard when a real person is typing
static int ui_col = 0;      //which column the cursor is on, needed for word wrap

//speaker colors
static const char *CHAR_COLORS[CREW_COUNT] = {
    "\x1b[96m",   //Ren    bright cyan
    "\x1b[92m",   //Leo    bright green
    "\x1b[95m",   //Nix    bright magenta
    "\x1b[94m",   //Okafor bright blue
    "\x1b[93m"    //Pico   bright yellow (the hi-vis jacket)
};

const char *ui_char_color(CharId who) { return CHAR_COLORS[who]; }

static void sleep_ms(int ms)
{
#ifdef _WIN32
    Sleep((DWORD)ms);
#else
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (long)(ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
#endif
}

#ifdef _WIN32
static int enable_vt(void)
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if(h == INVALID_HANDLE_VALUE || !GetConsoleMode(h, &mode)) return 0;
    return SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
}
#endif

void ui_init(int fast, int nocolor)
{
    int out_tty, in_tty;
#ifdef _WIN32
    out_tty = _isatty(_fileno(stdout));
    in_tty = _isatty(_fileno(stdin));
    if(out_tty && !enable_vt()) nocolor = 1;
#else
    out_tty = isatty(STDOUT_FILENO);
    in_tty = isatty(STDIN_FILENO);
#endif
    g_color = out_tty && !nocolor;
    g_typing = out_tty && !fast;
    g_can_skip = g_typing && in_tty;
    srand((unsigned)time(NULL));
}

//a key press skips the typing for the rest of the current block
static int skip_pressed(void)
{
#ifdef _WIN32
    if(_kbhit()) { _getch(); return 1; }
    return 0;
#else
    fd_set fds;
    struct timeval tv = {0, 0};
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    if(select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0)
    {
        char tmp[64];
        if(!fgets(tmp, sizeof tmp, stdin)) return 1;
        return 1;
    }
    return 0;
#endif
}

//how long to wait after each character, like undertale:
//pauses on punctuation, and glitchy uneven speed when the speaker is corrupted
static int char_delay(char c, char next, int stage)
{
    int end = (next == 0 || next == ' ' || next == '\n' || next == '"');

    if(c == '.' && next == '.') return 110;
    if((c == '.' || c == '!' || c == '?') && end) return stage >= 2 ? 120 : 260;
    if((c == ',' || c == ';' || c == ':') && end) return stage >= 2 ? 50 : 110;
    if(stage >= 2) return 6 + rand() % 22;
    return 18;
}

static void put_char(char c, char next, int stage, int typing, int *skipping)
{
    putchar(c);
    ui_col++;
    if(!typing || *skipping) return;

    fflush(stdout);
    if(g_can_skip && skip_pressed()) { *skipping = 1; return; }
    sleep_ms(char_delay(c, next, stage));
}

//prints text word by word so lines never break in the middle of a word
static void type_impl(const char *s, const char *color, int indent, int stage, int typing)
{
    int skipping = 0, pending_space = 0;
    if(g_color && color && color[0]) fputs(color, stdout);

    while(*s)
    {
        if(*s == '\n')
        {
            putchar('\n');
            ui_col = 0;
            pending_space = 0;
            s++;
            if(typing && !skipping) { fflush(stdout); sleep_ms(140); }
            continue;
        }
        if(*s == ' ')
        {
            if(ui_col > 0) pending_space = 1;   //spaces at the start of a line get dropped
            s++;
            continue;
        }

        const char *e = s;
        while(*e && *e != ' ' && *e != '\n') e++;
        int wl = (int)(e - s);

        if(ui_col > indent && ui_col + pending_space + wl > UI_WIDTH)
        {
            putchar('\n');
            for(int i = 0; i < indent; i++) putchar(' ');
            ui_col = indent;
        }
        else if(pending_space)
            put_char(' ', *s, stage, typing, &skipping);
        pending_space = 0;

        for(const char *p = s; p < e; p++)
            put_char(*p, p[1], stage, typing, &skipping);
        s = e;
    }

    if(g_color && color && color[0]) fputs("\x1b[0m", stdout);
    fflush(stdout);
}

void ui_type(const char *text, const char *color, int indent, int stage)
{
    type_impl(text, color, indent, stage, g_typing);
}

//same wrapping but instant, used for the menus
void ui_wrap(const char *text, const char *color, int indent)
{
    type_impl(text, color, indent, 0, 0);
}

void ui_pause(int ms)
{
    if(g_typing) sleep_ms(ms);
}

//printf but with a color, instant
void ui_print(const char *color, const char *fmt, ...)
{
    char buf[2048];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);

    if(g_color && color && color[0]) fputs(color, stdout);
    fputs(buf, stdout);
    if(g_color && color && color[0]) fputs("\x1b[0m", stdout);

    for(const char *p = buf; *p; p++)
        ui_col = (*p == '\n') ? 0 : ui_col + 1;
}