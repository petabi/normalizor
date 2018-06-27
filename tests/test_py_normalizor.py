import sys


def main():
    libdir = sys.argv[1]
    filename = sys.argv[2]
    sys.path.append(libdir)
    print(sys.path)
    import py_normalizor as norm
    myln = norm.Line_normalizer()
    myln.set_input_stream(filename)
    mylines = myln.normalize()
    print("my lines len: ", len(mylines));

if __name__ == "__main__":
    main()
