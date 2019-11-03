#include <stdio.h>
#include <stdlib.h>

int main(int argc,char** argv){
	FILE* f = fopen(argv[1],"ab+");
	if(0==f){
		return 1;
	}

	fseek(f,0,SEEK_END);

	int len = ftell(f);

	fseek(f,0,SEEK_SET);
	
	char* buf = (char*)malloc(len+1);
	fread(buf,1,len,f);
	
	
	int n = 0;

	for(int i=0;i<len;i++){
		if(buf[i] == '\n'){
			n++;
		}
	}	
	printf("n=%d",n);
	fclose(f);
	return 0;
}
