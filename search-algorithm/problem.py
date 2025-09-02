from abc import ABC, abstractmethod


class Problem:
    """
    A class representing a problem in a search algorithm context.
    """

    def __init__(self, initial_state):
        self.initial_state = initial_state

    @abstractmethod
    def transition_model(self, state, action):
        pass

    @abstractmethod
    def goal_test(self, state) -> bool:
        pass

    @abstractmethod
    def actions(self, state) -> list:
        pass

    @abstractmethod
    def action_cost(self, state, action, new_state) -> int | float:
        pass
