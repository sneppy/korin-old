#pragma once

#include "core_types.h"
#include "containers/list.h"
#include "containers/pair.h"
#include "containers/set.h"
#include "containers/tuple.h"
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
		using RangeT = StateRange<AlphaT>;
		using LambdaT = StateLambda<AlphaT>;
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
			// TODO {
			// 	I'd like to use a smart allocator
			// 	but I guess each state class should
			// 	have its own allocator
			// }
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
		 * Creates and return a new builder
		 * for this automaton.
		 */
		FORCE_INLINE BuilderT createBuilder()
		{
			return BuilderT{*this};
		}

		/**
		 * Creates and returns a new optimizer
		 * for this automaton.
		 */
		FORCE_INLINE OptimizerT createOptimizer()
		{
			return OptimizerT{*this};
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
		using Visit = Tuple<const StateT*, AlphaStringT, int32>;

		// Create state queue
		List<Visit> visitQueue;
		Visit currVisit{startState, input, 0};
		
		do
		{
			int32 numRead = 0;
			const StateT * currState = currVisit.template get<const StateT*>();
			AlphaStringT & currInput = currVisit.template get<AlphaStringT>();
			int32 currNumRead = currVisit.template get<int32>();

			if (AlphabetTraitsT::isEOF(currInput) && isAcceptedState(currState))
			{
				// If input string has reached EOF and
				// we are in accepted state, return true
				return true;
			}

			// Try enter current state
			if (currState->enterState(currInput, numRead, currNumRead))
			{
				// Push next states to visit queue
				for (const StateT * nextState : currState->getNextStates())
				{
					visitQueue.pushBack(Visit{nextState, AlphabetTraitsT::consumeInput(currInput, numRead), currNumRead + numRead});
				}
			}
		} while (visitQueue.popBack(currVisit));

		return false;
	}

	template<typename AlphaT>
	String Automaton<AlphaT>::toString() const
	{
		using Visit = Tuple<const StateT* /* Current state */, uint32 /* Current depth = 0 */>;

		String out;
		
		// Keeps track of the visit queue
		// and already visited states
		List<Visit> visitQueue;
		Visit currVisit{startState, 0u};
		Set<const StateT*, typename StateT::FindState> visitedStates;
		Set<uint32> branches;
		
		do
		{
			const StateT * currState = currVisit.template get<0>();
			uint32 currDepth = currVisit.template get<1>();

			// Create new line
			ansichar * line = &out[out.getLength()];
			out += String{currDepth * 2, ' '};

			for (auto branchIt = branches.begin(); branchIt != branches.end() && *branchIt < currDepth; ++branchIt)
			{
				const uint32 branchDepth = *branchIt;
				line[branchDepth * 2] = '|';
				line[branchDepth * 2 + 1] = branchDepth + 1 == currDepth ? '-' : ' ';
			}

			if (!visitedStates.get(currState))
			{
				// State not yet visited
				visitedStates.set(currState);

				out += currState->getDisplayName();
				out += '\n';

				if (currState->getNextStates().getCount() > 1)
				{
					// Branch here
					branches.set(currDepth);
				}

				for (const StateT * nextState : currState->getNextStates())
				{
					visitQueue.pushBack(Visit{nextState, currDepth + 1});
				}
			}
			else
			{
				// State already visited
				out += currState->getDisplayName();
				out += " (repeated)\n";
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
			// of current group.
			currentState->addNextState(groupEnd[currentGroup - 1]);
			currentState = groupStart[currentGroup - 1];

			return *this;
		}

		/**
		 * Creates a shortcut, similar to
		 * a branch, that skips the current
		 * group (equivalent to star `*`
		 * operation in regex syntax).
		 * 
		 * Example:
		 * ```
		 * before: B->1->2->(3)->| E
		 * 
		 * after: B->1->2->3->(e)->|
		 *        \___________^
		 * ```
		 * 
		 * Note that the same effect can
		 * be achieved using a combination
		 * of group and branch. However that
		 * would not be possible for a single
		 * state (i.e. the single state would
		 * need to be inside a proper group).
		 * 
		 * Example:
		 * ```
		 * before: e->(1)->|
		 * 
		 * after: e->1->(e)->|
		 *        \_____^
		 * ```
		 * 
		 * @return ref to self
		 */
		FORCE_INLINE AutomatonBuilder & pushSkip()
		{
			// We need an epsilon state to
			// skip the current group
			StateT * epsilon = automaton.template pushState<EpsilonT>();
			
			currentState->addNextState(epsilon);
			currentState = epsilon;

			// Create shortcut
			groupStart[currentGroup]->addNextState(epsilon);

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
		 * 
		 * @return ref to self
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
					// TODO: Dealloc state
				}
				else if (epsilon->getNextStates().getCount() == 1)
				{
					// Merge into next state
					epsilon->mergeNextState();
					// TODO: Dealloc state
				}
			}
		}

		return *this;
	}
} // namespace Re
