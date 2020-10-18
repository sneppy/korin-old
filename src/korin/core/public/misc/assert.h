#pragma once

/**
 * CHECK prints a warning if condition evaluates
 * to false. CHECKF prints a formatted warning
 */

#if BUILD_RELEASE

	#define CHECK(expr)
	#define CHECKF(expr, format, ...)

#else

	#define CHECK(expr)\
		{\
			if (UNLIKELY(!(expr)))\
			{\
				fprintf(stderr, "Check failed at " __FILE__ ":%u: " #expr, __LINE__);\
			}\
		}
	
	#define CHECKF(expr, format, ...)\
		{\
			if (UNLIKELY(!(expr)))\
			{\
				fprintf(stderr, "Check failed at " __FILE__ ":%u: " format "\n", __LINE__, ##__VA_ARGS__);\
			}\
		}

#endif

#define ASSERT(expr) CHECK(expr)
#define ASSERTF(expr, format, ...) CHECKF(expr, format, __VA_ARGS_)