# Dice Wars

This is portable and multi-platform version of Dice Wars - a Risk-like game, released originally as an online Flash game by Taro Ito (http://www.gamedesign.jp/games/dicewars/flash/dice.html).

Back in 2007 I was in between of jobs and finishing my CS studies at the Helsinki univeristy and started looking into wonderful world of homebrew programming on Nintendo DS platform. This is the end result of one of the many hobby projects and experiments on that platform.

Now looking back at the codebase it's not pretty - I was still learning on how to write C++ then so it's a bit of mix of C and C++ in partly very unorganized way. Neverthless I thought it's relevant enough for sharing as it still stands as an example of one way of doing multi-platform development with SDL and DevkitPro. 

While the Nintendo DS version was the only publicly release version of the game, it was possible to compile the game on multiple different target platforms:

* Win32
* Mac OSX
* Linux
* Nintendo DS
* Nintendo Wii

There's also a server component for the multiplayer game, which works both on Linux and Mac OSX from the command line. The online version of the game uses UDP sockets for network communication.

On Nintendo DS it was possible to play local network games, but IIRC this got broken in the later releases - back in 2007 the used DSWiFi library was undergoing multiple changes. However, I still remember vividly how we played in one of the IGDA Helsinki chapter gatherings a local multiplayer match with 8 players, each around the same table with their NDS consoles. Boy, that was really something!

I doubt the codebase compiles with any recent DevkitARM releases, but who knows!
