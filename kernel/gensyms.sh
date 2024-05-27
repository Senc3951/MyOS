#!/bin/sh

set -e

TMP1=$(mktemp)
TMP2=$(mktemp)
TMP3=$(mktemp)

objdump -t ../output/kernel.bin | sed '/\bd\b/d' | sort > "$TMP1"
grep "\.text" < "$TMP1" | cut -d' ' -f1 > "$TMP2"
grep "\.text" < "$TMP1" | awk 'NF{ print $NF }' > "$TMP3"

cat <<EOF > ksyms.c
//
// AUTO GENERATED FILE.
// DO NOT EDIT THIS FILE.
//

#include <symbols.h>

const symbol_t _kernel_symbols[] = {
EOF

paste -d'$' "$TMP2" "$TMP3" | sed 's/^/    { .addr = 0x/g' | sed 's/\$/, .name = "/g' | sed 's/$/"},/g' >> ksyms.c

cat <<EOF >> ksyms.c
    { .addr = 0xffffffff, .name = ""}
};
EOF

rm -rf "$TMP1" "$TMP2" "$TMP3"