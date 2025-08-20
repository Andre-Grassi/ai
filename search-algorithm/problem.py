class Problem:
    """
    A class representing a problem in a search algorithm context.
    It contains the initial state, goal state, and methods to check if a state is the goal.
    """

    def __init__(self, initial_state, actions, transition_model, goal_test, path_cost):
        self.initial_state = initial_state
        self.actions = actions
        self.transition_model = transition_model
        self.goal_test = goal_test
        self.path_cost = path_cost

    def is_goal(self, state):
        """
        Check if the given state is the goal state.

        :param state: The state to check.
        :return: True if the state is the goal state, False otherwise.
        """
        return state == self.goal_state