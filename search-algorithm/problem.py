from abc import ABC, abstractmethod


class Problem:
    """
    A class representing a problem in a search algorithm context.
    """

    def __init__(self, initial_state):
        self.initial_state = initial_state

    @abstractmethod
    def transition_model(state, action):
        pass

    @abstractmethod
    def goal_test(state) -> bool:
        pass

    @abstractmethod
    def actions(state) -> list:
        pass

    @abstractmethod
    def action_cost(action) -> int | float:
        pass
