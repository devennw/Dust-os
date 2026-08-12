#ifndef _DTOSDEF_
#define _DTOSDEF_

/* Pada sistem UP, *spin lock* frekuensi tinggi secara alami selaras (*aligned*)
 * sedangkan pada sistem MP, *spin lock* tersebut selaras dengan *cache*. */

#if defined(DUST_UP)

#define ALIGNED_SPIN_LOCK    KSPIN_LOCK

#else 
