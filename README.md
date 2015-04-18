# ogre_window_manager

LMS module that manages OGRE windows. Each window can visualize an image data channel.

## Installation
Install this module like any other module. Important config files are configs/ogre\_default.cfg and configs/ogre\_plugins.cfg.

If you get any errors during start up then check if the PluginFolder configuration in ogre_plugins.cfg is existing. If not then check out [stackoverflow](http://stackoverflow.com/questions/24351449/could-not-load-dynamic-library-usr-lib-ogre-rendersystem-gl).

## Dependencies
- [ogre_binding](https://github.com/Phibedy/ogre_binding)

## TODO
- constant image ratio
- render 2D lines, rectangles...

