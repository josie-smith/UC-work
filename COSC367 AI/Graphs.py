# Define DFS, BFS, LCFS, A* search and some test graphs.

from search import *
from collections import deque
import math
import copy
import heapq

BLANK = ' '

class DFSFrontier(Frontier):
    """Depth First Search - a concrete subclass of Frontier """


    def __init__(self):
        """The constructor takes no argument. It initialises the
        container to an empty stack."""
        self.container = deque()

    def add(self, path):
        """Adds a new path to the frontier.
        """
        self.container.append(path)

    def __iter__(self):
        """The object returns itself because it is implementing a __next__
        method and does not need any additional state for iteration."""
        return self


    def __next__(self):
        """Selects, removes, and returns a path on the frontier if there is
        any. DFS frontier is LIFO.
        """
        if len(self.container) > 0:
            return self.container.pop()
        else:
            raise StopIteration

class BFSFrontier(Frontier):
    """Breadth First Search - a concrete subclass of Frontier """


    def __init__(self):
        """The constructor takes no argument. It initialises the
        container to an empty stack."""
        self.container = deque()

    def add(self, path):
        """Adds a new path to the frontier.
        """
        self.container.append(path)

    def __iter__(self):
        """The object returns itself because it is implementing a __next__
        method and does not need any additional state for iteration."""
        return self


    def __next__(self):
        """Selects, removes, and returns a path on the frontier if there is
        any. BFS is FIFO
        """
        if len(self.container) > 0:
            return self.container.popleft()
        else:
            raise StopIteration

class LCFSFrontier(Frontier):
    """Low Cost First Search - a concrete subclass of Frontier """


    def __init__(self):
        """The constructor takes no argument. It initialises the
        container to an empty stack."""
        self.container = []
        heapq.heapify(self.container)

    def add(self, path):
        """Adds a new path to the frontier. Path is a tuple of arc objects.
        """
        totalcost = sum([arc.cost for arc in path])
        heapq.heappush(self.container, (totalcost, path)) # adds (total cost of all arcs in path, path) to heap
        #print('pushed: '+ str((totalcost, path)) +'\n')

    def __iter__(self):
        """The object returns itself because it is implementing a __next__
        method and does not need any additional state for iteration."""
        return self


    def __next__(self):
        """Selects, removes, and returns a path on the frontier if there is
        any. LCFS returns the lowest cost path
        """
        if len(self.container) > 0:
            expanded = heapq.heappop(self.container)
            #print('popped: '+ str(expanded) +'\n')
            return expanded[1]
        else:
            raise StopIteration

class AStarFrontier(Frontier):
    """A* Search with pruning"""


    def __init__(self, graph):
        """The constructor needs the graph, to access the estimated_cost_to_goal
        function. It initialises the container to an empty stack."""
        self.container = []
        heapq.heapify(self.container)
        self.estimated_cost_to_goal = graph.estimated_cost_to_goal
        self.expanded=set()
        self.counter = 0

    def add(self, path):
        """Adds a new path to the frontier. Path is a tuple of arc objects.
        Cost = cost(p) + h(n)
        where cost(p) is the total cost of the path so far
        h(n) is the heuristic of the last node in the path
        """
        node = path[-1].head
        if node not in self.expanded:
            cost = sum([arc.cost for arc in path]) + self.estimated_cost_to_goal(node)
            heapq.heappush(self.container, (cost, self.counter, path))
            self.counter +=1
            #print('pushed: '+ str((cost, self.counter, path)))
        #else:
            #print('!!Not pushed: '+ str(path))


    def __iter__(self):
        """The object returns itself because it is implementing a __next__
        method and does not need any additional state for iteration."""
        return self


    def __next__(self):
        """Selects, removes, and returns a path on the frontier if there is
        any. LCFS returns the lowest cost path
        """
        while len(self.container) > 0:
            expandedPath = heapq.heappop(self.container)[2]
            expandedNode = expandedPath[-1].head
            if expandedNode not in self.expanded:
                self.expanded.add(expandedNode)
                #print('expanded: '+ str(self.expanded))
                #print('popped: '+ str(expandedPath))
                return expandedPath
            #else:
                #print('!!Not popped: '+ str(expandedPath))

        else:
            raise StopIteration


class FunkyNumericGraph(Graph):
    """This is a concrete subclass of Graph which is generated on the fly.
    Each state is an integer.
    There is a single starting state
    Each state has 2 actions, 1down and 2up
    States divisiblee by 10 are goal nodes.
    """

    def __init__(self, start):
        """Initialises an implicit graph starting at node start.
        """
        self.starting_number = start

    def starting_nodes(self):
        """Returns a list of 1 starting node."""
        return [self.starting_number]

    def is_goal(self, node):
        """Returns true if the given node is a goal node."""
        return node % 10 == 0

    def outgoing_arcs(self, node):
        """Returns a sequence of Arc objects that go out from the given
        node. Only 2 actions from each node, 1up and 2down
        """
        return [Arc(node, node-1, action="1down", cost=1),
                Arc(node, node+2, action="2up", cost=1)]

class SlidingPuzzleGraph(Graph):
    """Implicit graph, which describes the n^2 sliding block puzzle.
    Nodes are n by n arrays
    """

    def __init__(self, starting_state):
        """Initialises an implicit graph starting at the starting state
        """
        self.starting_state = starting_state

    def starting_nodes(self):
        """Returns a list of 1 starting node."""
        return [self.starting_state]

    def is_goal(self, state):
        """Returns true if the given state is the goal state, False
        otherwise."""
        n = len(state)
        goal = [[BLANK]]
        for i in range(n):
            if i == 0:
                goal[i].extend(range(1,n))
            else:
                goal.append(list(range(n*i,n*i+n)))
        return state == goal


    def outgoing_arcs(self, state):
        """Given a puzzle state (node) returns a list of arcs. Each arc
        represents a possible action (move) and the resulting state."""
        n = len(state)
        i, j = next((i, j) for i in range(n) for j in range(n)
                    if state[i][j] == BLANK) # find the blank tile
        arcs = []
        if i > 0:
            action = "Move {} down".format(state[i-1][j]) # or blank goes up
            new_state = copy.deepcopy(state)
            new_state[i][j], new_state[i-1][j] = new_state[i-1][j], BLANK
            arcs.append(Arc(state, new_state, action, 1))
        if i < n - 1:
            action = "Move {} up".format(state[i+1][j]) # or blank goes down
            new_state = copy.deepcopy(state)
            new_state[i][j], new_state[i+1][j] = new_state[i+1][j], BLANK
            arcs.append(Arc(state, new_state, action, 1))
        if j > 0:
            action = "Move {} right".format(state[i][j-1]) # or blank goes left
            new_state = copy.deepcopy(state)
            new_state[i][j], new_state[i][j-1] = new_state[i][j-1], BLANK
            arcs.append(Arc(state, new_state, action, 1))
        if j < n - 1:
            action = "Move {} left".format(state[i][j+1]) # or blank goes right
            new_state = copy.deepcopy(state)
            new_state[i][j], new_state[i][j+1] = new_state[i][j+1], BLANK
            arcs.append(Arc(state, new_state, action, 1))
        return arcs

class LocationGraph(Graph):
    """This is a concrete subclass of Graph where vertices have locations in the 2D plane,
    described by a pair of numbers."""

    def __init__(self, nodes, locations, edges, starting_nodes, goal_nodes, estimates=None):
        """Initialises a location graph.
        Keyword arguments:
        nodes -- a set of nodes
        edges -- a sequence of tuples in the form (tail, head). All edges are bidirectional.
        starting_nodes -- the list of starting nodes. We use a list
                          to remind you that the order can influence
                          the search behaviour.
        goal_node -- the set of goal nodes. It's better if you use a set
                     here to remind yourself that the order does not matter
                     here. This is used only by the is_goal method.
        locations -- a dictionary specifies the locations of each node.
        """

        # A few assertions to detect possible errors in
        # instantiation. These assertions are not essential to the
        # class functionality.
        assert all(tail in nodes and head in nodes for tail, head, *_ in edges)\
           , "An edge must link two existing nodes!"
        assert all(node in nodes for node in starting_nodes),\
            "The starting_states must be in nodes."
        assert all(node in nodes for node in goal_nodes),\
            "The goal states must be in nodes."

        self.nodes = nodes
        # Adds all reverse edges to the edge set.
        edges.update(set(map(lambda x: x[::-1] , edges)))
        self.edge_list = edges
        self._starting_nodes = starting_nodes
        self.goal_nodes = goal_nodes
        self.estimates = estimates
        self.locations = locations

    def starting_nodes(self):
        """Returns a sequence of starting nodes."""
        return self._starting_nodes

    def is_goal(self, node):
        """Returns true if the given node is a goal node."""
        return node in self.goal_nodes

    def outgoing_arcs(self, node):
        """Returns a sequence of Arc objects that go out from the given
        node. The action string is automatically generated.

        """
        arcs = []
        for edge in self.edge_list:
            tail, head = edge
            cost = math.sqrt((self.locations[head][0] - self.locations[tail][0])**2 +
                             (self.locations[head][1] - self.locations[tail][1])**2)
            if tail == node:
                arcs.append(Arc(tail, head, str(tail) + '->' + str(head), cost))
        arcs.sort()
        return arcs


class RoutingGraph(Graph):
    """This is a concrete subclass of Graph which is generated on the fly.
    Each state is an integer.
    There is a single starting state
    Each state has 2 actions, 1down and 2up
    States divisible by 10 are goal nodes.
    """

    def __init__(self, map_str):
        """Initialises an implicit graph from the given map. S are starting
        nodes and G are goal nodes.
        """
        self.map = [list(row) for row in map_str.split('\n')]
        self.map.pop()
        self.rows = len(self.map)
        self.columns = len(self.map[0])

        self.agents = []
        for row in range(self.rows):
            for col in range(self.columns):
                if self.map[row][col] == "S":
                    self.agents.append((row, col, math.inf))
                if self.map[row][col].isdigit():
                    self.agents.append((row, col, int(self.map[row][col])))

        self.callpoints = []
        for row in range(self.rows):
            for col in range(self.columns):
                if self.map[row][col] == "G":
                    self.callpoints.append((row, col))


    def starting_nodes(self):
        """Returns a list of starting nodes."""
        return self.agents

    def is_goal(self, node):
        """Returns true if the given node is a goal node."""
        row, col = node[0:2]
        return (row, col) in self.callpoints

    def outgoing_arcs(self, node):
        """Returns a sequence of Arc objects that go out from the given
        node. Only 2 actions from each node, 1up and 2down
        """
        row, column, fuel = node
        arcs=[]
        if self.map[row-1][column] not in '|-+X' and fuel>0:
            arcs.append(Arc(node, (row-1,column,fuel-1), action="N", cost=5))
        if self.map[row][column+1] not in '|-+X' and fuel>0:
            arcs.append(Arc(node, (row,column+1,fuel-1), action="E", cost=5))
        if self.map[row+1][column] not in '|-+X' and fuel>0:
            arcs.append(Arc(node, (row+1,column,fuel-1), action="S", cost=5))
        if self.map[row][column-1] not in '|-+X' and fuel>0:
            arcs.append(Arc(node, (row,column-1,fuel-1), action="W", cost=5))
        if self.map[row][column] == 'F' and fuel<9:
            arcs.append(Arc(node, (row,column,9), action="Fuel up", cost=15))
        return arcs


    def estimated_cost_to_goal(self, node):
        """Return the Manhatten distance to the nearest callpoint,
        ignoring any obstacles and ignoring fuel.
        """
        row, col = node[0:2]
        mindis = math.inf
        for G in self.callpoints:
            g_row, g_col = G
            distance = abs(g_row-row)+abs(g_col-col)
            if distance < mindis:
                mindis = distance
        return mindis*5


def print_map(graph, frontier, solution) :
    """Takes a routing graph, a frontier and a solution path, then prints a map such that:
    ~ map is the same as the original map
    ~ expanded free spaces are marked with a .
    ~ free spaces part of the solution are marked with *
    """
    graphmap = graph.map
    for node in frontier.expanded:
        row, column = node[0:2]
        if graphmap[row][column] == ' ':
            graphmap[row][column] = '.'

    if solution:
        for arc in solution:
            row, column = arc.head[0:2]
            if graphmap[row][column] == ' ' or graphmap[row][column] == '.':
                graphmap[row][column] = '*'

    print('\n'.join([''.join(row) for row in graphmap]))


def main() :
    map_str = """\
+---------+
|  G XXXX |
|    X  F |
| F  X    |
|       3 |
+---------+
"""

    map_graph = RoutingGraph(map_str)
    frontier = AStarFrontier(map_graph)
    solution = next(generic_search(map_graph, frontier), None)
    print_actions(solution)
    print_map(map_graph, frontier, solution)

# only run locally
if __name__ == "__main__":
    main()
