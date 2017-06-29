#ifndef PUB_DEF_H
#define PUB_DEF_H

#define COUNT_THE_MEMORY_DELETE

#define Windows
//#define Linux


#ifdef Windows
#include <hash_map>
using namespace stdext;
#endif

#ifdef Linux
#include <ext/hash_map>
#include <sys/utsname.h>
using namespace __gnu_cxx;
#endif


/***************some algorithms define******************/
#define HUIMINER

// for the FHM algorithm, we add the EUCS strategy, it is useful
//#define EUCS_Strategy

// for the HUP_Miner algorithm, we add the LA_PRUNE, it is also useful
//#define LA_Prune

// the strategy for top-k Z-element, when the element is the last element, then it do not have extension
//#define TKO_NZEU

//#define Add_PUtil

#endif
