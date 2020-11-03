#pragma once

#include "core_types.h"
#include "templates/function.h"
#include "containers/list.h"
#include "containers/tuple.h"
#include "containers/set.h"
#include "./regex_types.h"

namespace Re
{
	/**
	 * Base class for all states classes.
	 * A state has no real use outside of
	 * an automaton.
	 * 
	 * @param AlphaT type of the alphabet.
	 */
	template<typename AlphaT>
	struct StateBase
	{
		/**
		 * Compare type used to find states
		 * inside set.
		 */
		struct FindState
		{
			FORCE_INLINE int32 operator()(const StateBase & a, const StateBase & b) const
			{
				return ThreeWayCompare{}(a.id, b.id);
			}

			FORCE_INLINE int32 operator()(const StateBase * a, const StateBase * b) const
			{
				return (*this)(*a, *b);
			}
		};

		using StateT = StateBase;
		using AutomatonT = Automaton<AlphaT>;
		using AlphabetTraitsT = AlphabetTraits<AlphaT>;
		using AlphaSymbolT = typename AlphabetTraitsT::SymbolT;
		using AlphaStringT = typename AlphabetTraitsT::StringT;
		using SetT = Set<StateT*, typename StateT::FindState>;

		friend AutomatonT;

		/**
		 * State class name, for debug purpose.
		 */
		static constexpr Name debugName = "Base";

		/**
		 * Construct a new state. Assigns
		 * the new state a unique id.
		 */
		FORCE_INLINE StateBase()
			: id{0}
			, nextStates{}
			, prevStates{}
		{
			// Generate unique id
			static uint64 counter = 0;
			id = ++counter;
		}

		/**
		 * When copying a state, the id
		 * and the sets of connections
		 * are not copied.
		 */
		FORCE_INLINE StateBase(const StateBase & other)
			: StateBase{}
		{
			//
		}

		/**
		 * Returns the immutable set of
		 * next states (i.e. a set of states
		 * into which this state may
		 * transition).
		 */
		FORCE_INLINE const SetT & getNextStates() const
		{
			return nextStates;
		}

		/**
		 * Returns the immutable set of
		 * previous states (i.e. a set
		 * of states that may transition
		 * into the current state).
		 */
		FORCE_INLINE const SetT & getPrevStates() const
		{
			return prevStates;
		}

		/**
		 * Add a state to the set of
		 * next states. Also add itself
		 * to the set of previous states
		 * of the other state.
		 * 
		 * @param other another state
		 * @return ptr to the other state
		 */
		FORCE_INLINE StateBase * addNextState(StateBase * other)
		{
			// Add to this set of next states
			// and to other's set of prev states
			nextStates.set(other);
			other->prevStates.set(this);

			return other;
		}

		/**
		 * Merge this state into the next
		 * state. The state must have
		 * exactly one next state.
		 */
		FORCE_INLINE StateBase * mergeNextState()
		{
			CHECK(nextStates.getSize() == 1);

			// Get next state and unlink
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
		 * Merge this state into the previous
		 * state. The state must have exactly
		 * one previous state.
		 */
		FORCE_INLINE StateBase * mergePrevState()
		{
			CHECK(prevStates.getSize() == 1);

			// Get next state and unlink
			StateBase * prevState = *prevStates.begin();
			prevState->nextStates.remove(this);

			for (StateBase * nextState : nextStates)
			{
				nextState->prevStates.remove(this);
				prevState->addNextState(nextState);
			}

			return prevState;
		}

		/**
		 * Cast this state to the provided
		 * dynamic state type.
		 * 
		 * @param StateAnyT state type to
		 * 	cast to
		 * @return cast state if applicable,
		 * 	nullptr otherwise
		 * @{
		 */
		template<typename StateAnyT>
		FORCE_INLINE const StateAnyT * as() const
		{
			return dynamic_cast<const StateAnyT*>(this);
		}

		template<typename StateAnyT>
		FORCE_INLINE StateAnyT * as()
		{
			return dynamic_cast<StateAnyT*>(this);
		}
		/** @} */

		/**
		 * Returns true if the dynamic type
		 * of the state is the same as the
		 * provided one.
		 * 
		 * @param StateAnyT state type to
		 * 	test
		 * @return true if dynamic type is
		 * 	equal to the provided type
		 */
		template<typename StateAnyT>
		FORCE_INLINE bool is() const
		{
			return !!as<StateAnyT>();
		}

		/**
		 * Attempt to consume zero or more
		 * symbols by reading the input string.
		 * It returns true if input was read
		 * correctly and set `outNumRead` to
		 * the number of symbols read.
		 * 
		 * This method must be overriden by
		 * subclasses.
		 * 
		 * @param input sequence of input
		 * 	symbols, i.e. input string
		 * @param outNumRead returned number
		 * 	of symbols read from input string
		 * @param numRead number of symbols
		 * 	already read
		 * @return true if input read, false
		 * 	otherwise
		 */
		virtual bool enterState(const AlphaStringT & input, int32 & outNumRead, int32 numRead = 0) const = 0;

		/**
		 * Clone this state, and push it
		 * to automaton.
		 * 
		 * @param automaton automaton onto
		 * 	which to push the cloned state
		 * @return ptr to created state
		 */
		virtual StateT * cloneState(AutomatonT & automaton) const = 0;

		/**
		 * Returns a string representation
		 * of the state.
		 */
		virtual String getDisplayName() const
		{
			return String::format("?#%u", id);
		}

	protected:
		/// State id, used to identify state
		uint64 id;

		/// Set of next states
		SetT nextStates;

		/// Set of prev states
		SetT prevStates;
	};

	template<typename AlphaT>
	String printStateGraph(const StateBase<AlphaT> * startState, const StateBase<AlphaT> * acceptedState)
	{
		using State = StateBase<AlphaT>;
		using Visit = Tuple<const State* /* Current state */, uint32 /* Current depth = 0 */>;

		String out;
		
		// Keeps track of the visit queue
		// and already visited states
		List<Visit> visitQueue;
		Visit currVisit{startState, 0u};
		Set<const State*, typename State::FindState> visitedStates;
		Set<uint32> branches;
		
		do
		{
			const State * currState = currVisit.template get<0>();
			uint32 currDepth = currVisit.template get<1>();

			// Create new line
			sizet currLen = out.getLength();
			out += String{currDepth * 2, ' '};
			ansichar * line = &out[currLen];

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

				// TODO: Replace with start and accept state that derive StateEpsilon
				if (currState == startState)
				{
					out += "[Start]\n";
				}
				else if (currState == acceptedState)
				{
					out += "[Accept]\n";
				}
				else
				{
					out += currState->getDisplayName();
					out += '\n';
				}

				if (currState->getNextStates().getCount() > 1)
				{
					// Branch here
					branches.set(currDepth);
				}

				for (const State * nextState : currState->getNextStates())
				{
					visitQueue.pushBack(Visit{nextState, currDepth + 1});
				}
			}
			else
			{
				// State already visited

				if (currState == acceptedState)
				{
					out += "[Accept]\n";
				}
				else
				{
					out += currState->getDisplayName();
					out += " (repeated)\n";
				}
			}
		} while (visitQueue.popBack(currVisit));

		return out;
	}


#define DECLARE_VIRTUAL_STATE_TYPE(Type, AlphaT)\
	using StateT = StateBase<AlphaT>;\
	using typename StateT::AutomatonT;\
	using typename StateT::AlphabetTraitsT;\
	using typename StateT::AlphaSymbolT;\
	using typename StateT::AlphaStringT;\
	using typename StateT::SetT;\
	\
	static constexpr Name debugName = #Type;\
	\
	virtual const Name & getDebugName() const\
	{\
		return debugName;\
	}

#define DECLARE_STATE_TYPE(Type, AlphaT)\
	DECLARE_VIRTUAL_STATE_TYPE(Type, AlphaT)\
	\
	virtual FORCE_INLINE StateT * cloneState(AutomatonT & automaton) const override\
	{\
		return automaton.template pushState<State##Type>(*this);\
	}

#define DECLARE_STATE_TYPE_DEFAULT(Type)\
	DECLARE_STATE_TYPE(Type, AlphaT)\
	\
	virtual FORCE_INLINE String getDisplayName() const override\
	{\
		return String::format("%s#%u", *debugName, this->id);\
	}

	/**
	 * An epsilon state is a state that
	 * reades any input string without
	 * consuming any symbol.
	 */
	template<typename AlphaT>
	struct StateEpsilon : public StateBase<AlphaT>
	{
		DECLARE_STATE_TYPE_DEFAULT(Epsilon)

		//////////////////////////////////////////////////
		// StateBase interface
		//////////////////////////////////////////////////
		
		virtual bool enterState(const AlphaStringT&, int32&, int32) const override;
	};

	template<typename AlphaT>
	FORCE_INLINE bool StateEpsilon<AlphaT>::enterState(const AlphaStringT & input, int32 & outNumRead, int32 /* numRead */) const
	{
		// Always reads zero symbols and returns true
		return (outNumRead = 0, true);
	}

	/**
	 * A state symbol is a state
	 * that reads a syngle symbol
	 * if it matches the symbol
	 * bound to the state.
	 * 
	 * @param AlphaT type of the
	 * 	alphabet
	 */
	template<typename AlphaT>
	struct StateSymbol : public StateBase<AlphaT>
	{
		DECLARE_STATE_TYPE(Symbol, AlphaT)

		/**
		 * Construct a new state with the
		 * given symbol.
		 * 
		 * @param inSymbol symbol bound to
		 * 	the state
		 */
		FORCE_INLINE StateSymbol(const AlphaSymbolT & inSymbol)
			: symbol{inSymbol}
		{
			//
		}

		//////////////////////////////////////////////////
		// StateBase interface
		//////////////////////////////////////////////////
		
		virtual bool enterState(const AlphaStringT&, int32&, int32) const override;
		virtual String getDisplayName() const override;

	protected:
		/// Symbol to be matched against
		const AlphaSymbolT symbol;
	};

	template<typename AlphaT>
	FORCE_INLINE bool StateSymbol<AlphaT>::enterState(const AlphaStringT & input, int32 & outNumRead, int32 /* numRead */) const
	{
		return outNumRead = (*input == symbol);
	}

	template<typename AlphaT>
	FORCE_INLINE String StateSymbol<AlphaT>::getDisplayName() const
	{
		return String{"Symbol<"} + symbol + ">#" + this->id;
	}

	/**
	 * Reads a subset of symbol,
	 * specified as a range (i.e.
	 * `a-z` in regex syntax).
	 * The range is inclusive.
	 * 
	 * @param AlphaT type fo the
	 * 	alphabet
	 */
	template<typename AlphaT>
	struct StateRange : StateBase<AlphaT>
	{
		DECLARE_STATE_TYPE(Range, AlphaT)

		/**
		 * Initialize with min and
		 * max symbols.
		 * 
		 * @param inMin min symbol
		 * @param inMax max symbol
		 */
		FORCE_INLINE explicit StateRange(const AlphaSymbolT & inMin, const AlphaSymbolT & inMax)
			: min{inMin}
			, max{inMax}
		{
			//
		}

		//////////////////////////////////////////////////
		// StateBase interface
		//////////////////////////////////////////////////
		
		virtual bool enterState(const AlphaStringT&, int32&, int32) const override;
		virtual String getDisplayName() const override;

	protected:
		/// Min and max range
		/// @{
		AlphaSymbolT min;
		AlphaSymbolT max;
		/// @}
	};

	template<typename AlphaT>
	FORCE_INLINE bool StateRange<AlphaT>::enterState(const AlphaStringT & input, int32 & outNumRead, int32 /* numRead */) const
	{
		return outNumRead = (*input >= min && *input <= max);
	}

	template<typename AlphaT>
	FORCE_INLINE String StateRange<AlphaT>::getDisplayName() const
	{
		return String{"Range<"} + min + '-' + max + ">#" + this->id;
	}

	/**
	 * A state that reads any symbol
	 * (except the terminal symbol)
	 * and always consumes one input
	 * symbol.
	 * 
	 * @param AlphaT type of the
	 * 	alphabet
	 */
	template<typename AlphaT>
	struct StateAny : StateBase<AlphaT>
	{
		DECLARE_STATE_TYPE_DEFAULT(Any)

		//////////////////////////////////////////////////
		// StateBase interface
		//////////////////////////////////////////////////
		
		virtual bool enterState(const AlphaStringT&, int32&, int32) const override;
	};

	template<typename AlphaT>
	FORCE_INLINE bool StateAny<AlphaT>::enterState(const AlphaStringT & input, int32 & outNumRead, int32 /* numRead */) const
	{
		return outNumRead = (*input != AlphabetTraitsT::terminalSymbol);
	}

	/**
	 * Flexible state that accepts a
	 * lambda function.
	 * 
	 * @param AlphaT type of the
	 * 	alphabet
	 */
	template<typename AlphaT>
	struct StateLambda : StateBase<AlphaT>
	{
		DECLARE_STATE_TYPE(Lambda, AlphaT)

		using LambdaT = Function<bool(const AlphaStringT&, int32&, int32)>;

		/**
		 * Construct a new lambda state with
		 * the given lambda function.
		 * 
		 * @param inLambda lambda function to
		 * @param inName optional lambda name
		 * 	be called when state is entered
		 */
		template<typename LambdaAnyT>
		FORCE_INLINE StateLambda(LambdaAnyT && inLambda, const String & inName = "")
			: lambda{forward<LambdaAnyT>(inLambda)}
			, name{inName}
		{
			//
		}

		//////////////////////////////////////////////////
		// StateBase interface
		//////////////////////////////////////////////////
		
		virtual bool enterState(const AlphaStringT&, int32&, int32) const override;
		virtual String getDisplayName() const override;

	protected:
		/// Lambda enter function
		LambdaT lambda;

		/// Optional lambda name
		// FIXME: Investigate possible memory leak
		String name;
	};

	template<typename AlphaT>
	FORCE_INLINE bool StateLambda<AlphaT>::enterState(const AlphaStringT & input, int32 & outNumRead, int32 numRead) const
	{
		return lambda(input, outNumRead, numRead);
	}

	template<typename AlphaT>
	FORCE_INLINE String StateLambda<AlphaT>::getDisplayName() const
	{
		return String::format("Lambda<%s>#%u", *name, this->id);
	}

	/**
	 * Base class for states that
	 * require a nested automaton
	 * (e.g. positive/negative
	 * lookahed). This class is
	 * abstract. You must derive
	 * it and override the
	 * @c execute method.
	 * 
	 * @param AlphaT type of the
	 * 	alphabet
	 */
	template<typename AlphaT>
	struct StateMacro : StateBase<AlphaT>
	{
		DECLARE_VIRTUAL_STATE_TYPE(Macro, AlphaT)

		using ExecutorT = Executor<AlphaT>;

		StateMacro() = delete;

		/**
		 * 
		 */
		FORCE_INLINE explicit StateMacro(const StateT * inStartState, const StateT * inAcceptedState)
			: startState{inStartState}
			, acceptedState{inAcceptedState}
		{
			//
		}

		/**
		 * 
		 */
		virtual bool execute(ExecutorT && executor, int32 & outNumRead) const = 0;

		//////////////////////////////////////////////////
		// StateBase interface
		//////////////////////////////////////////////////
		
		virtual bool enterState(const AlphaStringT&, int32&, int32) const override;
		virtual String getDisplayName() const;

	protected:

		/// Automaton start state
		const StateT * startState;

		/// Automaton accepted state
		const StateT * acceptedState;
	};

	template<typename AlphaT>
	bool StateMacro<AlphaT>::enterState(const AlphaStringT & input, int32 & outNumRead, int32 numRead) const
	{
		return execute(ExecutorT{startState, acceptedState, input, numRead}, outNumRead);
	}

	template<typename AlphaT>
	String StateMacro<AlphaT>::getDisplayName() const
	{
		String name = getDebugName();
		name += "<\n";
		name += printStateGraph(startState, acceptedState);
		name += ">";
		name += String::format("#%llu", this->id);

		return name;
	}

#define DECLARE_STATE_MACRO(Type, AlphaT)\
	DECLARE_STATE_TYPE(Type, AlphaT)\
	\
	using MacroT = StateMacro<AlphaT>;\
	using MacroT::MacroT;\
	using typename MacroT::ExecutorT;

#define DECLARE_STATE_MACRO_DEFAULT(Type)\
	DECLARE_STATE_MACRO(Type, AlphaT)

	/**
	 * A macro state that looks
	 * ahead without consuming
	 * any symbol.
	 * 
	 * @param AlphaT type of the
	 * 	alphabet
	 */
	template<typename AlphaT>
	struct StatePositiveLookahed : public StateMacro<AlphaT>
	{
		DECLARE_STATE_MACRO_DEFAULT(PositiveLookahed)

		//////////////////////////////////////////////////
		// StateMacro interface
		//////////////////////////////////////////////////
		
		virtual bool execute(ExecutorT&&, int32&) const override;
	};

	template<typename AlphaT>
	bool StatePositiveLookahed<AlphaT>::execute(ExecutorT && executor, int32 & outNumRead) const
	{
		bool isAccepted = false;
		while (!executor.step(isAccepted));

		outNumRead = 0;
		return isAccepted;
	}

	/**
	 * Like a positive lookahed, but
	 * enters state only if the given
	 * subautomaton does not accept
	 * the input string.
	 * 
	 * @param AlphaT type of the
	 * 	alphabet
	 */
	template<typename AlphaT>
	struct StateNegativeLookahed : public StateMacro<AlphaT>
	{
		DECLARE_STATE_MACRO_DEFAULT(NegativeLookahed)

		//////////////////////////////////////////////////
		// StateMacro interface
		//////////////////////////////////////////////////
		
		virtual bool execute(ExecutorT&&, int32&) const override;
	};

	template<typename AlphaT>
	bool StateNegativeLookahed<AlphaT>::execute(ExecutorT && executor, int32 & outNumRead) const
	{
		bool isAccepted = false;
		while (!executor.step(isAccepted));

		outNumRead = 0;
		return !isAccepted;
	}
} // namespace Re
