Warlocks v1.01 (Network 23, 1993) - static recompilation
========================================================

A recipe: this repository holds no game data. It rebuilds the game as a
native executable from your own copy of the discs, recognised by content
(recipe.json), so it can be shared without distributing the game.

    ./build.sh "Warlocks (1993)(Network 23)(Disk 1 of 2).adf" "Warlocks (1993)(Network 23)(Disk 2 of 2).adf"
    cd recomp && ./warlocks

build.sh runs recipe.json's steps with SuperSpider's tools/weave.py
(~/stuff/superspider, or $SUPERSPIDER): it finds the two discs among the
inputs, extracts them, unpacks the code the program loads from disc 2
(tools/unpack), analyses and translates it, and links it with libroger
(~/stuff/libroger, or make ROGER=...). It takes under a minute. Needs
python3, OCaml/dune, gcc and SDL3.

Inputs: the TOSEC set's two disc images, `(Disk 1 of 2).adf` (the
program) and `(Disk 2 of 2).adf` (the data). The set's third file,
`Warlocks (1993)(Network 23).zip`, isn't a zip: it's a Spark archive
(SuperSpider's tools/spark.py reads it) of a cracked hard-disc version,
v1.01+ ("Crackers Nightmare ... took me 15 min", says its !Run). Its data
files are disc 2's, byte for byte, but its !RunImage is re-packed and
encrypted, and it has a different loader (TitlePict, Restore), so the
recipe doesn't use it.

What it makes:

    extracted/disc1, disc2   $SS/tools/adf_extract.py's output, via weave.py
    game/                    !Warlocks (disc 1) and disc 2's files beside it:
                             libroger's appdir
    unpacked/!Runimage       !Runimage with WM unpacked into it (tools/unpack)
    analysis/warlocks.json   SuperSpider's analysis
    recomp/gen/              its C translation
    recomp/warlocks          the game

Committed: recipe.json, build.sh, tools/ (unpack.c, Makefile), recomp/
(Makefile, game.c), this file.

How the game is put together
----------------------------

!Run sets War$Dir, a 512K WimpSlot and the current directory (*Dir
<War$Dir>), then runs Startup, a utility: it shrinks every other dynamic
area, gives the screen 160K, selects mode 13, loads the LS title picture
into the second screen bank and copies it into the first. *FX 200 3 and
*FX 247 1 make Break clear memory, betty2 (a BBC font file) defines the
character set, and !Runimage, a plain absolute at &8000, starts. It asks
for disc 2 ("INSERT DISC TWO AND PRESS SPACE"), sets the current
directory to :0.$ and loads its data by leaf name: CT (the title
screen), WM, and for each level its C, N, A, S and l files. Every one is
compressed, and the program unpacks it as it reads (its own LZW, &BE18).

- WM is code as well as music: it unpacks to &F514-&19F0A, over data in
  !Runimage, and the program calls it. make regen runs the program with
  libroger's interpreter that far (tools/unpack), and translates the
  image with WM in place. That image is also the one the game runs, so
  when the program unpacks WM again it writes the same bytes and the code
  stays translated.
- recomp/game.c: the rest of !Run. Startup runs interpreted as a loader
  program; *FX and betty2 in the init hook.
- Disc 2's files sit at the top of game/, beside !Warlocks, so :0.$.CT and
  the leaf names after *Dir :0.$ find them.

The play area is a 336 by 224 overscan display: the game programs VIDC's
horizontal and vertical display and border registers itself, and scrolls
by moving the display start 2 pixels at a time. Its border hides the
extra pixels; the window shows the 304 that the original's border left.
The border also covers the play area's top 16 lines (VBSR is 16 lines
below VDSR), which hold half-drawn graphics.

Everything translates: headless runs through the title, the first level,
game over (the CC screen) and new games interpret nothing but Startup.

What libroger and SuperSpider gained
------------------------------------

- libroger: a current directory (*Dir, OS_FSControl 0, and
  roger_config.directory for the one !Run leaves); OS_Exit from a loader
  program ends just that program; loader programs run interpreted even
  when interpret_gaps is off; VIDC's horizontal timing (HDSR/HDER,
  HBSR/HBER) in the displayed frame.
- SuperSpider: tools/spark.py (Spark archives) and weave.py reading them;
  "$" in a recipe's game_dirs (a whole disc into game/).

Speed and sound
---------------

The game waits for the vsync event between frames (its event handler
sets a flag it spins on), so it runs at the original's speed with no
--mips setting. Sound: its own voices through the sound system
(Sound_Configure, Sound_InstallVoice, Sound_Control). It tries the
Joystick module (Joystick_Read, X form), and finding none uses the keys.

Controls
--------

Any key on the title. Z left, X right, ' up (jump), / down (crouch),
Return fire: the keys VTjoystick lists.

A headless scenario (start, walk right firing and jumping, screenshots in
screenshot-N.bmp):

    ./warlocks --limit=4e8 --full-os --script=6000:key:99,10000:key:67:60000,\
    12000:key:74:150,14000:key:80:300,30000:shot,60000:shot

Not done
--------

- Only the first level, and game over, have been played (headlessly).
  Later levels load other C, N, A, S and l files; if one of them holds code
  too, --trace-gaps will show it (make roots, make regen).
- A one-line stripe at the bottom of the play area, just above the panel,
  in some places (e.g. after walking right for 20 seconds): it's in the
  game's own drawing (an interpreted run draws it too), not hidden by the
  border, so perhaps the original shows it as well. Unconfirmed.
- Joysticks (Joystick_Read).
- The cracked hard-disc version in the Spark archive.
