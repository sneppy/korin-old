#pragma once

#include "core_types.h"
#include "./types.h"
#include "./name.h"

/**
 * 
 */
template<typename T>
struct TypeDebugInfo;

/**
 * 
 */
template<>
struct TypeDebugInfo<void>
{
	/**
	 * Default debug name.
	 */
	static constexpr Name getDebugName()
	{
		return "void";
	}

	static constexpr Name debugName = "void";
};

#define DEFINE_DEBUG_TYPE(Type)\
	template<> struct TypeDebugInfo<Type> : public TypeDebugInfo<void>\
	{\
		static constexpr Name debugName = #Type;\
	};

/**
 * Debug info specialization for
 * built-in types
 */
DEFINE_DEBUG_TYPE(int8);
DEFINE_DEBUG_TYPE(int16);
DEFINE_DEBUG_TYPE(int32);
DEFINE_DEBUG_TYPE(int64);
DEFINE_DEBUG_TYPE(uint8);
DEFINE_DEBUG_TYPE(uint16);
DEFINE_DEBUG_TYPE(uint32);
DEFINE_DEBUG_TYPE(uint64);
/** @} */