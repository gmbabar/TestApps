#!/usr/bin/python3

# pip install python-statemachine
from statemachine import StateMachine, State

class Ma2ClientState(StateMachine):

    # creating states
    appl_init = State(initial = True)
    helo_sent = State()
    hrbt_sent = State()
    pdrq_sent = State()
    ornw_sent = State()
    oprq_sent = State()
    blrq_sent = State()
    orcn_sent = State()
    all_sent = State()

    # transitions of the state
    nextState = (
        appl_init.to(helo_sent)
        | helo_sent.to(hrbt_sent)
        | hrbt_sent.to(pdrq_sent)
        | pdrq_sent.to(ornw_sent)
        | ornw_sent.to(oprq_sent)
        | oprq_sent.to(blrq_sent)
        | blrq_sent.to(orcn_sent)
        | orcn_sent.to(all_sent)
    )

    #  Initialization
    def before_cycle(self, event: str, source: State, target: State, message: str = ""):
        message = ". " + message if message else ""
        return f"Running {event} from {source.id} to {target.id}{message}"

    #  All Done
    def on_enter_all_sent(self):
        print("All Done. Exiting.")
        exit()

if __name__ == "__main__":
    ma2_client = Ma2ClientState()
    print(ma2_client.current_state)
    ma2_client.nextState()
    print(ma2_client.current_state)
    ma2_client.nextState()
    print(ma2_client.current_state)
    ma2_client.nextState()
    print(ma2_client.current_state)
    ma2_client.nextState()
    print(ma2_client.current_state)
    ma2_client.nextState()
    print(ma2_client.current_state)
    ma2_client.nextState()
    print(ma2_client.current_state)
    ma2_client.nextState()
    print(ma2_client.current_state)
    ma2_client.nextState()

