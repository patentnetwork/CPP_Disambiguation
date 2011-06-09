/* strcmp95.c   Version 2						      */

#include "strcmp95.h"
#include "stdlib.h"

//double  strcmp95(char *ying, char *yang, long y_length, int *ind_c[])
double strcmp95_modified (const char *ying, const char *yang)
{
/* Arguments:

   ying and yang are pointers to the 2 strings to be compared.  The strings
   need not be NUL-terminated strings because the length is passed.

   y_length is the length of the strings. 

   ind_c is an array that is used to define whether certain options should be 
   activated.  A nonzero value indicates the option is deactivated.  
   The options are:
     ind_c[0] Increase the probability of a match when the number of matched
              characters is large.  This option allows for a little more
              tolerance when the strings are large.  It is not an appropriate
              test when comparing fixed length fields such as phone and
              social security numbers.
     ind_c[1] All lower case characters are converted to upper case prior
              to the comparison.  Disabling this feature means that the lower 
              case string "code" will not be recognized as the same as the 
              upper case string "CODE".  Also, the adjustment for similar 
              characters section only applies to uppercase characters.

   The suggested values are all zeros for character strings such as names.    */

static	int	pass=0,	adjwt[91][91];
static	const unsigned char	sp[39][2] =
 { {'A','E'},  {'A','I'},  {'A','O'},  {'A','U'},  {'B','V'},  {'E','I'},  {'E','O'},  {'E','U'},
	{'I','O'},  {'I','U'},  {'O','U'},  {'I','Y'},  {'E','Y'},  {'C','G'},  {'E','F'},
	{'W','U'},  {'W','V'},  {'X','K'},  {'S','Z'},  {'X','S'},  {'Q','C'},  {'U','V'},
	{'M','N'},  {'L','I'},  {'Q','O'},  {'P','R'},  {'I','J'},  {'2','Z'},  {'5','S'},
	{'8','B'},  {'1','I'},  {'1','L'},  {'0','O'},  {'0','Q'},  {'C','K'},  {'G','J'},
	{'E',' '},  {'Y',' '},  {'S',' '}
 };

char    ying_hold[MAX_VAR_SIZE],
        yang_hold[MAX_VAR_SIZE],
        ying_flag[MAX_VAR_SIZE],
        yang_flag[MAX_VAR_SIZE];

double  weight,	Num_sim;

long    minv,   search_range,   lowlim,    ying_length,
        hilim,  N_trans,        Num_com,   yang_length;

int	yl1,	/*yi_st,*/	N_simi;

register int     i,      j,      k;

/* Initialize the adjwt array on the first call to the function only.
   The adjwt array is used to give partial credit for characters that 
   may be errors due to known phonetic or character recognition errors.
   A typical example is to match the letter "O" with the number "0"           */
if (!pass) {
  pass = 1;
  for (i=0; i<91; i++) for (j=0; j<91; j++) adjwt[i][j] = 0;
  for (i=0; i<36; i++) {
    adjwt[sp[i][0]][sp[i][1]] = 3;
    adjwt[sp[i][1]][sp[i][0]] = 3;
} }

//modified by edward
#if 0
// If either string is blank - return - added in Version 2                    */
if (!strncmp(ying,NULL60,y_length)) return(0.0);                 
if (!strncmp(yang,NULL60,y_length)) return(0.0);                 
if(ying[0] == '\0' || yang[0] == '\0') return(0.0);

/* Identify the strings to be compared by stripping off all leading and 
 	 trailing spaces.							      */
k = y_length - 1;
for(j = 0;((ying[j]==' ') && (j < k));j++);
//for(i = k;((ying[i]==' ') && (i > 0));i--);
for(i=k;((ying[i]!='\0') && (i > 0));i--);
ying_length = i + 1 - j;
yi_st = j;

for(j = 0;((yang[j]==' ') && (j < k));j++);
//for(i = k;((yang[i]==' ') && (i > 0));i--);
for(i = k;((yang[i]!='\0') && (i > 0));i--);
yang_length = i + 1 - j;
#endif

//added by edward
ying_length = strlen(ying) ;
yang_length = strlen(yang) ;

//ying_hold[0]=yang_hold[0]=0;
//memcpy(ying_hold,&ying[yi_st],ying_length);
//memcpy(yang_hold,&yang[j],yang_length);

//memcpy(ying_hold, ying, ying_length);
//memcpy(yang_hold, yang, yang_length);
strcpy(ying_hold, ying);
strcpy(yang_hold, yang);


if (ying_length > yang_length) {
  search_range = ying_length;
  minv = yang_length;
  }
 else {
  search_range = yang_length;
  minv = ying_length;
  }

/* If either string is blank - return                                         */
/* if (!minv) return(0.0);                   removed in version 2             */

/* Blank out the flags							      */
ying_flag[0] = yang_flag[0] = 0;
strncat(ying_flag,NULL60,search_range);
strncat(yang_flag,NULL60,search_range);
search_range = (search_range/2) - 1;
if (search_range < 0) search_range = 0;   /* added in version 2               */

/* Convert all lower case characters to upper case.                           */
//deanotated by edward
//if (!ind_c[1]) {
  for (i = 0;i < ying_length;i++) if (islower(ying_hold[i])) ying_hold[i] -= 32;
  for (j = 0;j < yang_length;j++) if (islower(yang_hold[j])) yang_hold[j] -= 32;
//}

/* Looking only within the search range, count and flag the matched pairs.    */
Num_com = 0; 
yl1 = yang_length - 1;
for (i = 0;i < ying_length;i++) {
  lowlim = (i >= search_range) ? i - search_range : 0;
  hilim = ((i + search_range) <= yl1) ? (i + search_range) : yl1;
  for (j = lowlim;j <= hilim;j++)  {
    if ((yang_flag[j] != '1') && (yang_hold[j] == ying_hold[i])) {
        yang_flag[j] = '1';
        ying_flag[i] = '1';
        Num_com++;
        break;
} } }

/* If no characters in common - return                                        */
if (!Num_com) return(0.0);                          

/* Count the number of transpositions                                         */
k = N_trans = 0;
for (i = 0;i < ying_length;i++) {
  if (ying_flag[i] == '1') {
    for (j = k;j < yang_length;j++) {
        if (yang_flag[j] == '1') { 
         k = j + 1;
         break;
    } }
    if (ying_hold[i] != yang_hold[j]) N_trans++;
} }
N_trans = N_trans / 2;

/* adjust for similarities in nonmatched characters                           */
N_simi = 0;
if (minv > Num_com) {   
  for (i = 0;i < ying_length;i++) {
    if (ying_flag[i] == ' ' && INRANGE(ying_hold[i])) { 
      for (j = 0;j < yang_length;j++) {
        if (yang_flag[j] == ' ' && INRANGE(yang_hold[j])) {
          if (adjwt[(unsigned char)ying_hold[i]][(unsigned char)yang_hold[j]] > 0) {
            N_simi += adjwt[(unsigned char)ying_hold[i]][(unsigned char)yang_hold[j]];
            yang_flag[j] = '2';
            break;
} } } } } }
Num_sim = ((double) N_simi)/10.0 + Num_com;

/* Main weight computation.						      */
weight= Num_sim / ((double) ying_length) + Num_sim / ((double) yang_length) + ((double) (Num_com - N_trans)) / ((double) Num_com);
weight = weight / 3.0;

/* Continue to boost the weight if the strings are similar                    */
if (weight > 0.7) {

  /* Adjust for having up to the first 4 characters in common                 */
  j = (minv >= 4) ? 4 : minv;
  for (i=0;((i<j)&&(ying_hold[i]==yang_hold[i])&&(NOTNUM(ying_hold[i])));i++); 
  if (i) weight += i * 0.1 * (1.0 - weight);

  /* Optionally adjust for long strings.                                      */
  /* After agreeing beginning chars, at least two more must agree and 
       the agreeing characters must be > .5 of remaining characters.          */
  //deanotated by edward
  //if ((!ind_c[0]) && (minv>4) && (Num_com>i+1) && (2*Num_com>=minv+i))
  if ((minv>4) && (Num_com>i+1) && (2*Num_com>=minv+i))
  if (NOTNUM(ying_hold[0])) 
    weight += (double) (1.0-weight) *
   ((double) (Num_com-i-1) / ((double) (ying_length+yang_length-i*2+2)));
 }

return(weight);

} /* strcmp95 */







//==========================================================


unsigned int LCS_Incontinuous(const char * s1, const char * s2, char * output ) {
	const unsigned int m = strlen(s1) + 1;
	const unsigned int n = strlen(s2) + 1;
	const unsigned int mat_size = m * n * sizeof(int);
	int * pmat;
	register int i, j;
	register int ** lcs;
	unsigned int cnt = 0;

	if ( m == 1 || n == 1 ) {
		output = '\0';
		return 0;
	}

	pmat = (int*) malloc ( mat_size );
	memset( pmat, 0, mat_size );

	lcs = ( int **) malloc ( m * sizeof ( int * ));
	for ( i = 0 ; i < m; ++i )
		lcs[i] = pmat + i * n;

	for ( i = 1; i < m ; ++i ) {
		for ( j = 1 ; j < n ; ++j ) {
			if ( s1[i-1] == s2[j-1] )
				lcs[i][j] = lcs[i-1][j-1] + 1;
			else
				lcs[i][j] = lcs[i-1][j] >= lcs[i][j-1] ? lcs[i-1][j] : lcs[i][j-1];//get the upper or lefter max value
		}
	}

	i = m-2;
	j = n-2;
	while ( i != -1 && j != -1 ) {
		if ( s1[i] == s2[j]  )	{
			*output++ = s1[i];
			++cnt;
			i--;
			j--;
		}
		else {
			if ( lcs[i+1][j+1] == lcs[i][j] ) 	{
				i--;
				j--;
			}
			else {
				if( lcs[i][j+1] >= lcs[i+1][j] )
					i--;
				else
					j--;
			}
		}
	}

	*output = '\0';


	free(pmat);
	free(lcs);
	return cnt;

}



int is_misspell( const char * s1, const char * s2 ) {
	const int size_diff = strlen(s1)- strlen(s2);
	const char * plong = NULL, *pshort = NULL;
	int hit = 0;


	if ( size_diff == 1 || size_diff == -1  ) {
		// one character is missing
		if ( size_diff == 1 ) {
			plong = s1;
			pshort = s2;
		}
		else {
			plong = s2;
			pshort = s1;
		}

		while ( *pshort != '\0' ) {
			if ( *plong++ != *pshort++ ) {
				if ( hit )
					return 0;
				++plong;
				hit = 1;
			}
		}

		if ( *pshort != '\0' && *plong != '\0' )
			return 0;
		else
			return 1;
	}

	else if ( size_diff == 0) {
		//switch or misspell
		while ( *s1 != '\0' ) {
			if ( *s1 != *s2 ) {
				if ( hit )
					return 0;
				else {
					hit = 1;
					plong = s1;
					pshort = s2;
					++s1;
					++s2;
					if ( *s1 == '\0' )
						return 3; //misspelling of last char
				}

			}
			++s1;
			++s2;
		}
		if ( hit == 0 )
			return 4; //exact match
		else {
			if ( *plong != *pshort && *( plong + 1 ) == *(pshort + 1) )
				return 3; //misspelling
			else if ( *plong == * (pshort + 1) && *pshort == *(plong + 1 ))
				return 2; //switch of 2 chars
			else
				return 0;
		}
	}
	else
		return 0;

}

int is_abbreviation( const char * s1, const char * s2 ) {
        int cnt = 0;
	while ( *s1 != '\0' && *s2 != '\0' && *s1 == *s2 ) {
		++s1;
		++s2;
	    ++cnt;
	}
	if ( *s1 != '\0' && *s2 != '\0' )
		return 0;
	//else if ( *s1 == '\0' && *s2 == '\0' )
	//	return 2;
	else
		return cnt;
}


























