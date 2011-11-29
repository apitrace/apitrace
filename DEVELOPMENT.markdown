Coding Style
============

XXX: These are mostly guidelines for new code, as some of existing hasn't been
updated to these conventions yet.

Whitespace (all languages):

 * indentation is 4 spaces

 * never use tabs as indents

 * otherwise tab equals to 8 spaces

 * separate classes with two empty lines

Naming convention:

 * camelCase for functions/methods

 * UpperCase for structures/classes

 * lowercase for namespaces/modules

 * `UPPER_CASE` for #defines

C++:

 * enclose single statement `if` clauses in { }, specially for automatically
   generated code

 * } else {

 * use inlines for functions/methods which are called with high-frequency

CMake:

 * `lower_case`

 * space between ( and precedent name


When in doubt, be consistent with the existing code.


Commit policy
=============

Feature development:

* Existing features in master branch should not degrade at any time, for any
  platform.  (Unless it is not widely used and there is agreement.)

* It's fine to add new features for only some platforms.

* Non-trivial changes should be staged in a branch, to enable peer-review and
  regression testing.  Branch should be deleted once code has been merged.

* Releases are tagged commits from master.  There are no stable branches.


Backwards compatibility:

* Backwards binary compatibility with old traces must be always maintained: all
  tools, including glretrace, must handle old traces without regressions.

* No backwards compatibility guarantees for derived data (ASCII dumps, state,
  images, etc).

* There should be no gratuitous change to command line tool interfaces, but no
  guarantees are given.



Regression testing
==================

There is a regression test suite under development in
https://github.com/apitrace/apitrace-tests .

