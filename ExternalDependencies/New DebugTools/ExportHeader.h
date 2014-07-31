#ifndef EXPORTDEBUGHEADER_H
#define EXPORTDEBUGHEADER_H

#ifdef	DEBUG_EXPORTS
#define DEBUG_SHARED __declspec( dllexport )
#else
#define DEBUG_SHARED __declspec( dllimport )
#endif

#endif // EXPORTHEADER_H