import sys


def main():
    libdir = sys.argv[1]
    sys.path.append(libdir)
    import py_normalizor as norm
    lines = ['Oct 12 00:00:00 lewiston newsyslog[40328]: logfile turned over\n',
             'Oct 12 03:01:00 lewiston sendmail[40915]: gethostbyaddr(10.0.170.1) failed: 1\n',
             'Oct 16 03:13:34 lewiston sendmail[79024]: STARTTLS=client, relay=[127.0.0.1], version=TLSv1.2, verify=FAIL, cipher=DHE-RSA-AES256-GCM-SHA384, bits=256/256\n']
    with open("my_test.log", 'w') as fo:
        for ll in lines:
            fo.write(ll)
    myln = norm.Line_normalizer()
    myln.set_input_stream("my_test.log")
    mylines = myln.normalize()
    s0 = norm.section2dict(mylines[0].sections)
    assert s0 == {0: (1, 15), 35: (6, 40)}
    s1 = norm.section2dict(mylines[1].sections)
    assert s1 == {0: (1, 15), 34: (6, 39), 56: (2, 66), 76: (6, 77)}


if __name__ == "__main__":
    main()
