cl client.cpp -I..\include -DWIN32 ..\*.c ws2_32.lib winmm.lib /MDd /DDEBUG /Zi /Oy-
cl server.cpp -I..\include -DWIN32 ..\*.c ws2_32.lib winmm.lib /MDd /DDEBUG /Zi /Oy-
