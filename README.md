# Atom Heart Engine
Micro game engine

Needs [`tup`](http://gittup.org) to build.

```shell
$ git clone git@github.com:/MarceColl/atom-heart-engine.git
$ cd atom-heart-engine
$ tup init
$ tup
$ ./glfw_game
```

## Dependencies
* `sdl2`
* `inotify`
* `glut`
* `fuse` (for `tup`)

If you're on archlinux
```shell
$ sudo pacman -S sdl2 tup fuse-common fuse3 freeglut
$ sudo reboot now
... wait and login ...
$ sudo modprobe fuse
```