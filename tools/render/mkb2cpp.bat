del b2cpp.exe
del b2hpp.exe
cl b2cpp.cpp -I ../deps -I ../deps/glm /Ox /Oy /MD
cl b2hpp.cpp -I ../deps -I ../deps/glm /Ox /Oy /MD
b2cpp Ubuntu-B.ttf > Ubuntu-B.cpp
b2hpp Ubuntu-B.ttf > Ubuntu-B.hpp
type Ubuntu-B.hpp | more
type Ubuntu-B.cpp | more

