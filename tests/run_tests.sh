#!/usr/bin/env bash
# Build get_next_line (mandatory and bonus) with several BUFFER_SIZE values
# and compare the output against an independent reference implementation.
set -u
cd "$(dirname "$0")"
ROOT=..
OUT=$(mktemp -d)
trap 'rm -rf "$OUT"' EXIT
CFLAGS="-Wall -Wextra -Werror -I$ROOT"
VALGRIND=""
if command -v valgrind >/dev/null 2>&1 && [ "${NO_VALGRIND:-0}" != 1 ]; then
	VALGRIND="valgrind -q --leak-check=full --errors-for-leak-kinds=all --error-exitcode=42"
fi
fail=0

# Reference: "1|line" for each line ending in \n, then "0|rest" at EOF.
expected() {
	python3 - "$1" <<'PY'
import sys
data = open(sys.argv[1], 'rb').read().decode()
parts = data.split('\n')
for p in parts[:-1]:
    print('1|' + p)
print('0|' + parts[-1])
PY
}

check() {
	local name=$1; shift
	local exp=$1; shift
	local got
	got=$("$@" 2>"$OUT/stderr")
	local status=$?
	if [ "$status" -ne 0 ] || [ "$got" != "$exp" ]; then
		echo "FAIL: $name (exit=$status)"
		diff <(echo "$exp") <(echo "$got") | head -20
		head -20 "$OUT/stderr"
		fail=1
	else
		echo "OK:   $name"
	fi
}

for bs in 1 2 5 42 4096 10000000; do
	for variant in mandatory bonus; do
		if [ $variant = mandatory ]; then
			src="$ROOT/get_next_line.c $ROOT/get_next_line_utils.c"
		else
			src="$ROOT/get_next_line_bonus.c $ROOT/get_next_line_utils_bonus.c"
		fi
		bin="$OUT/gnl_${variant}_$bs"
		# shellcheck disable=SC2086
		if ! cc $CFLAGS -D BUFFER_SIZE=$bs test_gnl.c $src -o "$bin"; then
			echo "FAIL: build $variant BUFFER_SIZE=$bs"; fail=1; continue
		fi
		vg=$VALGRIND
		[ "$bs" = 10000000 ] && vg=""
		for f in files/*.txt; do
			check "$variant BUFFER_SIZE=$bs $(basename "$f")" "$(expected "$f")" $vg "$bin" "$f"
		done
		check "$variant BUFFER_SIZE=$bs stdin" "$(expected files/trailing_nl.txt)" \
			sh -c "$vg $bin /dev/stdin < files/trailing_nl.txt"
		check "$variant BUFFER_SIZE=$bs errors" \
			"$(printf 'neg_fd=-1\nbad_fd=-1\nnull_line=-1\nclosed_pipe=0')" $vg "$bin" --errors
		if [ $variant = bonus ]; then
			check "bonus BUFFER_SIZE=$bs multi-fd" \
				"$(printf 'fd0:1|line1\nfd1:1|aaaaa\nfd0:1|line2\nfd1:1|bbb\nfd0:0|\nfd1:1|\nfd1:1|cccccccccc\nfd1:0|dddd')" \
				$vg "$bin" --multi files/trailing_nl.txt files/no_trailing_nl.txt
		fi
	done
done

if [ $fail -ne 0 ]; then
	echo "Some tests FAILED"; exit 1
fi
echo "All tests passed"
