<h1 align="center">DAISY SEED, TREMOLO FX PEDAL</h1>

Please find the project final report here: [Daisy Tremolo, Final Project Report](https://github.com/elliottohmiller/tremolo/blob/master/Final%20Project%20Report%20-%20Making%20Embedded%20Systems%20-%20Elliott%20Miller.pdf)

For more Electro-Smith Daisy info, check out the [Getting Started Wiki page!](https://github.com/electro-smith/DaisyWiki/wiki)

This repo is a final project submission for Classpert's _Making Embedded Systems_ class. The README.md is an edit of the README.md authored by Electro-Smith found here: https://github.com/electro-smith/DaisyExamples/blob/master/README.md

Included are:

Electro-Smith's "libDaisy" and "DaisySP" hardware and dsp libraries + build scripts. 
Tremolo project files located in tremolo/seed/tremolo.

All files found in tremolo/seed/tremolo are original and copyright elliottohmiller with the exception of "interrupt.cpp," which is modified source code copied from stm32duino: https://github.com/stm32duino/Arduino_Core_STM32/blob/main/libraries/SrcWrapper/src/stm32/interrupt.cpp

Edits were made by elliottohmiller to libDaisy/src/usbd/usb_cdc_if.c and libDaisy/src/reusable to accommodate read/write commands over the virtual COM port. 
Edits were also made by elliottohmiller to DaisySP/Source/Effects/tremolo.cpp to filter control voltage waveforms. 
Finally, libDaisy/src/reusable was added to the libDaisy build as an experiment in using CMake and Make. As a result, some Cmakelists.txt and Makefile docs were edited. 

## BUILD INSTRUCTIONS

### Getting the Source

First off, there are a few ways to clone and initialize the repo (with its submodules).

You can do either of the following:

```sh
git clone --recursive https://github.com/elliottohmiller/tremolo
```

or

```sh
git clone https://github.com/elliottohmiller/tremolo
git submodule update --init
```

### Compiling the Source

Once you have the repository and the submodules (libDaisy/DaisySP) properly cloned, and the toolchain installed (for details see the [Daisy Wiki](https://github.com/electro-smith/DaisyWiki/wiki) for platform specific instructions) it's time to build the libraries.

To build both libraries at once simply run:

`./ci/build_libs.sh`

This is the same as going to each library's directory and running `make`.

This may take a few minutes depending on your computer's hardware. But should have the following output when finished:

```sh
$ ./ci/build_libs.sh 
building libDaisy . . .
rm -fR build
arm-none-eabi-ar: creating build/libdaisy.a
done.
building DaisySP . . .
rm -fR build
done.
```

### Flashing "tremolo" to the Daisy Seed

The tremolo build can be programmed via the [Daisy Web Programmer](https://electro-smith.github.io/Programmer/)
Or it can the be programmed on the commandline:

```sh
# using USB (after entering bootloader mode)
make program-dfu
# using JTAG/SWD adaptor (like STLink)
make program
```

## Updating the submodules

To pull everything for the repo and submodules:

```sh
git pull --recurse-submodules
```

to only pull changes for the submodules:

```sh
git submodule update --remote
```

Alternatively, you can simply run git commands from within the submodule and they will be treated as if you were in that repository instead of the project root folder.

Whenenever there are changes to the libraries (whether from pulling from git or manually editing the libraries) they will need to be rebuilt. This can be done by either running `./ci/build_libs.sh` or entering the directory of the library with changes and running `make`.

To check style before the automated style fixing happens, run:

`./ci/local_style_check.sh`

**Note: this requires clang-format to be installed, and accessible from `$PATH`. Automated style checking and fixing is done with clang-format-10**
