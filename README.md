# Mozzarella Engine ♟️

Mozzarella is a from-scratch **chess engine** written in **C++**, focusing on search correctness, performance, and clean engine architecture.

It implements **iterative deepening minimax with alpha-beta pruning** and speaks the **Universal Chess Interface (UCI)** protocol, allowing it to run inside standard chess GUIs such as [Arena](http://www.playwitharena.de/), [Cute Chess](https://cutechess.com/), [Scid vs. PC](https://scidvspc.sourceforge.net/), and others.

Mozzarella also powers my public bot account on [Lichess](https://lichess.org/@/mozzarella-engine).


## Project Goals

Mozzarella is a learning-focused engine designed to explore classical search techniques and engine architecture. It prioritizes correctness and clarity over feature completeness or competitive strength.


## Features

- Bitboard-based board representation
- Alpha-beta pruning with iterative deepening
- Depth-limited search via `go depth N`
- Transposition table using Zobrist hashing
- UCI protocol support


## Build

### Requirements

To build Mozzarella, you need the following tools installed on your system:

- **C++17-compatible compiler** (e.g., g++, clang++, or MSVC)
- **Make** (version 4.0 or higher)

### Instructions

1. Clone the repository:

   `$ git clone https://github.com/ProbablyNotBrandon/mozzarella-engine`

3. Switch to the `src` directory:

   `$ cd mozzarella-engine/src`

5. Build the executable by running:

   `$ make mozzarella`


## Usage
Mozzarella implements the core Universal Chess Interface (UCI) protocol and can be run directly or loaded into any UCI-compatible chess GUI.

Below is a minimal example of interacting with the engine via the command line:

```
$ ./mozzarella
uci
id name Mozzarella Engine
id author Brandon Thiessen
uciok
isready
readyok
position startpos
go depth 6
bestmove e2e3
quit
```

For more information on interacting with chess engines using the UCI protocol, check out [this page](https://www.wbec-ridderkerk.nl/html/UCIProtocol.html). Please note that Mozzarella does not necessarily support all commands listed on this page.

> For interactive play or analysis, add the `mozzarella` executable as a UCI engine in a compatible chess GUI (e.g. Arena, Cute Chess, Scid vs. PC). Once loaded, the engine accepts standard UCI commands such as `position` and `go depth N`.


## Project Roadmap

Below are some of the upcoming features and improvements that are coming to Mozzarella:

- Multithreaded search
- Improved move ordering and search heuristics
- Additional UCI support (e.g. hash size, pondering)
- Opening book support
- Endgame tables
- Magic bitboards for move generation
