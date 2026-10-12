#!/bin/sh
# Build Warlocks from your own copy of the discs:
#     ./build.sh "Warlocks (1993)(Network 23)(Disk 1 of 2).adf" "...(Disk 2 of 2).adf"
# then: cd recomp && ./warlocks
# The steps are recipe.json's "build", run by SuperSpider's tools/weave.py.
set -e
HERE="$(cd "$(dirname "$0")" && pwd)"
SUPERSPIDER="${SUPERSPIDER:-$HOME/stuff/superspider}"
[ $# -gt 0 ] || { echo "usage: $0 DISC1.adf DISC2.adf | GAME.zip" >&2; exit 2; }
(cd "$SUPERSPIDER" && dune build)
WEAVE_RECIPES="$HERE" exec python3 -I "$SUPERSPIDER/tools/weave.py" build "$@"
