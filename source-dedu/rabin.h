#ifndef _RABIN_H
#define _RABIN_H
//The original scope is 2K-64K,,here want to ajust the tcp MSS;
// what is the best scope ?FIXME
#define MIN_CHUNK_SIZE  1000
#define MAX_CHUNK_SIZE  1400  
 void chunk_alg_init();
 void chunk_finger(unsigned char *buf, uint32_t len,unsigned char hash[]);
 int   chunk_data (unsigned char *p, int n);


#endif
