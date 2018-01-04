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
* `glfw`
* `inotify`
* `fuse` (for `tup`)

If you're on archlinux
```shell
$ sudo pacman -S glfw tup fuse-common fuse3
$ sudo reboot now
... wait and login ...
$ sudo modprobe fuse
```