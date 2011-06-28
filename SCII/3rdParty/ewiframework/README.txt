This is kind off diificult to use but is a set of Widgets and
a plugin for QtCreator/QtDesigner. The plugin allows you to 
drag and drop some items to a UI.

If you aren't changing the GUI then you should be able to use
it without to much hassle.

This a copy from http://code.google.com/p/ewiframework/


It has some slight changes to make it build under Windows 7.

To get the Plugin to work under Windows 7 with MingW. You need to:

1) Download QtCreator Source
2) Download Qt Source Code
3) Build QtCreator pointing to Qt Source - build "release"!
4) Build ewiframework and copy the dll's to the QtCreator above - build "release"

For some stupid reason the Qt Creator that comes with the Windows MingW QtSDK
is built with Visual Studio. So if you try to use a plugin created with MingW
it fails to load. There does not appear to be a MingW prebuilt QtCreator so 
your only option is to build your own. 

If you are build the project with Visual Studio you'll probably not get the
error.

Once it is working you will be able to move around the tacho etc and add new items.

 