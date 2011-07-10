/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/

#ifndef _RANGE_HPP_
#define _RANGE_HPP_


/*
 * Templates for dealing with ranges and set of ranges.
 *  
 * XXX: Just enough functionality for apitrace needs -- not truly generic yet.
 */


#include <list>
#include <ostream>
#include <iostream>


namespace range {

    template< class T >
    class range {
    public:
        T start;
        T stop;

        range() {}

        range(const T &_start, const T &_stop) :
            start(_start),
            stop(_stop)
        {
            assert(_start < _stop);
        }

        inline bool intersects(const range &other) const {
            return start < other.stop && other.start < stop;
        }

        inline bool contains(const range &other) const {
            return start <= other.start && other.stop <= stop;
        }

        inline bool contained(const range &other) const {
            return other.contains(*this);
        }
    };

    template<class T>
    inline std::ostream & operator<< (std::ostream & os, const range<T> &r) {
        return os << r.start << "-" << r.stop;
    }

    template< class T >
    class range_set {
    public:
        typedef std::list< range<T> > range_list;
        range_list ranges;
        
        range_set() {}

        range_set(const range<T> &r) {
            add(r);
        }

        typedef typename range_list::const_iterator const_iterator;
        typedef typename range_list::iterator iterator;

        const_iterator begin(void) const {
            return ranges.begin();
        }

        const_iterator end(void) const {
            return ranges.end();
        }

        iterator add(const range<T> &r) {
            // XXX: Assumes non-overlapping
            return ranges.insert(ranges.end(), r);
        }

        void sub(const range<T> &r) {
            typename range_list::iterator it = ranges.begin();
            while (it != ranges.end()) {
                if (it->contained(r)) {
                    it = ranges.erase(it);
                } else {
                    if (it->intersects(r)) {
                        if (it->start < r.start && r.stop < it->stop) {
                            range<T> new_range(r.stop, it->stop);
                            it->stop = r.start;
                            assert(it->start < it->stop);
                            ++it;
                            it = ranges.insert(it, new_range);
                        } else if (r.stop < it->stop) {
                            assert(r.stop > it->start);
                            it->start = r.stop;
                            assert(it->start < it->stop);
                        } else if (r.start > it->start) {
                            assert(r.start < it->stop);
                            it->stop = r.start;
                            assert(it->start < it->stop);
                        } else {
                            assert(0);
                        }
                    }
                    ++it;
                }
            }
        }
    };

    template<class T>
    inline std::ostream & operator<< (std::ostream & os, const range_set<T> &r) {
        typename range_set<T>::const_iterator it = r.begin();
        if (it == r.end()) {
            os << "/";
        } else {
            os << *it++;
            while (it != r.end()) {
                os << "," << *it++;
            }
        }
        return os;
    }

}

#endif /* _RANGE_HPP_ */
