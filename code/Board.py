
import ctypes

LIB = None

def LoadLib():
    board_lib_path = "bazel-bin/main/libboard.so"
    global LIB
    LIB = ctypes.cdll.LoadLibrary(board_lib_path)

    LIB.NewBoard.argtypes = [ctypes.c_int]
    LIB.NewBoard.restype = ctypes.c_void_p

    LIB.BoardSize.argtypes = [ctypes.c_void_p]
    LIB.BoardSize.restype = ctypes.c_int

    LIB.LegalMove.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int]
    LIB.LegalMove.restype = ctypes.c_bool

    LIB.StoneList.argtypes = [ctypes.c_void_p]
    LIB.StoneList.restype = StoneList

    LIB.MakeMove.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int]

    LIB.PlayerScore.argtypes = [ctypes.c_void_p, ctypes.c_int]
    LIB.PlayerScore.restype = ctypes.c_int

    LIB.GetMCTSMove.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int]
    LIB.GetMCTSMove.restype = MCTSMove

class MCTSMove(ctypes.Structure):
    _fields_ = [('row', ctypes.c_int), ('col', ctypes.c_int),
                ('confidence', ctypes.c_double)]

class Stone(ctypes.Structure):
    _fields_ = [('row', ctypes.c_int), ('col', ctypes.c_int),
                ('stone_type', ctypes.c_char)]

class StoneList(ctypes.Structure):
    _fields_ = [('length', ctypes.c_int),
                ('stones', ctypes.POINTER(Stone))]

class Board():
    def __init__(self, board_size):
        if LIB is None:
            LoadLib()
        self.c_board = LIB.NewBoard(board_size)
        self.current_player = 0
        self.size = board_size

    def __len__(self):
        return LIB.BoardSize(self.c_board)

    def __repr__(self):
        board_list = self.BoardList()
        result = ''
        for row in board_list:
            result += str(row) + '\n'
        return result

    def LegalMove(self, row, col):
        return LIB.LegalMove(self.c_board, row, col)

    def BoardList(self):
        stone_list = LIB.StoneList(self.c_board)
        board_list = [['-'] * self.size for _ in range(self.size)]
        for i in range(stone_list.length):
            stone = stone_list.stones[i]
            board_list[stone.row][stone.col] = stone.stone_type.decode()
        return board_list

    def MakeMove(self, row, col):
        LIB.MakeMove(self.c_board, row, col, self.current_player)
        self.current_player = 1 - self.current_player

    def GetPlayerScore(self, player):
        return LIB.PlayerScore(self.c_board, player)

    def GetMCTSMove(self, threads, seconds):
        return LIB.GetMCTSMove(self.c_board, threads, seconds * 1000, self.current_player)


if __name__ == "__main__":
    LoadLib()

    size = 13
    b = Board(size)
    print(len(b))
    print(b)

    b.MakeMove(1, 1)
    print(b)

    b.MakeMove(2, 2)
    print(b)
