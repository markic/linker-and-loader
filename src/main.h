// main.h - Course: System Programming
// author: Marin Markic
#ifndef _MAIN_H
#define	_MAIN_H

// Macros
#define error(x){\
	fputs (x,stderr);\
	fclose (file.ptr);\
	free (file.buffer);\
	continue;\
}

#define STACK_SIZE 1024*1024

// Data Structures

struct FileDescriptor
{
	char *name;
	FILE *ptr;
	unsigned long size;
	char *buffer;

	unsigned long numOfSymbols;
	unsigned long numOfRelocs;

	unsigned long symTableOff;
	unsigned long sectionStrTabOff;
	unsigned long symbolStrTabOff;

	void* myPrintfAddr;
	void* myScanfAddr;

	void *mainVA;

}file;

struct Errors
{
	const char* cannotOpen;
	const char* cannotAlloc;
	const char* cannotRead;
	const char* notElfFile;
	const char* mapFailed;
	const char* munFailed;

}err;


#endif	/* main.h */
