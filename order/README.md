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

## Client and Server
The job of calculating the smallest algorithm for all 1260 orders appears to be
greater than can be reasonably done on a single machine, so a distributed
approach is required. The client and server applications facilicate this work.

The server is a fairly lightweight application that hands out jobs to clients
and accepts status and results. As such it is not expected to be loaded down
to any great extent.

Client and server are written in a mixture of C and C++. The Cube and Algorithm
code was written in pure C++.

* [Server Data Store](#server-data-store)
* [Order Protocol](#order-protocol)
* [Server Assignment Semantics](#server-assignment-semantics)

### Server Data Store
Data is stored at the server level in a series of JSON formatted files
organized and indexed by directory structure. This avoids the complexity of
setting up and maintaining a separate database.

The downside to this approach is the potential for performance bottlenecks in
areas where data changes often. This can be mitigated by breaking the schema
into smaller files to allow for more granular write activity. But it still does
not make up for all of the missing features in a modern database engine.

For the time being, I think the problem space is sufficiently small that
simplicity wins over performance and features.

### Server Data Store Format
* [Server Data Store Definitions](#server-data-store-format)
* [Directory Layout](#directory-layout)
* [Format of next.json](#nextjson)
* [Format of result.json](#resultjson)
* [Format of user.json](#format-of-userjson)
* [Format of session.json](#sessionjson)
* [Format of checkpoint.log](#checkpointlog)

#### Server Data Store Definitions
```
${ALG_NUMBER}    => 32 bit algorithm number, relative to ${ALGORITHM}.
${ALGORITHM}     => See: Cube/Algorithm.h
${BOOL}          => true or false
${CHECK_TYPE}    => Checkpoint type - S(TART), R(ESULT), P(ROGRESS), E(END).
${CERT}          => An x509 certificate or key. PEM files are expected.
${CPUID}         => CPU ID String
${DATA_ROOT}     => The root directory of all data.
${DATETIME}      => ${ISO8601 DateTime - YYYY-mm-ddTHH:MM:SSz}
${IP_ADDRESS}    => An IPv4 or IPv6 address.
${LOG_FILE}      => Log file that will receive log entries. Rolls to ${LOG_FILE}.#
${MAX_LOG_SIZE}  => Log files with a greater number of bytes will be rolled.
${MAX_LOGS}      => Greater than this number of rolled logs will be deleted.
${MAX_SESSIONS}  => The max number of active sessions allowed.
${NUM_ALGS}      => Number of algorithms assigned.
${NUM_PROCS}     => Integer number of processors.
${ORDER}         => An integer from 1 - 1260
${PORT}          => A TCP port number. Default port is 32125.
${ROLE}          => User role - DISABLED, USER, ADMIN.
${SESSION_STATE} => Session state - ACTIVE, EXPIRED, COMPLETE.
${SESSION}       => ${UUID}
${USER}          => ${UUID}
${UUID}          => https://en.wikipedia.org/wiki/Universally_unique_identifier
```

Note: Session and User UUIDs are _always_ different.

#### Directory Layout
```
    ${DATA_ROOT}
    ├── server_config.json
    ├── results
    |   └── ${USER} -> ../users/${USER}
    └── sessions
    |   ├── all
    |   |   └── ${SESSION} -> ../users/${USER}/sessions/${SESSION}
    |   ├── expired
    |   |   └── ${SESSION} -> ../users/${USER}/sessions/${SESSION}
    |   └── incomplete
    |       ├── next.json
    |       └── ${SESSION} -> ../users/${USER}/sessions/${SESSION} 
    └── users
        └── ${USER}
            ├── results.json
            ├── user.json
            └── sessions
                └── ${SESSION}
                    ├── checkpoint.log
                    └── session.json
```

#### Format of server_config.json
```
{
    "session_size": "${NUM_ALGS}",
    "data_root": "${DATA_ROOT}",
    "network": {
        "tcp_port": ${PORT},
        "tls_pub_key": "${CERT}",
        "tls_priv_key": "${CERT}"
    },
    "logging": {
        "log_file": "${LOG_FILE}",
        "max_log_size": ${MAX_LOG_SIZE},
        "max_logs": ${MAX_LOGS}
    }
}
```

| Field | Description |
|---|---|
| session_size | 32 bit integer that defines the number of algorithms in the next assignment. |
| data_root | Root directory of the data store. |
| network.tcp_port | Server listening port. |
| network.tls_pub_key | The PEM file containing the x509 public cert. |
| network.tls_priv_key | The PEM file containing the x509 private key. |
| logging.log_file | The file to send log entries to. |
| logging.max_log_size | Log files larger than this many bytes will be rolled. |
| logging.max_logs | More than this number of rolled log files will be deleted. |

<span id="nextjson"></span>
#### Format of next.json
```
{
    "next_start_alg": "${ALGORITHM}",
    "session_size": ${NUM_ALGS}
}
```

| Field | Description |
|---|---|
| next_start_alg | The algorithm the session will start with. |
| session_size | The number of algorithms to process in the next session. |

See: [Server Assignment Semantics](#server-assignment-semantics) for details.

<span id="resultsjson"></span>
#### Format of results.json
 ```
 {
    "user": "${USER}",
    "results": [
        {
            "algorithm": "${ALGORITHM}",
            "order": ${ORDER},
            "session": "${SESSION}"
        }
    ]
 }
 ```

#### Format of user.json
```
{
    "name": "${NAME}",
    "email": "${EMAILADDRESS}",
    "user": "${USER}",
    "role": "${ROLE}",
    "max_active_sessions": ${MAX_SESSIONS},
    "checkpoint_interval": ${SECONDS},
    "strikes": ${STRIKES},
    "max_strikes": ${STRIKES}
}
```

<span id="sessionjson"></span>
 #### Format of session.json
 ```
 {
    "user": "${USER}",
    "session": "${SESSION}",
    "dateTimeStarted": "${DATETIME}",
    "checkpointLast": "${DATETIME}",
    "dateTimeEnded": "${DATETIME}",
    "state": "${SESSION_STATE}",
    "nproc": ${NUM_PROCS},
    "start_alg": "${ALGORITHM}",
    "session_size": ${NUM_ALGS},
 }
 ```

| Field | Description |
|---|---|
| user | [See: ${USER}](#server-data-store-definitions) |
| session | [See: ${SESSION}](#server-data-store-definitions) |
| dateTimeStarted | When the server received the Session Started command. |
| dateTimeEnded | When the session first became inactive (ended or expired). |
| state | [See: ${SESSION_STATE}](#server-data-store-definitions) |
| nproc | The number of processors the client has committed to this session. |
| assignment | The assignment information associated with this session. |

<span id="checkpointlog"></span>
#### Format of checkpoint.log
```
${DATETIME} ${USER} ${SESSION} ${IP_ADDRESS} ${PORT} ${ALGORITHM} ${CHECK_TYPE}
```

| Field | Description |
|---|---|
| checkpoint_log | Periodic state information sent from the client. |
| user | [See: ${USER}](#server-data-store-definitions) |
| session | [See: ${SESSION}](#server-data-store-definitions) |
| dateTime | The server time when the state information arrived. |
| net | The network information associated with the checkpoint. |

This file is meant to grow arbitrarily large, and generally only appended to.
The format is consistent, so it should be fairly easy to parse it, but in
general it is really only meant to be read by humans.

### Order Protocol
The order protocol uses TCP. The default port is 32125 unless otherwise
specified.

* [Order Protocol Background](#order-protocol-background)
* [Order Protocol Field Definitions](#order-protocol-field-definitions)
* [Order Protocol Format](#order-protocol-format)
* [Client Request Format](#client-request-format)
* [Server Response](#server-response)

#### Order Protocol Background
I considered JSON encapsulated in HTTP, but all of the C/C++ HTTP libraries I
was able to find were either bloated, too complex, or they would have made it
hard(er) to run clients on non Linux operating systems. What I really needed
was an HTTP implementation that does both sides of a TLS encrypted PUT in a few
hundred lines of copy/paste code (encryption handled via libraries of course).
I may create something like that in the future, but for a humble project like
this, the reward was not worth the effort.

Basic concepts/constraints driving this protocol:
* It is only needed to support the search for 1260 algorithms. 
* UUIDs are essentially impossible to guess.
* User UUIDs can be shared for team effort.
* Denial of Service attacks are trivial, boring, and do not affect data integrity.
* Primary threat is requesting an allocation and reporting bad or no results.
    * Mitigated by validation checking.
    * Mitigated by session expiration.
    * Mitigated by user session limits.
    * Mitigated by user strikes.

#### Order Protocol Field Definitions
| Field Name | Field Description | Field Size |
|---|---|---|
| A_NUM | Algorithm number. Relative to session. | 4 Bytes |
| Algorithm | A valid algorithm string. | Variable Size |
| Command | Client command to the server. | 1 Byte |
| Missing Orders | Comma delimited string of missing orders. | Variable Size |
| N_PROC | Number of processors the client will be using. | 2 Bytes |
| Result Code | Command result code. First byte is command code, second byte is return code. | 2 Bytes |
| Retry Interval | Number of seconds until retry. | 4 Bytes |
| S_SIZE | Session size | 8 Bytes |
| Size | Precedes a variable sized field. | 2 Bytes |
| S_UUID | Session UUID. | 16 Bytes |
| U_UUID | User UUID. | 16 Bytes |
| Version | Protocol version. Interpreted unsigned. | 1 Byte |

#### Order Protocol Format
A session consists of a starting algorithm and a 32 bit number that defines
the number of algorithms that will be calculated during that session. Each
session requires multiple protocol conversations. Protocol conversations are
initiated by clients, and ended with a reply from the server.

All sessions must start with a session initiation conversation, and then 
periodic update conversations are expected as the client progresses with its
order calculations. If periodic updates are not received in a timely manner,
the session will be expired and the range reassigned. A user is deactivated

#### Client Request Format
```
| Version | U_UUID | Command Code | Command Data |
```

| Command Code | Description | Command Data |
|---|---|---|
| 0x01 | Initiate New Session | ```| N_PROC |``` |
| 0x02 | Client Restarted During Session | ```| S_UUID |``` |
| 0x03 | Checkpoint | ```| S_UUID | A_NUM |``` |
| 0x04 | Order Discovered | ```| S_UUID | A_NUM |``` |
| 0x05 | Session Ended | ```| S_UUID | A_NUM |``` |
| 0xA0 | Admin - Expire Session | ```| S_UUID |``` |
| 0xA1 | Admin - Expire All Sessions | No command data. |
| 0xA2 | Admin - Disallow New Sessions | ```| Retry Interval |``` |
| 0xA3 | Admin - Allow New Sessions | No command data. |
| 0xAF | Admin - Drain and shut down server. | No command data. |

#### Server Response
```
| Result Code | Result Data |
```

| Result Code | Description | Result Data |
|---|---|---|
| 0x0101 | Session assigned successfully. | ```| S_UUID | Size | Algorithm | S_SIZE | Size | Missing Orders |``` |
| 0x0201 | Client restarted during session. | ```| Size | Algorithm | S_SIZE | A_NUM | Size | Missing Orders |``` |
| 0x0301 | Progress reported successfully. | ```| Size | Missing Orders |``` |
| 0x0401 | Order discovery reported successfully. | ```| Size | Missing Orders |``` |
| 0x0501 | Session ended successfully. | No result data. |
| 0xAC01 | Admin command completed successfully. | No result data. |
| 0xFF00 | Invalid command code. | ```| Reason Code |``` |
| 0xFF01 | Invalid version. | ```| Reason Code |``` |
| 0xFF02 | Invalid user UUID (U_UUID). | ```| Reason Code |``` |
| 0xFF03 | Invalid session UUID (S_UUID). | ```| Reason Code |``` |
| 0xFF04 | Invalid algorithm number (A_NUM). | ```| Reason Code |``` |
| 0xFF80 | Failed to assign session. | ```| Reason Code |``` |
| 0xFF81 | Failed to restart session. | ```| Reason Code |``` |
| 0xFF82 | Session terminated. | ```| Reason Code |``` |
| 0xFF83 | Admin command failed. | ```| Reason Code |``` |

| Reason Code | Description | Reason Data |
|---|---|---|
| 0x00 | No reason code. |
| 0x01 | All orders calculated. Project is complete! | No reason data. |
| 0x02 | No sessions available. | ```| Retry Interval |``` |
| 0x03 | User max sessions limit reached. | No reason data. |
| 0x04 | Session timed out. | No reason data. |
| 0x05 | Discovered order is invalid. | No reason data. |
| 0x06 | Expired administratively. | No reason data. |

### Server Assignment Semantics
Session size is always a power of 12, and conceptually equivalent to memory
pages and disk blocks - alignment is important so the size should not be
changed without careful consideration. In particular, session size should never
be changed while there are expired sessions available because alignment
problems can be created.

Expired sessions will be allocated before new assignments are handed out. This
ensures that the lowest algorithm is selected when an order is discovered. It
should also be noted that the concept of "lowest algorithm" is relative to the,
more or less, arbitrarily chosen [fundamental move](#fundamental-moves) order.

* [Validation Checking](#validation-checking)
* [Useful Assignments](#useful-assignments)
* [Checkpoint Interval](#checkpoint-interval)
* [Session Interval](#session-interval)
* [Assignment Expiration](#assignment-expiration)

#### Validation Checking
Secondary checks to validate ranges can be done by starting additional server
instances on non-standard TCP ports.

#### Useful Assignments
The following assignment is useless:
```
S_SIZE    => 429,981,696
ALGORITHM => F F' F U' B L' L' R D' B R
```

Since [algorithms](#algorithms) are a Base-12 sequence and 429,981,696 is 12^8,
only the eight least signficant turns will change during the processing of the
above assignment. However, the three most significant digits have no downstream
effect, making every algorithm in the assignment [invalid](#valid-algorithms).
The effect is to have the client essentially count to 429,981,696 and return,
without ever calculating any orders.

The server will avoid assignments like this by masking out the digits that make
up the assignment size to get the prefix of the algorithm. It will then check
this prefix for [validity](#valid-algorithms). If the algorithm is invalid, the
least significant digit of the prefix is incremented by one and the test is run
again.

In the above example, the prefix ```F F' F``` contains
[inversions](#inversions) which causes all algorithms in the assignment to be
invalid. In order to remedy this, the prefix would be incremented by one, to
get ```F F' F'```. Note that this sequence is also invalid, because ```F F'```
is an [inversion](#inversions). Further increments would be required
in order to find a valid algorithm.

Justification for this approach is the same as the one used for avoiding
[invalid algorithms](#valid-algorithms).

Client code is responsible for skipping invalid algorithms within useful
assignments.

#### Checkpoint Interval
Checkpoint interval is 600 seconds, and is measured at the server starting from
the completion of the last checkpoint interval, or from the time the session
was initially assigned. The 600 second value is based on the single CPU
calculation time of ~186 seconds per 1e6 algorithms on an 800Mhz AMD
Opteron(tm) Processor 6176 SE (the oldest and slowest CPU I had access to)
with no compiler optimizations.

This creates a low end estimate of ~3e6 algorithms checked over a 600 second
checkpoint interval, which gives us 12^8 / 3e6 = 143 checkpoints if the
assignment size is about a half billion (12^8).

It is expected that the majority of CPUs will be faster than the test CPU, so
200 seconds per million algorithms is probably a reasonable baseline. That
being said, testing has not been done on embedded platforms like the
RaspberryPi and Arduino, so those numbers may need to be revised.

#### Session Interval
Session interval is simply an extension of the checkpoint interval assumption
of one million algorithms completed in 200 seconds. Given a session size of
12^8, the session interval would be:

200 * (12^8 / 1e6) = 200 * 1e3 = ~86,000 seconds = ~1 day

#### Assignment Expiration
* Sessions are expired in three ways:
    * Administratively expired.
    * Three checkpoint intervals are missed.
    * Session not completed before expiration interval.

As long as checkpoint intervals are met and are inclusive of at least one
million algorithms checked, there is little need to be concerned with the
session interval. If a particular CPU wants to participate, but cannot keep up,
an exception can be made in the user's configuration.

There is no "shame" in letting a session expire. Another node will pick it up
eventually and the state transferred during execution will be used by the next
session, so nothing is lost.
