#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "hashtable.h"

#define HT_SIZE TABLE_SIZE
#define FILENAMESIZE 256
#define BLOCKSIZE 4096
#define DEBUG 1

int extract_jpg(FILE *fstream, unsigned long long start, long int bytes) {
	FILE *jpg_file; 
	static unsigned int jpg_nr=0;
	char filename[FILENAMESIZE]; 
	unsigned long long streamptr = ftell(fstream);
	
	if (DEBUG) if (jpg_nr > 100) return EXIT_FAILURE;
	printf("bytes: %lx\n", bytes);
	snprintf(filename, FILENAMESIZE, "pics/jpg_%d.jpg", jpg_nr);
	jpg_file = fopen(filename, "w");
	//streamcpy = fdopen (dup (fileno (fstream)), "r"); // copy of fd fstream 
	fseek(fstream, start, SEEK_SET);

	for (long int i = 0; i < bytes; i++) {
		fputc(fgetc(fstream), jpg_file);
	}
	//set back stream
	fseek(fstream, streamptr, SEEK_SET);

	fclose(jpg_file);
	jpg_nr++;
	return EXIT_SUCCESS;
}

void skip_headers(FILE *fstream, unsigned long long start) {
	unsigned char headerbuf[4];
	unsigned short headersize;

	fseek(fstream, start, SEEK_SET);
	fread(headerbuf, 1, 4, fstream);
	headersize = headerbuf[2] << 8 | headerbuf[3];
	if (DEBUG) printf("headersize: %x\n", headersize);
	fseek(fstream, start + 2 + headersize, SEEK_SET); 
	printf("byte after skip header: %lx\n", ftell(fstream));
}

int find_jpg_markers(FILE *fstream, unsigned long long start, markers *jpg_markers[]) {
	unsigned char marker_buf[BLOCKSIZE + 1];
	unsigned char apphbuf[6];
	bool eof_flag = false;
	unsigned short apphsize, current_marker, current_bytes;
	unsigned long long pos;

	//skip application header 0x(ffd8)ffe?. Bytes 5&6 contain the size application header (excluding the count for the ffe? marker) when counting from ffd8 start 
	fseek(fstream, start, SEEK_SET);
	fread(apphbuf, 1, 6, fstream);
	apphsize = apphbuf[4] << 8 | apphbuf[5];
	if (DEBUG) printf("apphsize: %x\n", apphsize);

	fseek(fstream, start + 4 + apphsize, SEEK_SET); 
	
	if (DEBUG) {
		memset(apphbuf, '\0', 6);
		fread(apphbuf, 1, 6, fstream);
		printf("Anfangsbytes: %x %x %x %x\n", apphbuf[0], apphbuf[1], apphbuf[2], apphbuf[3]);
	}

	while (true) {
		fread(marker_buf, 1, BLOCKSIZE, fstream);
		if (feof(fstream)) 	eof_flag = true; //checking before other functions change fstream position
		else if (ferror(fstream)) {
			printf("read error.");
			return EXIT_FAILURE;
		}
		for (int i=0; i <= BLOCKSIZE -2; i++) {
			current_bytes = marker_buf[i] << 8 | marker_buf[i+1];
			//compare current_bytes with hashtable markers
			if ( (current_marker = in_hashtable(jpg_markers, current_bytes)) >= 0){
				pos = ftell(fstream) - (BLOCKSIZE - i);
				switch (current_marker)
				{
				case 3: jpg_markers[3]->found = true;
						printf("ffdb at: %llx\n", pos);
						skip_headers(fstream, pos); // skip headers muss einen wert zurückgeben um den i nach vorne gesetzt wird
					break;
				case 0: if (jpg_markers[3]->found) {
							jpg_markers[0]->found = true;
							printf("ffc0 at: %llx\n", pos);
							skip_headers(fstream, pos);
						}
					break;
				case 4: if (jpg_markers[0]->found) {
							jpg_markers[4]->found = true;
							printf("ffc4 (huffman table) at: %llx\n", pos);
							skip_headers(fstream, pos);
						}
					break;
				case 2: if (jpg_markers[4]->found) {
							jpg_markers[2]->found = true;
							printf("ffda at: %llx\n", pos);
							skip_headers(fstream, pos);
						}
					break;
				case 1: if (jpg_markers[2]->found) {
							printf("ffd9 at: %llx\n", pos);
							//jpg abspeichern
						}
					break;
				default:
					break;
				}
			}
		}

		fseek(fstream, ftell(fstream) - 2, SEEK_SET);
		if (eof_flag) break;
		memset(marker_buf, '\0', BLOCKSIZE);
	}

	//set back fstream ptr 
	//fseek(fstream, nr of processed bytes, SEEK_SET);
	return EXIT_SUCCESS;
}


int search_jpgs(FILE *fstream, fpos_t fpos) {
	unsigned long long soi;
	unsigned char buffer[BLOCKSIZE + 1]; 
	markers *jpg_markers[HT_SIZE];
	bool eof_flag = false;

	fill_hashtable(jpg_markers);
	if (DEBUG) print_hashtable(jpg_markers);

	while (true) {
		fread(buffer, 1, BLOCKSIZE, fstream);
		if (feof(fstream)) 	eof_flag = true; //checking before other functions change fstream position
		else if (ferror(fstream)) {
			printf("read error.");
			return EXIT_FAILURE;
		}
		for (int i = 0; i <= BLOCKSIZE - 4; i++) { 
			// find start of image (ffd8ffex) - read until BLOCKSIZE -4 (4th last entry)
			if ( buffer[i] == 0xff && buffer[i+1] == 0xd8 && buffer[i+2] == 0xff && (buffer[i+3]==0xee||buffer[i+3]==0xed||buffer[i+3]==0xec||buffer[i+3]==0xe0||buffer[i+3]==0xe1||buffer[i+3]==0xe2||buffer[i+3]==0xe3)) {
				soi = ftell(fstream) - (BLOCKSIZE - i);
				if (DEBUG) printf("%x %x %x %x bei %llx\n", buffer[i], buffer[i+1], buffer[i+2], buffer[i+3], soi);
				find_jpg_markers(fstream, soi, jpg_markers);
				printf("fstream after find jpg markers: %llx\n", ftell(fstream));
				break; //nach find_jpg_marker muss aus for loop raus gesprungen werden 
			}
		}
		//set fptr back by 3, check eof, empty buffer
		if (!eof_flag)	fseek(fstream, ftell(fstream) - 3, SEEK_SET);
		//printf("stream: %ld\n", ftell(fstream));
		if (eof_flag) break;
		memset(buffer, '\0', BLOCKSIZE);
	}
	return EXIT_SUCCESS;
}

int main (int argc, char *argv[]) {
	FILE *fstream;
	fpos_t fpos;
	int err; 
	
	if (argc < 2) {
		printf("Please give a filename/block device as argument\n");
		return EXIT_FAILURE;
	}
	fstream = fopen(argv[1], "r");
	if (fstream == NULL) {
		printf("File could not be opened");
		return EXIT_FAILURE;
	} else {
		err = search_jpgs(fstream, fpos);
		if (err) return EXIT_FAILURE;
	}
	fclose(fstream);

	return EXIT_SUCCESS;
}
