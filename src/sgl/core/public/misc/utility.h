#pragma once

/**
 * Defines an alias for a function
 */
#define FUNCTION_ALIAS(alias, func) \
	const auto & alias = func;

/**
 * Defines an alias for a templated function
 */
#define FUNCTION_ALIAS_TEMPLATE(alias, func) \
	template<typename... Args> \
	FORCE_INLINE auto alias(Args &&... args) -> decltype(func(forward<Args>(args)...)) \
	{ \
		return func(forward<Args>(args)...); \
	}

/**
 * Defines an alias for a object method
 */
#define METHOD_ALIAS(alias, func) \
	template<typename... Args> \
	FORCE_INLINE auto alias(Args &&... args) -> decltype(func(forward<Args>(args)...)) \
	{ \
		return func(forward<Args>(args)...); \
	}

/**
 * Defines an alias for a object const method
 */
#define METHOD_ALIAS_CONST(alias, func) \
	template<typename... Args> \
	FORCE_INLINE auto alias(Args &&... args) const -> decltype(func(forward<Args>(args)...)) \
	{ \
		return func(forward<Args>(args)...); \
	}
