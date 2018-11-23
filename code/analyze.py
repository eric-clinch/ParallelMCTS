
def average(L):
    return sum(L) / len(L)

def averageSublist(L, n):
    s = 0
    length = 0
    for i in range(0, len(L), n):
        sublist = L[i : min(i + n, len(L))]
        s += max(sublist)
        length += 1
    return s / length

file = open("res.txt")
text = file.read()
text = text.split(",")
nums = list(map(int, text))


base = averageSublist(nums, 1)
baseIters = 6000

for i in [1, 2, 4, 8]:
    expectedIters = baseIters * base / averageSublist(nums, i)
    print(expectedIters)

print(nums)