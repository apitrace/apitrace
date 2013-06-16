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

#include "os.hpp"
#include "trace_fast_callset.hpp"

using namespace trace;

#define MAX_LEVEL 16

FastCallRange::FastCallRange(CallNo first, CallNo last, int level)
{
    this->first = first;
    this->last = last;
    this->level = level;

    next = new FastCallRange*[level];
}

bool
FastCallRange::contains(CallNo call_no) const
{
    return (first <= call_no && last >= call_no);
}

bool
FastCallSet::empty(void) const
{
    return max_level == 0;
}

FastCallSet::FastCallSet(): head(0, 0, MAX_LEVEL)
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
    long int bits = os::random() | os::random();
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
FastCallSet::add(CallNo first, CallNo last)
{
    FastCallRange **update[MAX_LEVEL];
    FastCallRange *node, *next;
    int i, level;

    /* Find node immediately before insertion point. */
    node = &head;
    for (i = max_level - 1; i >= 0; i--) {
        while (node->next[i] && first > node->next[i]->last) {
            node = node->next[i];
        }
        update[i] = &node->next[i];
    }

    /* Can we contain first by expanding tail of current range by 1? */
    if (node != &head && node->last == first - 1) {

        node->last = last;
        goto MERGE_NODE_WITH_SUBSEQUENT_COVERED_NODES;

    }

    /* Current range could not contain first, look at next. */
    node = node->next[0];

    if (node) {
        /* Do nothing if new range is already entirely contained. */
        if (node->first <= first && node->last >= last) {
            return;
        }

        /* If last is already included, we can simply expand
         * node->first to fully include the range. */
        if (node->first <= last && node->last >= last) {
            node->first = first;
            return;
        }

        /* This is our candidate node if first is contained */
        if (node->first <= first && node->last >= first) {
            node->last = last;
            goto MERGE_NODE_WITH_SUBSEQUENT_COVERED_NODES;
        }
    }

    /* Not possible to expand any existing node, so create a new one. */

    level = random_level();

    /* Handle first node of this level. */
    if (level > max_level) {
        level = max_level + 1;
        update[max_level] = &head.next[max_level];
        max_level = level;
    }

    node = new FastCallRange(first, last, level);

    /* Perform insertion into all lists. */
    for (i = 0; i < level; i++) {
        node->next[i] = *update[i];
        *update[i] = node;
    }

MERGE_NODE_WITH_SUBSEQUENT_COVERED_NODES:
    next = node->next[0];
    while (next && next->first <= node->last + 1) {
        if (next->last > node->last)
            node->last = next->last;

        /* Delete node 'next' */
        for (i = 0; i < node->level && i < next->level; i++) {
            node->next[i] = next->next[i];
        }

        for (; i < next->level; i++) {
            *update[i] = next->next[i];
        }

        delete next;

        next = node->next[0];
    }
}

void
FastCallSet::add(CallNo call_no)
{
    this->add(call_no, call_no);
}

bool
FastCallSet::contains(CallNo call_no) const
{
    const FastCallRange *node;
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
