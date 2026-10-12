/* Warlocks' code that isn't in !Runimage: the program loads WM from disc 2
 * with its own LZW unpacker (&BE18) into its own memory and calls it
 * (&B52C: bl &F514). This runs the program with libroger's ARM2
 * interpreter, files and keys as the game has them (recomp/game.c), until
 * control first arrives at --stop; then it writes !Runimage with the bytes
 * the program has unpacked into [R1, R2) (the destination and end
 * &BE18 leaves) put in place: the image that actually runs, for
 * SuperSpider to translate.
 *
 *   unpack IMAGE --stop=ADDR --write=OUT
 *
 * Nothing is translated, so the run takes a few seconds. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <roger.h>

static uint8_t nocode[0x80000 / 32];
static struct roger_image image = {0x8000, 0x8000 + 0x80000, NULL, 0, nocode, 'P'};

static uint32_t stop = ~0u;
static const char *in, *out;

static int arg(const char *a)
{
    if (!strncmp(a, "--stop=", 7)) { stop = strtoul(a + 7, 0, 16); return 1; }
    if (!strncmp(a, "--write=", 8)) { out = a + 8; return 1; }
    return 0;
}

static int stop_hook(uint32_t a)
{
    if (a != stop || !icount) return 0;
    uint32_t from = R[1], to = R[2];
    printf("stopped at %08x after %llu instructions: unpacked %08x-%08x\n", a,
           (unsigned long long)icount, from, to);
    if (from < image.base || to > image.end || from >= to) { fprintf(stderr, "unpack: not inside the image\n"); exit(1); }
    FILE *f = fopen(in, "rb");
    static uint8_t buf[0x80000];
    size_t n = f ? fread(buf, 1, sizeof buf, f) : 0;
    if (f) fclose(f);
    memcpy(buf + (from - image.base), MEM + from, to - from);
    if (!(f = fopen(out, "wb"))) { perror(out); exit(1); }
    fwrite(buf, 1, n, f);
    fclose(f);
    printf("wrote %s\n", out);
    exit(0);
}

/* as recomp/game.c */
static const char *const prefixes[] = {":0.$.", NULL};
static const char *const variables[] = {"War$Dir", "!Warlocks", NULL};
static const struct roger_key keys[] = {{99, 2000000, 3000000}};   /* Space: "insert disc two" */

static struct roger_config config = {
    .title = "unpack",
    .image = &image,
    .appdir = "../game",
    .dir_prefixes = prefixes,
    .variables = variables,
    .directory = "<War$Dir>",
    .initial_mode = 13,
    .wimpslot = 512 * 1024,
    .screen_size = 160 * 1024,
    .keys = keys,
    .nkeys = sizeof keys / sizeof keys[0],
    .interpret_gaps = 1,
    .arg = arg,
    .call_addr = stop_hook,
    .usage = "  --stop=ADDR --write=FILE   (hex address)\n",
};

int main(int argc, char **argv)
{
    for (int k = 1; k < argc; k++)
        if (arg(argv[k])) ;
        else if (argv[k][0] != '-') {                        /* the file: the (empty) image's extent */
            struct stat st;
            in = argv[k];
            if (!stat(in, &st) && st.st_size <= (long)sizeof nocode * 32) image.end = 0x8000 + st.st_size;
        }
    if (!in || !out) { fprintf(stderr, "usage: unpack IMAGE --stop=ADDR --write=OUT\n"); return 2; }
    roger_main(argc, argv, &config);
}
