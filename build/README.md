## [...]

This is *NOT* the folder where you should build your library :(
A kitten dies everytime you build a project inside a subfolder of its own project :D
Instead, choose a build folder that is not contained in the project :)

Example:

You have cloned the git repository in c:\prj\moon9.
Then you may create a c:\prj\moon9-build folder which is clearly not inside c:\prj\moon9 tree structure.
From there, you type cmake commands like following ones to build the whole project depending on your setup:

c:\prj\moon9-build> cmake c:\prj\moon9 -G "Visual Studio 11" && root.sln && rem this is the choice for most users! :D
c:\prj\moon9-build> cmake c:\prj\moon9 -G "NMake Makefiles" && nmake     && rem this is the one I like the most! :)
c:\prj\moon9-build> cmake c:\prj\moon9 -G "Unix Makefiles" && make       && rem this will work someday! :S
