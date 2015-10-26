
#ifndef _SHA1_H_
#define _SHA1_H_

/*
 * If you do not have the ISO standard stdint.h header file, then you
 * must typdef the following:
 *    name              meaning
 *  uint32_t         unsigned 32 bit integer
 *  uint8_t          unsigned 8 bit integer (i.e., unsigned char)
 *  int32_t          integer of 32 bits
 *
 */

#ifndef _SHA_enum_        
#define _SHA_enum_
enum
{
    shaSuccess = 0,
    shaNull,            /* Null pointer parameter */
    shaInputTooLong,    /* input data too long */
    shaStateError       /* called Input after Result */
};
#endif
#define SHA1HashSize 20     //

/*
 *  This structure will hold context information for the SHA-1
 *  hashing operation
 */
typedef struct SHA1Context
{
    uint32_t Intermediate_Hash[SHA1HashSize/4]; /* Message Digest  */

    uint32_t Length_Low;            /*因为SHA1对 输入数据的长度限制是2^64，所以用2个变量保存      */
    uint32_t Length_High;           /* Message length in bits      */

			       /* Index into message block array   */
    int32_t Message_Block_Index;
    uint8_t Message_Block[64];      /* 512-bit message blocks  ，块的大小    */

    int Computed;               /* Is the digest computed?         */
    int Corrupted;             /* Is the message digest corrupted? */
} SHA1Context;

/*
 *  Function Prototypes
 */

int SHA1Init(SHA1Context *);
int SHA1Update(SHA1Context *,
	       const uint8_t *,
	       unsigned int);
int SHA1Final(SHA1Context *,
	       uint8_t Message_Digest[SHA1HashSize]);

int SHA1Buf(unsigned char *buf, unsigned int len, uint8_t Message_Digest[SHA1HashSize]);
int SHA1File(char *file, uint8_t Message_Digest[SHA1HashSize]);
 void digestToHash(unsigned char digest[20],char hash[41]);
#endif

