#!/bin/sh

USAGE="Usage: ./test [target] [timeout] [fuzz seed dir]"

[ -z "$1" ] && echo "$USAGE" && exit 1
TARGET="./$1.out"
[ ! -f "$TARGET" ] && echo "$TARGET not found" && exit 1

TIME="${2:-3s}"
FUZZ_SEED_DIR="${3:-fuzz_input}"
FREQ="$(grep 'freq' ../config.txt | cut -d' ' -f2)"
SEED="$(grep 'seed' ../config.txt | cut -d' ' -f2)"

OUT_DIR="./fuzz_output_$1" 
rm -rf "$OUT_DIR" "output_$1.txt"
mkdir -p "$OUT_DIR"


timeout "$TIME" ../build/fuzzer "$TARGET" "$FUZZ_SEED_DIR" "$OUT_DIR" "--freq $FREQ" --seed "$SEED" --dict "fuzzing_dict_$1" > "output_$1.txt" || :
