import subprocess
import time

def cleanLine(line):
    line = str(line)
    line = line[2:-3]
    return line.strip()

#taken from http://www.cs.cmu.edu/~112/notes/notes-graphics.html
def rgbString(red, green, blue):
    return "#%02x%02x%02x" % (red, green, blue)

# animation framework taken from the 15-112 course website
# https://www.cs.cmu.edu/~112/notes/notes-animations-part2.html

from tkinter import *

def init(data):
    data.process = subprocess.Popen(['./mcts'], stdout=subprocess.PIPE, 
                                    stdin=subprocess.PIPE)
    data.boardLen = 19
    data.board = [['-'] * data.boardLen for _ in range(data.boardLen)]
    data.gameOver = False
    data.turn = None
    data.waitingOnMove = False
    data.move = None

    data.backgroundColor = rgbString(219, 190, 122)
    data.margin = 10
    data.cellWidth = (data.width - 2 * data.margin) / data.boardLen
    data.cellHeight = (data.height - 2 * data.margin) / data.boardLen
    data.circleMargin = 2

def mousePressed(event, data):
    row = (event.y - data.margin) // data.cellHeight
    col = (event.x - data.margin) // data.cellWidth
    if (0 <= row < data.boardLen and 0 <= col < data.boardLen):
        data.move = (row, col)
        message = "%d %d\n" % (row, col)
        data.process.stdin.write(message.encode())
        data.process.stdin.flush()
        data.waitingOnMove = False

def keyPressed(event, data):
    # use event.char and event.keysym
    pass

def parseBoard(data):
    line = cleanLine(data.process.stdout.readline())
    data.turn = line[0]
    assert(data.turn == 'B' or data.turn == 'W')
    for row in range(data.boardLen):
        line = cleanLine(data.process.stdout.readline())
        cells = line.split(' ')
        assert(len(cells) == data.boardLen)
        for col in range(data.boardLen):
            data.board[row][col] = cells[col]
    line = ""
    while('Move' not in line):
        line = cleanLine(data.process.stdout.readline())

def timerFired(data):
    if data.waitingOnMove:
        return
    startTime = time.time()
    while(time.time() - startTime < .1): # listen for .1 second
        result = data.process.poll()
        if result is not None: # the process has terminated
            data.gameOver = True
            break
        line = cleanLine(data.process.stdout.readline())
        if 'USER MOVE' in line:
            parseBoard(data)
            data.waitingOnMove = True
            break
        elif 'try again' in line:
            data.waitingOnMove = True
            break


def redrawAll(canvas, data):
    canvas.create_rectangle(0, 0, data.width, data.height, 
                            fill=data.backgroundColor)
    for row in range(data.boardLen):
        for col in range(data.boardLen):
            left = data.margin + data.cellWidth * col
            right = left + data.cellWidth
            top = data.margin + data.cellHeight * row
            bot = top + data.cellWidth
            canvas.create_rectangle(left, top, right, bot)
            if data.board[row][col] != '-':
                color = 'red'
                if data.board[row][col] == 'B':
                    color = 'black'
                if data.board[row][col] == 'W':
                    color = 'white'
                canvas.create_oval(left + data.circleMargin, 
                    top + data.circleMargin, right - data.circleMargin, 
                    bot - data.circleMargin, fill=color)

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
    root = Tk()
    root.resizable(width=False, height=False) # prevents resizing window
    init(data)
    # create the root and the canvas
    canvas = Canvas(root, width=data.width, height=data.height)
    canvas.configure(bd=0, highlightthickness=0)
    canvas.pack()
    # set up events
    root.bind("<Button-1>", lambda event:
                            mousePressedWrapper(event, canvas, data))
    root.bind("<Key>", lambda event:
                            keyPressedWrapper(event, canvas, data))
    timerFiredWrapper(canvas, data)
    # and launch the app
    root.mainloop()  # blocks until window is closed
    print("bye!")

run(600, 600)