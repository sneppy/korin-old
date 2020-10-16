#include "core_types.h"
#include "containers/tree.h"
#include "containers/string.h"
#include "containers/tuple.h"
#include "containers/map.h"
#include "algorithm/sort.h"
#include "math/mat4.h"
#include "math/vec3.h"
#include "algorithm/sort.h"
#include "templates/function.h"
#include "templates/functional.h"
#include "templates/types.h"

template<typename AlphaT>
struct AlphabetTraits
{
	
};

template<>
struct AlphabetTraits<char>
{
	static constexpr char terminalSymbol = '\0';
};

template<typename AlphaT>
struct StateBase
{
	/**
	 * 
	 */
	struct FindState
	{
		FORCE_INLINE int32 operator()(const StateBase * a, const StateBase * b) const
		{
			return ThreeWayCompare()(a->id, b->id);
		}
	};

	/// State unique identifier
	uint64 id;

	/// Out connections
	BinaryTree<StateBase*, FindState> nextStates;

	/// List of input connections
	BinaryTree<StateBase*, FindState> prevStates;

	/**
	 * 
	 */
	StateBase()
		: id{0}
		, nextStates{}
		, prevStates{}
	{
		// Use incremental counter as state id
		static uint64 globalCounter = 0;
		id = ++globalCounter;
	}

	/**
	 * 
	 */
	virtual ~StateBase()
	{
		//
	}

	/**
	 * 
	 */
	virtual bool enter(const AlphaT * input, int32 & numRead) const = 0;

	/**
	 * 
	 */
	template<typename StateU>
	FORCE_INLINE constexpr bool is() const
	{
		return IsSameType<StateBase, StateU>::value;
	}

	/**
	 * 
	 */
	FORCE_INLINE StateBase * addNextState(StateBase * other)
	{
		// Add to list of next states, and
		// add self to other's list of prev
		// states
		nextStates.insertUnique(other);
		other->prevStates.insertUnique(this);

		return other;
	}

	/**
	 * 
	 */
	FORCE_INLINE StateBase * removeNextState(StateBase * other)
	{
		// Find next state
		auto nextStateIt = nextStates.find(other);

		if (nextStateIt != nextStates.end())
		{
			(*nextStateIt)->prevStates.remove(this);
			nextStates.remove(nextStateIt);
		}

		return other;
	}

	/**
	 * 
	 */
	FORCE_INLINE StateBase * mergeNextState()
	{
		// TODO: Assert only one next state
		StateBase * nextState = *nextStates.begin();
		nextState->prevStates.remove(this);

		for (StateBase * prevState : prevStates)
		{
			prevState->nextStates.remove(this);
			prevState->addNextState(nextState);
		}

		return nextState;
	}

	/**
	 * 
	 */
	FORCE_INLINE StateBase * mergePrevState()
	{
		// TODO: Assert only one prev state
		StateBase * prevState = *prevStates.begin();
		prevState->nextStates.remove(this);

		for (StateBase * nextState : nextStates)
		{
			nextState->prevStates.remove(this);
			prevState->addNextState(nextState);
		}

		return prevState;
	}

#if BUILD_DEBUG
	/**
	 * @brief Returns node debug name
	 */
	virtual FORCE_INLINE String getDebugName() const
	{
		return String::format("StateBase#%u", id);
	}
#else
	/**
	 * @brief Just to avoid compatibility problems
	 */
	static FORCE_INLINE const String & getDebugName()
	{
		static String debugName = "";
		return stateName
	}
#endif
};

template<typename AlphaT>
struct StateEpsilon : public StateBase<AlphaT>
{
	//////////////////////////////////////////////////
	// StateBase interface
	//////////////////////////////////////////////////
	
	virtual bool enter(const AlphaT*, int32&) const;

#if BUILD_DEBUG
	virtual FORCE_INLINE String getDebugName() const override
	{
		return String::format("Epsilon#%u", this->id);
	}
#endif
};

template<typename AlphaT>
bool StateEpsilon<AlphaT>::enter(const AlphaT * input, int32 & numRead) const
{
	// Doesn't read characters, alwasy returns true
	numRead = 0;
	return true;
}

template<typename AlphaT>
struct StateAny : public StateBase<AlphaT>
{
	//////////////////////////////////////////////////
	// StateBase interface
	//////////////////////////////////////////////////
	
	virtual bool enter(const AlphaT*, int32&) const;

#if BUILD_DEBUG
	virtual FORCE_INLINE String getDebugName() const override
	{
		return String::format("Any#%u", this->id);
	}
#endif
};

template<typename AlphaT>
bool StateAny<AlphaT>::enter(const AlphaT * input, int32 & numRead) const
{
	// Reads any one symbol except terminal, always returns true
	return (numRead = (*input != AlphabetTraits<AlphaT>::terminalSymbol));
}

template<typename AlphaT>
struct StateSymbol : public StateBase<AlphaT>
{
	/**
	 * 
	 */
	FORCE_INLINE StateSymbol(const AlphaT & inSymbol)
		: symbol{inSymbol}
	{
		//
	}

	/**
	 * 
	 */
	FORCE_INLINE const AlphaT & getSymbol() const
	{
		return symbol;
	}

	//////////////////////////////////////////////////
	// StateBase interface
	//////////////////////////////////////////////////
	
	virtual bool enter(const AlphaT*, int32&) const override;

#if BUILD_DEBUG
	virtual FORCE_INLINE String getDebugName() const override
	{
		return String{"Symbol<"} + symbol + String::format(">#%u", this->id);
	}
#endif

private:
	/// Symbol to match against
	const AlphaT symbol;
};

template<typename AlphaT>
bool StateSymbol<AlphaT>::enter(const AlphaT * input, int32 & numRead) const
{
	return !!(numRead = (*input == symbol));
}

template<typename AlphaT>
struct StateString : public StateBase<AlphaT>
{
	using StateSymbolT = StateSymbol<AlphaT>;

	/**
	 * 
	 */
	FORCE_INLINE StateString(const AlphaT * inString, sizet len)
		: string{nullptr}
		, length{len}
	{
		// TODO:
	}

	template<typename ...StateSymbolListT>
	FORCE_INLINE StateString(const StateSymbolListT && ...symbols)
		: string{nullptr}
		, length{sizeof...(symbols)}
	{
		// TODO:
	}

	//////////////////////////////////////////////////
	// StateBase interface
	//////////////////////////////////////////////////
	
	virtual ~StateString() override;
	virtual bool enter(const char*, int32&) const override;

protected:
	/// Sequence of symbols
	const AlphaT * string;

	/// Sequence length
	const sizet length;
};

template<>
struct StateString<char> : public StateBase<char>
{
	using StateSymbolT = StateSymbol<char>;

	/**
	 * 
	 */
	FORCE_INLINE StateString(const String & inString)
		: string{inString}
	{
		//
	}

	/**
	 * 
	 */
	FORCE_INLINE StateString(String && inString)
		: string{move(inString)}
	{
		//
	}

	/**
	 * 
	 */
	FORCE_INLINE StateString(const char * inString, sizet len)
		: string{inString, len}
	{
		//
	}

	/**
	 * 
	 */
	template<typename It>
	FORCE_INLINE StateString(It beginSymbol, It endSymbol, sizet len = 0)
		: string{len}
	{
		// TODO: Compute iterator length

		for (uint32 idx = 0; beginSymbol != endSymbol; ++beginSymbol, ++idx)
		{
			const_cast<String&>(string)[idx] = (*beginSymbol)->getSymbol();
		}
	}

	//////////////////////////////////////////////////
	// StateBase interface
	//////////////////////////////////////////////////
	
	virtual bool enter(const char*, int32&) const override;

#if BUILD_DEBUG
	virtual FORCE_INLINE String getDebugName() const override
	{
		return String::format("String<%s>#%u", *string, this->id);
	}
#endif

protected:
	/// Sequence of symbols
	const String string;
};

bool StateString<char>::enter(const char * input, int32 & numRead) const
{
	if (string == input)
	{
		// Input matches string, return true
		numRead = string.getLength();
		return true;
	}
	
	numRead = 0;
	return false;
}

struct StateAlpha : public StateBase<char>
{
	//////////////////////////////////////////////////
	// StateBase interface
	//////////////////////////////////////////////////
	
	virtual bool enter(const char*, int32&) const override;
};

bool StateAlpha::enter(const char * input, int32 & numRead) const
{
	return !!(numRead = (*input >= 'a' && *input <= 'z') || (*input >= 'A' && *input <= 'Z'));
}

struct StateDigit : public StateBase<char>
{
	//////////////////////////////////////////////////
	// StateBase interface
	//////////////////////////////////////////////////
	
	virtual bool enter(const char*, int32&) const override;
};

bool StateDigit::enter(const char * input, int32 & numRead) const
{
	return !!(numRead = (*input >= '0' && *input <= '9'));
}

template<typename AlphaT>
class NFA
{
	using StateT = StateBase<AlphaT>;

private:
	/**
	 * 
	 */
	template<typename StateTT, typename ...StateArgsT>
	FORCE_INLINE StateT * createState(StateArgsT && ...stateArgs) const
	{
		return new (stateMalloc->alloc(sizeof(StateTT), alignof(StateTT))) StateTT{forward<StateArgsT>(stateArgs)...};
	}

	/**
	 * 
	 */
	FORCE_INLINE void destroyState(StateT * state) const
	{
		// Destruct state
		state->~StateT();

		// Free memory
		stateMalloc->free(state);
	}

public:
	/**
	 * 
	 */
	NFA();

private:
	/**
	 * 
	 */
	FORCE_INLINE bool isStart(const StateT * state) const
	{
		return startState->id == state->id;
	}

	/**
	 * 
	 */
	FORCE_INLINE bool isAccepted(const StateT * state) const
	{
		return acceptedState->id == state->id;
	}

public:
	/**
	 * 
	 */
	template<typename StateTT, typename ...StateArgsT>
	FORCE_INLINE StateT * pushState(StateArgsT && ...stateArgs)
	{
		// Create state and push in allocated states
		StateT * newState = createState<StateTT>(forward<StateArgsT>(stateArgs)...);
		allocatedStates.add(newState);

		return newState;
	}

	/**
	 * 
	 */
	const StateT * accept(const AlphaT * input) const;

private:
	/**
	 * 
	 */
	void optRemoveEpsilons();
	/**
	 * 
	 */
	void optConcatSymbols();

public:

	/**
	 * 
	 */
	void optimize(uintp flags);

	/**
	 * 
	 */
	String toString() const;

	/**
	 * 
	 */
	static NFA * parseRegex(const AlphaT * pattern, uintp flags = 0x0);

public: // TODO: Private
	/// State allocator
	mutable MallocBase * stateMalloc;

	/// Set of allocated states
	Array<StateT*> allocatedStates;

	/// Single start state
	StateT * startState;

	/// Single accepted state
	StateT * acceptedState;
};

template<typename AlphaT>
NFA<AlphaT>::NFA()
	: stateMalloc{nullptr}
	, allocatedStates{}
	, startState{nullptr}
	, acceptedState{nullptr}
{
	if (!stateMalloc)
	{
		// Create state allocator
		stateMalloc = new MallocAnsi;
	}

	// Create start state and accepted state
	using Epsilon = StateEpsilon<char>;
	startState = createState<Epsilon>();
	acceptedState = createState<Epsilon>();
}

template<typename AlphaT>
const typename NFA<AlphaT>::StateT * NFA<AlphaT>::accept(const AlphaT * input) const
{
	using ConnectionT = Pair<StateT*, const AlphaT*>;
	int32 numRead = 0;

	// Stack of connections to explore
	List<ConnectionT> connections;

	if (startState->enter(input, numRead))
	{
		// Push first connection
		connections.pushBack(ConnectionT{startState, input + numRead});

		ConnectionT connection{nullptr, nullptr}; while (connections.popBack(connection))
		{
			// Get input and state
				  StateT * currState = connection.first;
			const AlphaT * currInput = connection.second;

			// TODO: Add to list of matches
			if (isAccepted(currState))
			{
				return currState;
			}

			for (auto nextState : currState->nextStates)
			{
				if (nextState->enter(currInput, numRead))
				{
					// Push next state on stack
					connections.pushBack(ConnectionT{nextState, currInput + numRead});
				}
			}
		}
	}

	return nullptr;
}

template<typename AlphaT>
void NFA<AlphaT>::optRemoveEpsilons()
{
	using State = NFA::StateT;
	using Epsilon = StateEpsilon<AlphaT>;

	for (State * state : allocatedStates)
	{
		if (dynamic_cast<Epsilon*>(state))
		{
			State * mergedState = nullptr;

			if (state->prevStates.getCount() == 1)
			{
				// We can remove this epsilon state
				mergedState = state->mergePrevState();
			}
			else if (state->nextStates.getCount() == 1)
			{
				// We can merge with next state
				mergedState = state->mergeNextState();
			}
		}
	}
}

template<typename AlphaT>
void NFA<AlphaT>::optConcatSymbols()
{
	using State = NFA::StateT;
	using Symbol = StateSymbol<AlphaT>;
	using SString = StateString<AlphaT>;

	// Use a queue and a map of visited
	// to traverse automaton
	List<State*> visitQueue;
	BinaryTree<State*, typename State::FindState> visited;
	Tuple<Symbol*, 2> symSequence{};
	Array<Symbol*> symbols;
	uint32 sequenceLen = 0;

	for (State * nextState : startState->nextStates)
	{
		// Push states next to start state to visit queue
		visitQueue.pushBack(startState);
	}

	State * currState; while (visitQueue.popBack(currState))
	{
		if (!visited.findNode(currState))
		{
			Symbol * symbol = nullptr;
			bool commitSequence = false;

			// If not visited yet, visit
			visited.insert(currState);

			if ((symbol = dynamic_cast<Symbol*>(currState)))
			{
				const bool hasLinearNext = symbol->nextStates.getCount() <= 1;
				const bool hasLinearPrev = symbol->prevStates.getCount() <= 1;

				if (!symSequence[0] && hasLinearNext)
				{
					// Start new sequence
					symSequence[0] = symbol;
					symbols(0) = symbol;
					sequenceLen = 1;
				}
				else if (symSequence[0] && hasLinearPrev)
				{
					// Append to sequence
					symSequence[1] = symbol;
					symbols(sequenceLen) = symbol;
					sequenceLen += 1;
				}
				else
				{
					// Set resolve flag
					commitSequence = true;
				}
			}
			else
			{
				// Set resolve flag
				commitSequence = true;
			}

			if (symSequence[0] && commitSequence)
			{
				if (symSequence[1])
				{
					// Create string state from sequence of symbols
					State * string = createState<SString>(symbols.begin(), symbols.end(), sequenceLen);

					for (State * prevState : symSequence[0]->prevStates)
					{
						prevState->addNextState(string);
					}

					// TODO: Unlink start symbol

					for (State * nextState : symSequence[1]->nextStates)
					{
						string->addNextState(nextState);
					}

					// TODO: Unlink end symbol
				}

				// Reset sequence
				symSequence[0] = nullptr;
			}

			for (State * nextState : currState->nextStates)
			{
				// Add next states to visit queue
				visitQueue.pushBack(nextState);
			}
		}
	}
}

template<typename AlphaT>
void NFA<AlphaT>::optimize(uintp /* flags */)
{
	// Remove suprious epsilons
	optRemoveEpsilons();

	// Convert sequences of symbols to strings
	//optConcatSymbols();
}

template<typename AlphaT>
String NFA<AlphaT>::toString() const
{
	using State = Pair<NFA::StateT*, uint32>;
	
	String outString = "NFA:\n";

	List<State> visitQueue{};
	BinaryTree<uint64, ThreeWayCompare> visitedIds;

	// Push start state
	visitQueue.pushBack(State{startState, 0u});
	
	State visiting{nullptr, 0u}; while (visitQueue.popBack(visiting))
	{
		// Get current state and depth
		auto currState = visiting.first;
		auto currDepth = visiting.second;

		if (!visitedIds.findNode(currState->id))
		{
			// Add to already visited
			visitedIds.insert(currState->id);

			// Append state name
			for (uint32 depth = 0; depth < currDepth; ++depth) outString += '|';
			outString.appendFormat("%s ->\n", *currState->getDebugName());

			for (auto nextStates : currState->nextStates)
			{
				// Add next states to visit queue
				visitQueue.pushBack(State{nextStates, currDepth + 1});
			}
		}
		else
		{
			// Append repeated state, truncate search
			for (uint32 depth = 0; depth < currDepth; ++depth) outString += '|';
			outString.appendFormat("(repeated %s) -> (...)\n", *currState->getDebugName());
		}
	}

	return (outString += ';');
}

template<>
NFA<char> * NFA<char>::parseRegex(const char * pattern, uintp /* flags */)
{
	using State = NFA<char>::StateT;
	using Epsilon = StateEpsilon<char>;
	using Any = StateAny<char>;
	using Symbol = StateSymbol<char>;
	using Alpha = StateAlpha;
	using Digit = StateDigit;

	// Create new automaton
	NFA<char> * automaton = new NFA<char>;

	// Valuable states
#define MAX_GROUP_DEPTH 127
	State * currState = automaton->startState;
	State * groups[MAX_GROUP_DEPTH] = {automaton->startState, currState};
	State * junctions[MAX_GROUP_DEPTH] = {automaton->acceptedState};
	int8 currGroup = 1;

	for (; *pattern != '\0'; ++pattern)
	{
		switch (*pattern)
		{
			case '[':
			{
				// Add epsilon state for begin or
				State * epsilon = automaton->pushState<Epsilon>();

				// TODO

				break;
			}

			case ']':
			{
				// Add epsilon state for end or
				State * epsilon = automaton->pushState<Epsilon>();

				// TODO

				break;
			}

			case '(':
			{
				// Add epsilon state for begin group
				State * epsilon = automaton->pushState<Epsilon>();

				currState->addNextState(epsilon);
				currState = epsilon;

				if (currGroup + 1 < MAX_GROUP_DEPTH)
				{
					// Push group on stack
					groups[currGroup] = epsilon;
					junctions[currGroup] = automaton->pushState<Epsilon>();
					groups[++currGroup] = epsilon;
				}

				break;
			}

			case ')':
			{
				if (currGroup > 0)
				{
					// Get group junction
					State * epsilon = junctions[--currGroup];
					
					currState->addNextState(epsilon);
					currState = epsilon;
				}
				else
					; // TODO: Set error code, or exception maybe?

				break;
			}
			
			case '|':
			{
				// Attach to current group
				State * epsilon = junctions[currGroup - 1];

				currState->addNextState(epsilon);
				currState = groups[currGroup - 1];

				break;
			}
			
			case '+':
			{
				// Add epsilon state
				State * epsilon = automaton->pushState<Epsilon>();
				
				//* At least once
				epsilon->addNextState(groups[currGroup]);
				currState->addNextState(epsilon);

				currState = epsilon;

				break;				
			}

			case '*':
			{
				// Add epsilon state
				State * epsilon = automaton->pushState<Epsilon>();
				
				//* At most once
				groups[currGroup]->addNextState(epsilon);

				//* At least once
				epsilon->addNextState(groups[currGroup]);
				currState->addNextState(epsilon);

				currState = epsilon;

				break;	
			}

			case '?':
			{
				// Add epsilon state
				State * epsilon = automaton->pushState<Epsilon>();

				//* At most once
				groups[currGroup]->addNextState(epsilon);
				currState->addNextState(epsilon);

				currState = epsilon;

				break;
			}

			case '.':
			{
				// Create any state
				State * epsilon = automaton->pushState<Epsilon>();
				State * any = automaton->pushState<Any>();

				currState
					->addNextState(epsilon)
					->addNextState(any);
				
				currState = any;

				// Replace current group
				groups[currGroup] = epsilon;

				break;
			}

			case '\\':
			{
				// Created state depends on sequence
				State * epsilon = automaton->pushState<Epsilon>();
				State * lambda = nullptr;

				switch (*(++pattern))
				{
					case 'd':
					{
						// Any digit symbol
						lambda = automaton->pushState<Digit>();
						break;
					}
					
					default:
					{
						// Treat as normal character
						lambda = automaton->pushState<Symbol>(*pattern);
						break;
					}
				}
				
				currState
					->addNextState(epsilon)
					->addNextState(lambda);

				currState = lambda;

				// Replace current group
				groups[currGroup] = epsilon;
				
				break;
			}
			
			default:
			{
				// Epsilon is used to form one-item group
				State * epsilon = automaton->pushState<Epsilon>();
				State * symbol = automaton->pushState<Symbol>(*pattern);
				
				currState
					->addNextState(epsilon)
					->addNextState(symbol);
				
				currState = symbol;

				// Replace current group
				groups[currGroup] = epsilon;

				break;
			}
		}
	}

	// Set single accepted state
	currState->addNextState(automaton->acceptedState);

	return automaton;
}

int32 main()
{
	//compileRegex("^https?://\\w+[\\.\\w+]+$");
	//NFA<char> * automaton = NFA<char>::parseRegex("https?://.+(\\..+)+");
	NFA<char> * automaton = NFA<char>::parseRegex("http|ftp|ssh");
	printf("%s\n", *automaton->toString());
	automaton->optimize(0);
	printf("%s\n", *automaton->toString());
	printf("%p\n", automaton->accept("ssl"));

	return 0;
}
