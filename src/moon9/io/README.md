## Moon9 framework: io framework
- An Input/Output framework.
- MIT licensed.

## Features
- [moon9::binary](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/binary), a data/size pair.
- [moon9::bistring](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/bistring), a key/value pair.
- [moon9::chdir](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/chdir), safe directory changer
- [moon9::compress](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/compress), compress memory blocks quickly. LZ4 library required.
- moon9::constants,
- [moon9::file](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/file), handy file class(es).
- moon9::format,
- [moon9::http](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/http), HTTP interface.
- [moon9::netstring](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/netstring), a netstring serializer.
- moon9::pack,
- [moon9::pak](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/pak), a package archiver. Miniz library required.
- [moon9::stream](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/stream), attach/detach loggers to ostreams. Also create ostreams.
- moon9::string
- [moon9::tcp](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/tcp), TCP based client/server.
- [moon9::udp](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/udp), UDP based client/server. ENet library required.
- moon9::xml

## To do:

## Dependencies
- [moon9::compress](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/compress), [lz4](https://github.com/r-lyeh/moon9/tree/master/deps/io/lz4) library required.
- [moon9::pak](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/pak), [miniz](https://github.com/r-lyeh/moon9/tree/master/deps/io/miniz) library required.
- [moon9::udp](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/udp), [ENet](https://github.com/r-lyeh/moon9/tree/master/deps/io/enet) library required.
- moon9::xml, [pugixml](https://github.com/r-lyeh/moon9/tree/master/deps/io/pugixml) library required.
