/*---  vonzhou
    We just send the packet of specific length no the buf
    solve the TCP stick package problem using the wait method,
   not good ,need to optimize ti TODO

*/


#include "global.h"

typedef struct {
	unsigned char fp[20];
	int chunk_id;
	int chunk_len;
	char data[MAX_CHUNK_SIZE];
}unit;

int main(int argc, char ** argv){
	int i = 0, id = 1, result, count=0;	
	int sockfd, fd;
	unit pl;
	struct sockaddr_in servaddr;
	//char unit[1500];
	char buf[MAX_CHUNK_SIZE];
	char reply[100];
	FileInfo *fi=file_new();
	FingerChunk *p;
	
	if(argc != 3)
		err_quit("usage:client <file> <serverIP>");
	
	strcpy(fi->file_path, argv[1]);
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8888);
	Inet_pton(AF_INET, argv[2], &servaddr.sin_addr);

	chunk_file(fi);
	printf("File size : %lld\n",fi->file_size);
	printf("Chunk Num : %d\n",fi->chunknum);
	p= fi->first;
	
	//after chunking complete, we sentd the wraped pkt <fp,id,chunk>	
	// after chunk_file() the file closed, we open it again for read data;
	if ((fd = open(fi->file_path, O_RDWR)) < 0){ 
		printf("%s,%d: open file %s error\n",__FILE__, __LINE__, fi->file_path);
		exit(-1);
	}

	Connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	
	while(p){
		 char hash[41];
                digestToHash(p->chunk_hash,hash);
                printf("chunklen : %d ",p->chunklen);
                printf("Fingerprint : %s \n",hash);
		for(i=0; i< 20;i++)
			pl.fp[i] = p->chunk_hash[i];// 20B fingerprint;
		pl.chunk_id = id; // 4B chunk ID
		pl.chunk_len = p->chunklen;
		// read the corresponding data from thef file;
		result = read(fd, buf, p->chunklen);
		if(result <= 0) err_quit("read file error.");
		
		strncpy(pl.data, buf, p->chunklen);	

		count =  send(sockfd,(void *)&pl, 28 + p->chunklen, 0);
		
		if(count<0) err_quit("send file failed.");
		
		/*// we get some reply from server,maybe indicate if deduplicated..
		*/
		count = read(sockfd, reply, sizeof(reply)); 
		if(count < 1) err_quit("File upload failed,bcs reply from server error.");
		//maybe we should first send the file metadata FIXME
		// we just send the chunkdata sequencely,donnot need lseek;	
		/*
		if((result = lseek(fd, p->chunklen, SEEK_CUR)) == -1)
                        err_quit("lseek failed.");
		*/
		id++;
		p = p->next;
	}
	
	close(fd);
	close(sockfd);
	file_free(fi);
}
