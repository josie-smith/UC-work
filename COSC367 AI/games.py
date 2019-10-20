# You can represent a perfect-information zero-sum turn-based game as a game
# tree. Layers in the tree calculate the minimum or maximum utility alternately.
# We then define a function for returning the optimal move in a game of noughts
# and crosses.


import copy

def max_value(tree) :
    if type(tree) == int: return tree
    return max(min_value(child) for child in tree)

def min_value(tree):
    if type(tree) == int: return tree
    return min(max_value(child) for child in tree)

def max_action_value(game_tree) :
    if type(game_tree) == int: return None, game_tree
    mins = [min_value(child) for child in game_tree]
    return mins.index(max(mins)), max(mins)

def min_action_value(game_tree) :
    if type(game_tree) == int: return None, game_tree
    maxs = [max_value(child) for child in game_tree]
    return maxs.index(min(maxs)), min(maxs)



def option_list(board, player) :
    options = []
    n = len(board)
    for row in range(n):
        for col in range(n):
            if board[row][col] == '.':
                newboard = copy.deepcopy(board)
                newboard[row][col] = player
                options.append(newboard)
    return options

def is_win(board, player):
    n = len(board)
    for row in range(n):
        win = True
        for col in range(n):
            if board[row][col] != player: win = False
        if win: return True

    for col in range(n):
        win = True
        for row in range(n):
            if board[row][col] != player: win = False
        if win: return True

    win = True
    for row in range(n):
        if board[row][row] != player: win = False
    if win: return True

    win = True
    for row in range(n):
        if board[row][n-row-1] != player: win = False
    return win

def is_draw(board):
    n = len(board)
    full = True
    for row in range(n):
        for col in range(n):
            if board[row][col] == '.': full = False
    if full:
        if not is_win(board, 'X') and not is_win(board, 'O'):
            return True
    return False

def other(player) :
    if player == 'X': return 'O'
    elif player == 'O': return 'X'
    else: return Null

def find_utility(board, player): # player has just played
    if is_win(board, player): utility = 1
    elif is_win(board, other(player)): utility = -1
    elif is_draw(board): utility = 0
    else:
        other_utility = -5
        for next_board in option_list(board, other(player)):
            next_utility = find_utility(next_board, other(player))
            if next_utility > other_utility:
                other_utility = next_utility
            if other_utility == 1: break
        utility = (-1)*other_utility
    return utility


def optimal_move(board, player) :
    n = len(board)
    utility = -5
    for new_board in option_list(board, player):
        next_utility = find_utility(new_board, player)
        if next_utility > utility:
            utility = next_utility
            next_board = new_board
        if utility == 1: break
    for row in range(n):
        for col in range(n):
            if next_board[row][col] != board[row][col]:
                return row,col
    return None


board = [['X', 'X','O'], ['.', 'O','.'],['.', '.','.']]
player = 'X'

print(optimal_move([['X', 'X', 'O'], ['.', 'O', '.'], ['.', '.', '.']], 'X'))
