# Normalizor

Normalizor is a simple library to apply high-speed regular
expression matching against a textual input and to `normalize`
that input by identifying regions that meet certain regular
expression descriptions defining regions that might need
to be normalized.  In this case, normalization implies the removal
or standardization of these particular regions.
The result from Normalizor is a list comprised of
normal lines data structures.  Each normal line data structure
includes the raw line, and a data structure containing any regions
that might need normalization where each region is defined by the
start and end indices (referenced from the start of the line)
As well as the type of normalization that might be needed
(expalined further in normal types below).
You may then use these results as you see fit.

Normalizor is designed to work in blocks (on larger documents).
It amortizes processing by processing these larger blocks.
The end result is increased speed and efficiency for larger amounts of data.
However, if you are matching against a single line or a small amount of
data it is possible that more traditional methods (such as regex
replace) will perform better.

Finally, Normalizor offers python bindings so that it can be used
in Python.  The goal is to aid in the parsing of huge log files
where parsing line-by-line can take minutes or hours.

## Caveats and Known Issues

* The performance of Normalizor depends on two things:
  1. The number and complexity of Regular Expressions (more regex == more effort).
  2. The number of matches per line (more matches == more effort == slower).
* The tool currently assumes that the line end is the newline char.
  It will define lines by this character.  If the input has no newline chars it will be treated
  as one big line (which may be problematic).
  
## Requirements

Normalizor uses hyperscan for fast and efficient regular
expression matching.  Normalizor also uses boost, googletest,
and Python3.

* Hyperscan
* Boost::filesystem
* Boost::program_options (for test binary)
* Boost::python
* Python3
* Google Test (for testing)
* cmake
* google profiler (for testing--optional)

## API

Normalizor relies on a couple of structures.

### Normal_line

The Normal_line structure holds the results of normalization.  It is comprised of the
following:

```
std::string line;  # The original line
Sections sections; # The indexes for matches found
```

### Sections

The Sections struct is used to manage the matching regions within a line.
All offsets are from the beginnig of the referenced line (i.e. original line in the
Normal_line structure).  The ID is the ID of the Normal_type (see below) that matched for
the section.  Sections is just a standard data structure:

```
std::map<size_t, std::pair<int, size_t>>
```

* map-key = start offset for a region.
* map-value-first = ID for the type found (Normal_type).
* map-value-second = end offset for the region.

Use this data to find the regions in the line that matched to a particular Normal_type.

### Normal_type

The Normal_type represents the types or regions of interest.
There is a default set to be explained later.
This set can be changed, but must be set prior to normalization.
The set is maintained as map in the normalizer object.  Those elements with
lower IDs (IDs closer to zero) have precedence over elements with higher
IDs (please see matching behavior below).  The first element in this map
must have the id of the public member variable line_end_id.  The regular expression
for this Normal_type must define the end of `lines` (where `lines` are considered
the atomic elements examined).  By default this is works on newlines.  Please
do not change this value unless you know what you are doing as changing it
will have unspecified results.  The Normal_types at other indices may all
be adjusted to meet requirements of the normalization.
The public variable line_end_id is just an alias of ID `0`.
Each Normal_type is comprised of the following:

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

#### Matching Behavior:

When regions are identified within a line there are a couple of rules in effect to
handle overlapping matches:

* Longer matches have precedence over shorter matches.
* Lower Normal_type IDs have precedence of Higher Normal_type IDs for any ties.
* There can only be one match for overlapping regions.

This can create some confusing results depending on the regular expressions
used.  First, try to avoid regular expressions where the beginning of the regex
for a Normal_type can intersect with the end of another Normal_type.  This
will help eliminate potential overlap.  Second, make use of Normal_type precedence
to give higher precedence to more specific Normal_types and lesser precedence
to more premissive Normal_types.

## Usage

### C++ Usage

Normalizor uses a default constructor.  So, to create a normalizor object please just
use something like:

```
Line_normalizor ln;
```

Please use the following function to update the normal types used by Normalizor:

```
ln.modify_current_normal_types(id, my_normal_type);
```

Please use this function to get the current map of Normal_types:

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

When everything is ready you can get the next block of normalized
data from the input stream with the following function.

```
auto my_normal_lines = ln.get_normalized_block();
```

To get all lines, simply keep calling this function until it returns an empty
data structure. The normalizer object retains state, so to read the same
file again you will need to create a new normalizer object.

```
auto my_normal_lines = ln.get_normalized_block();
while (!my_normal_lines.empty()) {
   ... do something ...
   my_normal_lines = ln.get_normalized_block();
}
```

### Python Usage

Note:  Just as in C++ each call to `get_normalize_block()` will return one block of
lines.  Keep calling this function until it returns empty to read the entire file.

```
import py_normalizor as norm
myln = norm.Line_normalizer()
myln.set_input_stream(filename)
mylines = myln.get_normalized_block()
```

### Command Line tool: testor

The command line tool for normalizor is called testor.
It is mostly designed to provide statistics for reading files, but may offer some example
usage.  It can be found in the tools directory for reference.

Usage:

```
./testor myfilename
```

The option `-p` allows you to use google profiler and `-d` will print all the lines read to the screen.
The statistics printed after a run represent just the time spent in Normalizor.
