#ifndef RECIPE_H_
#define RECIPE_H_

#define FILE_NAME_LEN  256
typedef unsigned char Fingerprint[20];
typedef char Chunkaddress[64];
typedef struct finger_chunk_tag {
    Fingerprint chunk_hash;
    int is_new;
    int32_t chunklen;
    Chunkaddress chunkaddress;
    struct finger_chunk_tag *next;
}FingerChunk;

typedef struct file_info_tag {
    Fingerprint file_hash;
    Fingerprint rep_finger;
    char file_path[FILE_NAME_LEN];
    int is_new;
    int64_t file_size;
    int32_t chunknum;
    FingerChunk *first;
    FingerChunk *last;
}FileInfo;


FileInfo* file_new();

void file_free(FileInfo *fileinfo);

FingerChunk * fingerchunk_new();

void fingerchunk_free(FingerChunk *fc);

FileInfo* file_append_fingerchunk(FileInfo *fileinfo, FingerChunk *fchunk);



#endif /* RECIPE_H_ */


