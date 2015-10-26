#include "global.h"

int readn(int fd, char *vptr, int n)
{
   int    nleft;
    int   nread;
    char    *ptr;
    ptr = vptr;
    nleft = n;
    while (nleft > 0) 
	{
        if ( (nread = read(fd, ptr, nleft)) < 0) 
		{
            if (errno == EINTR)
                nread = 0;
            else
                return(ERROR); 
        } else if (nread == 0)
            break;  
        nleft -= nread;
        ptr += nread;
    }
    return(n - nleft); 
}


int chunk_file(FileInfo *fileinfo) 
{
	int subFile;
    int32_t srclen=0, left_bytes = 0;
    int32_t size=0,len=0; 
    int32_t n = MAX_CHUNK_SIZE;

	unsigned char *p;
	FingerChunk *fc;
    unsigned char *src = (unsigned char *)malloc(MAX_CHUNK_SIZE*2);	 //为啥*2？每个char占2B？

    chunk_alg_init();
	if(src == NULL) {
       // err_msg1("Memory allocation failed");
	  printf("Memory allocation failed\n");
		return FAILURE;
    }

	if ((subFile = open(fileinfo->file_path, O_RDONLY)) < 0) {
	    printf("%s,%d: open file %s error\n",__FILE__, __LINE__, fileinfo->file_path);
		free((char*)src);
		return FAILURE;
	}

	while(1) 
	{
	 	if((srclen = readn(subFile, (char *)(src+left_bytes), MAX_CHUNK_SIZE)) <= 0)
		    break;

		if (left_bytes > 0){ 
			srclen += left_bytes;
			left_bytes = 0;
		} 
		  
//		printf("srclen: %d\n",srclen);


		if(srclen < MIN_CHUNK_SIZE)
		 	break;
		
		p = src;
		len = 0;
		while (len < srclen) 
		{
          	n = srclen -len;
			size = chunk_data(p, n);
			if(n==size && n < MAX_CHUNK_SIZE)
			{ 	
          		memmove(src, src+len, n );
          		left_bytes = n;
                break;
			}  
      			
			fc = fingerchunk_new();
			chunk_finger(p, size, fc->chunk_hash);
			fc->chunklen = size;
			file_append_fingerchunk(fileinfo, fc);
			
			p = p + size;
			len += size;
		}
    }//end while
	
	/******more******/
	len = 0;
	if(srclen > 0)
	    len=srclen;
	else if(left_bytes > 0)
	 	len=left_bytes;
	if(len > 0){
	 	p= src;
	 	fc = fingerchunk_new();
		chunk_finger(p, len, fc->chunk_hash);
		fc->chunklen = len;
		file_append_fingerchunk(fileinfo, fc);
	 }	
	 close(subFile);
   	 free(src);
   	 return SUCCESS;
}
