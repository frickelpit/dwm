/* See LICENSE file for copyright and license details. */

/* includes */
#include "movestack.c"

/* appearance */
static const unsigned int borderpx  = 1;	/* border pixel of windows */
static const unsigned int snap      = 5;	/* snap pixel */
static const int showbar            = 1;	/* 0 means no bar */
static const int topbar             = 1;	/* 0 means bottom bar */
static const char *fonts[]          = { "Novo Sans:size=9" };
static const char dmenufont[]       = "Novo Sans:size=9";
static const char col_gray1[]       = "#101010";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#c5c8c6";
static const char col_gray4[]       = "#f8f8f2";
static const char col_cyan[]        = "#a1efe4";
static const char col_yellow[]      = "#f4bf75";
static const char col_red[]         = "#cc6666";
static const char col_magenta[]     = "#ae81ff";
static const char col_black[]       = "#75715e";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel] =  { col_red,   col_gray1, col_red  },
};

/* tagging */
static const char *tags[] = { "web", "irc", "mail", "term", "office", "media", "misc" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class            instance    title       tags mask       isfloating      monitor */
	{ "Firefox",        NULL,       NULL,       1,              0,              -1 },
	{ "Thunderbird",    NULL,       NULL,       1 << 2,         0,              -1 },
	{ "Pcmanfm",        NULL,       NULL,       1 << 6,         1,              -1 },
	{ "Keepassx2",      NULL,       NULL,       1 << 6,         1,              -1 },
	{ NULL,             NULL,       "weechat",  1 << 1,         0,              -1 },
	{ NULL,             NULL,       "mpv",      1 << 5,         0,              -1 },
};

/* layout(s) */
static const float mfact	= 0.55;	/* factor of master area size [0.05..0.95] */
static const int nmaster	= 1;	/* number of clients in master area */
static const int resizehints	= 0;	/* 1 means respect size hints in tiled resizals */
static const int clicktofocus	= 1;	/* Change focus only on click */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[T]",      tile },    /* first entry is default */
	{ "[F]",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "[-]",      bstack },
	{ "[=]",      bstackhoriz },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]	= { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_gray1, "-sf", col_red, NULL };
static const char *termcmd[]	= { "urxvtc", NULL };

/* my commands */
static const char *brightup[]		= { "xbacklight", "-inc", "5", NULL };
static const char *brightdown[]		= { "xbacklight", "-dec", "5", NULL };
static const char *browsercmd[]		= { "firefox", NULL };
static const char *filecmd[]		= { "pcmanfm", NULL };
static const char *irccmd[]		= { "urxvtc", "-title", "weechat", "-e", "weechat-curses", NULL };
static const char *mailcmd[]		= { "thunderbird", NULL };
static const char *musiccmd[]		= { "urxvtc", "-title", "ncmpcpp", "-e", "ncmpcpp", NULL };
static const char *quitcmd[]		= { "killall", "startdwm", NULL };
static const char *rangercmd[]		= { "urxvtc", "-title", "ranger", "-e", "ranger", NULL };
static const char *rebootcmd[]		= { "systemctl", "reboot", NULL };
static const char *screenshotcmd[]	= { "scrot", "-e", "mv $f ~/bilder/screenshots/ 2>/dev/null", NULL };
static const char *showmpdcmd[]		= { "/home/pit/.bin/showmpd", NULL };
static const char *shutdowncmd[]	= { "systemctl", "poweroff", NULL };
static const char *voldown[]		= { "amixer", "-q", "sset", "Master", "4%-", "unmute", NULL };
static const char *volmute[]		= { "amixer", "-q", "sset", "Master", "toggle", NULL };
static const char *volup[]		= { "amixer", "-q", "sset", "Master", "4%+", "unmute", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_0,       view,           {.ui = ~0 } },
	{ MODKEY,                       XK_Tab,     view,           {0} },
	{ MODKEY,                       XK_p,       spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return,  spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_comma,   focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period,  focusmon,       {.i = +1 } },
	{ MODKEY,                       XK_Left,    cycle,          {.i = -1 } },
	{ MODKEY,                       XK_Right,   cycle,          {.i = +1 } },
	{ MODKEY,                       XK_b,       togglebar,      {0} },
	{ MODKEY,                       XK_d,       incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_i,       incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_j,       focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,       focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_l,       setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_h,       setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_space,   setlayout,      {0} },
	{ MODKEY,                       XK_f,       setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,       setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_o,       setlayout,      {.v = &layouts[4]} },
	{ MODKEY,                       XK_t,       setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_u,       setlayout,      {.v = &layouts[3]} },
	{ MODKEY|ControlMask,           XK_Left,    tagcycle,       {.i = -1 } },
	{ MODKEY|ControlMask,           XK_Right,   tagcycle,       {.i = +1 } },
	{ MODKEY|ShiftMask,		XK_Return,  zoom,           {0} },
	{ MODKEY|ShiftMask,             XK_c,       killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_space,   togglefloating, {0} },
	{ MODKEY|ShiftMask,             XK_0,       tag,            {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_comma,   tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period,  tagmon,         {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_j,       movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,       movestack,      {.i = -1 } },


	/* my keybindings */
	{ MODKEY|ControlMask,           XK_i,           spawn,          {.v = irccmd } },
	{ 0,                            0xff61,         spawn,          {.v = screenshotcmd } },
	{ 0,                            0x1008ff02,     spawn,          {.v = brightup } },
	{ 0,                            0x1008ff03,     spawn,          {.v = brightdown } },
	{ 0,                            0x1008ff11,     spawn,          {.v = voldown } },
	{ 0,                            0x1008ff12,     spawn,          {.v = volmute } },
	{ 0,                            0x1008ff13,     spawn,          {.v = volup } },
	{ MODKEY|ControlMask,           XK_f,           spawn,          {.v = browsercmd } },
	{ MODKEY|ShiftMask,             XK_i,           spawn,          {.v = showmpdcmd } },
	{ MODKEY|ControlMask,           XK_m,           spawn,          {.v = musiccmd } },
	{ MODKEY|ShiftMask,		XK_p,		spawn,		{.v = filecmd } },
	{ MODKEY|ControlMask,           XK_q,           spawn,          {.v = quitcmd } },
	{ MODKEY|ShiftMask,             XK_r,           spawn,          {.v = rebootcmd } },
	{ MODKEY|ControlMask,		XK_r,		spawn,		{.v = rangercmd } },
	{ MODKEY|ShiftMask,             XK_s,           spawn,          {.v = shutdowncmd } },
	{ MODKEY|ControlMask,           XK_t,           spawn,          {.v = mailcmd } },

	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

