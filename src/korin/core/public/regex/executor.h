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
		using StateT = StateBase<AlphaT>;
		using FindStateT = typename StateT::FindState;
		using AlphabetTraitsT = typename StateT::AlphabetTraitsT;
		using AlphaSymbolT = typename StateT::AlphaSymbolT;
		using AlphaStringT = typename StateT::AlphaStringT;
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
		 * @param inNumRead number of symbols
		 * 	already consumed (required by
		 * 	certain states to work correctly)
		 */
		FORCE_INLINE Executor(const StateT * inStartState, const StateT * inAcceptedState, const AlphaStringT & inInput, int32 inNumRead = 0)
			: startState{inStartState}
			, acceptedState{inAcceptedState}
			, input{inInput}
			, visitQueue{}
			, currVisit{inStartState, inInput, inNumRead}
		{
			//
		}

		/**
		 * Returns current state.
		 * @{
		 */
		FORCE_INLINE const StateT * getCurrentState() const
		{
			return currVisit.template get<0>();
		}

		FORCE_INLINE StateT * getCurrentState()
		{
			return currVisit.template get<0>();
		}
		/** @} */

		/**
		 * Returns current input.
		 * @{
		 */
		FORCE_INLINE const AlphaStringT & getCurrentInput() const
		{
			return currVisit.template get<1>();
		}

		FORCE_INLINE AlphaStringT & getCurrentInput()
		{
			return currVisit.template get<1>();
		}
		/** @} */

		/**
		 * Returns number of read symbols.
		 */
		FORCE_INLINE int32 getNumRead() const
		{
			return currVisit.template get<2>();
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
		 * @param inNumRead set initial number
		 * 	of symbols read
		 */
		FORCE_INLINE void reset()
		{
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
		bool step(bool & outIsAccepted);

	protected:
		/// Start state
		const StateT * startState;

		/// Accepted state
		const StateT * acceptedState;

		/// Original input
		AlphaStringT input;

		/// Queue of states to visit
		List<VisitT> visitQueue;

		/// Current visited state and input
		VisitT currVisit;
	};

	template<typename AlphaT>
	bool Executor<AlphaT>::step(bool & outIsAccepted)
	{
		outIsAccepted = false;

		int32 numRead = 0;

		// Get current state and input
		const StateT * currState = currVisit.template get<0>();
		AlphaStringT & currInput = currVisit.template get<1>();
		int32 currNumRead = currVisit.template get<2>();

		if (currState->enterState(currInput, numRead, currNumRead))
		{
			for (const StateT * nextState : currState->getNextStates())
			{
				visitQueue.pushBack(VisitT{nextState, AlphabetTraitsT::consumeInput(currInput, numRead), currNumRead + numRead});
			}
		}


		// Pop next to-visit state from queue
		if (!visitQueue.popBack(currVisit))
		{
			// If queue is empty, reject the string
			return true;
		}
		
		if (outIsAccepted = AlphabetTraitsT::isEOF(currVisit.template get<1>()) && isAcceptedState())
		{
			// If string is terminated and state
			// is accepted state, set accepted flag
			return outIsAccepted;
		}

		// Neither accepted, nor rejected
		return false;
	}
} // namespace Re
