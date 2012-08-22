/*********************************************************************
 *
 * Copyright 2006 Keith Packard and Carl Worth
 * Copyright 2012 Intel Corporation
 * Copyright 2012 VMware, Inc.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <limits>

#include "trim_callset.hpp"

using namespace trim;

#define MAX_LEVEL 16

CallRange::CallRange(CallNo call_no, int level)
{
    first = call_no;
    last = call_no;
    this->level = level;

    next = new CallRange*[level];
}

bool
CallRange::contains(CallNo call_no)
{
    return (first <= call_no && last >= call_no);
}

CallSet::CallSet(): head(0, MAX_LEVEL)
{
    head.first = std::numeric_limits<CallNo>::max();
    head.last = std::numeric_limits<CallNo>::min();

    for (int i = 0; i < MAX_LEVEL; i++)
        head.next[i] = NULL;

    max_level = 0;
}

/*
 * Generate a random level number, distributed
 * so that each level is 1/4 as likely as the one before
 *
 * Note that level numbers run 1 <= level < MAX_LEVEL
 */
static int
random_level (void)
{
    /* tricky bit -- each bit is '1' 75% of the time */
    long int bits = random() | random();
    int	level = 1;

    while (level < MAX_LEVEL)
    {
	if (bits & 1)
	    break;
        level++;
	bits >>= 1;
    }

    return level;
}

void
CallSet::add(CallNo call_no)
{
    CallRange **update[MAX_LEVEL];
    CallRange *node;
    int i, level;

    /* Find node immediately before insertion point. */
    node = &head;
    for (i = max_level - 1; i >= 0; i--) {
        while (node->next[i] && call_no > node->next[i]->last) {
            node = node->next[i];
        }
        update[i] = &node->next[i];
    }

    /* Can we contain call_no by expanding tail of current range by 1? */
    if (node != &head && node->last == call_no - 1) {

        CallRange *next = node->next[0];

        node->last = call_no;

        /* Also merge with next node as needed. */
        if (next && next->first == call_no + 1) {
            node->last = next->last;

            /* Delete node 'next' */
            for (i = 0; i < node->level && i < next->level; i++) {
                node->next[i] = next->next[i];
            }

            for (; i < next->level; i++) {
                *update[i] = next->next[i];
            }

            delete next;
        }

        return;
    }

    /* Current range could not contain call_no, look at next. */
    node = node->next[0];

    if (node) {
        /* Do nothing if call_no is already contained. */
        if (node->first <= call_no) {
            return;
        }

        /* Can we exand head of range by 1? */
        if (node->first == call_no + 1) {
            node->first = call_no;
            return;
        }
    }

    /* Not possible to expand any existing node, so create new one. */

    level = random_level();

    /* Handle first node of this level. */
    if (level > max_level) {
        level = max_level + 1;
        update[max_level] = &head.next[max_level];
        max_level = level;
    }

    node = new CallRange(call_no, level);

    /* Perform insertion into all lists. */
    for (i = 0; i < level; i++) {
        node->next[i] = *update[i];
        *update[i] = node;
    }
}

bool
CallSet::contains(CallNo call_no)
{
    CallRange *node;
    int i;

    node = &head;
    for (i = max_level - 1; i >= 0; i--) {
        while (node->next[i] && call_no > node->next[i]->last) {
            node = node->next[i];
        }
    }

    node = node->next[0];

    if (node == NULL)
        return false;

    return node->contains(call_no);
}
