#ifndef DIGGLER_PLATFORM_FIXES_GLIBC
#define DIGGLER_PLATFORM_FIXES_GLIBC

// Disable some names (major, minor, makedev) inappropriately reserved as macros
// by the morons glibc devs are.
// https://bugzilla.redhat.com/show_bug.cgi?id=130601
#ifndef _SYS_SYSMACROS_H
  #define _SYS_SYSMACROS_H 1
  #define _SYS_SYSMACROS_H_OUTER 1
#endif

#ifdef major
  #undef major
  #undef minor
  #undef makedev
#endif

#endif /* DIGGLER_PLATFORM_FIXES_GLIBC */
