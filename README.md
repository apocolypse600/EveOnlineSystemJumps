Eve Online Recent System Jumps
============================

This is just a little project that a friend requested I make to help him in Eve Online. It takes data output from the Eve api, and combines that with data within a local sqlite database to show how many jumps have been done in each system recently.

The api data has jumps combined with system id, so this application combines that data with a local database with names linked to the ids.

Problems:
----------------------
I had all sorts of issues getting the download of the api data under Windows. So at the moment, I can only confirm it works under Linux.

One line needs a slight change to compile under qt5,


Compiling Instructions:
----------------------

Linux:

Dependancies (take these with a grain of salt, I havn't checked these exactly):
For Debian/Ubuntu

- libqt4-core
- libqt4-gui
- libqt4-xml
- libqt4-sql
- libqt4-sql-sqlite
- openssl

To compile you will also need:
- qt4-qmake
- libqt4-dev

For Arch:
- qt-4
- openssl

In the folder run qmake && make. In some cases due to qt4 being out of date, you may need to run qmake-qt4 && make instead.

Windows + OSX:
No idea

License
-------

Eve Online Recent System Jumps is copyrighted free software made available under the terms of the GPLv3

Copyright: (C) 2013 by apocolypse600. All Rights Reserved.
