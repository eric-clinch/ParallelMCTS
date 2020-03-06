import subprocess
import time
import sys
from Board import Board

SECONDS_PER_MOVE = 5
THREADS = 12
BOARD_SIZE = 9

#taken from http://www.cs.cmu.edu/~112/notes/notes-graphics.html
def rgbString(red, green, blue):
    return "#%02x%02x%02x" % (red, green, blue)

# animation framework taken from the 15-112 course website
# https://www.cs.cmu.edu/~112/notes/notes-animations-part2.html

from tkinter import *

def init(data):
    data.board = Board(BOARD_SIZE)
    data.boardLen = BOARD_SIZE
    data.gameOver = False
    data.userWon = False
    data.userPlayers = [0]
    data.confidence = None

    data.backgroundColor = rgbString(219, 190, 122)
    data.margin = 50
    data.botMargin = 20
    data.cellWidth = (data.width - 2 * data.margin) / data.boardLen
    data.cellHeight = (data.height - data.botMargin - 2 * data.margin) / data.boardLen
    data.circleMargin = 2

    # Where the most recent stone was placed
    data.placedRow = None
    data.placedCol = None

    data.passButton = Button(data.root, text="Pass", relief=GROOVE, command=lambda: makePassMove(data))
    data.passButtonWidth = 40

def mousePressed(event, data):
    if data.board.current_player not in data.userPlayers:
        return
    row = int((event.y - data.margin) // data.cellHeight)
    col = int((event.x - data.margin) // data.cellWidth)
    if (0 <= row < data.boardLen and 0 <= col < data.boardLen):
        if data.board.LegalMove(row, col):
            data.placedRow = row
            data.placedCol = col
            data.board.MakeMove(row, col)

def keyPressed(event, data):
    # use event.char and event.keysym
    pass

def makePassMove(data):
    if data.waitingOnMove and not data.gameOver:
        data.board.MakeMove(-1, -1)
        data.waitingOnMove = False

def timerFired(data):
    if data.board.current_player not in data.userPlayers:
        mcts_move = data.board.GetMCTSMove(THREADS, SECONDS_PER_MOVE)
        data.board.MakeMove(mcts_move.row, mcts_move.col)
        data.placedRow = mcts_move.row
        data.placedCol = mcts_move.col
        data.confidence = mcts_move.confidence

def redrawAll(canvas, data):
    canvas.create_rectangle(0, 0, data.width, data.height,
                            fill=data.backgroundColor)

    top = data.margin
    bot = top + data.cellHeight
    board = data.board.BoardList()
    for row in range(data.boardLen):
        for col in range(data.boardLen):
            left = data.margin + data.cellWidth * col
            right = left + data.cellWidth
            if (row < data.boardLen - 1 and col < data.boardLen - 1):
                canvas.create_rectangle(left + data.cellWidth / 2, 
                    top + data.cellHeight / 2, right + data.cellWidth / 2, 
                    bot + data.cellHeight / 2)
            if board[row][col] != '-':
                color = 'red'
                if board[row][col] == 'B':
                    color = 'black'
                if board[row][col] == 'W':
                    color = 'white'
                outline = 'black'
                width = 1
                if row == data.placedRow and col == data.placedCol:
                    outline = 'red'
                    width = 4
                canvas.create_oval(left + data.circleMargin, 
                    top + data.circleMargin, right - data.circleMargin, 
                    bot - data.circleMargin, fill=color, outline=outline, width=width)
        top = bot
        bot += data.cellHeight

    if (data.confidence is not None):
        canvas.create_text(data.margin, data.margin / 2, 
                       text = "AI confidence: %.2f" % (data.confidence), anchor=W)

    canvas.create_text(data.margin, data.height - (data.margin + data.botMargin) / 3, 
                       text = "White score: %d" % data.board.GetPlayerScore(1), anchor=SW)
    canvas.create_text(data.width / 2, data.height - (data.margin + data.botMargin) / 3, 
                       text = "B score: %d" % data.board.GetPlayerScore(0), anchor=SW)
    canvas.create_window(data.width - data.margin, data.margin / 2,
                width = data.passButtonWidth, window = data.passButton, anchor=NE)

    if data.gameOver:
        message = "YOU WON" if data.userWon else "YOU LOST"
        canvas.create_text(data.width / 2, data.margin / 2, text=message)
    elif data.board.current_player in data.userPlayers:
        canvas.create_text(data.width / 2, data.margin / 3, text="Your Turn")
        if data.board.current_player == 0:
            color = 'black'
        else:
            color = 'white'
        cellRadius = data.cellWidth / 2 - data.circleMargin
        cellRadius /= 2
        canvas.create_oval(data.width / 2 - cellRadius, data.margin - cellRadius,
                           data.width / 2 + cellRadius, data.margin + cellRadius, fill=color)

def run(width=300, height=300):
    def redrawAllWrapper(canvas, data):
        canvas.delete(ALL)
        canvas.create_rectangle(0, 0, data.width, data.height,
                                fill='white', width=0)
        redrawAll(canvas, data)
        canvas.update()    

    def mousePressedWrapper(event, canvas, data):
        mousePressed(event, data)
        redrawAllWrapper(canvas, data)

    def keyPressedWrapper(event, canvas, data):
        keyPressed(event, data)
        redrawAllWrapper(canvas, data)

    def timerFiredWrapper(canvas, data):
        timerFired(data)
        redrawAllWrapper(canvas, data)
        # pause, then call timerFired again
        canvas.after(data.timerDelay, timerFiredWrapper, canvas, data)
    # Set up data and call init
    class Struct(object): pass
    data = Struct()
    data.width = width
    data.height = height
    data.timerDelay = 100 # milliseconds

    # create the root and the canvas
    root = Tk()
    root.wm_title("Go")
    root.resizable(width=False, height=False) # prevents resizing window

    data.root = root
    canvas = Canvas(root, width=data.width, height=data.height)
    canvas.configure(bd=0, highlightthickness=0)
    canvas.pack()

    init(data)

    # set up events
    root.bind("<Button-1>", lambda event:
                            mousePressedWrapper(event, canvas, data))
    root.bind("<Key>", lambda event:
                            keyPressedWrapper(event, canvas, data))
    timerFiredWrapper(canvas, data)
    # and launch the app
    root.mainloop()  # blocks until window is closed
    print("bye!")

if __name__ == "__main__":

    run(800, 800)
