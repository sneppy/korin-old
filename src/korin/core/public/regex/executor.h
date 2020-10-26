#pragma once

#include "core_types.h"
#include "containers/tuple.h"
#include "./regex_types.h"

namespace Re
{
	/**
	 * An executor is the component used
	 * to visit states inside an automaton
	 * and determine if an input is accepted
	 * and collect any match information.
	 * 
	 * ```cpp
	 * auto executor = automaton.createExecutor(inputString);
	 * while (executor.step()) ; // Check if string accepted
	 * ```
	 * 
	 * @param AlphaType type of the alphabet
	 */
	template<typename AlphaT>
	class Executor
	{
		using AutomatonT = Automaton<AlphaT>;
		using StateT = typename AutomatonT::StateT;
		using FindStateT = typename StateT::FindState;
		using AlphabetTraitsT = AlphabetTraits<AlphaT>;
		using AlphaSymbolT = typename AlphabetTraitsT::SymbolT;
		using AlphaStringT = typename AlphabetTraitsT::StringT;
		using VisitT = Tuple<const StateT* /* currentState */, AlphaStringT /* input */, int32 /* totRead */>;

	public:
		Executor() = delete;

		/**
		 * Initialize executor.
		 * 
		 * @param inStartState state from
		 * 	which to start execution
		 * @param inAcceptedState accepted
		 * 	state
		 * @param inInput input string bound
		 * 	to this instance
		 */
		FORCE_INLINE Executor(const StateT * inStartState, const StateT * inAcceptedState, const AlphaStringT & inInput)
			: startState{inStartState}
			, acceptedState{inAcceptedState}
			, input{inInput}
			, visitQueue{}
			, currVisit{inStartState, inInput, 0}
		{
			//
		}

		/**
		 * Return true if the provided state
		 * is the accepted state. If no state
		 * is provided, returns true if the
		 * current state is the accepted state.
		 * 
		 * @param state state to test
		 * @return true if state is accepted
		 * 	state
		 * @{
		 */
		FORCE_INLINE bool isAcceptedState(const StateT * state)
		{
			return FindStateT{}(state, acceptedState) == 0;
		}

		FORCE_INLINE bool isAcceptedState()
		{
			return isAcceptedState(currVisit.template get<0>());
		}
		/** @} */

		/**
		 * Reset executor to initial state.
		 * Allows to change the bound input
		 * string.
		 * 
		 * @param inInput change input string
		 * 	with this one
		 */
		FORCE_INLINE void reset()
		{
			totRead = 0;

			// Empty visit queue
			visitQueue.empty();

			// Reset current visit with
			// start state and input
			currVisit = VisitT{startState, input, 0};
		}

		FORCE_INLINE void reset(const AlphaStringT & inInput)
		{
			reset();

			// Change input string
			input = inInput;
		}
		/** @} */

		/**
		 * Step in next to-visit state
		 * and returns true iff input is
		 * accepted. This method is
		 * guaranteed to only step one
		 * state at a time.
		 * 
		 * @return false if string is
		 * 	rejected
		 */
		bool step();

	protected:
		/// Start state
		const StateT * startState;

		/// Accepted state
		const StateT * acceptedState;

		/// Original input
		AlphaStringT input;

		/// Total num read characters
		int32 totRead;

		/// Queue of states to visit
		List<VisitT> visitQueue;

		/// Current visited state and input
		VisitT currVisit;
	};

	template<typename AlphaT>
	bool Executor<AlphaT>::step()
	{
		int32 numRead = 0;

		// Get current state and input
		const StateT * currState = currVisit.template get<0>();
		AlphaStringT currInput = currVisit.template get<1>();
		int32 currNumRead = currVisit.template get<2>();

		if (currState->enterState(currInput, numRead, currNumRead))
		{
			for (const StateT * nextState : currState->getNextStates())
			{
				visitQueue.pushBack(VisitT{nextState, AlphabetTraitsT::consumeInput(currInput, numRead), currNumRead + numRead});
			}
		}

		// Pop next to-visit state from queue
		return visitQueue.popBack(currVisit);
	}
} // namespace Re
