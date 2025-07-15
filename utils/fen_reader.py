#!/usr/bin/python3

import json


def display_fen(fen: str):
    board = fen.split(" ")[0]
    hline = "\n" + "+---" * 8 + "+" + "\n|"
    print(hline, end='')
    for c in board:
        if c == "/":
            print(hline, end='')
            continue
        if c.isdigit():
            tmp = int(c)
            c = " "
            for _ in range(tmp):
                print(f"   |", end='')
        else:
            print(f" {c} |", end='')
    hline_last = "\n" + "+---" * 8 + "+"
    print(hline_last)


if __name__ == "__main__":
    with open("example_fens.json", "r") as f:
        stuff = json.load(f)

    for item in stuff:
        display_fen(item["fen"])

    display_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
