/*\
 * showenv.cpp
 *
 * Copyright (c) 2014-2020 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#ifdef _MSC_VER
#include <windows.h>
#include <tchar.h>
#define MCDECL _cdecl
#else // !_MSC_VER
#include <stdlib.h>
#include <unistd.h>
#include <string.h> // for strlen(), ...
typedef char _TCHAR;
#define MCDECL
#define _environ __environ
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif
#include <iostream>
#include "showenvvers.h"

// reference additional headers here
#include "se_list.h"

#ifndef ANSI
#define ANSI
#endif

#ifdef ANSI             /* ANSI compatible version          */
#include <stdarg.h>
int average( int first, ... );
#else                   /* UNIX compatible version          */
#include <varargs.h>
int average( va_list );
#endif

//extern char **_environ;

#define  ISNUM(a) (( a >= '0' ) && ( a <= '9' ))

typedef struct tagENVLST {
   LE list;
   _TCHAR name[1];
}ENVLIST, * PENVLIST;

#ifdef WIN32
#define SEP_CHR ';'
#else
#define SEP_CHR ':'
#endif

#define  MEOR  "\n"

int verbal = 1;

#define VERB1   (verbal >= 1)
#define VERB2   (verbal >= 2)
#define VERB3   (verbal >= 3)
#define VERB4   (verbal >= 4)
#define VERB5   (verbal >= 5)
#define VERB9   (verbal >= 9)


const _TCHAR * pvers = { SE_VERS };
const _TCHAR szDate[] = __DATE__;
const _TCHAR szTime[] = __TIME__;
_TCHAR * prt_buf = 0; // allocated storage
_TCHAR * test_buf = 0;  // allocated for test

int icnt_env = 0;
int icnt_found = 0;
int tot_icnt_found = 0;
int g_iret = 0;   // return value

// users request(s)
LE env_list = { &env_list, &env_list };
// forward refernce
void DO( _TCHAR * pb );

#define  CHKMEM(a)   if( a == NULL ) { DO((char *)"ERROR: MEMORY FAILED!" MEOR); exit(-3); }

void Exit_Actions(void)
{
   PLE   ph = &env_list;

   KillLList(ph);
   if(prt_buf) delete prt_buf;
   if(test_buf) free(test_buf);
   prt_buf = 0;
   test_buf = 0;   
}

PLE   Add2List( _TCHAR * stg )
{
   PENVLIST pe;
   PLE   ph = &env_list;
   PLE   pn = (PLE)MALLOC( (sizeof(ENVLIST) + strlen(stg)) );
   CHKMEM(pn);
   pe = (PENVLIST)pn;
   strcpy(pe->name, stg);
   InsertTailList(ph,pn);
   return pn;
}

void DO( _TCHAR * pb )
{
   if(VERB1)
      printf((_TCHAR *)"%s",pb);
}

int MCDECL prt( _TCHAR * form, ... )
{
   int  iRet;
   if(prt_buf == 0) prt_buf = new _TCHAR[2048];
   va_list arglist;
   va_start(arglist, form);
   iRet = vsprintf( prt_buf, form, arglist );
   va_end(arglist);
   DO(prt_buf);
   return iRet;
}

void show_version(_TCHAR* self)
{
    _TCHAR* p = strrchr(self, '\\');
    _TCHAR* dt = strdup(szDate);
    _TCHAR* tm = strdup(szTime);
    size_t i;

    verbal = 1; // ensure output is ON
    if (!p)
        p = strrchr(self, '/');
    if (p) {
        p++;
    }
    else {
        p = self;
    }

    i = strlen(dt);
    while (i--) {
        if (dt[i] > ' ')
            break;
        dt[i] = 0;
    }

    i = strlen(tm);
    while (i--) {
        if (tm[i] > ' ')
            break;
        tm[i] = 0;
    }

    prt((_TCHAR*)"%s: version: %s - %s %s" MEOR, p, pvers, dt, tm);

    free(dt);
    free(tm);

}

void give_help( _TCHAR * self )
{
    show_version(self);

   prt( (_TCHAR *)"usage: [OPTIONS] 'variable-to-search-for' - wild chars allowed ..." MEOR );
   prt( (_TCHAR *)"OPTIONS:" MEOR );
#ifdef   ADD_TEST_VAR
   prt( (_TCHAR *)" -t\"string to find\" - test the variable for this string. If yes, exit 1, else 0" MEOR );
#endif /// #ifdef   ADD_TEST_VAR
   prt( (_TCHAR *)" -v[n]  Set verbosity (def=1). -v2 separates items per line, v3 adds line numbers, v5 test path validity..." MEOR );
   prt( (_TCHAR *)" -? or -h gives this brief help ... and exit with ERRORLEVEL 2" MEOR );

   // CLEAN UP
   Exit_Actions(); // clean up at exit

   // return to OS ...
   exit(2); // note HELP exit is 2
}


int InStri( _TCHAR * str, _TCHAR * s )
{
   int bret = 0;
   int i = (int)strlen(str);
   int j = (int)strlen(s);
   if( j == i ) {
      if(stricmp(str,s) == 0 )
         bret = 1;
   } else if ( j < i ) {
      int k = toupper(*s);
      int x = i - j;
      int m;
      for( m = 0; m < x; m++ )
      {
         if( toupper(str[m]) == k ) {
            // found first
            int n = 1;
            for( ; n < j; n++ ) {
               m++;
               if( toupper(str[m]) != toupper(s[n]) )
                  break;
            }
            if( n == j )
               return 1;
         }
      }
   }
   return bret;
}

static struct stat _s_buf;
static _TCHAR _s_timebuf[32];

bool is_valid_path( _TCHAR * path )
{
    static _TCHAR _s_cbuf[264];
    static _TCHAR _s_ebuf[264];
    _TCHAR * cp = _s_cbuf;
    _TCHAR * ep = _s_ebuf;
    _TCHAR * bgn;
    size_t i, off, len;
    int c;
    if (stat(path,&_s_buf) == 0)
        return true;

    // strip any quotes
    if (path[0] == '"')
        strcpy(cp,&path[1]);
    else
        strcpy(cp,path);

    len = strlen(cp);
    if (len > 0) {
        len--;  // back up to remove any trailing '/' or '\' char
        if (len > 0) {
            if (cp[len] == '"') {
                cp[len] = 0;
                len--;  // back up to remove any trailing '/' or '\' char
            }
        }
        if ((cp[len] == '/')||(cp[len] == '\\')) {
            cp[len] = 0;
        }
        if (stat(cp,&_s_buf) == 0)
            return true;


        // what about if it contains an ENV variable, like 0
        //  42: %USERPROFILE%\AppData\Local\Microsoft\WindowsApps                     [*NOT VALID PATH*!]
        if (strchr(cp, '%')) {
            _TCHAR * env;
            _TCHAR d;
            len = strlen(cp);
            off = 0;
            for (i = 0; i < len; i++) {
                c = cp[i];
                if (c == '%') {
                    cp[i] = 0;
                    i++;
                    bgn = &cp[i];   // begin of variable
                    for (; i < len; i++) {
                        d = cp[i];
                        if (d == '%') {
                            cp[i] = 0;  // end of varaible
                            break;
                        }
                    }
                    // get the value for the variable
                    env = getenv(bgn);
                    if (env) {
                        strcpy(ep, env);
                        off = strlen(ep);
                        if (off) {
                            off--;
                            c = ep[off];
                        }
                    }
                }
                ep[off++] = c;
            }
            ep[off] = 0;
            if (stat(ep, &_s_buf) == 0) {
                prt((char *)"\nMOD: %s", ep);
                return true;
            }
        }
    }
    
    return false;
}

_TCHAR *get_last_date() 
{
    _TCHAR *pt = ctime(&_s_buf.st_mtime);
    if (pt) {
        strcpy(_s_timebuf,pt);
        size_t len = strlen(_s_timebuf);
        while(len--) {
            if (_s_timebuf[len] > ' ')
                break;
            _s_timebuf[len] = 0;
        }
    } else {
        strcpy(_s_timebuf,"Bad time value!");
    }
    return _s_timebuf;
}

bool has_sig_chars( _TCHAR * path )
{
    size_t len = strlen(path);
    size_t i;
    for (i = 0; i < len; i++) {
        if (path[i] > ' ')
            return true;
    }
    return false;
}

void add_ok(_TCHAR *bgn)
{
    if (is_valid_path(bgn)) {
        prt((_TCHAR *)" [ok]");
        if (VERB9)
            prt((_TCHAR *)" %s", get_last_date());
    } else {
        prt((_TCHAR *)" [*NOT VALID PATH*!]");
    }
}

void showenv( _TCHAR * arg, _TCHAR * env )
{
   int icnt = 0;
   if(VERB2) {
      _TCHAR * buf = strdup(env);  // allocate a new buffer
      _TCHAR * bgn = buf;   // set first 'moving/changing' pointer
      _TCHAR * p = strchr(bgn,SEP_CHR); // get first location of ';' (or ':' for unix)
      size_t min, len;

      min = 0;
      if (VERB9) {
          if(p) {
             while(p) {
                *p = 0; // zero the char
                if (has_sig_chars(bgn)) {
                    len = strlen(bgn);
                    if (len > min)
                        min = len;
                }
                *p = SEP_CHR;   // put it back
                p++;
                bgn = p;    // set new bgn
                p = strchr(bgn,SEP_CHR);
             }
             if ( has_sig_chars(bgn) ) {
                    len = strlen(bgn);
                    if (len > min)
                        min = len;
             }
          }
          if (min)
              min++;
      }

      bgn = buf;   // set first 'moving/changing' pointer
      p = strchr(bgn,SEP_CHR); // get first location of ';' (or ':' for unix)
      prt((_TCHAR *)"%s=" MEOR, arg);
      if(p) {
         while(p) {
            *p = 0;
            if ( (strlen(bgn) > 0) && has_sig_chars(bgn) ) {
                icnt++;
                g_iret = 1; // set FOUND
                if (VERB3)
                    prt((_TCHAR *)"%3d: ", icnt);
                prt((_TCHAR *)"%s", bgn);
                if (VERB5) {
                    if (min) {
                        len = strlen(bgn);
                        len = (len < min) ? min - len : 1;
                        while(len--) prt((_TCHAR *)" ");
                    }
                    add_ok(bgn);
                }
                prt((_TCHAR *)MEOR);
            }
            p++;
            bgn = p;
            p = strchr(bgn,SEP_CHR);
         }
         if ( (strlen(bgn) > 0) && has_sig_chars(bgn) ) {
            icnt++;
            g_iret = 1; // set FOUND

            if (VERB3)
                prt((_TCHAR *)"%3d: ", icnt);

            prt((_TCHAR *)"%s", bgn);

            if (VERB5) {
                if (min) {
                    len = strlen(bgn);
                    len = (len < min) ? min - len : 1;
                    while(len--) prt((_TCHAR *)" ");
                }
                add_ok(bgn);
            }

            prt((_TCHAR *)MEOR);
         }
         p = bgn;
      } else {
         if ( (strlen(bgn) > 0) && has_sig_chars(bgn) ) {
            icnt++;
            g_iret = 1; // set FOUND

            if (VERB3)
                prt((_TCHAR *)"%3d: ", icnt);

            prt((_TCHAR *)"%s", bgn);

            if (VERB5) {
                if (min) {
                    len = strlen(bgn);
                    len = (len < min) ? min - len : 1;
                    while(len--) prt((_TCHAR *)" ");
                }
                add_ok(bgn);
            }

            prt((_TCHAR *)MEOR);
         }
         p = bgn;
      }
      if (VERB4)
          prt((_TCHAR *)"Shown %d items for [%s]..." MEOR, icnt, arg);

      // cleanup
      free(buf);
   } else {
      prt((_TCHAR *)"%s=%s" MEOR, arg,env);
   }
   if(test_buf) {
      if( InStri( env, test_buf ) ) {
         g_iret = 1;
      }
   }
   icnt_found++; // count a FOUND
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : WildComp2
// Return type: int 
// Arguments  : _TCHAR * lp1
//            : _TCHAR * lp2
// Description: Compare two components of a file name
//              FIX20010503 - Should NOT return TRUE on "c" with "cab"
//             FIX20060202 - move to showenv CPP coding
///////////////////////////////////////////////////////////////////////////////
int WildComp2( _TCHAR * lp1, _TCHAR * lp2 )
{
	int   flg = 0;
	size_t i1, i2, j1, j2, ilen;
	int  c, d;
	i1 = strlen(lp1);
	i2 = strlen(lp2);
	if( i1 && i2 )    // if BOTH have lengths
	{
      ilen = i1;
      if( i2 > i1 )
         ilen = i2;
		j2 = 0;
		for( j1 = 0; j1 < ilen; j1++ )
		{
			c = toupper(lp1[j1]);   // extract next char from each
			d = toupper(lp2[j2]);
         if( c == d )
         {
            j2++;
            continue;
         }

			// they are NOT equal, but some things are allowed
         if( c == 0 ) {
            if( d == '*' ) { // if this wild card is the last char
               j2++;
               if( lp2[j2] == 0 )   // get NEXT
               {
                  // ended with this asteric, so
                  flg = 1; // this matches NO rest of 2
               }
            }
            break;
         }
         if( d == 0 ) {
            if( c == '*' ) {
               j1++;
               if( lp1[j1] == 0 )   // get NEXT
               {
                  // ended with this asteric, so
                  flg = 1; // this matches NO rest of 2
               }
            }
            break;
         }
			{
				if( c == '*' )
				{
               j1++;
               if( lp1[j1] == 0 )   // get NEXT
               {
                  // ended with this asteric, so
                  flg = 1; // this matches all the rest of 2
					   break;   // out of here with a MATCH
               }
               // else we have somehting like *abc, which mean the asteric
               // matched what ever was in 2, up until this letter encountered
               c = toupper(lp1[j1]);
               j2++;    // asteric matched at least this one
      			if( lp2[j2] == 0 )   // 2 ended, but 1 has more
                  break;   // so no MATCH
               for( ; j2 < ilen; j2++ )
               {
         			d = toupper(lp2[j2]);
                  if( c == d )
                     break;
                  if( d == 0 )
                     break;
               }
               if( c == d )
               {
                  // found next of 1 in 2
                  j2++;
                  continue;
               }
               // else the char in 1 not present in two;
               break;   // no MATCH
				}

				if( d == '*' )
				{
               j2++;
               if( lp2[j2] == 0 )
               {
                  // 2 ends with asteric, so matches all rest in 1
                  flg = 1;
                  break;
               }
               d = toupper(lp2[j2]);
               j1++;    // asteric matched at least this one
      			if( lp1[j1] == 0 )   // 1 ended, but 2 has more
                  break;   // so no MATCH
               for( ; j1 < ilen; j1++ )   // find the 2 in 1
               {
         			c = toupper(lp1[j1]);
                  if( c == d )   // found it?
                     break;
                  if( c == 0 )   // or ran out of chars
                     break;
               }
               if( c == d )
               {
                  // found next of 2 in 1
                  j2++;
                  continue;
               }
               // else the char in 2 is not present in 1
					break;
				}

            if( ( c == '?' ) || ( d == '?' ) )
				{
					// One match char ok.
				}
				else
				{
					if( toupper( c ) != toupper( d ) )
						break;
				}
			}
			j2++;
		}
		if( !flg && ( j1 == ilen ) )
			flg = 1;
	}
	return flg;
}


int GotWild( _TCHAR * ps )
{
   if( strchr(ps,'*') || strchr(ps,'?') )
      return 1;
	return 0;
}

int count_env( void )
{
   int cnt = 0;
   _TCHAR ** env = _environ;
   _TCHAR * p = *env;
   while(p) {
      cnt++;
      env++;
      p = *env;
   }
   return cnt;
}

void search_approx( _TCHAR * arg )
{
   _TCHAR ** env = _environ;
   _TCHAR * p = *env;
   int iswild = GotWild(arg);
   while(p)
   {
      _TCHAR * p1 = strdup(p);
      _TCHAR * p2 = strchr(p1,'=');
      if(p2) {
         *p2 = 0;
         if( iswild ) {
            if( WildComp2( p1, arg ) ) {
               p2++;
               showenv( p1, p2 );
            }
         } else {
            if( InStri( p1, arg ) ) {
               p2++;
               showenv( p1, p2 );
            }
         }
      }
      free(p1);
      env++;
      p = *env;
   }
}

int main(int argc, _TCHAR* argv[])
{
   _TCHAR * arg;
   int c, icnt; 
   _TCHAR * env;
   int verb = 1;
   _TCHAR * pgm = argv[0];
    int i;
   PLE   ph = &env_list;
   PLE   pn;
   PENVLIST pe;

   prt_buf = 0;
   icnt_env = 0;
   icnt_found = 0;
   // environment
   if( argc < 2 ) {
      give_help(pgm);
   } else {
      for( i = 1; i < argc; i++ ) {
         arg = argv[i];
         if (strcmp(arg, "--version") == 0) {
             show_version(pgm);
             goto exit;
         }
         c = *arg;
         if(( c == '/' ) || ( c == '-' )) {
            arg++;
            c = toupper(*arg);
            switch(c)
            {
#ifdef   ADD_TEST_VAR
            case 'T':
               if(test_buf) {
                  prt((_TCHAR *)"ERROR: Already have a test item! [%s] ..." MEOR, test_buf);
                  give_help(pgm);
               }
               arg++;
               c = *arg;
               if( c == 0 ) {
                  prt((_TCHAR *)"ERROR: -t must have a string!" MEOR );
                  give_help(pgm);
               } else if((c == '"') || (c == '\'')) {
                  arg++;
                  test_buf = strdup(arg);
                  arg = test_buf;
                  icnt = (int)strlen(arg);
                  if(*arg == c) {
                     prt((_TCHAR *)"ERROR: -t must have a string!" MEOR );
                     give_help(pgm);
                  }
                  icnt--;
                  if( arg[icnt] == c ) {
                     arg[icnt] = 0;
                  } else {
                     prt((_TCHAR *)"ERROR: Since -t string starts with %c, it must end the same!" MEOR );
                     give_help(pgm);
                  }
               } else {
                  test_buf = strdup(arg);
               }
               break;
#endif // #ifdef   ADD_TEST_VAR
            case 'V':
               verb = verbal + 1; // we have a v, increase
               arg++;
               if(*arg) {
                  if( ISNUM(*arg) ) {
                     verb = atoi(arg);
                  } else {
                     prt((_TCHAR *)"ERROR: -v only takes a numeric value! [%s] ..." MEOR, arg);
                     give_help(pgm);
                  }
               }
               verbal = verb;
               break;
            case '?':
            case 'H':
               give_help(pgm);
               break;
            default:
               arg--;
               prt((_TCHAR *)"ERROR: Invalid option [%s] ..." MEOR, arg);
               give_help(pgm);
               break;
            }
         } else { // non switched argument
            Add2List(arg); // add to LIST to find
         }
      }
   }

   ListCount2(ph,&i);
   if(i == 0) {
      prt((_TCHAR *)"ERROR: No environment variable found in command!" MEOR);
      give_help(pgm);
   }
   
   icnt_env = count_env(); // get the environment count of variables

   Traverse_List(ph,pn) {
      pe = (PENVLIST)pn;
      arg = pe->name;
      env = getenv(arg);
      icnt = 0;
      if(env) {
         showenv(arg,env);
      } else {
         prt((_TCHAR *)"Unique variable [%s] NOT FOUND!" MEOR
            "Doing approximate search ..." MEOR, arg );
         search_approx(arg);
      }
      if( icnt_found ) {
         prt((_TCHAR *) "Found %d matching [%s] in %d variables." MEOR, icnt_found, arg, icnt_env );
      } else {
         prt((_TCHAR *) "NONE found matching [%s] in %d variables." MEOR, arg, icnt_env );
      }
      tot_icnt_found += icnt_found;
      icnt_found = 0;
   }
   if( i > 1 ) {
       prt((_TCHAR *) "Found total %d match(es) in %d variables." MEOR, tot_icnt_found, icnt_env );
   }

 exit:
   Exit_Actions();

   if (tot_icnt_found)
       g_iret = 1;  // set FOUND

	return g_iret;
}

// eof - showenv.cpp


