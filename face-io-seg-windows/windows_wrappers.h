#ifndef __wtypes_h__
#include <wtypes.h>
#endif

#ifndef __WINDEF_
#include <windef.h>
#endif


/*
 * Generic handle type - intended to extend uniqueness beyond
 * that available with a simple pointer. It should scale for either
 * IA-32 or IA-64.
 */
//typedef struct {
//    void * p;                   /* Pointer to actual object */
 //   unsigned int x;             /* Extra information - reuse count etc */
//} ptw32_handle_t;

//typedef ptw32_handle_t pthread_t;
//typedef struct pthread_attr_t_ * pthread_attr_t;

int clock_gettime(int X, struct timeval *tv);

//LARGE_INTEGER getFILETIMEoffset();