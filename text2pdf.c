/**************************************************************************
This ugly, sparsely-commented program is the source for text2pdf version
1.1.  It should be ANSI-conforming and compile on most platforms.  You'll
need to change LF_EXTRA to 1 for machines which write 2 characters for \n.
These include PCs, of course.
You may distribute the source or compiled versions free of charge.  You may
not alter the source in any way other than those mentioned above without
the permission of the author, Phil Smith <phil@bagobytes.co.uk>.
Please send any comments to the author.
Copyright (c) Phil Smith, 1996
REVISION HISTORY
Version 1.1
11 Oct 96 Added handling of form-feed characters, removed need for tmp file,
          put reference to resources in each page (avoid bug in Acrobat),
	  changed date format to PDF-1.1 standard.
12 Jun 96 Added check to avoid blank last page
12 Jun 96 Added LINE_END def to get round platform-specific \r, \n etc.
18 Mar 96 Added ISOLatin1Encoding option
version 1.2 
08 Ago 18 Output not required, use the same name and path as input name,
			max character per line set at 160, landscape by default,
			paramet -L turn off landscape mode.
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#define LF_EXTRA 0  /* how many extra characters are written for \n */
                    /* change to 1 for PCs (where \n => <CR><LF>) */

#define LINE_END '\015'  /* CR used in xref table */
#define FF 12            /* formfeed character (^L) */

char *appname = "text2pdf v1.1";
char *progname = "text2pdf";

FILE *infile, *fo;
int pageNo = 0;
int pageObs[500];
int curObj = 5;  /* object number being or last written */
long locations[1000];

char font[256];
char *defaultFont = "Courier";
int ISOEnc = 0;
int doFFs = 1;
int tab = 8;
int pointSize = 10;
int vertSpace = 12;
int lines = 0;
int cols = 160;  /* max chars per output line */
int columns = 1;  /* number of columns */

/* Default paper is Letter size, as in distiller */
int pageHeight = 792;
int pageWidth = 612;

unsigned char buf[1024];
unsigned long fpos = 0;


void writestr( char *path, char *message) {
  FILE *file_name;
  file_name = fopen(path,"a");
  fpos += strlen(message);
  fprintf(file_name,message);
    while (*message) {
    if (*message == '\n') fpos += LF_EXTRA;
   // putchar(*message++);
    *message++;
    
  
  }
  
  fclose(file_name);
  
    
}
/*void writestr(char *str) {
   Everything written to the PDF file goes through this function. 
   This means we can keep track of the file position without using
   ftell on a real (tmp) file.  However, PCs write out 2 characters 
   for \n, so we need this ugly loop to keep fpos correct 

  fpos += strlen(str);
  while (*str) {
    if (*str == '\n') fpos += LF_EXTRA;
    putchar(*str++);
  }
}
*/

void WriteHeader(char *title, char *filedest){

  struct tm *ltime;
  time_t clock;
  char datestring[30];
   

  time(&clock);
  ltime = localtime(&clock);

  strftime(datestring, 30, "D:%Y%m%d%H%M%S", ltime);

  writestr(filedest, "%%PDF-1.1\n");
  locations[1] = fpos;
  writestr(filedest, "1 0 obj\n");
  writestr(filedest, "<<\n");
  sprintf(buf, "/CreationDate (%s)\n", datestring); writestr(filedest, buf);
  sprintf(buf, "/Producer (%s (\\251 Phil Smith, 1996))\n", appname); writestr(filedest, buf);
  if (title) {sprintf(buf, "/Title (%s)\n", title); writestr(filedest, buf);}
  writestr(filedest, ">>\n");
  writestr(filedest, "endobj\n");

  locations[2] = fpos;
  writestr(filedest, "2 0 obj\n");
  writestr(filedest, "<<\n");
  writestr(filedest, "/Type /Catalog\n");
  writestr(filedest, "/Pages 3 0 R\n");
  writestr(filedest, ">>\n");
  writestr(filedest, "endobj\n");

  locations[4] = fpos;
  writestr(filedest, "4 0 obj\n");
  writestr(filedest, "<<\n");
  writestr(filedest, "/Type /Font\n");
  writestr(filedest, "/Subtype /Type1\n");
  writestr(filedest, "/Name /F1\n");
  sprintf(buf, "/BaseFont %s\n", font); writestr(filedest, buf);
  if (ISOEnc) {
    writestr(filedest, "/Encoding <<\n");
    writestr(filedest, "/Differences [ 0 /.notdef /.notdef /.notdef /.notdef\n");
    writestr(filedest, "/.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n");
    writestr(filedest, "/.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n");
    writestr(filedest, "/.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n");
    writestr(filedest, "/.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n");
    writestr(filedest, "/.notdef /.notdef /.notdef /.notdef /space /exclam\n");
    writestr(filedest, "/quotedbl /numbersign /dollar /percent /ampersand\n");
    writestr(filedest, "/quoteright /parenleft /parenright /asterisk /plus /comma\n");
    writestr(filedest, "/hyphen /period /slash /zero /one /two /three /four /five\n");
    writestr(filedest, "/six /seven /eight /nine /colon /semicolon /less /equal\n");
    writestr(filedest, "/greater /question /at /A /B /C /D /E /F /G /H /I /J /K /L\n");
    writestr(filedest, "/M /N /O /P /Q /R /S /T /U /V /W /X /Y /Z /bracketleft\n");
    writestr(filedest, "/backslash /bracketright /asciicircum /underscore\n");
    writestr(filedest, "/quoteleft /a /b /c /d /e /f /g /h /i /j /k /l /m /n /o /p\n");
    writestr(filedest, "/q /r /s /t /u /v /w /x /y /z /braceleft /bar /braceright\n");
    writestr(filedest, "/asciitilde /.notdef /.notdef /.notdef /.notdef /.notdef\n");
    writestr(filedest, "/.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n");
    writestr(filedest, "/.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n");
    writestr(filedest, "/dotlessi /grave /acute /circumflex /tilde /macron /breve\n");
    writestr(filedest, "/dotaccent /dieresis /.notdef /ring /cedilla /.notdef\n");
    writestr(filedest, "/hungarumlaut /ogonek /caron /space /exclamdown /cent\n");
    writestr(filedest, "/sterling /currency /yen /brokenbar /section /dieresis\n");
    writestr(filedest, "/copyright /ordfeminine /guillemotleft /logicalnot /hyphen\n");
    writestr(filedest, "/registered /macron /degree /plusminus /twosuperior\n");
    writestr(filedest, "/threesuperior /acute /mu /paragraph /periodcentered\n");
    writestr(filedest, "/cedilla /onesuperior /ordmasculine /guillemotright\n");
    writestr(filedest, "/onequarter /onehalf /threequarters /questiondown /Agrave\n");
    writestr(filedest, "/Aacute /Acircumflex /Atilde /Adieresis /Aring /AE\n");
    writestr(filedest, "/Ccedilla /Egrave /Eacute /Ecircumflex /Edieresis /Igrave\n");
    writestr(filedest, "/Iacute /Icircumflex /Idieresis /Eth /Ntilde /Ograve\n");
    writestr(filedest, "/Oacute /Ocircumflex /Otilde /Odieresis /multiply /Oslash\n");
    writestr(filedest, "/Ugrave /Uacute /Ucircumflex /Udieresis /Yacute /Thorn\n");
    writestr(filedest, "/germandbls /agrave /aacute /acircumflex /atilde /adieresis\n");
    writestr(filedest, "/aring /ae /ccedilla /egrave /eacute /ecircumflex\n");
    writestr(filedest, "/edieresis /igrave /iacute /icircumflex /idieresis /eth\n");
    writestr(filedest, "/ntilde /ograve /oacute /ocircumflex /otilde /odieresis\n");
    writestr(filedest, "/divide /oslash /ugrave /uacute /ucircumflex /udieresis\n");
    writestr(filedest, "/yacute /thorn /ydieresis ]\n");
    writestr(filedest, ">>\n");
  }

  writestr(filedest, ">>\n");
  writestr(filedest, "endobj\n");

  locations[5] = fpos;
  writestr(filedest, "5 0 obj\n");
  writestr(filedest, "<<\n");
  writestr(filedest, "  /Font << /F1 4 0 R >>\n");
  writestr(filedest, "  /ProcSet [ /PDF /Text ]\n");
  writestr(filedest, ">>\n");
  writestr(filedest, "endobj\n");
}

long StartPage(char *filedest){
  long strmPos;

  locations[++curObj] = fpos;
  pageObs[++pageNo] = curObj;
  sprintf(buf, "%d 0 obj\n", curObj); writestr(filedest, buf);
  writestr(filedest, "<<\n");
  writestr(filedest, "/Type /Page\n");
  writestr(filedest, "/Parent 3 0 R\n");
  writestr(filedest, "/Resources 5 0 R\n");
  sprintf(buf, "/Contents %d 0 R\n", ++curObj); writestr(filedest, buf);
  writestr(filedest, ">>\n");
  writestr(filedest, "endobj\n");
  
  locations[curObj] = fpos;
  sprintf(buf, "%d 0 obj\n", curObj); writestr(filedest, buf);
  writestr(filedest, "<<\n");
  sprintf(buf, "/Length %d 0 R\n", curObj + 1); writestr(filedest, buf);
  writestr(filedest, ">>\n");
  writestr(filedest, "stream\n");
  strmPos = fpos;

  writestr(filedest, "BT\n");
  sprintf(buf, "/F1 %d Tf\n", pointSize); writestr(filedest, buf);
  sprintf(buf, "1 0 0 1 50 %d Tm\n", pageHeight - 40); writestr(filedest, buf);
  sprintf(buf, "%d TL\n", vertSpace); writestr(filedest, buf);

  return strmPos;
}

void EndPage(long streamStart, char *filedest){
  long streamEnd;

  writestr(filedest, "ET\n");
  streamEnd = fpos;
  writestr(filedest, "endstream\n");
  writestr(filedest, "endobj\n");

  locations[++curObj] = fpos;
  sprintf(buf, "%d 0 obj\n", curObj); writestr(filedest, buf);
  sprintf(buf, "%lu\n", streamEnd - streamStart); writestr(filedest, buf);
  writestr(filedest, "endobj\n");
}

void WritePages(char *filedest){
  int atEOF = 0;
  int atFF;
  int atBOP;
  long beginstream;
  int lineNo, charNo;
  int ch, column;
  int padding, i;

  while (!atEOF) {
    beginstream = StartPage(filedest);
    column = 1;
    while (column++ <= columns) {
      atFF = 0;
      atBOP = 0;
      lineNo = 0;
      while (lineNo++ < lines && !atEOF && !atFF) {
	writestr(filedest, "(");
	charNo = 0;
	while (charNo++<cols &&
	       (ch = getc(infile))!=EOF &&
	       !(ch==FF && doFFs) &&
	       ch!='\n') {
	  if (ch >= 32 && ch <= 127) {
	    if (ch == '(' || ch == ')' || ch == '\\') writestr(filedest, "\\");
	    sprintf(buf, "%c", (char)ch); writestr(filedest, buf);
	  } else {
	    if (ch == 9) {
	      padding = tab - ((charNo - 1) % tab);
	      for (i = 1; i <= padding; i++) writestr(filedest, " ");
	      charNo += (padding - 1);
	    } else {
	      if (ch != FF) {
		/* write \xxx form for dodgy character */
		sprintf(buf, "\\%.3o", ch); writestr(filedest, buf);
	      } else {
		/* don't print anything for a FF */
		charNo--;
	      }
	    }
	  }
	}
	writestr(filedest, ")'\n");

	/* messy stuff to handle formfeeds.  Yuk! */
	if (ch == EOF) atEOF = 1;
	if (ch == FF) atFF = 1;
	if (lineNo == lines) atBOP = 1;
	if (atBOP) {
	  ch = getc(infile);
	  if (ch == FF) ch = getc(infile);
	  if (ch == EOF) atEOF = 1;
	  else ungetc(ch, infile);
	}
	else if (atFF) {
	  ch = getc(infile);
	  if (ch == EOF) atEOF = 1;
	  else ungetc(ch, infile);
	}
      }

      if (column <= columns) {
	sprintf(buf, "1 0 0 1 %d %d Tm\n",
		(pageWidth / 2) + 25, pageHeight - 40);
	writestr(filedest, buf);
      }
    }
    EndPage(beginstream,filedest);
  }
}

void WriteRest(char *filedest){
  long xref;
  int i;

  locations[3] = fpos;
  writestr(filedest, "3 0 obj\n");
  writestr(filedest, "<<\n");
  writestr(filedest, "/Type /Pages\n");
  sprintf(buf, "/Count %d\n", pageNo); writestr(filedest, buf);
  sprintf(buf, "/MediaBox [ 0 0 %d %d ]\n", pageWidth, pageHeight); writestr(filedest, buf);
  writestr(filedest, "/Kids [ ");
  for (i = 1; i <= pageNo; i++) {sprintf(buf, "%d 0 R ", pageObs[i]); writestr(filedest, buf);}
  writestr(filedest, "]\n");
  writestr(filedest, ">>\n");
  writestr(filedest, "endobj\n");

  xref = fpos;
  writestr(filedest, "xref\n");
  sprintf(buf, "0 %d\n", curObj + 1); writestr(filedest, buf);
  /* note that \n is translated by writestr */
  sprintf(buf, "0000000000 65535 f %c", LINE_END); writestr(filedest, buf);
  for (i = 1; i <= curObj; i++) {
    sprintf(buf, "%.10ld 00000 n %c", locations[i], LINE_END);
    writestr(filedest, buf);
  }

  writestr(filedest, "trailer\n");
  writestr(filedest, "<<\n");
  sprintf(buf, "/Size %d\n", curObj + 1); writestr(filedest, buf);
  writestr(filedest, "/Root 2 0 R\n");
  writestr(filedest, "/Info 1 0 R\n");
  writestr(filedest, ">>\n");

  writestr(filedest, "startxref\n");
  sprintf(buf, "%ld\n", xref); writestr(filedest, buf);
  writestr(filedest, "%%EOF\n");
}


void ShowHelp(){

  printf("\n%s [options] [filename]\n\n", progname);
  printf("  %s makes a 7-bit clean PDF file (version 1.1) from any input file.\n", progname);
  printf("  It reads from standard input or a named file, and writes the PDF file\n");
  printf("  to standard output.\n");
  printf("\n  There are various options as follows:\n\n");
  printf("  -h\t\tshow this message\n");
  printf("  -f<font>\tuse PostScript <font> (must be in standard 14, default: Courier)\n");
  printf("  -I\t\tuse ISOLatin1Encoding\n");
  printf("  -s<size>\tuse font at given pointsize (default %d)\n", pointSize);
  printf("  -v<dist>\tuse given line spacing (default %d points)\n", vertSpace);
  printf("  -l<lines>\tlines per page (default 60, determined automatically\n\t\tif unspecified)\n");
  printf("  -c<chars>\tmaximum characters per line (default 80)\n");
  printf("  -t<spaces>\tspaces per tab character (default 8)\n");
  printf("  -F\t\tignore formfeed characters (^L)\n");
  printf("  -A4\t\tuse A4 paper (default Letter)\n");
  printf("  -A3\t\tuse A3 paper (default Letter)\n");
  printf("  -x<width>\tindependent paper width in points\n");
  printf("  -y<height>\tindependent paper height in points\n");
  printf("  -2\t\tformat in 2 columns\n");
  printf("  -L\t\toff landscape mode\n");
  printf("\n  Note that where one variable is implied by two options, the second option\n  takes precedence for that variable. (e.g. -A4 -y500)\n");
  printf("  In landscape mode, page width and height are simply swapped over before\n  formatting, no matter how or when they were defined.\n");
  printf("\n%s (c) Phil Smith, 1996\n", appname);
}

int main(int argc, char **argv){
  int i = 1;
  int tmp, landscape = 1;
  char *ifilename = NULL;

  strcpy(font, "/");
  strcat(font, defaultFont);
  infile = stdin;  /* default */
  
  

  while (i < argc) {
    if (*argv[i] != '-') {  /* input filename */
      ifilename = argv[i];
      if (!(infile = fopen(ifilename, "r"))) {
	fprintf(stderr, "%s: couldn't open input file `%s'\n", progname, ifilename);
	exit(0);
      }
    } else {
      switch (*++argv[i]) {
      case 'h':
	ShowHelp();
	exit(0);
      case 'f':
	strcpy(font, "/");
	strcat(font, ++argv[i]);
	break;
      case 'I':
	ISOEnc = 1;
	break;
      case 'F':
	doFFs = 0;
	break;
      case 's':
	pointSize = atoi(++argv[i]);
	if (pointSize < 1) pointSize = 1;
	break;
      case 'v':
	vertSpace = atoi(++argv[i]);
	if (vertSpace < 1) vertSpace = 1;
	break;
      case 'l':
	lines = atoi(++argv[i]);
	if (lines < 1) lines = 1;
	break;
      case 'c':
	cols = atoi(++argv[i]);
	if (cols < 4) cols = 4;
	break;
      case '2':
	columns = 2;
	break;
      case 't':
	tab = atoi(++argv[i]);
	if (tab < 1) tab = 1;
	break;
      case 'A':
	switch (*++argv[i]) {
	case '3':
	  pageWidth = 842;
	  pageHeight = 1190;
	  break;
	case '4':
	  pageWidth = 595;
	  pageHeight = 842;
	  break;
	default:
	  fprintf(stderr, "%s: ignoring unknown paper size: A%s\n", progname, argv[i]);
	}
	break;
      case 'x':
	pageWidth = atoi(++argv[i]);
	if (pageWidth < 72) pageWidth = 72;
	break;
      case 'y':
	pageHeight = atoi(++argv[i]);
	if (pageHeight < 72) pageHeight = 72;
	break;
      case 'L':
	landscape = 0;
	break;
      default:
	fprintf(stderr, "%s: ignoring invalid switch: -%s\n", progname, argv[i]);
      }
    }
    i++;
    
  }

  if (landscape) {
    tmp = pageHeight;
    pageHeight = pageWidth;
    pageWidth = tmp;
  }

  if (lines == 0) lines = (pageHeight - 72) / vertSpace;
  if (lines < 1) lines = 1;
  /* happens to give 60 as default */
 char *fileconcat = ifilename;
  char *pdf =".pdf";
  strcat(fileconcat,pdf);
 // printf("Texto concatenado: %s\n",fileconcat); 
 // char *fileconcat ="hola.pdf";
 fclose(fopen(fileconcat, "w"));
  WriteHeader(ifilename,fileconcat);
  WritePages(fileconcat);
  WriteRest(fileconcat);
    /*
    fo = fopen(filedest, "a+"); 
    fprintf(fo, output);
    fclose (stdout);
   */
  return 0;
}
