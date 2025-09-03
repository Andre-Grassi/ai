from ..node import Node
from ..problem import Problem

import random
from typing import List, Tuple, Optional

State = List[List[int]]  # In this puzzle, a state is a 2D grid


class SlidingTileProblem(Problem):
    GOAL_STATE = [[0, 1, 2], [3, 4, 5], [6, 7, 8]]

    def __init__(self):
        initial_state = self._randomize_board()
        super().__init__(initial_state)

    def _randomize_board(self) -> State:
        """Return a random 3x3 board with unique values 0..8."""
        flat = random.sample(range(9), 9)  # unique values 0..8
        return [flat[i * 3 : (i + 1) * 3] for i in range(3)]

    # Get the index of a value in a 2D matrix
    def _get_index_2D(self, matrix, value):
        for i, row in enumerate(matrix):
            for j, v in enumerate(row):
                if v == value:
                    return (i, j)

    def transition_model(self, state: State, action):
        # Get the index of 0 in the state 2D matrix
        blank_row, blank_col = self._get_index_2D(state, 0)

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

    def goal_test(self, state: State):
        return state == self.GOAL_STATE

    def actions(self, state: State):
        blank_row, blank_col = self._get_index_2D(state, 0)

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

    def action_cost(self, state, action, new_state):
        return 1  # Each action cost just 1 (moving one tile)
