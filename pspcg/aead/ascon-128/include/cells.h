#ifndef CELLS_H_
#define CELLS_H_

#define MDTYPE int

/* architecture-specific */
#define AND2(a,b,r) {r=a&b;}  //                              \
    __asm volatile("and %[r_], %[a_], %[b_]\n\t"              \
        : [r_]"=r" (r)                                      \
        : [a_] "r" (a), [b_] "r" (b) :);                    \
}

#define OR2(a,b,r) {  r=a|b;} //                                     \
    asm volatile("orr %[r_], %[a_], %[b_]\n\t"              \
        : [r_]"=r" (r)                                      \
        : [a_] "r" (a), [b_] "r" (b) :);                    \
}

#define BIC2(a,b,r) { r = a & ~(b);} //                                      \
    asm volatile("bic %[r_], %[a_], %[b_]\n\t"              \
        : [r_]"=r" (r)                                      \
        : [a_] "r" (a), [b_] "r" (b) :);                    \
}

#define XOR2(a,b,r) { r = a^ b;}
#define ORN2(a,b,r) { r = a | (~b);}

#define NOT1(a,r) { r = ~a;}

/* general */

#define BUF1(a,r) { r = a;}

#define DFF(clk,d,q) { q = d; }

#define DFFSR(clk,d, q, rst, set) {q = rst ? 0 : (set ? 1 : d);}

void ascon(MDTYPE clk, MDTYPE rst, MDTYPE* npub, MDTYPE* key, MDTYPE* bdi, MDTYPE* exp_tag, MDTYPE key_ready, MDTYPE* key_updated, MDTYPE key_needs_update, MDTYPE npub_ready, MDTYPE* npub_read, MDTYPE bdi_ready, MDTYPE bdi_proc, MDTYPE bdi_ad, MDTYPE* bdi_wait, MDTYPE bdi_decrypt, MDTYPE bdi_eot, MDTYPE bdi_eoi, MDTYPE* bdi_read, MDTYPE* bdi_size, MDTYPE exp_tag_ready, MDTYPE bdo_ready, MDTYPE* bdo_write, MDTYPE* bdo, MDTYPE* bdo_size, MDTYPE tag_ready, MDTYPE* tag_write, MDTYPE* tag, MDTYPE* msg_auth_done, MDTYPE* msg_auth_valid);


#endif /* CELLS_H_ */
