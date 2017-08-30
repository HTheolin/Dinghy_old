/*
$GPGSV,3,3,12,57,52,188,,16,44,147,,06,44,237,,17,41A,6537.0261,N,02208.2115,E,0                                                           .36,180.30,221116,,,A*6F
$GPGGA,12217,E,1,04,4.0,-25.4,M,25.0,M,,0000*48
$GPGSA,A,3,03,22,23,01,,,,122622.347,A,6537.0257,N,02208.2117,E,0.69,182.66,2211                                                           16,,,A*60$GPGGA,122623.347,6537.0257,N,02208.2117,E,1,04,4.0,-25.4,M,25.02,23,01                                                           ,,,,,,,,,5.1,4.0,3.2*30
$GPRMC,122623.347,A,6537.0257,N,116,,,A*64
$GPGGA,122624.347,6537.0258,N,02208.2112,E,1,04,4.0,GSA,A,3,03,22,23,01,,,,,,,,,                                                           5.1,4.0,3.2*30
$GPRMC,122624.347,A,6,68.75,221116,,,A*53
$GPGGA,122625.347,6537.0258,N,02208.2112,000*45
$GPGSA,A,3,03,22,23,01,,,,,,,,,5.1,4.0,3.2*30
$GPRMC,12.2112,E,0.20,152.16,221116,,,A*6A
$GPGGA,122626.347,6537.0259,N,M,25.0,M,,0000*40
$GPGSA,A,3,03,22,23,01,,,,,,,,,5.1,4.0,3.2*35,22,56,131,32,23,43,205,36,01,23,16                                                           8,31*73
$GPGSV,3,2,12,14,0071,,29,52,098,*76
$GPGSV,3,3,12,57,52,188,,16,44,147,,06,44,23726.347,A,6537.0259,N,02208.2112,E,0

GPGGA
FAIL $GPGGA,12217,E,1,04,4.0,-25.4,M,25.0,M,,0000*48
OK   $GPGGA,122623.347,6537.0257,N,02208.2117,E,1,04,4.0,-25.4,M,25.02,23,01
OK   $GPGGA,122624.347,6537.0258,N,02208.2112,E,1,04,4.0,GSA,A,3,03,22,23,01,,,,,,,,,
FAIL $GPGGA,122625.347,6537.0258,N,02208.2112,000*45
FAIL $GPGGA,122626.347,6537.0259,N,M,25.0,M,,0000*40

*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>




//TODO:
/*
 * Buggar ur när man kör program, kan ej läsa fil??
 * Testa så att nya implementationen av return värdena i parseGps funkar
 *
 */

//Bygg ut denna struct ifall mer data lagring behövs
//Ingenting i koden bygger på storleken av denna så det är lungt
/*
struct GPS_data{
	double time;
	double lat;
	char  latdir;
	double lon;
	char  londir;
};
*/
#define TIME_CHRLEN 10
#define LAT_CHRLEN 9
#define LON_CHRLEN 10
#define GPSDATA_ARRSIZE 10
#define CMDBUF_ROWS 20
#define CMDBUF_COLS 200

struct GPS_data{
	char time[TIME_CHRLEN];
	char lat[LAT_CHRLEN];
	char latdir;
	char lon[LON_CHRLEN];
	char londir;
};

int isNumber(char inp);
int isTime(char* inp, int inpLen);
int isLong(char* inp, int inpLen);
int isLat(char* inp, int inpLen);
int findCharPos(char* inp, int inpBufLen, int startpos, char tofind);
int parseGPGGA(char* inp, int len, struct GPS_data* out);
int parseGPS(char* inpBuf, int inpBufLen, struct GPS_data* tar, int gpsdataarrlen);

int main(void){
	struct GPS_data gpstest[GPSDATA_ARRSIZE];

	//Används för debugging från .txt fil med testdata!
	char * buffer = 0;
	long length;
	FILE * f = fopen ("C:/Users/Jesper/Desktop/NMEA_parser/GPS_parser/src/testdata.txt", "r");
	if (f)
	{
	  fseek (f, 0, SEEK_END);
	  length = ftell (f);
	  fseek (f, 0, SEEK_SET);
	  buffer = malloc (length);
	  if (buffer)
	  {
	    fread (buffer, 1, length, f);
	  }
	  fclose (f);
	}
	if (buffer)
	{
	  // start to process your data / extract strings here...
		parseGPS(buffer, (int)length, &gpstest, GPSDATA_ARRSIZE);
	}
	return 1;
}

int parseGPS(char* inpBuf, int inpBufLen, struct GPS_data* tar, int gpsdataarrlen){
	int pos1 = 0, pos2 = 0;
	int commandcnt = 0;
	int len = 0;
	/*
	 * 	cmdbuf[rader][kolumner]
	 * 	varje rad har ett command, ett command är X kolumner långt.
	 *
	 * 	cmdlen[rader]
	 * 	värdet på varje rad korresponderar med längden X i cmdbuf.
	 *
	 * 	Behövs ifall att inpBuf innehåller null chars utspridda randomly
	 * 	(kan ej förlita sig på strlen() etc..)
	 * 	Klumpig men robust lösning
	 */
	char cmdbuf[CMDBUF_ROWS][CMDBUF_COLS];
	int cmdlen[CMDBUF_ROWS];

	//Command List
	char gpgga[5] = "GPGGA";
	char gpgsv[5] = "GPGSV";
	char gpgsa[5] = "GPGSA";
	char gprmc[5] = "GPRMC";

	//Loopa igenom buffer och hitta alla commands tills buffer slut nås
	//dvs då pos1 eller pos2 är = inpBufLen  (se findCharPos() function def)
	while((pos1!=inpBufLen) && (pos2!=inpBufLen)){
		pos1 = findCharPos(inpBuf, inpBufLen, pos2, '$');
		pos2 = findCharPos(inpBuf, inpBufLen, pos1+1, '$');
		len = pos2-pos1-1;
		cmdlen[commandcnt] = len;
		strncpy(&cmdbuf[commandcnt][0], &inpBuf[pos1+1], (pos2-pos1-1));
		commandcnt++;
	}

	int i;
	int gpggacnt = 0;
	for(i = 0; i<commandcnt; i++){
		if(strncmp(&cmdbuf[i][0], gpgga, 5)==0){
			//Kod för parsing av GPGGA
			//Positionerings data
			if(parseGPGGA(&cmdbuf[i][0], cmdlen[i], &tar[gpggacnt])){
				gpggacnt++;
			}
		}
		else if(strncmp(&cmdbuf[i][0], gpgsv, 5)==0){
			//Kod för parsing av GPGSV
			//Sattelit data
			continue;
		}
		else if(strncmp(&cmdbuf[i][0], gpgsa, 5)==0){
			//Kod för parsing av GPGSA
			//GPS DOP och aktiva satteliter
			continue;
		}
		else if(strncmp(&cmdbuf[i][0], gprmc, 5)==0){
			//Kod för parsing av GPRMC
			//Rekommenderad minimum GPS/Transit data
			continue;
		}
	}
	return 1;
}


//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//tar input sträng "inp" med/utan nullchar och spottar ut gps data till "out"
//function returnerar 0 om tid conversion ej gick att utföra (kritisk). returnerar 1
//om någon annan conversion ej gick. returnerar 2 om allt gick bra.
int parseGPGGA(char* inp, int inpLen, struct GPS_data* out){
	char partial[20];
	int comma1 = 0;
	int comma2 = findCharPos(inp, inpLen, comma1, ',');
	int statFlag = 2;

	// Parsing antar följande input struktur
	// $GPGGA,float,float,char,float,char,w/e
	// $GPGGA,time,lat,latdir,lon,londir
	// Algoritmen är väldigt hårdkodad. Får vi ej data
	// I rätt ordning och form så ignoreras den.
	int counter = 1;
	int datalen = 0;
	char errmsg[3] = "ERR";
	while (counter<=5){
		comma1 = comma2;
		comma2 = findCharPos(inp, inpLen, comma1+1, ',');
		datalen = comma2-comma1-1;
		strncpy(partial, &inp[comma1+1], (comma2-comma1-1));
		switch(counter){
			case 1:
				//Time parse
				if(isTime(partial, datalen)){
					//out->time = atof(partial);
					strncpy(&out->time, partial, TIME_CHRLEN);
				} else {
					// Felhantering, var ej tidformat
					// hhmmss.sss
					return 0;
				}
				break;
			case 2:
				//latitude parse
				if(isLat(partial, datalen)){
					//out->lat = atof(partial);
					strncpy(&out->lat, partial, LAT_CHRLEN);
				} else {
					// Felhantering, var ej latitude format
					// llll.llll
					statFlag = 1;
					strncpy(&out->lat, errmsg, 3);
				}
				break;
			case 3:
				//latitude dir parse
				if(partial[0] == 'N' || partial[0] == 'S'){
					out->latdir = partial[0];
				} else {
					// Felhantering, var ej latitude dir format
					// N eller S
					statFlag = 1;
					out->latdir = 'X';
				}
				break;
			case 4:
				//longitude parse
				if(isLong(partial, datalen)){
					//out->lon = atof(partial);
					strncpy(&out->lon, partial, LON_CHRLEN);
				} else {
					// Felhantering, var ej longitude format
					// yyyyy.yyyy
					statFlag = 1;
					strncpy(&out->lon, errmsg, 3);
				}
				break;
			case 5:
				//longitude dir parse
				if(partial[0] == 'E' || partial[0] == 'W'){
					out->londir = partial[0];
				} else {
					// Felhantering, var ej latitude dir format
					// E eller W
					statFlag = 1;
					out->londir = 'X';
				}
				break;
		};
		counter++;
	}
	return statFlag;
}

//Returnerar position av char "tofind" i string "inp", "inp" behöver ej ha null char i slutet.
//Börjar söka på pos "inp[startpos]", om slutet nås utan en träff så returneras "maxlen"
int findCharPos(char* inp, int inpLen, int startpos, char tofind){
	int charpos = 0;
	int n;
	for(n = startpos; n<inpLen; n++){
		if(inp[n] == tofind){
			charpos = n;
			break;
		}
		if(n==(inpLen-1)){
			charpos = inpLen;
		}
	}
	return charpos;
}

// Ser till att strängen har rätt format
// 'llll.llll' med avslutande \0
int isLat(char* inp, int inpLen){
	int i;
	if(inpLen==LAT_CHRLEN){
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
int isLong(char* inp, int inpLen){
	int i;
	if(inpLen==LON_CHRLEN){
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
int isTime(char* inp, int inpLen){
	int i;
	if(inpLen==TIME_CHRLEN){
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
