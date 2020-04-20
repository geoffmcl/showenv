
// showenvvers.h
#ifndef _showenvvers_h_
#define _showenvvers_h_

#define SE_VERS "1.0.3" // FIX20120408 - if -v3, add LINE COUNTS
// SE_VERS "1.0.2" // FIX20100714 - if -v, add if path valid

// "1.0.1" // FIX20061201 - add -t"string to find" return 1
// if found, else 0 ...
#define  ADD_TEST_VAR

// SE_VERS "1.0.0" // FIX20061115 - In Dell01, using MSVC8
#if (defined(_MSC_VER) && (_MSC_VER > 1300))
#pragma warning( disable:4996 )
#endif // #if (defined(_MSC_VER) && (_MSC_VER > 1300))

#define  MALLOC(a)   new _TCHAR[a]
#define  MFREE(a)    delete a

// FIX20010503 - Should NOT return TRUE on "c" with "cab"
// FIX20060202 - move to showenv CPP coding
// SE_VERS  "0.9.0"   // first version Feb 2006 - geoff mclane

#endif // #ifndef _showenvvers_h_
