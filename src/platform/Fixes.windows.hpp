#ifndef DIGGLER_PLATFORM_FIXES_WINDOWS
#define DIGGLER_PLATFORM_FIXES_WINDOWS

// Microsoft has a shitty habit of polluting the user namespace with frequently used names

#define WIN32_LEAN_AND_MEAN 1
#define NOMINMAX 1
#include <windows.h>
#undef near
#undef far

#endif /* DIGGLER_PLATFORM_FIXES_WINDOWS */
