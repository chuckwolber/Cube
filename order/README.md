# Rubik's Cube Order Calculator

## Contents
* [Overview](#overview)
* [Fundamental Moves](#fundamental-moves)
* [Algorithms](#algorithms)
* [Valid Algorithms](#valid-algorithms)
* [CLI](#cli)

## Overview
Repeating a set of turns on a Rubik's Cube a sufficient number of times will
always return the cube to the state in which it started. The set of turns that
is repeated is called the "algorithm", and the number of times the algorithm is
repeated is called the "order".

The [largest possible order](https://en.wikipedia.org/wiki/Rubik's_Cube_group#Group_structure)
is 1260, while the smallest is one.

## Fundamental Moves
For the sake of simplicity, algorithms are limited to the 12 primary [Standard
Moves](https://www.speedsolving.com/wiki/index.php/NxNxN_Notation) (e.g. F, F',
U, U', R, R', D, D', L, L', B, B').

The purpose of these limitations is to simplify algorithm generation and
in-memory layer turns. All possible valid cube combinations for a 3x3x3 cube
can be achieved using the 12 fundamental moves without orientation changes, so
there is little to be gained by adding additional complexity.

## Algorithms
Refer to Algorithm.hpp for a conceptual description of algorithms.

## Valid Algorithms
Despite being an algorithm that can be performed on a cube, some algorithms
contain unnecessary redundancy, making their order a waste of time to compute.
Such algorithms are considered "invalid" and, when practical to do so, are
excluded from the sequence of algorithms used to calculate order.

It is important to note that when algorithms are generated in sequence, as they
are during order calculations, it is safe to assume that an algorithm with
unnecessary redundancy has been calculated without the redundancy at an earlier
time. Hence, omitting the algorithm loses only the resources required to
identify it.

Refer to the Algorithm::isRedundant method for details.`

## CLI
The Command Line Interface allows for "quick and dirty" order calculations
given a range of algorithms. Use `make fast` to build the optimized version of
the cli.

