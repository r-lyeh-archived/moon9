## Moon9
Moon9 is a collection of game oriented C++ frameworks. Warning: wip, dev, undocumented, unstable, radiation hazard, defcon 3

## Supported Desktop Platforms
- Microsoft Windows XP, 7

## Supported Mobile Platforms
- None :D

## Supported compilers
- Microsoft Visual Studio 2012

## Supported build systems
- CMake

## Features
- To write

## Roadmap for 'next' branch
- To write

## Frameworks included
- [moon9::debug](https://github.com/r-lyeh/moon9/tree/master/src/moon9/debug)
- [moon9::hid](https://github.com/r-lyeh/moon9/tree/master/src/moon9/hid)
- [moon9::io](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io)
- [moon9::memtracer](https://github.com/r-lyeh/moon9/tree/master/src/moon9/memtracer)
- [moon9::os](https://github.com/r-lyeh/moon9/tree/master/src/moon9/os)
- [moon9::play](https://github.com/r-lyeh/moon9/tree/master/src/moon9/play)
- [moon9::spatial](https://github.com/r-lyeh/moon9/tree/master/src/moon9/spatial)
- [moon9::string](https://github.com/r-lyeh/moon9/tree/master/src/moon9/string)
- [moon9::test](https://github.com/r-lyeh/moon9/tree/master/src/moon9/test)
- [moon9::time](https://github.com/r-lyeh/moon9/tree/master/src/moon9/time)

## Libraries included
- [moon9::alert](https://github.com/r-lyeh/moon9/tree/master/src/moon9/os/alert), a popup alert dialog
- [moon9::any](https://github.com/r-lyeh/moon9/tree/master/src/moon9/play/factory/any), an opaque container library
- [moon9::app](https://github.com/r-lyeh/moon9/tree/master/src/moon9/os/app), a few handy functions for any app
- [moon9::arc4](https://github.com/r-lyeh/moon9/tree/master/src/moon9/string/arc4), ARC4 encryptor/decryptor
- [moon9::base91](https://github.com/r-lyeh/moon9/tree/master/src/moon9/string/base91), custom base91 encoder/decoder (XML friendly)
- [moon9::binary](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/binary), a data/size pair.
- [moon9::bistring](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/bistring), a key/value pair.
- [moon9::browse](https://github.com/r-lyeh/moon9/tree/master/src/moon9/os/browse), an URL browser
- [moon9::callback](https://github.com/r-lyeh/moon9/tree/master/src/moon9/play/callback), a callback library
- [moon9::callstack](https://github.com/r-lyeh/moon9/tree/master/src/moon9/debug/callstack), shows/retrieves a (saved) call stack
- [moon9::clipboard](https://github.com/r-lyeh/moon9/tree/master/src/moon9/os/levels/clipboard), a clipboard getter/setter
- [moon9::component](https://github.com/r-lyeh/moon9/tree/master/src/moon9/play/component), a simple component library for modern game object composition pattern
- [moon9::compress](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/compress), compress memory blocks quickly. LZ4 library required.
- [moon9::confirm](https://github.com/r-lyeh/moon9/tree/master/src/moon9/os/confirm), a yes/no confirm dialog
- [moon9::demangle](https://github.com/r-lyeh/moon9/tree/master/src/moon9/debug/callstack/demangle), a library to demangle mangled C++ symbols
- [moon9::dialog](https://github.com/r-lyeh/moon9/tree/master/src/moon9/os/dialog), a file pickup dialog
- [moon9::directives](https://github.com/r-lyeh/moon9/tree/master/src/moon9/os/endian/directives), compiler/os directives header
- [moon9::dll](https://github.com/r-lyeh/moon9/tree/master/src/moon9/os/dll), a dynamic library loader/caller
- [moon9::dt](https://github.com/r-lyeh/moon9/tree/master/src/moon9/time/dt), measure time diff in seconds
- [moon9::endian](https://github.com/r-lyeh/moon9/tree/master/src/moon9/os/endian), a few handy functions to handle endianness
- [moon9::environment](https://github.com/r-lyeh/moon9/tree/master/src/moon9/os/levels/environment), an environment getter/setter
- [moon9::error](https://github.com/r-lyeh/moon9/tree/master/src/moon9/os/error), human readable os errors
- [moon9::event](https://github.com/r-lyeh/moon9/tree/master/src/moon9/play/event), a simple event library
- [moon9::factory](https://github.com/r-lyeh/moon9/tree/master/src/moon9/play/factory), a lightweight abstract factory
- [moon9::file](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/file), handy file class(es).
- [moon9::format](https://github.com/r-lyeh/moon9/tree/master/src/moon9/string/format), C-formatted C++ standard string
- [moon9::fsm](https://github.com/r-lyeh/moon9/tree/master/src/moon9/play/fsm), a safe finite state machine implementation
- [moon9::hash](https://github.com/r-lyeh/moon9/tree/master/src/moon9/string/hash), uniform hash class; provides interface for CRC32, GCRC, RS, JS, PJW, ELF, BKDR, SBDM, DJB, DJB2, BP, FNV, AP, BJ1, MH2, SHA1
- [moon9::headers](https://github.com/r-lyeh/moon9/tree/master/src/moon9/os/headers), platform specific OS headers
- [moon9::http](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/http), HTTP interface.
- [moon9::id](https://github.com/r-lyeh/moon9/tree/master/src/moon9/string/id), compile-time string to id (size_t) conversion
- [moon9::is_debug](https://github.com/r-lyeh/moon9/tree/master/src/moon9/debug/is_debug), handy is_debug boolean flag
- [moon9::is_release](https://github.com/r-lyeh/moon9/tree/master/src/moon9/debug/is_release), handy is_release boolean flag
- [moon9::keycodes](https://github.com/r-lyeh/moon9/tree/master/src/moon9/hid/keycodes), portable keyboard codes
- [moon9::module](https://github.com/r-lyeh/moon9/tree/master/src/moon9/play/module), a lightweight game module state manager
- [moon9::mutex](https://github.com/r-lyeh/moon9/tree/master/src/moon9/os/mutex), an improved mutex class
- [moon9::netstring](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/netstring), a netstring serializer.
- [moon9::obj](https://github.com/r-lyeh/moon9/tree/master/src/moon9/play/obj), an dynamic/agnostic/serializable object class for POD data
- [moon9::pak](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/pak), a package archiver. Miniz library required.
- [moon9::prompt](https://github.com/r-lyeh/moon9/tree/master/src/moon9/os/prompt), a string prompt dialog
- [moon9::roman](https://github.com/r-lyeh/moon9/tree/master/src/moon9/string/roman), integer to roman library
- [moon9::sleep](https://github.com/r-lyeh/moon9/tree/master/src/moon9/time/sleep), sleep a thread for given seconds
- [moon9::stream](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/stream), attach/detach loggers to ostreams. Also create ostreams.
- [moon9::string](https://github.com/r-lyeh/moon9/tree/master/src/moon9/string/string), extended C++ standard string
- [moon9::tcp](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/tcp), TCP based client/server.
- [moon9::tree](https://github.com/r-lyeh/moon9/tree/master/src/moon9/play/tree),
- [moon9::trie](https://github.com/r-lyeh/moon9/tree/master/src/moon9/string/trie), string autocompletion
- [moon9::types](https://github.com/r-lyeh/moon9/tree/master/src/moon9/hid/types), abstract and portable HID types
- [moon9::types](https://github.com/r-lyeh/moon9/tree/master/src/moon9/os/endian/types), hopefully portable types
- [moon9::udp](https://github.com/r-lyeh/moon9/tree/master/src/moon9/io/udp), UDP based client/server. ENet library required.
- [moon9::unicode](https://github.com/r-lyeh/moon9/tree/master/src/moon9/string/unicode), vector of unicode codepoints
- [moon9::wink](https://github.com/r-lyeh/moon9/tree/master/src/moon9/wink/sleep), sleep a thread for shortest time

## Bug Reporting and Feature Requests
If you find a bug in a Sample, or have an enhancement request, simply file an
[Issue](https://github.com/r-lyeh/moon9/issues) and send a message (via github messages)
to the Committers to the project to let them know that you have filed
an [Issue](https://github.com/r-lyeh/moon9/issues).

## License
The project is open sourced under the MIT license.

## Disclaimer
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
