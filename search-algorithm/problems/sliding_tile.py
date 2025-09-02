from ..node import Node
from ..problem import Problem


class SlidingTileProblem(Problem):
    GOAL_STATE = [[0, 1, 2], [3, 4, 5], [6, 7, 8]]

    # Get the index of a value in a 2D matrix
    def get_index_2D(matrix, value):
        for i, row in enumerate(matrix):
            for j, v in enumerate(row):
                if v == value:
                    return (i, j)

    def transition_model(self, state, action):
        # Get the index of 0 in the state 2D matrix
        blank_row, blank_col = self.get_index_2D(state, 0)

        swap_with_row = blank_row
        swap_with_col = blank_col
        if action == "Up" and blank_row > 0:
            swap_with_row = blank_row - 1
        elif action == "Down" and blank_row < 2:
            swap_with_row = blank_row + 1
        elif action == "Left" and blank_col > 0:
            swap_with_col = blank_col - 1
        elif action == "Right" and blank_col < 2:
            swap_with_col = blank_col + 1
        else:
            return None  # Invalid move for the given state

        new_state = [row[:] for row in state]

        new_state[blank_row][blank_col] = new_state[swap_with_row][swap_with_col]
        new_state[swap_with_row][swap_with_col] = 0

        return new_state

    def goal_test(self, state):
        return state == self.GOAL_STATE

    def actions(self, state):
        blank_row, blank_col = self.get_index_2D(state, 0)

        available_actions = []
        if blank_row > 0:
            available_actions.append("Up")
        if blank_row < 2:
            available_actions.append("Down")
        if blank_col > 0:
            available_actions.append("Left")
        if blank_col < 2:
            available_actions.append("Right")

        return available_actions
