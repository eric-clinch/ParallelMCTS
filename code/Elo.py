import sys

FILENAME = "tournament.txt"

if (len(sys.argv) > 1):
    FILENAME = sys.argv[1]

# ideas taken from https://blog.mackie.io/the-elo-algorithm

import math

class Player(object):
    # Having N more elo points should mean you win ~WIN_RATIO times as often
    N = 100 
    WIN_RATIO = 2
    INIT_RATING = 400
    K = 20

    def __init__(self):
        self.rating = Player.INIT_RATING

    # Estimates the probability that player A beats player B given their
    # respective ratings Ra, Rb
    @staticmethod
    def scoreEstimation(p1, p2):
        x = p1.rating - p2.rating
        scale = Player.N / math.log(Player.WIN_RATIO)
        exponent = - (x / scale)
        expected = 1 / (1 + math.exp(exponent))
        return expected

    # given two players, updates their elo ratings based off the match outcome.
    # match outcome should be 1 if p1 won and 0 if p2 won
    @staticmethod
    def updateRatings(p1, p2, matchOutcome):
        predictedOutcome = Player.scoreEstimation(p1, p2)
        update = Player.K * (matchOutcome - predictedOutcome)
        p1.rating += update
        p2.rating -= update


# returns a list of (winner, loser) pairs representing the matches played,
# where the winner and loser are represented by their names as strings
def getMatchHistory(fileName):
    f = open(fileName)
    s = f.read()
    f.close()

    lines = s.splitlines()
    lines = list(filter(lambda x: " defeated " in x, lines))

    def getNames(line):
        names = line.split(" defeated ")
        names = list(map(lambda x: x.split(":")[1], names))
        return names

    matches = list(map(lambda x: getNames(x), lines))
    return matches

# returns a sorted list of strings representing the names of the players
# in the given match history
def getPlayerNames(matchHistory):
    result = set()
    for p1, p2 in matchHistory:
        result.add(p1)
        result.add(p2)
    result = list(result)
    result.sort()
    return result


matchHistory = getMatchHistory(FILENAME)
print("%d matches played" % len(matchHistory))
playerNames = getPlayerNames(matchHistory)
playerDict = {name : Player() for name in playerNames}

for match in matchHistory:
    winner, loser = match
    Player.updateRatings(playerDict[winner], playerDict[loser], 1)

for playerName in playerNames:
    print("%s: %2.f" % (playerName, playerDict[playerName].rating))

