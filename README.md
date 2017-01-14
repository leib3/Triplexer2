# Triplexer

### Update
**In `OPTIONS` in the Makefile, replace `-DUSB_SERIAL` with `-DUSB_MIDI_SERIAL` to
enable USB MIDI and serial simultaneously.**

STEVE, DANIEL, and MATT:
These directions below walk you through the template I have used for the triplexer project.
This setup allows you to work with the entire Teensy codebase at your fingertips, which is 
something the Arduino IDE prevents you from doing. There is nothing stopping you from using an IDE
but it will take a considerable amount of time to setup. If you're at all familiar with makefiles,
the instructions below should be pretty straightfoward. Otherwise, you're going to need to do some
reading. Basically, you'll notice a file called `Makefile` in the triplexer directory. The creator of
the Teensy template, apmorton, wrote it. What it does is handle the actual compilation of the entire
codebase  in an efficient way -- in particular, each time you type `make` in a terminal in the triplexer 
directory, only source files you have modified and other source files that depend on those modified files
are recompiled, saving time. 
### Git
Now about git itself. Two links follow. The first 
is a "quick and dirty" introduction to git commands, and the second is a link to the official git document-
ation. I would strongly suggest you read the first 2-3 chapters of the git documentation. Note that if you
would prefer to abstract away the command-line usage of git that there are graphical front-ends around, but
you'll have to do some googling. 

1. (http://rogerdudler.github.io/git-guide/)
2. (https://git-scm.com/book/en/v2/Getting-Started-About-Version-Control)

**IMPORTANT NOTE:** the code I've written is in the `src` folder!

Some decent introductory material on Makefiles:
(http://matt.might.net/articles/intro-to-make/)

**The instructions below are from the teensy template github page: (https://github.com/apmorton/teensy-template)**

Teensy 3.X Project Template
===========================

Purpose
-------

An easy starting point for a Teensy 3.X project which might not fit inside the
arduino build environment.


Reasons to Use
--------------

- You need to modify the teensy core
- You don't love Java IDE's
- You love Make
- Because


Setup
-----

Install the Teensy udev rule: `sudo cp tools/49-teensy.rules /etc/udev/rules.d/`

Then unplug your Teensy and plug it back in.


Using
-----

1. Put your code in `src/main.cpp`
2. Put any libraries you need in `libraries`
3. Set the TEENSY variable in `Makefile` according to your teensy version
4. Build your code ```make```
5. Upload your code ```make upload```


Make Targets
------------

- `make` alias for `make hex`
- `make build` compiles everything and produces a .elf
- `make hex` converts the elf to an intel hex file
- `make post_compile` opens the launcher with the correct file
- `make upload` uploads the hex file to a teensy board
- `make reboot` reboots the teensy


Where everything came from
--------------------------

- The `teensy3` sub-folder is taken from [The Teensy 3 Cores](https://github.com/PaulStoffregen/cores/tree/master/teensy3)
- The `tools` sub-folder is taken from [Teensyduino](http://www.pjrc.com/teensy/td_download.html)
- The `src/main.cpp` file is moved, unmodified from `teensy3/main.cpp`
- The `Makefile` file is moved, modified from `teensy3/Makefile`
- The `49-teensy.rules` file is taken from [PJRC's udev rules](http://www.pjrc.com/teensy/49-teensy.rules)

Modifications to `Makefile` include
- Add support for arduino libraries
- Change tools directory
- Calculate target name from current directory
- Prettify rule output
- Do not upload by default, only build
=======
