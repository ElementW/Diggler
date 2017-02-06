#ifndef DIGGLER_PLATFORM_PREPROC_UTILS_HPP
#define DIGGLER_PLATFORM_PREPROC_UTILS_HPP

#define nocopyconstruct(c) c(const c&)=delete
#define nomoveconstruct(c) c(c&&)=delete

#define nocopy(c) c(const c&)=delete;c& operator=(const c&)=delete
#define nomove(c) c(c&&)=delete;c& operator=(c&&)=delete
#define nocopymove(c) nocopy(c);nomove(c)

#define defaultcopy(c) c(const c&)=default;c& operator=(const c&)=default
#define defaultmove(c) c(c&&)=default;c& operator=(c&&)=default
#define defaultcopymove(c) defaultcopy(c);defaultmove(c)

#define virtualcopy(c) virtual c& operator=(const c&)
#define virtualmove(c) virtual c& operator=(c&&)
#define virtualcopymove(c) virtualcopy(c);virtualmove(c)

#define purevirtualcopy(c) virtual c& operator=(const c&)=0
#define purevirtualmove(c) virtual c& operator=(c&&)=0
#define purevirtualcopymove(c) purevirtualcopy(c);purevirtualmove(c)

#endif /* DIGGLER_PLATFORM_PREPROC_UTILS_HPP */
