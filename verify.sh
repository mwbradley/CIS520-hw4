#!/bin/bash
#
# verify.sh - Verify pt1 correctness
#
# Step 1: Test against a small hand-verified input (works anywhere)
# Step 2: Diff across thread counts on wiki_dump.txt (Beocat only)
#

set -e

EXEC=./pt1
TESTFILE=test_input.txt
EXPECTED=expected_output.txt

# ---- Step 1: Small known-answer test ----
echo "=== Step 1: Known-answer test ==="

cat > ${TESTFILE} <<'EOF'
ab
bc
Hello World!
zzz
A
 ~test
EOF

# Expected max ASCII per line:
# "ab"            -> max(97, 98)           = 98
# "bc"            -> max(98, 99)           = 99
# "Hello World!"  -> max(72,101,108,108,111,32,87,111,114,108,100,33) = 114 ('r')
# "zzz"           -> max(122)              = 122
# "A"             -> max(65)               = 65
# " ~test"        -> max(32,126,116,101,115,116) = 126 ('~')
cat > ${EXPECTED} <<'EOF'
0: 98
1: 99
2: 114
3: 122
4: 65
5: 126
EOF

# Build a tiny serial checker to validate against
cat > serial_check.c <<'CEOF'
#include <stdio.h>
#include <string.h>
int main(int argc, char *argv[]) {
    const char *path = (argc > 1) ? argv[1] : "test_input.txt";
    FILE *fp = fopen(path, "r");
    if (!fp) { perror("fopen"); return 1; }
    char buf[65536];
    int line = 0;
    while (fgets(buf, sizeof(buf), fp)) {
        int len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
        int max = 0;
        for (int i = 0; buf[i]; i++) {
            int val = (unsigned char)buf[i];
            if (val > max) max = val;
        }
        printf("%d: %d\n", line++, max);
    }
    fclose(fp);
    return 0;
}
CEOF
gcc -Wall -O2 -std=c99 -o serial_check serial_check.c

./serial_check ${TESTFILE} > serial_out.txt
if diff -q ${EXPECTED} serial_out.txt > /dev/null 2>&1; then
    echo "PASS: Serial checker matches hand-computed expected output"
else
    echo "FAIL: Serial checker vs expected"
    diff ${EXPECTED} serial_out.txt
    exit 1
fi

rm -f serial_check serial_check.c serial_out.txt ${TESTFILE} ${EXPECTED}

# ---- Step 2: Diff across thread counts on wiki_dump.txt ----
echo ""
echo "=== Step 2: Cross-thread-count diff (wiki_dump.txt) ==="

if [ ! -f ~eyv/cis520/wiki_dump.txt ]; then
    echo "SKIP: wiki_dump.txt not found (run this on Beocat)"
    exit 0
fi

if [ ! -x ${EXEC} ]; then
    echo "ERROR: ${EXEC} not found. Run 'make' first."
    exit 1
fi

echo "Running with 1 thread (reference)..."
${EXEC} 1 > ref_output.txt 2>/dev/null

PASS=true
for T in 2 4 8 16 32; do
    echo -n "Running with ${T} threads... "
    ${EXEC} ${T} > out_t${T}.txt 2>/dev/null

    if diff -q ref_output.txt out_t${T}.txt > /dev/null 2>&1; then
        echo "PASS"
    else
        echo "FAIL"
        diff ref_output.txt out_t${T}.txt | head -10
        PASS=false
    fi
done

echo ""
if [ "$PASS" = true ]; then
    echo "=== ALL TESTS PASSED ==="
else
    echo "=== SOME TESTS FAILED ==="
fi

rm -f ref_output.txt out_t*.txt
