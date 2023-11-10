#!/usr/bin/python3

# [https://pypi.org/project/python-statemachine/]

# pip install python-statemachine
from statemachine import StateMachine, State

class LightBulb(StateMachine):
    # creating states
    offState = State("off", initial = True)
    onState = State("on")

    # transitions of the state
    switchOn = offState.to(onState)
    switchOff = onState.to(offState)

    cycle = (
            offState.to(onState)
            | onState.to(offState)
        )


bulb = LightBulb()
print(bulb.current_state)

# Transition state: switchOn
bulb.send("switchOn")
print(bulb.current_state)

bulb.send("switchOff")
print(bulb.current_state)

# Transition state: Automatic
bulb.cycle()
print(bulb.current_state)

bulb.cycle()
print(bulb.current_state)
