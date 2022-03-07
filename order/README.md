# Rubik's Cube Order Calculator

## Contents
* [Overview](#overview)
* [Development Constraints](#development-constraints)
* [Fundamental Moves](#fundamental-moves)
* [Algorithms](#algorithms)
* [Valid Algorithms](#valid-algorithms)
* [CLI](#cli)
* [Client and Server](#client-and-server)

## Overview
Repeating a set of turns on a Rubik's Cube a sufficient number of times will
always return the cube to the state in which it started. The set of turns that
is repeated is called the "algorithm", and the number of times the algorithm is
repeated is called the "order".

The [largest possible order](https://en.wikipedia.org/wiki/Rubik's_Cube_group#Group_structure)
is 1260, while the smallest is one.

## Development Constraints
The following development constraints are core to this project:

* Do the minimum necessary.
* Keep the code simple and self-contained.
* Make an effort to be compatible with anything claiming POSIX compliance.

## Fundamental Moves
For the sake of simplicity, algorithms are limited to the 12 primary [Standard
Moves](https://www.speedsolving.com/wiki/index.php/NxNxN_Notation) (e.g. F, F',
U, U', R, R', D, D', L, L', B, B').

The purpose of these limitations is to simplify algorithm generation and
in-memory layer turns. All possible valid cube combinations for a 3x3x3 cube
can be achieved using the 12 fundamental moves without orientation changes, so
there is little to be gained by adding additional complexity.

## Algorithms
Refer to Algorithm.h for a conceptual description of algorithms.

## Valid Algorithms
* [Overview](#valid-algorithm-overview)
* [Overly Repeated Moves](#overly-repeated-moves)
* [Repeat Groups](#repeat-groups)
* [Inversions](#inversions)

### Valid Algorithm Overview
Despite being an algorithm that can be performed on a cube, some algorithms
contain unnecessary redundancy, making their order a waste of time to compute.
Such algorithms are considered "invalid" and, when practical to do so, are
excluded from the sequence of algorithms used to calculate order.

It is important to note that when algorithms are generated in sequence, as they
are during order calculations, it is safe to assume that an algorithm with
unnecessary redundancy has been calculated without the redundancy at an earlier
time. Hence, omitting the algorithm loses only the resources required to
identify it.

### Overly Repeated Moves
Any move repeated three times in a row can be replaced with a single move in
the oppsite direction.

Any move repeated four times in a row (which is a special case of a [Repeat
Group](#repeat-groups)) effectively does nothing.

Therefore, any algorithm that contains a sequence that is repeated three or 
more times can be considered invalid.

### Repeat Groups
We identify some common and easy to detect repeat groups, but complex repeat
groups become more costly to idenfity than the benefit gained from excluding
them from order calculation. This assumption falls apart if the algorithm is
very large, the nature of the processing being performed on the algorithm is
unusually complex, or the user is simply interested in determining which
algorithms contain repeat groups.

In the most general sense, a repeat group is a subset of an algorithm whose
order is one.

Given an algorithm ***A*** with order ***N***, if we define algorithm ***A'***
as algorithm ***A*** repeated ***N*** times, then algorithm ***A'*** always has
order one.

The algorithm ***A'*** can also appear within a larger algorithm. The effect is
an algorithm that, in terms of order testing, is identical to an algorithm with
***A'*** removed.

### Inversions
Quite often an algorithm will contain two moves in sequence that immediately
reverse each other. These are a special case of repeat groups called
"inversions".

Inversions of arbitrary length are always detectable because their inner two
moves always reverse each other. Hence it is only necessary to look for two
moves that immediately reverse each other.

Examples:
```
F U B R' R B' U' L
U B F' F R L
```

## CLI
The Command Line Interface allows for "quick and dirty" order calculations
given a range of algorithms. When the output is redirected to a file, the
cli_aggregate application is useful for indexing the output by order.
