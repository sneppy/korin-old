#pragma once

#include "core_types.h"
#include "containers/list.h"
#include "containers/pair.h"
#include "containers/set.h"
#include "./regex_types.h"
#include "./state.h"

namespace Re
{
	template<typename> class AutomatonBuilder;
	template<typename> class AutomatonOptimizer;

	/**
	 * This class implements a non-deterministic
	 * finite automaton. This kind of automata
	 * can be used to implements regex and other
	 * similar features.
	 * 
	 * Example:
	 * ```cpp
	 * using namespace Re;
	 * Automaton<char> automaton;
	 * AutomatonBuilder<char> builder{automaton}; // Building regex `(a|b)+`
	 * 
	 * builder.beginGroup();
	 * builder.pushState<StateSymbol<char>>('a');
	 * builder.pushBranch();
	 * builder.pushState<StateSymbol<char>>('b');
	 * builder.endGroup();
	 * builder.endGroup();
	 * 
	 * automaton.acceptString("abbabba"); // True
	 * automaton.acceptString("abbacba"); // False
	 * ```
	 * 
	 * @param AlphaT type of the automaton alhabet
	 */
	template<typename AlphaT>
	class Automaton
	{
		friend AutomatonBuilder<AlphaT>;
		friend AutomatonOptimizer<AlphaT>;
		friend Re::Regex;

		/// State types definitions
		/// @{
		using StateT = StateBase<AlphaT>;
		using FindStateT = typename StateT::FindState;
		using EpsilonT = StateEpsilon<AlphaT>;
		using AnyT = StateAny<AlphaT>;
		using SymbolT = StateSymbol<AlphaT>;
		using StringT = StateString<AlphaT>;
		/// @}

		using AlphabetTraitsT = AlphabetTraits<AlphaT>;
		using AlphaSymbolT = typename AlphabetTraitsT::SymbolT;
		using AlphaStringT = typename AlphabetTraitsT::StringT;

		using BuilderT = AutomatonBuilder<AlphaT>;
		using OptimizerT = AutomatonOptimizer<AlphaT>;
	
	private:
		/**
		 * Allocates and constructs a new state
		 * with the given parameters
		 * 
		 * @param StateAnyT type of the state
		 * @param createStateArgs arguments
		 * 	passed to the state constructor
		 * @return ptr to created state
		 */
		template<typename StateAnyT, typename ...CreateStateArgsT>
		FORCE_INLINE StateAnyT * createState(CreateStateArgsT && ...createStateArgs)
		{
			return new (MallocObject<StateAnyT>{}.alloc()) StateAnyT{forward<CreateStateArgsT>(createStateArgs)...};
		}

		/**
		 * Destroys and deallocates state.
		 * 
		 * @param state ptr to state to
		 * 	be deallocated
		 */
		FORCE_INLINE void destroyState(StateT * state)
		{
			state->~StateT();
			MallocObject<StateT>{}.free(state);
		}
	
	public:
		/**
		 * Default constructor.
		 */
		Automaton();

		/**
		 * Destroy automaton. Deallocates
		 * all states.
		 */
		~Automaton();

		/**
		 * Returns true if the provided
		 * sequence of symbols is accepted
		 * by the automaton, i.e. if
		 * the automaton can consume all
		 * the input symbols and terminate
		 * on the accepted state.
		 * 
		 * Example:
		 * ```
		 * (ab)+ acceptString "abc" = false
		 * (abc)+ acceptString "abc" = true
		 * ```
		 * 
		 * @param input sequence of input
		 * 	symbols
		 */
		bool acceptString(const AlphaStringT & input) const;

		/**
		 * Returns a pointer to the start
		 * state.
		 */
		FORCE_INLINE const StateT * getStartState() const
		{
			return startState;
		}

		/**
		 * Returns true if other is the
		 * start state.
		 */
		FORCE_INLINE bool isStartState(const StateT * other) const
		{
			return FindStateT{}(other, startState) == 0;
		}

		/**
		 * Returns a pointer to the accepted
		 * state.
		 */
		FORCE_INLINE const StateT * getAcceptedState() const
		{
			return acceptedState;
		}

		/**
		 * Returns true if other is the
		 * accepted state.
		 */
		FORCE_INLINE bool isAcceptedState(const StateT * other) const
		{
			return FindStateT{}(other, acceptedState) == 0;
		}

		/**
		 * Create a new state and add to
		 * the list of allocated states.
		 * 
		 * @param StateAnyT type of the
		 * 	state
		 * @param createStateArgs arguments
		 * 	passed to the state constructor
		 * @return ptr to created state
		 */
		template<typename StateAnyT, typename ...CreateStateArgsT>
		StateAnyT * pushState(CreateStateArgsT && ...createStateArgs)
		{
			// Create state and push into
			// allocated states list
			StateAnyT * state = createState<StateAnyT>(forward<CreateStateArgsT>(createStateArgs)...);
			allocatedStates.pushBack(state);
			return state;
		}

		/**
		 * Returns a string representation
		 * of the automaton (depth first).
		 */
		String toString() const;

	protected:
		/// List of allocated states
		List<StateT*> allocatedStates;

		/// Single start state
		StateT * startState;

		/// Single accepted state
		StateT * acceptedState;
	};

	template<typename AlphaT>
	Automaton<AlphaT>::Automaton()
		: allocatedStates{}
		, startState{nullptr}
		, acceptedState{nullptr}
	{
		// Create start and accepted states
		startState = createState<EpsilonT>();
		acceptedState = createState<EpsilonT>();
	}

	template<typename AlphaT>
	Automaton<AlphaT>::~Automaton()
	{
		for (StateT * state : allocatedStates)
		{
			// Destroy all allocated states
			destroyState(state);
		}

		// Destroy start and accepted states
		destroyState(startState);
		destroyState(acceptedState);
	}

	template<typename AlphaT>
	bool Automaton<AlphaT>::acceptString(const AlphaStringT & input) const
	{
		using Visit = Pair<const StateT*, AlphaStringT>;

		// Create state queue
		List<Visit> visitQueue;
		Visit currVisit{startState, input};
		
		do
		{
			int32 numRead = 0;
			const StateT * currState = currVisit.first;
			AlphaStringT & currInput = currVisit.second;

			if (AlphabetTraitsT::isEOF(currInput) && isAcceptedState(currState))
			{
				// If input string has reached EOF and
				// we are in accepted state, return true
				return true;
			}

			// Try enter current state
			if (currState->enterState(currInput, numRead))
			{
				// Push next states to visit queue
				for (const StateT * nextState : currState->getNextStates())
				{
					visitQueue.pushBack(Visit{nextState, AlphabetTraitsT::consumeInput(currInput, numRead)});
				}
			}
		} while (visitQueue.popBack(currVisit));

		return false;
	}

	template<typename AlphaT>
	String Automaton<AlphaT>::toString() const
	{
		// TODO: Remove compare type, not needed
		using Visit = Pair<const StateT*, sizet, typename StateT::FindState>;
		
		String out;

		// Create visit queue, and list of
		// already visited states
		List<Visit> visitQueue;
		Set<const StateT*, typename StateT::FindState> visitedStates;
		Visit currVisit{startState, 0ull};
		
		do
		{
			const StateT * currState = currVisit.first;
			sizet currDepth = currVisit.second;

			for (sizet depth = 0; depth < currDepth; ++depth) out += "| ";

			if (!visitedStates.get(currState))
			{
				// State not already visited, add
				// to visited set and add next
				// states to visit queue
				visitedStates.set(currState);

				for (const StateT * nextState : currState->getNextStates())
				{
					visitQueue.pushBack(Visit{nextState, currDepth + 1});
				}
				
				out += currState->getDisplayName();
				out += '\n';
			}
			else
			{
				// State already visited
				out += String::format("%s (repeated)\n", *(currState->getDisplayName()));
				for (sizet depth = 0; depth < currDepth + 1; ++depth) out += "| ";
				out += "...\n";
			}
		} while (visitQueue.popBack(currVisit));
		
		return out;
	}
	
	/**
	 * An helper class to build an
	 * automaton from scratch.
	 * 
	 * Example:
	 * ```
	 * AutomatonBuilder<char> builder{automaton};
	 * builder.pushState<StateSymbol<char>>('a'); // 'a'
	 * builder.pushJump(); // '+'
	 * builder.pushState<StateSymbol<char>>('b'); // 'b'
	 * builder.pushJump(); // '+'
	 * builder.endGroup(); // '\0'
	 * ```
	 * 
	 * @param AlphaT type of the
	 * 	alphabet
	 */
	template<typename AlphaT>
	class AutomatonBuilder
	{
		using AutomatonT = Automaton<AlphaT>;
		using StateT = typename AutomatonT::StateT;
		using EpsilonT = typename AutomatonT::EpsilonT;

	public:
		/**
		 * Create a builder for the
		 * given automaton.
		 * 
		 * @param inAutomaton ref to
		 * 	the automaton to build
		 */
		FORCE_INLINE explicit AutomatonBuilder(AutomatonT & inAutomaton)
			: automaton{inAutomaton}
			, currentState{inAutomaton.startState}
			, groupStart{inAutomaton.startState}
			, groupEnd{inAutomaton.acceptedState}
			, currentGroup{1}
		{
			//
		}

		/**
		 * Create and push a new state onto
		 * the current state. An epsilon
		 * state is also prepended before
		 * the created state.
		 * 
		 * @param StateAnyT explicit type of
		 * 	the state to be created
		 * @param createStateArgs arguments
		 * 	passed to the state constructor
		 * @return ref to self
		 */
		template<typename StateAnyT, typename ...CreateStateArgsT>
		FORCE_INLINE AutomatonBuilder & pushState(CreateStateArgsT && ...createStateArgs)
		{
			// Create state and epsilon node
			// to create temporary group
			StateT * epsilon = automaton.template pushState<EpsilonT>();
			StateT * state = automaton.template pushState<StateAnyT>(forward<CreateStateArgsT>(createStateArgs)...);

			// Link to current state and set current
			currentState->addNextState(epsilon)->addNextState(state);
			currentState = state;

			// Create temporary groups
			groupStart[currentGroup] = epsilon;
			groupEnd[currentGroup] = state;

			return *this;
		}

		/**
		 * Begins a new group (i.e. opens
		 * a parenthesis in regex syntax).
		 * 
		 * @return ref to self
		 */
		FORCE_INLINE AutomatonBuilder & beginGroup()
		{
			// Push new epsilon state for start
			// group and end group states
			StateT * startState = automaton.template pushState<EpsilonT>();
			StateT * endState = automaton.template pushState<EpsilonT>();

			currentState->addNextState(startState);
			currentState = startState;

			if (currentGroup < maxNumGroups - 1)
			{
				groupStart[currentGroup] = startState;
				groupEnd[currentGroup] = endState;
				currentGroup += 1;
			}
			else ; // TODO: Error

			return *this;
		}

		/**
		 * Ends innermost group (i.e. closes
		 * innermost parenthesis in regex
		 * syntax).
		 * 
		 * @return ref to self
		 */
		FORCE_INLINE AutomatonBuilder & endGroup()
		{
			if (currentGroup > 0)
			{
				StateT * endState = groupEnd[currentGroup - 1];

				currentState->addNextState(endState);
				currentState = endState;

				currentGroup -= 1;	
			}
			else ; // TODO: Error

			return *this;
		}

		/**
		 * Commits current sequence in group as
		 * a branch (i.e. like a pipe `|` in
		 * regex syntax).
		 * 
		 * Example:
		 * ```
		 * before: B->1->2->(3)->| E
		 * 
		 * after: (B)->1->2->3->E
		 *          \->|
		 * ```
		 * 
		 * @return ref to self
		 */
		FORCE_INLINE AutomatonBuilder & pushBranch()
		{
			// Terminate and push current branch
			// by linking current state to end
			// of current group
			currentState->addNextState(groupEnd[currentGroup - 1]);
			currentState = groupStart[currentGroup - 1];

			return *this;
		}

		/**
		 * Creates an unconditional jump from
		 * the end of the current group to
		 * the beginning of the same group
		 * (it's equivalent to the plus `+`
		 * symbol in regex syntax).
		 * 
		 * Example:
		 * ```
		 * before: B->1->2->3->(E)->|
		 * 
		 * after: B->1->2->3->(E)->|
		 *        ^\__________|
		 * ```
		 */
		FORCE_INLINE AutomatonBuilder & pushJump()
		{
			// Jump from end of current group
			// to the beginning of the group
			groupEnd[currentGroup]->addNextState(groupStart[currentGroup]);

			return *this;
		}

	protected:
		/// Max number of groups
		static constexpr sizet maxNumGroups = 127;

		/// Ref to building automaton
		AutomatonT & automaton;

		/// Current active state
		StateT * currentState;

		/// Groups buffers
		/// @{
		StateT * groupStart[maxNumGroups];
		StateT * groupEnd[maxNumGroups];
		/// @}

		/// Current active group
		int8 currentGroup;
	};

	/**
	 * 
	 */
	template<typename AlphaT>
	class AutomatonOptimizer
	{
		using AutomatonT = Automaton<AlphaT>;
		using StateT = typename AutomatonT::StateT;
		using EpsilonT = typename AutomatonT::EpsilonT;

	public:
		/**
		 * Creates a new optimizer for
		 * the given automaton.
		 * 
		 * @param inAutomaton the
		 * 	automaton to be optimized
		 */
		FORCE_INLINE AutomatonOptimizer(AutomatonT & inAutomaton)
			: automaton{inAutomaton}
		{
			//
		}

		/**
		 * 
		 */
		AutomatonOptimizer & removeEpsilons();

	protected:
		/// The automaton to be optimized
		AutomatonT & automaton;
	};

	template<typename AlphaT>
	AutomatonOptimizer<AlphaT> & AutomatonOptimizer<AlphaT>::removeEpsilons()
	{
		for (StateT * state : automaton.allocatedStates)
		{
			if (EpsilonT * epsilon = state->template as<EpsilonT>())
			{
				if (epsilon->getPrevStates().getCount() == 1)
				{
					// Merge into previous state
					epsilon->mergePrevState();
				}
				else if (epsilon->getNextStates().getCount() == 1)
				{
					// Merge into next state
					epsilon->mergeNextState();
				}
			}
		}

		return *this;
	}
} // namespace Re
