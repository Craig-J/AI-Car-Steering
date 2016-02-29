#ifndef _AGNOSTIC_GENERIC_STATE_H_
#define _AGNOSTIC_GENERIC_STATE_H_

#include <memory>

namespace agn
{
	template <class _State>
	using UniqueStatePtr = std::unique_ptr<_State>;

	template <class _StateMachine, class _State>
	class GenericState
	{
	public:

		// No default constructor.
		GenericState() = delete;

		// Generic state constructor
		// IN:	Reference to the templated state machine
		//		Reference to the templated state unique ptr
		explicit GenericState(_StateMachine& _state_machine, UniqueStatePtr<_State>& _state) :
			machine_(_state_machine),
			state_(_state)
		{
		}

		// Static Initialize function
		// 
		template <class _ConcreteState, class ... _Types>
		static void Initialize(_StateMachine& _state_machine, UniqueStatePtr<_State>& _state, _Types ... _args)
		{
			_state = UniqueStatePtr<_State>(new _ConcreteState(_state_machine, _state, _args...));
			_state->InitializeState();
		}

	protected:
		// ChangeState
		// Basic state transition implementation.
		// Terminate current state and calls the static initialize to create a new state.
		template <class _ConcreteState, class ... _Types>
		void ChangeState(_Types ... _args)
		{
			// Terminate current state.
			TerminateState();
			// Initialize new one.
			Initialize<_ConcreteState, _Types...>(machine_, state_, _args...);
		}

		// RenewState
		// Terminate and re-Initialize the current state.
		void RenewState()
		{
			TerminateState();
			InitializeState();
		}

	private:
		virtual void InitializeState() {}
		virtual void TerminateState() {}

	protected:
		_StateMachine& machine_;

	private:
		// Reference of the pointer to the current state.
		UniqueStatePtr<_State>& state_;
	};
}

#endif // _AGNOSTIC_GENERIC_STATE_H_