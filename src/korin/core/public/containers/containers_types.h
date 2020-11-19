#pragma once

#include "core_types.h"
#include "templates/functional.h"

template<typename>										struct Link;
template<typename>										struct InlineList;
template<typename, typename = ThreeWayCompare>			struct BinaryNode;
template<typename, typename, typename = NullCompare>	struct Pair;

template<typename, typename = void>											class Array;
template<typename>															class StringBase;
template<typename, typename = void>											class List;
template<typename, typename = ThreeWayCompare, typename = void>				class BinaryTree;
template<uint64, typename...>												class TupleBase;
template<typename, typename, typename = ThreeWayCompare, typename = void>	class Map;
template<typename, typename = ThreeWayCompare, typename = void>				class Set;

using String = StringBase<ansichar>;