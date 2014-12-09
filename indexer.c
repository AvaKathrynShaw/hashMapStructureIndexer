
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>

#include "tokenizer.c"
#include "indexer.h"





int getPosition(char *addingWord) {
	char letter;
	if(!addingWord) return -1;
	if(isalpha(addingWord[0])){
		letter = tolower(addingWord[0]);
		if (letter != '\0') {
			return (letter - 'a');
		} else {
			return -1;
		}
	}
	else if(isdigit(addingWord[0])){
		return (addingWord[0]-'0') + 26;
	}
	else{
		return -1;
	}
}
FileNode *newFNode(char *file){
	FileNode *fn = (FileNode *)malloc(sizeof(FileNode));
	fn->fName = file;
	fn->occurances =1;
	fn->next = NULL;
	return fn;
}

Node *addWord(char *addingWord) {
	addingWord = formatInput(addingWord);
	Node *temp = (Node *)malloc(sizeof(Node));
	printf("Creating node with: %s\n", addingWord);
	temp->fileOccuredIn = NULL;
	temp->data = addingWord;
	temp->next = NULL;
	temp->fileOccuredIn = NULL;
	return temp;
}


linkedList *newLinkedList() {
	linkedList *tempList = malloc(sizeof(linkedList));
	tempList->size = 0;
	return tempList;
}



h_TABLE *newTable() {
	h_TABLE *tempTable = malloc(sizeof(h_TABLE));
	tempTable->containers = malloc(sizeof(linkedList *) * 64);
	tempTable->size = 64;
	return tempTable;
}

char *formatInput(char *addingWord){
	int i;
	i=0;
	for(i = 0; i < strlen(addingWord); i++){
		addingWord[i] = tolower(addingWord[i]);
	}
	return addingWord;
}

void addToTable(h_TABLE *key, char *addingWord, char *fName){
	int i;
	i = getPosition(addingWord);
	
	if(!key){
		return;
	}
	if(i == -1){
		return;
	}
	if(key->containers[i] == NULL){
		key->containers[i] = newLinkedList();
		key->containers[i]->head = addWord(addingWord);
		key->containers[i]->size++;
		key->containers[i]->head->fileOccuredIn = newFNode(fName);
		return;
	}else{
		Node *pointer = key->containers[i]->head;

		while(pointer != NULL){
			if(strcmp(addingWord,pointer->data)== 0){
				FileNode *fpointer = pointer->fileOccuredIn;
				while(fpointer != NULL){
					if(strcmp(fName,fpointer->fName)==0){
						fpointer->occurances++;
						return;
					}
					if(fpointer->next == NULL){
						FileNode *newf = newFNode(fName);
						fpointer->next = newf;
						return;
					}
				fpointer=fpointer->next;
			}
		}
		if(pointer->next == NULL){
			Node *newOne = addWord(addingWord);
			pointer->next = newOne;
			FileNode *fn = newFNode(fName);
			newOne->fileOccuredIn = fn;
			return;
		}
		pointer = pointer->next;
		}
	}
}
char *formatSeperators(char *string){
	char *delims;
	delims = (char *)malloc(100*sizeof(char));
	int size,i,j;
	j = 0;
	size = 0;
	for(i = 0; string[i] != '\0'; i++){
		if(!isalnum(string[i])){
			if(j == 99){
				delims = realloc(delims, 2*sizeof(delims));
			}
			delims[j] = string[i];
			j++;
		}
	}
	return delims;
}

void fileParse(char *fName, h_TABLE *ftable){
	FILE *stream;
	char *contents;
	char *token;
	stream = fopen(fName, "rb+");
	if ( stream != NULL ){

    fseek(stream, 0L, SEEK_END);
    long s = ftell(stream);
    rewind(stream);
    contents = malloc(s +1);
    if ( contents != NULL )
    {
      fread(contents, s, 1, stream);
      fclose(stream); stream = NULL;
  	}
  }
  	char *sep = formatSeperators(contents);
	TokenizerT *tk = TKCreate(sep,contents);
	token = TKGetNextToken(tk);
	while(token != NULL){
		addToTable(ftable, token, fName);
		token = TKGetNextToken(tk);
	}
	free(sep);
}
int fileWrite(char * invfile, h_TABLE *key){
	FILE *fp;
	fp = fopen(invfile, "wb+");
	if(fp == NULL){
		return 0;
	}
	if(key == NULL){
		return 0;
	}
	int i;
	linkedList *ll;
	Node *pointer;
	
	for(i = 0; i < 36; i++){
		ll = key->containers[i];
		
		if(ll == NULL){
			continue;
		}
		pointer = ll->head;
		if(pointer == NULL){
			continue;
		}
		while(pointer != NULL){

			fprintf(fp,"<list> %s\n",pointer->data);
			FileNode *fnp = pointer->fileOccuredIn;
			while(fnp !=NULL){
				fprintf(fp, "%s %d ",fnp->fName,fnp->occurances);
				fnp = fnp->next;
			}
			fprintf(fp,"\n</list>\n");
			pointer = pointer->next;
		}
	}
	fclose(fp);
	return 1;
}

static void dTraverse(char *pathName, h_TABLE *table) {

	DIR *dir; struct dirent *fil;
	printf("READING DIRECTORY: %s\n", pathName);

	if(!(dir=opendir(pathName)))
		printf("ERROR CANNOT OPEN DIRECTORY");
	
	while((fil = readdir(dir))){
	
		char *fileName = (fil->d_name);
	
		if(strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0)
			continue;
		
		size_t pNamesize = (2 * sizeof(char)) + strlen(fileName) + (strlen(pathName));
		char *pName = malloc(pNamesize);
		strcpy(pName, pathName);
		strcat(pName, "/");
		strcat(pName, fileName);
		if(fil->d_type == DT_DIR){
			dTraverse(pName,table);
		}else if(fil->d_type == DT_REG){
			fileParse(pName, table);
			continue;
		}else
			printf("ERROR");
		free(pName);
	}
	
	closedir(dir);
}
void free_HT(h_TABLE *key){
	int i;
	linkedList *ll;
	Node *pointer;
	for(i = 0; i < 36; i++){
		ll = key->containers[i];
		if(ll == NULL){
			free(ll);
			continue;
		}
		pointer = ll->head;
		if(pointer == NULL){
			free(pointer);
			continue;
		}
		while(pointer != NULL){

			
			FileNode *fnp = pointer->fileOccuredIn;
			while(fnp !=NULL){
				free(fnp->fName);
				
				fnp = fnp->next;
			}
			free(pointer->data);
			free(pointer->fileOccuredIn);
			free(pointer);
			pointer = pointer->next;
		}
		free(ll);
	}
	free(key);
}


int main(int argc, char **argv){
	char *pathName = argv[2];
	char *inv = argv[1];
	h_TABLE *ht = newTable();
	struct stat s;

	if( stat(pathName,&s) == 0 )
	{
    	if( s.st_mode & S_IFDIR )
    	{
        	dTraverse(pathName,ht);
    	}
    	else if( s.st_mode & S_IFREG )
    	{
        	fileParse(pathName, ht);
    	}
    	else
    	{
        	printf("ERROR: INVALID FILENAME");
    	}
	}
	else
	{
    	printf("SOMETHING WENT WRONG THERE");
	}
	fileWrite(inv, ht);
	free_HT(ht); 

	
return 0;
}








