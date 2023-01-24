/*
   DETH - Doom Editor for Total Headcases, by Simon Oke and Antony Burden.
   HETH - Hexen Editor for Total Headcases, by Antony Burden.
   
   You are allowed to use any parts of this code in another program, as
   long as you give credits to the authors in the documentation and in
   the program itself.  Read the file README.1ST for more information.
   
   This program comes with absolutely no warranty.

   DEU-GO32.H - Some DJGPP/GO32 specific things.
   */

#ifndef DEU_GO32_H
#define DEU_GO32_H
#if defined(__GNUC__)

#include <pc.h>       /* For sound,nosound */
#include <libbcc.h>   /* For all borl**d-gfx */
#include <dpmi.h>     /* for _go32....memory() */

#define huge          /* No huge things in gcc */
/* Should be done in another way, but ... */

/*
#ifdef __cplusplus
extern "C" {
#endif
    extern int getch(void);
    extern int bioskey(int);
    extern void delay(unsigned);
#ifdef __cplusplus
}
#endif
*/


#ifndef max
#define max(x,y)   (((x)>(y))?(x):(y))
#define min(x,y)   (((x)<(y))?(x):(y))
#endif

#define farcoreleft() ((long)_go32_dpmi_remaining_physical_memory())


#define FreeSomeMemory()

#define GetMemory(x)         malloc(x)
#define FreeMemory(x)        free(x)
#define ResizeMemory(x,y)    realloc(x,y)
#define GetFarMemory(x)      malloc(x)
#define FreeFarMemory(x)     free(x)
#define ResizeFarMemory(x,y) realloc(x,y)

#endif  /* __GNUC__   */
#endif  /* DEU_GO32_H */



