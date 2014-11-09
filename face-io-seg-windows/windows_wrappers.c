#include "windows_wrappers.h"

//#ifdef WINDOWS_OS
LARGE_INTEGER
   getFILETIMEoffset()
{
   SYSTEMTIME s;
   FILETIME f;
   LARGE_INTEGER t;

   s.wYear = 1970;
   s.wMonth = 1;
   s.wDay = 1;
   s.wHour = 0;
   s.wMinute = 0;
   s.wSecond = 0;
   s.wMilliseconds = 0;
   SystemTimeToFileTime(&s, &f);
   t.QuadPart = f.dwHighDateTime;
   t.QuadPart <<= 32;
   t.QuadPart |= f.dwLowDateTime;
   return (t);
}

int clock_gettime(int X, struct timeval *tv)
{
   LARGE_INTEGER           t;
   FILETIME                f;
   double                  microseconds;
   static LARGE_INTEGER    offset;
   static double           frequencyToMicroseconds;
   static int              initialized = 0;
   static BOOL             usePerformanceCounter = 0;

   if (!initialized) {
      LARGE_INTEGER performanceFrequency;
      initialized = 1;
      usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
      if (usePerformanceCounter) {
         QueryPerformanceCounter(&offset);
         frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;
      } else {
         offset = getFILETIMEoffset();
         frequencyToMicroseconds = 10.;
      }
   }
   if (usePerformanceCounter) QueryPerformanceCounter(&t);
   else {
      GetSystemTimeAsFileTime(&f);
      t.QuadPart = f.dwHighDateTime;
      t.QuadPart <<= 32;
      t.QuadPart |= f.dwLowDateTime;
   }

   t.QuadPart -= offset.QuadPart;
   microseconds = (double)t.QuadPart / frequencyToMicroseconds;
   t.QuadPart = (LONGLONG)microseconds;
   tv->tv_sec = (long)t.QuadPart / 1000000;
   tv->tv_usec = t.QuadPart % 1000000;
   return (0);
}

/* Subtract the ‘struct timeval’ values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0. */

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}
//#endif