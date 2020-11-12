#pragma once

/**
 * CHECK prints a warning if condition evaluates
 * to false. CHECKF prints a formatted warning
 */

#if BUILD_RELEASE
	#define CHECKF(expr, format, ...)
#else
	#define CHECKF(expr, format, ...)\
		{\
			if (UNLIKELY(!(expr)))\
			{\
				fprintf(stderr, "Check failed at " __FILE__ ":%u: " format "\n", __LINE__, ##__VA_ARGS__);\
			}\
		}
#endif

#define CHECK(expr) CHECKF(expr, #expr)
#define ASSERTF(expr, format, ...) CHECKF(expr, format, __VA_ARGS_)
#define ASSERT(expr) ASSERT(expr, #expr)
