#include "global.h"

FileInfo* file_new()
{
	FileInfo *fileinfo = (FileInfo *)malloc(sizeof(FileInfo));
	memset(fileinfo->file_hash, 0, sizeof(Fingerprint));
	memset(fileinfo->rep_finger, 0, sizeof(Fingerprint));
	memset(fileinfo->file_path, 0, sizeof(fileinfo->file_path));
       fileinfo->chunknum = 0;
	fileinfo->is_new = -1;
	fileinfo->file_size = 0;
	fileinfo->first = NULL;
	fileinfo->last = NULL;
}

void file_free(FileInfo *fileinfo)
{
	FingerChunk *fchunk = fileinfo->first;
    while(fchunk){
        fileinfo->first = fchunk->next;
        free(fchunk);
        fchunk = fileinfo->first;    
    } 
    free(fileinfo);
}

/*
JCR* job_append_file(JCR *psJcr, FileInfo *fileinfo)
{
	fileinfo->next = NULL;
	if(psJcr->first == NULL) {
		psJcr->first = fileinfo;
	}else{
        psJcr->last->next = fileinfo;
	}
	psJcr->last = fileinfo;
	psJcr->file_count++;
	return psJcr;
}*/

FingerChunk * fingerchunk_new()
{
	FingerChunk * fc=(FingerChunk*)malloc(sizeof(FingerChunk));
	memset(fc->chunk_hash, 0, sizeof(Fingerprint));
	memset(fc->chunkaddress, 0 ,sizeof(fc->chunkaddress));
//	fc->is_new = NEW_CHUNK;
	fc->is_new= 0;
	fc->chunklen = 0;
	fc->next = NULL;
	return fc;
}

void fingerchunk_free(FingerChunk *fc)
{
	free(fc);
}


FileInfo* file_append_fingerchunk(FileInfo *fileinfo, FingerChunk *fchunk) 
{
    fchunk->next = NULL;
	if (fileinfo->first == NULL) {
		fileinfo->first = fchunk;
	}else{
        fileinfo->last->next = fchunk;
    }
	fileinfo->last = fchunk;
	fileinfo->chunknum++;
	fileinfo->file_size+=fchunk->chunklen;
	return fileinfo;
}
