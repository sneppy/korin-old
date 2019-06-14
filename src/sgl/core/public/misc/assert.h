#pragma once

#if BUILD_RELEASE

	#define CHECK(expr)
	#define CHECKF(expr)

#else

	#define CHECK(expr) \
		{ \
			if (UNLIKELY(!(expr))) \
			{ \
				fprintf(stderr, "Check failed at " __FILE__ ": " #expr) \
			} \
		} \
	
	#define CHECKF(expr, format, ...) \
		{ \
			if (UNLIKELY(!(expr))) \
			{ \
				fprintf(stderr, "Check failed at " __FILE__ ": " format "\n", ##__VA_ARGS__); \
			} \
		} \

#endif