#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct GPS_data{
	double time;
	double lat;
	char  latdir;
	double lon;
	char  londir;
};

void ResetString(char* inp, int len);
void CopyNChars(char* tar, char* inp, int cnt);
int FindComma(char* inp, int startpos, int maxlen);
int isNumber(char inp);
int isTime(char* inp);
int isLong(char* inp);
int isLat(char* inp);
struct GPS_data parseGPGGA(char*, int);

int main(void){
	//Test Data
	char gpgga[43] = "$GPGGA,122623.347,6537.0257,N,02208.2117,E,";

    struct GPS_data gpstest;
    gpstest = parseGPGGA(gpgga, 43);
    while(1){

    }
	return 0;
}

//Tar en array av längd "len" chars (behöver inte null char) innehållande GPGGA datan
//och returnerar datan som struct GPS_data
struct GPS_data parseGPGGA(char* inp, int len){
	struct GPS_data gpspos;
	int partial_len = 10;
	char partial[partial_len];
	int comma1 = 0;
	int comma2 = FindComma(inp, comma1, len);

	// Parsing antar följande input struktur
	// $GPGGA,float,float,char,float,char,w/e
	// $GPGGA,time,lat,latdir,lon,londir
	// Algoritmen är väldigt hårdkodad. Får vi ej data
	// I rätt ordning och form så ignoreras den.
	int counter = 1;
	while (counter<=5){
		comma1 = comma2;
		comma2 = FindComma(inp, comma1+1, len);
		ResetString(partial, partial_len);
		//strncpy(partial, &inp[comma1+1], (comma2-comma1-1));
		CopyNChars(partial, &inp[comma1+1], (comma2-comma1-1));
		switch(counter){
			case 1:
				//Time parse
				if(isTime(partial)){
					gpspos.time = atof(partial);
				} else {
					// Felhantering, var ej tidformat
					// hhmmss.sss
					gpspos.time = 3333.333;
				}
				break;
			case 2:
				//latitude parse
				if(isLat(partial)){
					gpspos.lat = atof(partial);
				} else {
					// Felhantering, var ej latitude format
					// llll.llll
					gpspos.lat = 222.222;
				}
				break;
			case 3:
				//latitude dir parse
				if(partial[0] == 'N' || partial[0] == 'S'){
					gpspos.latdir = partial[0];
				} else {
					// Felhantering, var ej latitude dir format
					// N eller S
					gpspos.latdir = 'Z';
				}
				break;
			case 4:
				//longitude parse
				if(isLong(partial)){
					gpspos.lon = atof(partial);
				} else {
					// Felhantering, var ej longitude format
					// yyyyy.yyyy
					gpspos.lon = 111.111;
				}
				break;
			case 5:
				//longitude dir parse
				if(partial[0] == 'E' || partial[0] == 'W'){
					gpspos.londir = partial[0];
				} else {
					// Felhantering, var ej latitude dir format
					// E eller W
					gpspos.londir = 'X';
				}
				break;
		};
		counter++;
	}
	return gpspos;
}

// Ser till att strängen har rätt format
// 'llll.llll' med avslutande \0
int isLat(char* inp){
	int i;
	if(strlen(inp)==9){
		for(i = 0; i < 4; i++){
			if(!isNumber(inp[i])){
				//char var ej siffra
				return 0;
			}
		}
		if(!(inp[4] == '.')){
			//char var ej .
			return 0;
		}
		for(i = 5; i < 9; i++){
			if(!isNumber(inp[i])){
				//char var ej siffra
				return 0;
			}
		}
		return 1;
	} else {
		return 0;
	}
}

// Ser till att strängen har rätt format
// 'yyyyy.yyyy' med avslutande \0
int isLong(char* inp){
	int i;
	if(strlen(inp)==10){
		for(i = 0; i < 5; i++){
			if(!isNumber(inp[i])){
				//char var ej siffra
				return 0;
			}
		}
		if(!(inp[5] == '.')){
			//char var ej .
			return 0;
		}
		for(i = 6; i < 10; i++){
			if(!isNumber(inp[i])){
				//char var ej siffra
				return 0;
			}
		}
		return 1;
	} else {
		return 0;
	}
}

// Ser till att strängen har rätt format
// 'hhmmss.sss' med avslutande \0
int isTime(char* inp){
	int i;
	if(strlen(inp)==10){
		for(i = 0; i < 6; i++){
			if(!isNumber(inp[i])){
				//char var ej siffra
				return 0;
			}
		}
		if(!(inp[6] == '.')){
			//char var ej .
			return 0;
		}
		for(i = 7; i < 10; i++){
			if(!isNumber(inp[i])){
				//char var ej siffra
				return 0;
			}
		}
		return 1;
	} else {
		return 0;
	}
}

// Kollar om charen är en siffra
int isNumber(char inp){
	if(inp >= 48 && inp <= 57){
		return 1;
	} else {
		return 0;
	}
}

//Givet en char array startposition so returnar den nästa
//position av ett , tecken. obs! Om ett komma finns på startpos så returneras
//startpos
int FindComma(char* inp, int startpos, int maxlen){
	int endpos = 0;
	int n;
	for(n = startpos; n<maxlen; n++){
		if(inp[n] == ','){
			endpos = n;
			break;
		}
	}
	return endpos;
}

//Skriver över arrayen av chars med längd len med null chars \0
void ResetString(char* inp, int len){
	int i;
	for(i = 0; i<len; i++){
		inp[i] = '\0';
	}
}

//Egen version av strncpy den vanliga intoducerade konstiga chars
//i slutet
void CopyNChars(char* tar, char* inp, int cnt){
	int i;
	for(i=0; i<cnt; i++){
		tar[i] = inp[i];
	}
	tar[cnt] = '\0';
}
