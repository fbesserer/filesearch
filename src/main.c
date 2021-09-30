#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "hashtable.h"

#define HT_SIZE TABLE_SIZE
#define FILENAMESIZE 256
#define BLOCKSIZE 4096
#define DEBUG 1

int extract_jpg(FILE *fstream, long int bytes) {
	// copies the bytes into new file 
	FILE *jpg_file; 
	static unsigned int jpg_nr=0;
	char filename[FILENAMESIZE]; 
	
	if (DEBUG) if (jpg_nr > 100) return EXIT_FAILURE;
	
	//printf("bytes: %lx\n", bytes);
	snprintf(filename, FILENAMESIZE, "pics/jpg_%d.jpg", jpg_nr);
	
	jpg_file = fopen(filename, "w");
	if (jpg_file == NULL) {
		printf("File could not be opened!\n");
		return EXIT_FAILURE;
	}
	
	for (long int i = 0; i < bytes; i++) {
		fputc(fgetc(fstream), jpg_file);
	}
	

	fclose(jpg_file);
	jpg_nr++;
	return EXIT_SUCCESS;
}

void skip_headers(FILE *fstream) {
	// skips header and positions the file descriptor 
	unsigned char headerbuf[2];
	unsigned short headersize;

	fread(headerbuf, 1, 2, fstream);
	headersize = headerbuf[0] << 8 | headerbuf[1];
	if (DEBUG) printf("headersize: %x\n", headersize);
	fseek(fstream, headersize - 2, SEEK_CUR); // -2 because the bytes containing the size have already been counted 
	//printf("byte after skip header: %lx\n", ftell(fstream));
}


int find_jpg_markers(FILE *fstream, markers *jpg_markers[], fpos_t *fpos) {
	unsigned char marker_buf[BLOCKSIZE + 1];
	unsigned char apphbuf[6];
	bool eof_flag = false;
	unsigned short apphsize, current_bytes;
	unsigned long long pos, start;
	unsigned short current_marker, ffdb=0xffdb, ffda=0xffda, ffc0=0xffc0, ffc4=0xffc4;
	//unsigned short ffe0=0xffe0, ffe1=0xffe1, ffe2=0xffe2, ffe3=0xffe3, ffec=0xffec, ffed=0xffed, ffee=0xffee, ffd9=0xffd9;
	unsigned int read;

	start = ftell(fstream);
	//save position
	fgetpos(fstream, fpos);

	//skip application header 0x(ffd8)ffe?. Bytes 5&6 contain the size application header (excluding the count for the ffe? marker) when counting from ffd8 start 
	fread(apphbuf, 1, 6, fstream);
	apphsize = apphbuf[4] << 8 | apphbuf[5];
	if (DEBUG) printf("apphsize: %x\n", apphsize);

	fseek(fstream, apphsize - 2, SEEK_CUR); // the 2 size bytes count themselves to the header part to be skipped
	
	if (DEBUG) {
		memset(apphbuf, '\0', 6);
		fread(apphbuf, 1, 6, fstream);
		printf("Anfangsbytes: %x %x %x %x\n", apphbuf[0], apphbuf[1], apphbuf[2], apphbuf[3]);//ff ed 29 5e
		fseek(fstream, -6, SEEK_CUR);
	}

	while (true) {
		read = fread(marker_buf, 1, BLOCKSIZE, fstream);
		if (feof(fstream)) {
			eof_flag = true; //checking before other functions change fstream position
		}
		else if (ferror(fstream)) {
			printf("read error.");
			return EXIT_FAILURE;
		}
		for (int i=0; i <= BLOCKSIZE -2; i++) {
			current_bytes = marker_buf[i] << 8 | marker_buf[i+1];
				
			//compare current_bytes with hashtable markers
			if ( (current_marker = in_hashtable(jpg_markers, current_bytes)) < 0xffff){
				if (!eof_flag) {
					pos = ftell(fstream) - (BLOCKSIZE - i);
					//set stream to end of marker 
					fseek(fstream, - (BLOCKSIZE - (i + 2)), SEEK_CUR);
				} else {
					pos = ftell(fstream);
				}
				//printf("ht value: %d\n", hash(&current_marker));

				switch (current_marker)
				{
				case 0xffe0: case 0xffe1: case 0xffe2: case 0xffe3: case 0xffec: case 0xffed: case 0xffee: 
						printf("found another application header: %llx\n", pos);
						skip_headers(fstream);
					break;
				case 0xffdb: jpg_markers[hash(&ffdb)]->found = true;
						printf("ffdb at: %llx\n", pos);
						skip_headers(fstream);
						//printf("byte after function skip header return: %lx\n", ftell(fstream));
					break;
				case 0xffc0: if (jpg_markers[hash(&ffdb)]->found) {
								jpg_markers[hash(&ffc0)]->found = true;
								printf("ffc0 at: %llx\n", pos);
								skip_headers(fstream);
							}
					break;
				case 0xffc4: if (jpg_markers[hash(&ffc0)]->found) {
								jpg_markers[hash(&ffc4)]->found = true;
								printf("ffc4 (huffman table) at: %llx\n", pos);
								printf("%lx\n", ftell(fstream));	
								skip_headers(fstream);
							}
					break;
				case 0xffda: if (jpg_markers[hash(&ffc4)]->found) {
								jpg_markers[hash(&ffda)]->found = true;
								printf("ffda at: %llx read blocks: %d\n", pos, read);
								printf("%lx\n", ftell(fstream));	
								skip_headers(fstream);
							}
					break;
				case 0xffd9: if (jpg_markers[hash(&ffda)]->found) {
							//printf("ffd9 at: %llx read blocks: %d at i: %d\n", pos, read, i);


							//jpg abspeichern
							fsetpos(fstream, fpos);
							//printf("%lx\n", ftell(fstream));	
							extract_jpg(fstream, pos - start);
							//delete all jpg markers again

							
							// return back to search_jpgs()
							return EXIT_SUCCESS;
						}
					break;
				default:
					break;
				}
/*				zur aktuellen Position wird oben schon gepsrungen
				if (!skipped) { // skip to current position before reloading marker buffer 
					fseek(fstream, -(BLOCKSIZE - i), SEEK_CUR);
					skipped = true;
				}*/
				break; // reload marker buffer after skipping headers 
			}
		}

		fseek(fstream, -1, SEEK_CUR);
		if (eof_flag) break;
		memset(marker_buf, '\0', BLOCKSIZE);
	}

	//set back fstream ptr 
	//fsetpos(fstream, fpos);
	return EXIT_SUCCESS;
}


int search_jpgs(FILE *fstream, fpos_t *fpos) {
	unsigned long long soi;
	unsigned char buffer[BLOCKSIZE + 1]; 
	markers *jpg_markers[HT_SIZE];
	//bool eof_flag = false;

	fill_hashtable(jpg_markers);
	if (DEBUG) print_hashtable(jpg_markers);

	while (true) {
		fread(buffer, 1, BLOCKSIZE, fstream);
		if (feof(fstream)) break; //checking before other functions change fstream position
		else if (ferror(fstream)) {
			printf("read error.");
			return EXIT_FAILURE;
		}
		for (int i = 0; i <= BLOCKSIZE - 4; i++) { 
			// find start of image (ffd8ffex) - read until BLOCKSIZE -4 (4th last entry)
			if ( buffer[i] == 0xff && buffer[i+1] == 0xd8 && buffer[i+2] == 0xff && (buffer[i+3]==0xee||buffer[i+3]==0xed||buffer[i+3]==0xec||buffer[i+3]==0xe0||buffer[i+3]==0xe1||buffer[i+3]==0xe2||buffer[i+3]==0xe3)) {
				
				soi = ftell(fstream) - (BLOCKSIZE - i);
				fseek(fstream, - (BLOCKSIZE - i), SEEK_CUR);
				if (DEBUG) printf("%x %x %x %x bei %llx\n", buffer[i], buffer[i+1], buffer[i+2], buffer[i+3], soi);
				if (find_jpg_markers(fstream, jpg_markers, fpos) == 1) {
					return EXIT_FAILURE;
				}
				//printf("fstream after find jpg markers: %lx\n", ftell(fstream));
				break; //nach find_jpg_marker muss aus for loop raus gesprungen werden 
			}
		}
		//set fptr back by 3, check eof, empty buffer
		fseek(fstream, ftell(fstream) - 3, SEEK_SET);
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
		err = search_jpgs(fstream, &fpos);
		if (err) return EXIT_FAILURE;
	}
	fclose(fstream);
	printf("Program finished successfully\n");

	return EXIT_SUCCESS;
}
