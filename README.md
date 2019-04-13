# nodeGraph

This is a project for cleaning up sample node editors of ImGUI to a more usable and extensible one. It will be used by Limon Engine when I am satisfied with it.

If you want to use it in your project, copy the folder in to your project, and set NODEGRAPH_AS_LIBRARY variable to "True".

example:

```cmake
set(NODEGRAPH_AS_LIBRARY "True")
add_subdirectory("libs/nodeGraph")
```