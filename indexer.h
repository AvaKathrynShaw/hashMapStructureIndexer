#include <stdio.h>
#include <stdlib.h> /* malloc */
#include <string.h>


typedef struct _h_TABLE {
	struct _linkedList **containers;
	int size;
} h_TABLE;


typedef struct _linkedList {
	int size;
	struct _Node *head;
} linkedList;

typedef struct _Node {
	struct _FileNode *fileOccuredIn;
	char *data;
	struct _Node *next;
} Node;

typedef struct _FileNode{
	struct _FileNode *next;
	char *fName;
	int occurances;
}FileNode;

Node *addWord(char *addingWord);
linkedList *newLinkedList();
int getPosition(char *addingWord);
FileNode *newFNode(char *file);
char *formatInput(char *addingWord);
void fileParse(char *fName, h_TABLE *ftable);
int filePrint(char * invfile, h_TABLE *key);
h_TABLE *newTable();
void addToTable(h_TABLE *key, char *addingWord,char *fName);
static void dTraverse(char *pathName, h_TABLE *table);
void free_HT(h_TABLE *key);