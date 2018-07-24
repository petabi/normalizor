# I. Overview

Normalizor is a simple library to apply high-speed regular
expression matching against a textual input and to `normalize'
that input by identifying regions that meet certain regular
expression descriptions defining a region that might need
to be normalized.  The result from Normalizor is a list comprised of
normal lines that include the raw line, and the indexes (start, stop, and type)
of any matches.  You may then use these results as seen fit.

Normalizor is designed to work in blocks (on larger documents).
It amortizes processing by processing these larger blocks.
The end result is increased speed and efficiency.  However,
if you are matching against a single line, or just a couple of
lines it is possible that normalizor will still process quite slowly.

Finally, Normalizor offers python bindings so that it can be used
in Python.  The goal is to aid in the parsing of huge log files
where parsing line-by-line can take minutes or hours.

# II. Caveats and Known Issues

* The performance of Normalizor depends on two things:
  1. The number and complexity of Regular Expressions (more regex == more effort).
  2. The number of matches per line (more matches == more effort == slower).
* The tool currently assumes that the line end is the newline char.
  It will define lines by this character.  If the input has no newline chars it will be treated
  as one big line (which may be problematic).
  
# III. Requirements

Normalizor uses hyperscan for fast and efficient regular
expression matching.  Normalizor also uses boost, googletest,
and Python3.

* Hyperscan
* Boost::filesystem
* Boost::program_options (for test binary)
* Python3 (Python3.6 specifically)
* Google Test (for testing)
* cmake
* google profiler (for testing--optional)

# IV. API

Normalizor relies on a couple of structures.

## Normal_line

The Normal_line structure holds the results of normalization.  It is comprised of the
following:

```
std::string line;  # The original line
Sections sections; # The indexes for matches found
```

## Sections

The Sections struct is used to manage the matching regions within a line.
All offsets are from the beginnig of the referenced line (i.e. original line in the
Normal_line structure).  Sections is just a standard data structure:

```
std::map<size_t, std::pair<int, size_t>>
```

* map-key = start offset for a region.
* map-value-first = ID for the type found (Normal_type).
* map-value-second = end offset for the region.

Use this data to find the regions and the types of those regions (i.e. what it matched to).

## Normal_type

The Normal_type represents the types of interest.  There is a default set to be explained
later.  This set can be changed, but must be set prior to normalization.  The Normal_type
is comprised of the following:

```
std::string regex;  # The regular expression to find a desired region.
unsigned int flags; # flags for said regex (as int) like caseless...
std::string replacement; # A replacement string.
```

By default, Normalizor provides several normal types from timestamps to base64.
Please update or add Normal_types as needed.  Also, please reference the header
for the default values.  The Normal_type are kept in a std::map within normalizor.
The key for each type is the ID for that Normal_type.  Note that the ID 0 must always
refer to the line ending.  If it does not, then that is undefined behavior.

# V. Usage

## C++ Usage

Normalizor uses a default constructor.  So, to create a normalizor object please just
use something like:

```
Line_normalizor ln;
```

Please use the function to update the normal types used by Normalizor:

```
ln.modify_current_normal_types(id, my_normal_type);
```

Please use this function to get teh current map of Normal_types:

```
auto my_n_types = ln.get_current_normal_types();
```

Please use the following to set the input source:

```
ln.set_input_stream(my_string_filename);
```

or

```
ln.set_input_stream(my_is_stream);
```

When everything is ready to go please use the following:

```
auto lines = ln.normalize();
```

## Python Usage

```
import py_normalizor as norm
myln = norm.Line_normalizer()
myln.set_input_stream(filename)
mylines = myln.normalize()
```

## Command Line tool: testor

The command line tool for normalizor is called testor.
It is mostly designed to provide statistics for reading files, but may offer some example
usage.  It can be found in the tools directory for reference.

Usage:

```
./testor myfilename
```

The option `-p` allows you to use google profiler and `-d` will print all the lines read to the screen.
The statistics printed after a run represent just the time spent in Normalizor.
