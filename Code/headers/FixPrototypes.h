#ifndef _FixPrototypes_Hpp
#define _FixPrototypes_Hpp

// -- If we're not using GNU C, elide __attribute__
// -- highly unlikely since this is a *nix based feature
// -- however should we change, for ease of conversion we
// -- have preset this here.
#ifndef __GNUC__
#  define  __attribute__(x) // -- do nothing because we are not GNUC compatable
#endif

#ifndef AUTO_DEFINE
#define AUTO_DEFINE(protos) protos
#endif

// -- basic prototyping
#define Prototype(_xName, _xProtos)                             extern _xName AUTO_DEFINE(_xProtos)
#define PrototypeNoReturn(_xName, _xProtos)                     extern _xName AUTO_DEFINE(_xProtos) __attribute__((noreturn))
#define PrototypeFormat(_xName, _xProtos, _startArg, _lastArg)  extern _xName AUTO_DEFINE(_xProtos) __attribute__((format(printf, _startArg, _lastArg)))

#endif


