# Rubik's Cube Order Calculator

## Contents
* [Overview](#overview)
* [Fundamental Moves](#fundamental-moves)
* [Sequence Generation](#sequence-generation)
* [Valid Algorithms](#valid-algorithms)

## Overview
Repeating a set of turns on a Rubik's Cube a sufficient number of times will
always return the cube to the state in which it started. The set of turns that
is repeated is called the "algorithm", and the number of times the algorithm is
repeated is called the "order".

The [largest possible order](https://en.wikipedia.org/wiki/Rubik's_Cube_group#Group_axioms_and_properties)
is 1260, while the smallest possible order is 1.

## Fundamental Moves
For the sake of simplicity, algorithms are limited to the 12 primary [Standard
Moves](https://www.speedsolving.com/wiki/index.php/NxNxN_Notation) (e.g. F, F',
U, U', R, R', D, D', L, L', B, B').

The purpose of these limitations is to simplify algorithm generation and
in-memory layer turns. All possible valid cube combinations for a 3x3x3 cube
can be achieved using the 12 fundamental moves without orientation changes, so
there is little to be gained by the added complexity.

## Algorithm Generation
Because there are 12 fundamental moves, algorithms are sequences of base 12
numbers logically mapped to a given [fundamental move](#fundamental-moves). For
the purposes of order testing, algorithms are generated in manner similar to
the way a car odometer operates, with one primary distinction.

When a car odometer rolls over to take up another significant digit (e.g. 99 to
100, or 999 to 1000, etc.), the new significant digit is the second value in
the number system being used; e.g. "1" in the decimal system. When a sequence
rolls over, the new significant digit starts over at the lowest value in the
number system being used; e.g. "0". This enables an arbitrarily long sequence
of the same value.

An odometer can be thought of as a sequence generator if one ignores the most
significant (left most) digit.

## Valid Algorithms
* [Overview](#overview)
* [Repeat Groups](#repeat-groups)
* [Overly Repeated Moves](#overly-repeated-moves)
* [Inversions](#inversions)

### Overview
Despite being an algorithm that can be performed on a cube, some algorithms
contain unnecessary redundancy, making their order a waste of time to compute.
Such algorithms are considered "invalid" and, when practical to do so, are
excluded from the sequence of algorithms used to calculate order.

It is important to note that when algorithms are [generated in
sequence](#algorithm-generated), as they are during order calculations, it is
safe to assume that an algorithm with unnecessary redundancy has been
calculated without the redundancy at an earlier time. Hence, omitting the
algorithm loses nothing.

### Repeat Groups
In the most general sense, a repeat group is a subset of an algorithm whose
order is one.

Repeat groups are worth mentioning, but are sufficiently rare and difficult to
identify, that it is not worth the resources to identify all of them during
order testing.

Given an algorithm ***A*** with order ***N***, if we define algorithm ***A'***
as algorithm ***A*** repeated ***N*** times, then algorithm ***A'*** always has
order one.

The algorithm ***A'*** can also appear within a larger algorithm. The effect is
an algorithm that, in terms of order testing, is identical to an algorithm with
***A'*** removed.

### Overly Repeated Moves
Any move repeated three times in a row can be replaced with a single move in
the oppsite direction.

Any move repeated four times in a row (which is a special case of a [Repeat
Group](#repeat-groups)) effectively does nothing.

Therefore, any algorithm that contains a sequence that is repeated three times
can be considered invalid.

### Inversions
Quite often an algorithm will contain two moves in sequence that immediately
reverse each other. These are called inversions.

Example: U B F' F R L

Algorithms that contain any inversions are considered invalid.

