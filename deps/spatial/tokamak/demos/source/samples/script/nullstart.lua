print("running startup script")

PathSep = "\\"

DirScript = neGetCWD() .. PathSep .. "script" .. PathSep

DirModel = neGetCWD() .. PathSep .. "model" .. PathSep

DirEffect = neGetCWD() .. PathSep .. "effect" .. PathSep

DirTexture = neGetCWD() .. PathSep .. "model" .. PathSep

---------------------------------------------------

dofile(DirScript .. "config.lua")

---------------------------------------------------

dofile(DirScript .. "effect_table.lua")

---------------------------------------------------

print("startup script completed")