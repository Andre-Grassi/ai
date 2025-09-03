from problem import Problem
from typing import Optional, List, Any

class Node:
    state: Any
    parent: Optional["Node"]
    action: Optional[Any]
    path_cost: float | int

    def __init__(self, state, parent=None, action=None, path_cost=0):
        self.state = state
        self.parent = parent
        self.action = action
        self.path_cost = path_cost

    def expand(self, problem: Problem):
        children = []
        state = self.state
        for action in problem.actions(state):
            new_state = problem.transition_model(state, action)
            cost = self.path_cost + problem.action_cost(state, action, new_state)
            children.append(Node(new_state, self, action, cost))

        return children
