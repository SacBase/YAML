SAC YAML Module
===============

About
-----

This is a SaC module that wraps around some of the [LibYAML](http://pyyaml.org/wiki/LibYAML).
With this we are able to emit YAML. Currently there is no support for parsing YAML.

Build Instructions
------------------

You'll need to have installed `sac2c` and have a copy of the Stdlib
installed as well.

```sh
$ cd YAML
$ git submodule init
$ git submodule update
$ mkdir build
$ cd build
$ cmake ..
$ make -j4
$ make install //you might need to use `sudo'
```

Examples
--------

Examples of how to use the YAML module are found in the `examples` directory. The module usage
matches closely to that of the LibYAML C-API in that we need to pass around the emitter object.

Variables that can be passed to CMake
-------------------------------------

When running CMake it is possible to pass the following variables:
  * `-DTARGETS=x;y;z` --- build stdlib for targets x, y and z. (defaults are `cuda`)
  * `-DSAC2C_EXEC=/path/to/sac2c` --- specify `sac2c` executable directly. Otherwise CMake will
    try to find `sac2c` on yout PATH.
  * `-DLINKSETSIZE=n` --- set `-linksetsize n` when calling `sac2c`.  This option is responsible
    for the number of C functions that are put in a single C file when compiling a SaC program.
    The rule of thumb:
    * value `0` is the fastest time-wise but potentially results in a large memory consumption
    * value `1` reduces the memory consumption to minimum, buy significantly increases compilation time.
    
      *Default value: 500.*
