import subprocess
import time
import sys

SECONDS_PER_MOVE = 15
THREADS = 4
BOARD_SIZE = 9
USER_VS_USER = False

if (len(sys.argv) > 1):
    SECONDS_PER_MOVE = int(sys.argv[1])
if (len(sys.argv) > 2):
    THREADS = int(sys.argv[2])
if (len(sys.argv) > 3):
    BOARD_SIZE = int(sys.argv[3])
if (len(sys.argv) > 4):
    USER_VS_USER = True

def cleanLine(line):
    line = str(line)
    line = line[2:-3] # strip irrelevant characters
    return line.strip()

#taken from http://www.cs.cmu.edu/~112/notes/notes-graphics.html
def rgbString(red, green, blue):
    return "#%02x%02x%02x" % (red, green, blue)

# animation framework taken from the 15-112 course website
# https://www.cs.cmu.edu/~112/notes/notes-animations-part2.html

from tkinter import *

modeFlag = '-v' if USER_VS_USER else '-u'

def init(data):
    data.process = subprocess.Popen(['./mcts', modeFlag, '-t', str(SECONDS_PER_MOVE), 
                                     '-i', str(THREADS), '-s', str(BOARD_SIZE)], 
                                     stdout=subprocess.PIPE, 
                                     stdin=subprocess.PIPE)
    data.boardLen = 19
    data.board = [['-'] * data.boardLen for _ in range(data.boardLen)]
    data.gameOver = False
    data.userWon = False
    data.turn = None
    data.waitingOnMove = False
    data.confidence = None

    data.backgroundColor = rgbString(219, 190, 122)
    data.margin = 50
    data.botMargin = 20
    data.cellWidth = (data.width - 2 * data.margin) / data.boardLen
    data.cellHeight = (data.height - data.botMargin - 2 * data.margin) / data.boardLen
    data.circleMargin = 2

    data.BTerritory = 0
    data.BCaptures = 0
    data.WTerritory = 0
    data.WCaptures = 0

    data.passButton = Button(data.root, text="Pass", relief=GROOVE, command=lambda: makePassMove(data))
    data.passButtonWidth = 40

def mousePressed(event, data):
    if not data.waitingOnMove:
        return
    row = int((event.y - data.margin) // data.cellHeight)
    col = int((event.x - data.margin) // data.cellWidth)
    if (0 <= row < data.boardLen and 0 <= col < data.boardLen):
        message = "%d %d\n" % (row, col)
        data.process.stdin.write(message.encode())
        data.process.stdin.flush()
        data.waitingOnMove = False

def keyPressed(event, data):
    # use event.char and event.keysym
    pass

def makePassMove(data):
    if data.waitingOnMove and not data.gameOver:
        message = "%d %d\n" % (-1, -1)
        data.process.stdin.write(message.encode())
        data.process.stdin.flush()
        data.waitingOnMove = False

def parseTurn(data, line):
    data.turn = line[0]    
    assert(data.turn == 'B' or data.turn == 'W')

def parseConfidence(data, line):
    data.confidence = float(line.split(" ")[-1])

def parseBoard(data):
    # parse board
    board = []
    line = cleanLine(data.process.stdout.readline())
    while(',' not in line):
        row = line.split(' ')
        assert(len(board) == 0 or len(row) == len(board[0]))
        board.append(row)

        line = cleanLine(data.process.stdout.readline())
    data.board = board
    assert(len(data.board) == len(data.board[0])) # the board should be square
    data.boardLen = len(board)
    data.cellWidth = (data.width - 2 * data.margin) / data.boardLen
    data.cellHeight = (data.height - data.botMargin - 2 * data.margin) / data.boardLen

    # parse territory
    lineParts = line.split(",")
    words = lineParts[0].split(" ")
    data.BTerritory = int(words[-1])
    words = lineParts[1].split(" ")
    data.WTerritory = int(words[-1])

    # parse captures
    line = cleanLine(data.process.stdout.readline())
    lineParts = line.split(",")
    words = lineParts[0].split(" ")
    data.BCaptures = int(words[-1])
    words = lineParts[1].split(" ")
    data.WCaptures = int(words[-1])

    line = ""
    while('END BOARD' not in line):
        line = cleanLine(data.process.stdout.readline())

def timerFired(data):
    if data.waitingOnMove:
        return
    startTime = time.time()
    while(time.time() - startTime < .1 and not data.gameOver): # listen for .1 second
        result = data.process.poll()
        if result is not None: # the process has terminated
            data.gameOver = True
            break
        line = cleanLine(data.process.stdout.readline())

        if '\'s move' in line:
            parseTurn(data, line)
            break
        elif 'confidence' in line:
            parseConfidence(data, line)
            break
        elif 'BOARD' in line:
            parseBoard(data)
            break
        elif 'Move (row col)' in line:
            data.waitingOnMove = True
            break
        elif '******' in line:
            data.waitingOnMove = False
            break
        elif "USER LOST" in line:
            data.gameOver = True
            data.userWon = False
            break
        elif "conceding" in line:
            data.gameOver = True
            data.userWon = True
            break
        else:
            print(line)
            break

def redrawAll(canvas, data):
    canvas.create_rectangle(0, 0, data.width, data.height,
                            fill=data.backgroundColor)

    top = data.margin
    bot = top + data.cellHeight
    for row in range(data.boardLen):
        for col in range(data.boardLen):
            left = data.margin + data.cellWidth * col
            right = left + data.cellWidth
            if (row < data.boardLen - 1 and col < data.boardLen - 1):
                canvas.create_rectangle(left + data.cellWidth / 2, 
                    top + data.cellHeight / 2, right + data.cellWidth / 2, 
                    bot + data.cellHeight / 2)
            if data.board[row][col] != '-':
                color = 'red'
                if data.board[row][col] == 'B':
                    color = 'black'
                if data.board[row][col] == 'W':
                    color = 'white'
                canvas.create_oval(left + data.circleMargin, 
                    top + data.circleMargin, right - data.circleMargin, 
                    bot - data.circleMargin, fill=color)
        top = bot
        bot += data.cellHeight

    if (data.confidence is not None):
        canvas.create_text(data.margin, data.margin / 2, 
                       text = "AI confidence: %.2f" % (data.confidence), anchor=W)

    canvas.create_text(data.margin, data.height - (data.margin + data.botMargin) / 3, 
                       text = "W territory: %d" % data.WTerritory, anchor=SW)
    canvas.create_text(data.width / 2, data.height - (data.margin + data.botMargin) / 3, 
                       text = "B territory: %d" % data.BTerritory, anchor=SW)
    canvas.create_text(data.margin, data.height - 2 * (data.margin + data.botMargin) / 3, 
                       text = "W captures: %d" % data.WCaptures, anchor=SW)
    canvas.create_text(data.width / 2, data.height - 2 * (data.margin + data.botMargin) / 3, 
                       text = "B captures: %d" % data.BCaptures, anchor=SW)

    canvas.create_window(data.width - data.margin, data.margin / 2,
                width = data.passButtonWidth, window = data.passButton, anchor=NE)

    if data.gameOver:
        message = "YOU WON" if data.userWon else "YOU LOST"
        canvas.create_text(data.width / 2, data.margin / 2, text=message)
    elif data.waitingOnMove:
        canvas.create_text(data.width / 2, data.margin / 3, text="Your Turn")
        if data.turn == 'B':
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

run(800, 800)
