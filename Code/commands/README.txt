This directory is explicity for the use of commands.  Quick and easy.

All commands work as follows:

[code]
#include "Copyright.h"
#include "Engine.h"

.... Insert Command Code Here ....
[/code]

We store only the commands themselves within the command.cpp's, and all 'required' 
functions will be held in other .cpp files as to keep things simple.

By keeping things simple in the command.cpp files it will allow us to easily debug
a change of a .cpp file quickly and efficiently with minimal overhead.

