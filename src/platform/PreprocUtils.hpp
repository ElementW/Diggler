#ifndef DIGGLER_PLATFORM_PREPROC_UTILS_HPP
#define DIGGLER_PLATFORM_PREPROC_UTILS_HPP

#define nocopy(c) c(const c&)=delete;c& operator=(const c&)=delete
#define nomove(c) c(c&&)=delete;c& operator=(c&&)=delete
#define nocopymove(c) nocopy(c);nomove(c)

#define defaultcopy(c) c(const c&)=default;c& operator=(const c&)=default
#define defaultmove(c) c(c&&)=default;c& operator=(c&&)=default
#define defaultcopymove(c) defaultcopy(c);defaultmove(c)

#endif /* DIGGLER_PLATFORM_PREPROC_UTILS_HPP */
