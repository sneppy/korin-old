#pragma once

#include "core_types.h"
#include "templates/functional.h"

template<typename, typename = void>											class Array;
template<typename>															class StringBase;
template<typename>															class Link;
template<typename, typename = void>											class List;
template<typename, typename = ThreeWayCompare>								class BinaryNode;
template<typename, typename = ThreeWayCompare, typename = void>				class BinaryTree;
template<typename, typename, typename = NullCompare>						class Pair;
template<typename, typename, typename = ThreeWayCompare, typename = void>	class Map;
template<typename, typename = ThreeWayCompare, typename = void>				class Set;

using String = StringBase<ansichar>;