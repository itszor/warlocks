/* Warlocks v1.01 (Network 23, 1993): what libroger needs to know about the
 * game.
 *
 * !Run: Set War$Dir to the application, WimpSlot 512K, then run Startup (a
 * utility: every other dynamic area shrunk, 160K of screen memory, mode
 * 13, the LS title picture loaded into the second screen bank and copied
 * to the first), *FX 200 3 and *FX 247 1 (Break clears memory), betty2 (a
 * BBC font file: the game's character set, as VDU 23s) and !Runimage. That
 * is a plain absolute at &8000. It asks for disc 2, *Dir's to :0.$ and
 * loads its data (CT, WM, the levels' C, A, N, S and l files) by leaf
 * name. Disc 2's files are at the top of game/ (recipe.json), beside
 * !Warlocks, so those leaf names find them.
 *
 * WM is code as well as music: the program unpacks it (its own LZW, as
 * for every data file) into its own memory at &F514-&19F0A and calls it.
 * make regen runs the program that far (../tools/unpack) and translates
 * !Runimage with WM in place, ../unpacked/!Runimage, which is also the
 * image run here: unpacking WM again then writes the same bytes, so its
 * code stays translated rather than self-modified. */
#include <roger.h>

extern const struct roger_image recomp_image;      /* gen/entries.c */

#define APPDIR "../game"

/* <War$Dir>.x is !Warlocks.x; :0.$.x (disc 2's root) is x */
static const char *const prefixes[] = {":0.$.", NULL};
static const char *const variables[] = {"War$Dir", "!Warlocks", NULL};
static const char *const loaders[] = {"<War$Dir>.Startup", NULL};

/* Headless runs (--limit=N): Space answers "insert disc two" (key numbers
 * are INKEY's: Space 99) */
static const struct roger_key keys[] = {{99, 2000000, 3000000}};

/* the rest of !Run, before !Runimage */
static void init(void)
{
    roger_cli("FX 200 3");
    roger_cli("FX 247 1");
    roger_cli("Run <War$Dir>.betty2");
}

static struct roger_config config = {
    .title = "Warlocks (recompiled)",
    .image_path = "../unpacked/!Runimage",       /* make regen: with WM unpacked into it */
    .image = &recomp_image,
    .appdir = APPDIR,
    .dir_prefixes = prefixes,
    .variables = variables,
    .directory = "<War$Dir>",                    /* !Run: Dir <War$Dir> */
    .loader_programs = loaders,
    .initial_mode = 13,                          /* Startup's */
    .wimpslot = 512 * 1024,                      /* !Run */
    .screen_size = 160 * 1024,                   /* Startup's, as !Help asks */
    /* a controller: the stick or D-pad Z, X, ' and / (VTjoystick's keys), A
     * fire (Return), B jump (' too), start Space (a key starts a game) */
    .pad_map = "left=Z,right=X,up=',down=/,a=Return,b=',start=Space",
    /* the controllers as the joystick, if one is connected: it reads
     * Joystick_Read every frame, and the keys too */
    .joystick = ROGER_JOY_ACORN,
    .keys = keys,
    .nkeys = sizeof keys / sizeof keys[0],
    .init = init,
};

int main(int argc, char **argv)
{
    roger_main(argc, argv, &config);
}
