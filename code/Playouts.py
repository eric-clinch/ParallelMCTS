import sys

FILENAME = "tournament.txt"

if (len(sys.argv) > 1):
    FILENAME = sys.argv[1]

# returns a list of (name, playouts) pairs representing the how many playouts,
# the given strategy ran
def getPlayouts(fileName):
    f = open(fileName)
    s = f.read()
    f.close()

    lines = s.splitlines()
    lines = list(filter(lambda x: " playouts: " in x, lines))
    names = list(map(lambda x: x.split(" iterations:")[0], lines))
    playouts = list(map(lambda x: int(x.split(" ")[-1]), lines))
    return list(zip(names, playouts))

# returns a sorted list of strings representing the names of the players
# in the given match history
def getPlayerNames(playouts):
    result = set()
    for p, _ in playouts:
        result.add(p)
    result = list(result)
    result.sort()
    return result

playoutsAndNames = getPlayouts(FILENAME)
playerNames = getPlayerNames(playoutsAndNames)

totalPlayouts = dict()
numPlayouts = dict()

for player, playouts in playoutsAndNames:
    totalPlayouts[player] = totalPlayouts.get(player, 0) + playouts
    numPlayouts[player] = numPlayouts.get(player, 0) + 1

for player in playerNames:
    print("%s: %.2f" % (player, totalPlayouts[player] / numPlayouts[player]))

