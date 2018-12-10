
# ideas taken from https://blog.mackie.io/the-elo-algorithm

import math

class Player(object):
    # Having 100 more elo points should mean you win ~2 times as often
    N = 100 
    WIN_RATIO = 2
    INIT_RATING = 500
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

import random

numPlayers = 8
skills = [10 * i for i in range(1, numPlayers + 1)]
players = [Player() for _ in range(numPlayers)]
players_skills = list(zip(players, skills))

games = 1000
for _ in range(games):
    p1, p1Skill = random.choice(players_skills)
    p2, p2Skill = random.choice(players_skills)

    matchResult = (1 if random.random() * (p1Skill + p2Skill) < p1Skill 
                    else 0)
    Player.updateRatings(p1, p2, matchResult)

for player, skill in players_skills:
    print(player.rating, skill)

