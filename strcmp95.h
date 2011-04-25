/* The strcmp95 function returns a double precision value from 0.0 (total
   disagreement) to 1.0 (character-by-character agreement).  The returned 
   value is a measure of the similarity of the two strings.                   */

/* Date of Release:  Jan. 26, 1994					      */
/* Modified: April 24, 1994  Corrected the processing of the single length
             character strings.
   Authors:  This function was written using the logic from code written by
             Bill Winkler, George McLaughlin and Matt Jaro with modifications
             by Maureen Lynch. 
   Comment:  This is the official string comparator to be used for matching 
             during the 1995 Test Census.                                     */

#include <ctype.h>
#include <string.h>
#include <stdio.h>

#define NOTNUM(c)	((c>57) || (c<48))
#define INRANGE(c)      ((c>0)  && (c<91))
#define MAX_VAR_SIZE 1024
#define NULL60 "                                                            "

//double  strcmp95(char *, char *, long, int **);
double strcmp95_modified (const char *ying, const char *yang);
int is_misspell( const char * s1, const char * s2 );
int is_abbreviation( const char * s1, const char * s2);

