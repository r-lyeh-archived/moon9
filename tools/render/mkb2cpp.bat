del b2cpp.exe
del b2hpp.exe
cl b2cpp.cpp -I ../../src /Ox /Oy /MD /DNDEBUG
cl b2hpp.cpp -I ../../src /Ox /Oy /MD /DNDEBUG
b2cpp Ubuntu-B.ttf > Ubuntu-B.cpp
b2hpp Ubuntu-B.ttf > Ubuntu-B.hpp
type Ubuntu-B.hpp | more
type Ubuntu-B.cpp | more

