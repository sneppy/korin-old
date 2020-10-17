#pragma once

#include "core_types.h"
#include "templates/functional.h"

template<typename, typename>								class Array;
															class String;
template<typename, sizet>									class Tuple;
template<typename>											class Link;
template<typename>											class List;
template<typename, typename = ThreeWayCompare>				class BinaryNode;
template<typename, typename, typename = NullCompare>		class Pair;
template<typename, typename, typename = ThreeWayCompare>	class Map;
template<typename, typename = ThreeWayCompare>				class Set;