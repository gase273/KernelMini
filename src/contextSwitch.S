.global _ZN3TCB13switchContextEPNS_13threadContextES1_
.type _ZN3TCB13switchContextEPNS_13threadContextES1_, @function
_ZN3TCB13switchContextEPNS_13threadContextES1_:
    sd sp, 0*8(a0)
    sd ra, 1*8(a0)

    ld sp, 0*8(a1)
    ld ra, 1*8(a1)
    ret