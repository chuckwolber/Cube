/**
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2019 Chuck Wolber
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE  1
#define FALSE 0
#define ORDER_MAX_CONST 1261

/**
 * Face Order: U L F R B D
 *      U   U   U   U   U   U   U   U   L   L   L   L   L   L   L   L   F   F   F   F   F   F   F   F   R   R   R   R   R   R   R   R   B   B   B   B   B   B   B   B   D   D   D   D   D   D   D   D  
 * S	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34	35	36	37	38	39	40	41	42	43	44	45	46	47	48
 * F	1	2	3	4	5	16	13	11	9	10	41	12	42	14	15	43	22	20	17	23	18	24	21	19	6	26	27	7	29	8	31	32	33	34	35	36	37	38	39	40	30	28	25	44	45	46	47	48
 * F'	1	2	3	4	5	25	28	30	9	10	8	12	7	14	15	6	19	21	24	18	23	17	20	22	43	26	27	42	29	41	31	32	33	34	35	36	37	38	39	40	11	13	16	44	45	46	47	48
 * U	6	4	1	7	2	8	5	3	17	18	19	12	13	14	15	16	25	26	27	20	21	22	23	24	33	34	35	28	29	30	31	32	9	10	11	36	37	38	39	40	41	42	43	44	45	46	47	48
 * U'	3	5	8	2	7	1	4	6	33	34	35	12	13	14	15	16	9	10	11	20	21	22	23	24	17	18	19	28	29	30	31	32	25	26	27	36	37	38	39	40	41	42	43	44	45	46	47	48
 * R	1	2	19	4	21	6	7	24	9	10	11	12	13	14	15	16	17	18	43	20	45	22	23	48	30	28	25	31	26	32	29	27	8	34	35	5	37	3	39	40	41	42	38	44	36	46	47	33
 * R'	1	2	38	4	36	6	7	33	9	10	11	12	13	14	15	16	17	18	3	20	5	22	23	8	27	29	32	26	31	25	28	30	48	34	35	45	37	43	39	40	41	42	19	44	21	46	47	24
 * D	1	2	3	4	5	6	7	8	9	10	11	12	13	38	39	40	17	18	19	20	21	14	15	16	25	26	27	28	29	22	23	24	33	34	35	36	37	30	31	32	46	44	41	47	42	48	45	43
 * D'	1	2	3	4	5	6	7	8	9	10	11	12	13	22	23	24	17	18	19	20	21	30	31	32	25	26	27	28	29	38	39	40	33	34	35	36	37	14	15	16	43	45	48	42	47	41	44	46
 * L	40	2	3	37	5	35	7	8	14	12	9	15	10	16	13	11	1	18	19	4	21	6	23	24	25	26	27	28	29	30	31	32	33	34	46	36	44	38	39	41	17	42	43	20	45	22	47	48
 * L'	17	2	3	20	5	22	7	8	11	13	16	10	15	9	12	14	41	18	19	44	21	46	23	24	25	26	27	28	29	30	31	32	33	34	6	36	4	38	39	1	40	42	43	37	45	35	47	48
 * B	27	29	32	4	5	6	7	8	3	10	11	2	13	1	15	16	17	18	19	20	21	22	23	24	25	26	48	28	47	30	31	46	38	36	33	39	34	40	37	35	41	42	43	44	45	9	12	14
 * B'	14	12	9	4	5	6	7	8	46	10	11	47	13	48	15	16	17	18	19	20	21	22	23	24	25	26	1	28	2	30	31	3	35	37	40	34	39	33	36	38	41	42	43	44	45	32	29	27
 */

static struct option longopts[] = {
    {"algstart",     required_argument, NULL, 'a'},
    {"count",        required_argument, NULL, 'c'},
    {"heartbeat",    required_argument, NULL, 'b'},
    {"find-orders",  required_argument, NULL, 'f'},
    {"print-config", optional_argument, NULL, 'p'},
    {"help",         optional_argument, NULL, 'h'},
    {NULL,           0,                 NULL,   0}
};

static const int ORDER_MAX = ORDER_MAX_CONST;
static const int DEFAULT_ALG_MAX = 1000000;

char* prog_name;
int found_orders[ORDER_MAX_CONST];
unsigned int algorithm_count;
unsigned int heartbeat;
unsigned int find_specific_orders = FALSE;
unsigned int print_config = FALSE;

void Usage() {
    printf("usage: %s [--algstart | -a] [--count | -c] [--heartbeat | -b] [--find-orders | -f] [--print-config | -p] [--help | -h]\n", prog_name);
    printf(" [--algstart | -a]     - The algorithm to start with. Default is \"F\". Base-12\n");
    printf("                         counting order is F F' U U' R R' D D' L L' B B'\n");
    printf(" [--count | -c]        - The number of algorithms to calculate. Input is interpreted\n");
    printf("                         as an unsigned integer. Default is 1,000,000.\n");
    printf(" [--heartbeat | -b]    - Display a heartbeat during --find-orders, equivalent to\n");
    printf("                         every arg attempts\n");
    printf(" [--find-orders | -f]  - Only find orders that are in this list. Omit or include\n");
    printf("                         an empty list to find all orders.\n");
    printf(" [--print-config | -p] - Print the runtime configuration.\n");
    printf(" [--help | -h]         - Display this messages.\n");
}

void PrintConfig() {
    if (find_specific_orders) {
        printf("Searching for specific orders:\n");
        for (int i = 1; i < ORDER_MAX; i++)
            if (!found_orders[i])
                printf("\tLooking for: %d\n", i);
    }
    printf("Algorithm Count: %d\n", algorithm_count);
}

void SetFindOrders(char* order_list) {
    for (int i = 0; i < ORDER_MAX; i++)
        found_orders[i] = TRUE;

    unsigned int curr_order = 0;
    while (*order_list != '\0') {
        char c = *order_list;

        if (c != ',' && (c < '0' || c > '9')) {
            printf("\nERROR: Parse error - \'%c\' is an invalid order character.\n\n", c);
            Usage();
            exit(1);
        }

        if (c == ',' && curr_order > 0 && curr_order < ORDER_MAX) {
            found_orders[curr_order] = FALSE;
            curr_order = 0;
        } else if (c == ',') {
            curr_order = 0;
        } else if (c >= '0' && c <= '9') {
            curr_order = curr_order*10 + (unsigned int)(c - '0');
        }

        order_list++;
    }

    if (curr_order < ORDER_MAX)
        found_orders[curr_order] = FALSE;
}

// __global__
void calculate_orders() {
    char cube[48] = {'U','U','U','U','U','U','U','U',
                     'L','L','L','L','L','L','L','L',
                     'F','F','F','F','F','F','F','F',
                     'R','R','R','R','R','R','R','R',
                     'B','B','B','B','B','B','B','B',
                     'D','D','D','D','D','D','D','D'};

}

int main(int argc, char *argv[]) {
    int ch;
    char* algorithm_start;

    algorithm_count = DEFAULT_ALG_MAX;
    heartbeat = 0;
    prog_name = argv[0];

    opterr = 0;
    while((ch = getopt_long(argc, argv, "a:c:b:f:ph", longopts, NULL)) != -1) {
        switch(ch) {
            case 'a':
                algorithm_start = optarg;
                break;
            case 'c':
                algorithm_count = (unsigned int)(strtol(optarg, NULL, 10));
                break;
            case 'b':
                heartbeat = (unsigned int)(strtol(optarg, NULL, 10));
                break;
            case 'f':
                SetFindOrders(optarg);
                find_specific_orders = TRUE;
                break;
            case 'p':
                print_config = TRUE;
                break;
            case 'h':
            default:
                Usage();
                return 0;
                break;
        }
    }

    if (print_config)
        PrintConfig();
}

