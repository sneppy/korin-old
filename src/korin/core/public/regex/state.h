#pragma once

#include "core_types.h"
#include "templates/function.h"
#include "containers/set.h"
#include "./regex_types.h"

#define DECLARE_STATE_TYPE(T, AlphaT)\
	using StateT = StateBase<AlphaT>;\
	using StatePtrT = StateT*;\
	using AutomatonT = Automaton<AlphaT>;\
	using AlphabetTraitsT = AlphabetTraits<AlphaT>;\
	using AlphaSymbolT = typename AlphabetTraitsT::SymbolT;\
	using AlphaStringT = typename AlphabetTraitsT::StringT;\
	using SetT = Set<StateT*, typename StateT::FindState>;\
	static constexpr Name debugName = #T;\

#define DECLARE_STATE_TYPE_DEFAULT(T, AlphaT)\
	DECLARE_STATE_TYPE(T, AlphaT)\
	virtual FORCE_INLINE String getDisplayName() const override { return String::format("%s#%u", *debugName, this->id); }

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
		template<typename> friend class Automaton;

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
		
		DECLARE_STATE_TYPE(Base, AlphaT)

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
		

	/**
	 * An epsilon state is a state that
	 * reades any input string without
	 * consuming any symbol.
	 */
	template<typename AlphaT>
	struct StateEpsilon : public StateBase<AlphaT>
	{
		DECLARE_STATE_TYPE_DEFAULT(Epsilon, AlphaT)

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
		DECLARE_STATE_TYPE_DEFAULT(Any, AlphaT)

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
		DECLARE_STATE_TYPE(StateLambda, AlphaT)

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
} // namespace Re
