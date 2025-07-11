#!/usr/bin/python3
def display_fen(fen: str):
    board = fen.split(" ")[0]
    print(board)
    hline = "\n" + "+---" * 8 + "+" + "\n|"
    print(hline, end='')
    for c in board:
        if c == "/":
            print(hline, end='')
            continue
        if c.isdigit():
            tmp = int(c)
            c = " "
            for j in range(tmp):
                print(f"   |", end='')
        else:
            print(f" {c} |", end='')
    hline_last = "\n" + "+---" * 8 + "+"
    print(hline_last)

if __name__ == "__main__":
    fen = "1R6/p2r4/2ppkp2/6p1/2PKP2p/P4P2/6PP/8 b - - 0 0"
    display_fen(fen)
