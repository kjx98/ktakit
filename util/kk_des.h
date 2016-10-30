#ifndef __DES_H__
#define __DES_H__
/* Allocate space and initialize DES lookup arrays
 * mode == 0: standard Data Encryption Algorithm
 * mode == 1: DEA without initial and final permutations for speed
 * mode == 2: DEA without permutations and with 128-byte key (completely
 *            independent subkeys for each round)
 * return -1 on FAIL
 */
int desinit(int mode);

/* Free up storage used by DES */
void desdone(void);

/* Set key (initialize key schedule array) */
int setkey(const char *key);
/* 64 bits (will use only 56 with high bit disabled) */

/* In-place encryption of 64-bit block
 * block in block out
 */
void endes(char *block);


/* In-place decryption of 64-bit block
 * block in block out
 */
void dedes(char *block);
#endif
