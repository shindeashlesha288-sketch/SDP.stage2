
#include <stdio.h>
#include <stdbool.h>

/* Mode field (2 bits) */
enum Mode {
    MODE0 = 0,
    MODE1 = 1,
    MODE2 = 2,
    MODE3 = 3
};

/* Documentation-only bit layout (do not cast this to integer for packing) */
struct StatusBits {
    unsigned int rsv0    : 1;  /* bit 0  */
    unsigned int rsv6    : 6;  /* bits 1..6 */
    unsigned int fault   : 1;  /* bit 7  */
    unsigned int busy    : 1;  /* bit 8  */
    unsigned int ready   : 1;  /* bit 9  */
    unsigned int mode    : 2;  /* bits 10..11 */
    unsigned int retries : 3;  /* bits 12..14 */
    unsigned int par     : 1;  /* bit 15 */
};

/* returns 1 if lower 15 bits have odd number of 1s, else 0 */
static int parity_bit_even15(unsigned int x15) {
    int p = 0;
    int i;
    for (i = 0; i < 15; i++) {
        p ^= (int)((x15 >> i) & 1u);
    }
    return p; /* set PAR to p so total parity over 16 bits is even */
}

int build_status_reg16(enum Mode mode,
                       int retries,
                       bool ready,
                       bool busy,
                       bool fault,
                       int *out_reg16) {
    unsigned int reg = 0u;
    unsigned int par;

    if (out_reg16 == NULL) return -1;
    if (retries < 0 || retries > 7) return -1;
    if (mode < MODE0 || mode > MODE3) return -1;

    /* Pack bits 0..14 (reserved bits remain 0) */
    reg |= ((unsigned int)retries & 0x7u) << 12;         /* bits 14:12 */
    reg |= ((unsigned int)mode    & 0x3u) << 10;         /* bits 11:10 */
    reg |= (ready ? 1u : 0u) << 9;                       /* bit 9 */
    reg |= (busy  ? 1u : 0u) << 8;                       /* bit 8 */
    reg |= (fault ? 1u : 0u) << 7;                       /* bit 7 */

    par = (unsigned int)parity_bit_even15(reg & 0x7FFFu);
    reg |= (par << 15);                                  /* bit 15 */

    *out_reg16 = (int)(reg & 0xFFFFu);
    return 0;
}

/* quick tests */
int main(void) {
    int reg, ret;

    ret = build_status_reg16(MODE0, 0, false, false, false, &reg);
    printf("1) ret=%d reg=0x%04X\n", ret, reg & 0xFFFF);

    ret = build_status_reg16(MODE3, 7, true, true, true, &reg);
    printf("2) ret=%d reg=0x%04X\n", ret, reg & 0xFFFF);

    ret = build_status_reg16(MODE1, 5, true, false, false, &reg);
    printf("3) ret=%d reg=0x%04X\n", ret, reg & 0xFFFF);

    ret = build_status_reg16(MODE1, -1, true, false, false, &reg);
    printf("4) ret=%d\n", ret);

    ret = build_status_reg16((enum Mode)4, 2, true, false, false, &reg);
    printf("5) ret=%d\n", ret);

    ret = build_status_reg16(MODE2, 3, false, true, false, NULL);
    printf("6) ret=%d\n", ret);

    return 0;
}
