class Node:
    def __init__(self, state, parent=None, action=None, path_cost=0):
        self.state = state
        self.parent = parent
        self.action = action
        self.path_cost = path_cost

    def get_path(self):
        path = []
        current = self
        while current:
            path.append(current.state)
            current = current.parent
        return path[::-1]
