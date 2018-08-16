import os
import sys


def main():
    libdir = sys.argv[1]
    filename = sys.argv[2]
    sys.path.insert(0, libdir)
    import normalizor as norm
    myln = norm.Line_normalizer()
    myln.set_input_stream(filename)
    mylines = myln.get_normalized_block()
    for l in mylines:
        if len(l.sections) == 0:
            return 1
    s0 = norm.section2dict(mylines[0].sections)
    b0 = bytes(norm.str2bytes(mylines[0]))
    assert s0 == {0: (1, 19), 19: (8, 20), 24: (8, 25), 27: (
        8, 28), 30: (8, 31), 34: (8, 35), 40: (8, 41)}
    assert b0.find(b'This is my log entry 0\n') > 0
    filename = 'test.log'
    with open(filename, 'w') as fo:
        fo.write(r'68.5.15.145 - - [30/May/2014:22:54:08 -0700] "GET /10.0-STABLE/amd64/m/e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855.gz HTTP/1.1" 200 20 "-" "freebsd-update (fetch, 10.0-STABLE)"\n')
        fo.write(
            r'24.12.222.2 - - [26/May/2014:14:52:12 -0700] "\x80w\x01\x03\x01\x00N\x00\x00\x00 \x00\x009\x00\x008\x00\x005\x00\x00\x16\x00\x00\x13\x00\x00" 400 172 "-" "-"\n')
    myln = norm.Line_normalizer()
    myln.set_input_stream(filename)
    mylines = myln.get_normalized_block()
    ans = [[b'GET', b'STABLE', b'amd', b'gz', b'HTTP',
            b'freebsd', b'update', b'fetch', b'STABLE'], []]
    for i, l in enumerate(mylines):
        section = norm.section2dict(l.sections)
        sections = [[ss[0], ss[1][1]] for ss in section.items()]
        line = bytes(norm.str2bytes(l))
        tokens = [line[0:sections[0][0]]] if sections[0][0] != 0 else []
        tokens.extend(line[sections[i][1]:s[0]]
                      for i, s in enumerate(sections[1:]))
        tokens.append(line[sections[-1][1]: -1])
        assert [tk for tk in tokens if len(tk) >= 2] == ans[i]
    os.remove(filename)


if __name__ == "__main__":
    main()
