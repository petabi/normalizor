import sys


def main():
    libdir = sys.argv[1]
    filename = sys.argv[2]
    sys.path.append(libdir)
    import py_normalizor as norm
    myln = norm.Line_normalizer()
    myln.set_input_stream(filename)
    mylines = myln.normalize()
    for l in mylines:
        if len(l.sections) == 0:
            return 1
    s0 = norm.section2dict(mylines[0].sections)
    assert s0 == {0: (1, 19), 19: (7, 20), 24: (7, 25), 27: (
        7, 28), 30: (7, 31), 34: (7, 35), 40: (7, 41), 41: (6, 42)}


if __name__ == "__main__":
    main()
