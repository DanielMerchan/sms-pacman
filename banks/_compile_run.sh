#!/bin/bash

# Stop on error
set -e

# ---------------------------------------------------------
# Build assets
# ---------------------------------------------------------
assets2banks ../assets --compile

# ---------------------------------------------------------
# Compile main.c
# ---------------------------------------------------------
sdcc -c -mz80 --opt-code-speed --peep-file peep-rules.txt ../main.c

# ---------------------------------------------------------
# Link everything into mspacman.ihx
# (line continuations use "\" on macOS)
# ---------------------------------------------------------
set +e
sdcc -o mspacman.ihx -mz80 --no-std-crt0 --data-loc 0xC000 \
    -Wl-b_BANK2=0x28000 \
    -Wl-b_BANK3=0x38000 \
    -Wl-b_BANK4=0x48000 \
    ../lib/crt0_sms.rel main.rel \
    bank2.rel \
    bank3.rel \
    bank4.rel \
    SMSlib.lib ../lib/PSGLib.rel
set -e

# ---------------------------------------------------------
# Build SMS ROM
# ---------------------------------------------------------
makesms -pp mspacman.ihx ../banks/mspacman.sms

# ---------------------------------------------------------
# Cleanup
# ---------------------------------------------------------
echo "Cleaning…"
rm -f *.lk *.map *.noi *.ihx *.rel *.lst *.sym *.asm

# ---------------------------------------------------------
# Launch Emulicious
# ---------------------------------------------------------
java -jar ../Emulicious/Emulicious.jar mspacman.sms
