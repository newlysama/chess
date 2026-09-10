# ♟️ Better Chess Engine

A chess engine written in **C++23**, built around **bitboards** and **magic bitboards** for fast, rigorously tested legal move generation (validated via *perft*). The project includes a playable console interface, a parallelized benchmark suite, and a comprehensive test suite.

> ⚠️ **Before you start**: this repository is a **rules and move-generation engine**, not (yet) an artificial intelligence. There is no evaluation function and no search algorithm (minimax / alpha-beta): you can play two human players locally, in the console.

---

## Table of Contents

- [Features](#features)
- [What's not implemented yet](#whats-not-implemented-yet)
- [Project architecture](#project-architecture)
- [Requirements](#requirements)
- [Installing dependencies](#installing-dependencies)
- [Building](#building)
- [Usage](#usage)
- [How to play](#how-to-play-in-the-console)
- [Tests](#tests)
- [Performance](#performance)
- [Generating magic numbers](#generating-magic-numbers-advanced)
- [License](#license)

---

## Features

### 🧠 Bitboard-based board representation
Each position is stored as a set of 64-bit `Bitboard`s (one bit per square), with a dedicated bitboard per color and per piece type. This representation enables very fast bitwise operations to compute occupancy, attacks, and moves.

### ⚡ Magic Bitboards for sliding pieces
Moves for rooks, bishops, and queens (which inherits from both) are computed using **magic bitboards**: precomputed "magic" numbers allow the attack bitboard for a given occupancy to be retrieved instantly via a single multiply-and-shift. A dedicated build mode (`generate_magics`) lets you regenerate these magic numbers from scratch.

### 📜 Full legal move generation
The engine generates legal moves piece by piece (pawns, knights, bishops, rooks, queens, king) and handles all special moves:
- Castling (kingside and queenside, for both colors)
- En passant capture
- Promotion (with piece selection)
- Pawn double push

### 🛡️ Check, double-check, and pin detection
The board state continuously computes squares attacked by the opponent, pieces giving check to the king, blocking squares against a check from a sliding piece, and the allowed destinations for each pinned piece — ensuring only strictly legal moves are generated.

### ↩️ Make / unmake system with a history stack
Every move played (`makeMove`) saves the previous state needed to undo it (`unmakeMove`): castling rights, en passant square, move counters, check-related flags, captured piece, etc. This history stack (up to 256 half-moves) allows instant move undo without recomputing the position.

### 📥 FEN parsing and validation
A position can be initialized from a standard **FEN** string, with strict validation of every field (piece placement, side to move, castling rights, en passant square, move counters) and explicit error messages when the FEN is invalid.

### 🖥️ Playable console interface
A console mode (`-Dconsole=true`) renders the board with colored Unicode pieces inside an ASCII grid, and lets two human players play against each other:
- Moves entered as "from square + to square", e.g. `A2B3`
- Undo the last move (`U`)
- Offer a draw after the 50-move rule (`draw`)
- Interactive prompt for the promotion piece

### 🚀 Performance benchmark (perft)
A benchmark mode runs the `perft` function (exhaustive count of reachable positions at a given depth) across 35 varied FEN positions, reporting nodes per second for each. The root-level computation is **parallelized with Intel oneTBB** to take advantage of multiple cores.

### 🧪 Test suite (GoogleTest)
Tests cover:
- FEN parsing (`test_fen_constructor.cpp`)
- Make/unmake symmetry (`test_make_unmake.cpp`)
- Move generation (`test_movegen.cpp`)
- Perft-based validation with detailed per-move-type counters — captures, castles, en passant, promotions, checks (`test_perft.cpp`)

### 📝 Logging (spdlog)
The project uses **spdlog** for logging. In `debug`/`profile` mode, logs (trace level) are printed to the console; in `release`/`test` mode, only `warning`-level logs and above are kept, written to `logs/engine.log`.

### 🔧 Continuous integration
A GitHub Actions pipeline (`.github/workflows/build.yml`) automatically builds the project across every relevant combination of modes and options, and runs the test suite on every pull request.

---

## What's not implemented yet

- **Multiplayer server mode**: not implemented and not currently planned in the build system.
- **AI / automated opponent**: no search algorithm (minimax, alpha-beta, MCTS, etc.) or position evaluation function exists yet. The engine produces legal moves but doesn't "choose" a move for you.

---

## Project architecture

```
better-chess-engine/
├── src/
│   ├── conf/               # Configuration paths (logs, console header...)
│   ├── engine/
│   │   ├── board/          # Bitboards, masks, magic numbers, State
│   │   ├── core/           # Enums (Piece, Color, MoveType...), types, constants
│   │   ├── game/            # Game (make/unmake), Move, MoveList (generation)
│   │   ├── magics_generator/# Magic number generation
│   │   └── benchmark/       # Parallelized perft benchmark (TBB)
│   ├── logging/             # spdlog wrapper
│   ├── main/
│   │   ├── main.cpp         # Entry point, dispatch based on build mode
│   │   ├── options.h        # CLI argument parsing
│   │   └── console_runner/  # Console game loop
│   ├── ui/console/          # Board rendering in the console (ANSI + Unicode)
│   └── utils/               # Various utility functions
├── tests/engine/             # GoogleTest unit tests
├── meson.build                # Main build file
├── meson_options.txt         # Build options (mode, console, generate_magics)
└── requirements.txt           # Required system packages (spdlog, gtest, oneTBB)
```

---

## Requirements

The project targets **Linux** (tested on Ubuntu 24.04) and requires:

| Tool / Library | Purpose |
|---|---|
| C++23 compiler (Clang 19 or GCC 14 recommended) | Compilation |
| [Meson](https://mesonbuild.com/) | Build system |
| [Ninja](https://ninja-build.org/) | Build backend |
| [Intel oneTBB](https://github.com/oneapi-src/oneTBB) (`libtbb-dev`) | Parallelizes the perft benchmark and the `generate_magics` mode |
| [spdlog](https://github.com/gabime/spdlog) (`libspdlog-dev`) | Logging |
| [GoogleTest](https://github.com/google/googletest) (`libgtest-dev`) | Required only for the `test` mode |

---

## Installing dependencies

On Ubuntu/Debian:

```bash
sudo apt-get update
sudo apt-get install -y \
  meson ninja-build \
  clang-19 lld-19 g++-14 build-essential \
  libspdlog-dev libgtest-dev libtbb-dev
```

Then, to use Clang as the compiler (recommended, as done in CI):

```bash
export CC=clang-19
export CXX=clang++-19
```

---

## Building

The project uses **Meson**, with a "single-mode build" system: each executable is compiled for **one** purpose at a time, selected via options passed to `meson setup`.

### Available build options

| Option | Values | Description |
|---|---|---|
| `-Dmode` | `debug` \| `profile` \| `release` \| `test` | Optimization and logging level |
| `-Dconsole` | `true` \| `false` | Builds the console-playable game |
| `-Dgenerate_magics` | `true` \| `false` | Builds the magic-number generation tool |

> ⚠️ `generate_magics` is incompatible with `console` and with `test` mode: these are mutually exclusive builds.

### Example: build the console-playable game (release)

```bash
meson setup build -Dmode=release -Dconsole=true
ninja -C build
```

The resulting executable is `build/src/main/chess`.

### Example: debug build (with sanitizers)

```bash
meson setup build -Dmode=debug -Dconsole=true
ninja -C build
```

To reconfigure an existing build directory with different options:

```bash
meson configure build -Dmode=release
ninja -C build
```

---

## Usage

Once built with `-Dconsole=true`, the `chess` executable accepts the following options:

```bash
# Start a game from the standard starting position
./build/src/main/chess

# Start a game from a custom FEN position
./build/src/main/chess --fen "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

# Run the performance benchmark (perft) at a given depth (1 to 10)
./build/src/main/chess --bench 6
```

The `--bench` mode runs a parallelized *perft* across 35 test positions and reports, for each one, the number of nodes searched, the elapsed time, and nodes per second, followed by an overall summary (min/max/average).

---

## How to play (in the console)

Once the game starts, the board is displayed and the program waits for input:

- **Play a move**: enter the starting square followed by the destination square, e.g. `A2B3` for a pawn push, or `E1G1` for White's kingside castle.
- **Undo the last move**: type `U`.
- **Offer a draw**: type `draw` (only available once the half-move clock without a capture or pawn push has reached 100, i.e. the 50-move rule); the opponent must answer `yes` or `no`.
- **Promotion**: if your move pushes a pawn to the last rank, the program will prompt you for the promotion piece (`queen`, `rook`, `bishop`, or `knight`).

The game ends automatically once checkmate is detected.

---

## Tests

Build and run the test suite using the dedicated `test` mode:

```bash
meson setup build-test -Dmode=test
ninja -C build-test
./build-test/tests/test_engine
```

Or via Meson's built-in test command:

```bash
meson test -C build-test
```

The tests notably validate move generation by comparing results against known **perft** reference values (node counts and per-move-type breakdowns at various depths) on a set of well-known test positions.

---

## Performance

The `--bench` mode included in the repository (depth 6, across the 35 test positions) reports the following, for reference, on the project's development machine:

- **~114 million nodes/second** at peak
- **~74 million nodes/second** on average

These numbers depend heavily on the hardware used (core count, clock speed, cache size); rerun `./build/src/main/chess --bench <depth>` on your own machine to get a representative measurement.

---

## Generating magic numbers (advanced)

The magic numbers used for rook and bishop attacks are normally already precomputed in the source code (`magic_const.h`). If you want to regenerate them (for example after modifying the attack masks), build with the dedicated option:

```bash
meson setup build-magics -Dmode=release -Dgenerate_magics=true
ninja -C build-magics
./build-magics/src/main/chess
```

The program searches, for each square and each sliding-piece type, for a valid magic number and prints the result to standard output — to be manually copied into the source code.

---

## License

This project is distributed under the **MIT License**. See the [`LICENSE`](./LICENSE) file for details.

---

*Author: Thibault Thomas*
