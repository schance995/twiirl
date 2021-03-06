# twiirl

Play sounds by moving a Nintendo Wii remote

Twiirl is my [DCC capstone project](http://dcc.umd.edu/learning/capstone/).

Source code is available at <https://github.com/schance995/twiirl>.

## Abstract

Twiirl (pronounced "twirl") is a cross-platform music synthesizer controlled by a Nintendo Wii remote. It continuously plays sound which can be altered by moving the remote. The effect is that it is possible to correlate motion and music, and to hear what a direction such as "forward" sounds like. Twiirl is free and open source software, enabling others the freedom to study and customize the code to produce other sounds and extend Twiirl as they please. Overall Twiirl integrates music, sound, motion, and software to explore a new audio, visual and physical representation of music as organized sound. The major contribution is to visualize motion as music and to demonstrate the potential for a tool that generates its own music for use in a dance performance.

## Setup

### 0. Hardware

You will need a Wii remote, preferably Wii motion plus. You'll also need a supported operating system, which includes Linux, Windows, and (in theory) MacOS.

### 1. Wiiuse

[Wiiuse](https://github.com/wiiuse/wiiuse) is a bluetooth driver for Nintendo Wii devices. You might have to [compile from source](https://github.com/wiiuse/wiiuse#platforms-and-dependencies).

**Linux**

Some Linux distributions such as Fedora may package this already, in which case run `sudo dnf install wiiuse` to install wiiuse.

Otherwise clone wiiuse and run cmake/make install per their instructions. You may have to run `sudo ldconfig` to refresh the libraries afterwards.

**Windows**

Use cmake to generate a Visual Studio solution. Then open Visual Studio and compile the solution in Release mode.

The resulting wiiuse.lib will be in `build/src/Release/wiiuse.lib`. Copy the lib and dll files to the twiirl folder.

### 2. Compile twiirl.c

**Linux**

In a terminal type:

```
cd path/to/twiirl/folder
make
```

This will produce `twiirl.exe`.

**Windows**

Open the Visual Studio developer command prompt then type:

```
cd path/to/twiirl/folder
cl.exe twiirl.c wiiuse.lib /I /path/to/wiiuse/src/folder
```

For example if wiiuse and twiirl are in the same parent directory, the path to the wiiuse folder is `../wiiuse/src`

This will also produce `twiirl.exe`.

### 3. Pyo

[Pyo](https://github.com/belangeo/pyo) is a Python module used to generate sounds.

**Linux**

Some Linux distributions such as Ubuntu may package this already, in which case run `sudo apt install python3-pyo` to install pyo.

Otherwise [compile pyo from source](http://ajaxsoundstudio.com/pyodoc/compiling.html). Make sure to enable the Jack audio server.

**Windows**

[Install pyo with pip](http://ajaxsoundstudio.com/pyodoc/download.html) and a compatible Python version.

## Run

**Linux**

Make sure bluetooth is on and that in the terminal you're in the twiirl folder, then type

```
python3 twiirl.py
```

Then press the red sync button at the back the Wii remote as Twiirl is starting.

**Windows**

Make sure bluetooth is on, then open Control Panel -> Add a device and add the Wii remote. The Wii remote should be paired before running Twiirl.

Then open a terminal (Powershell will do) and type

```
cd path/to/twiirl/folder
python.exe twiirl.py
```

## Controls

Twiirl does not play any sounds on startup, press the Home button to start it. Then press a button on the directional pad (D-pad) to start and stop a sound. Move the Wii remote to change the sound.

To quit, press Ctrl-C on the keyboard, or the power button on the Wii remote.

When holding the Wii remote in the vertical orientation:

Button           | Action
---------------- | -----------
Home             | toggle Wii Remote motion reporting (whether the sound changes with motion)
D-Pad Up         | toggle sine
D-Pad Down       | toggle saw
D-Pad Left       | toggle square
D-Pad Right      | toggle triangle
A                | toggle melody
B                | toggle chaos
Plus             | raise volume
Minus            | lower volume
One              | increase pitch transition delay (more smooth)
Two              | decrease pitch transition delay (more beepy)

Twiirl is CPU intensive, if there are delays when multiple sounds are playing, you should look into lowering your latency (such as with realtime privileges).

## How it works

`twiirl.exe` prints gyroscope values from the Wii remote. `twiirl.py` reads them from standard input via a subprocess and converts them into sound. See the source code for more details.

In theory Twiirl can be controlled by another game controller such as JoyCons. Additionally, an arbitrary image or video could be transformed into a series of numbers and feed into Twiirl, so we can hear what they sound like.

## License

> This program is free software: you can redistribute it and/or modify
> it under the terms of the GNU General Public License as published by
> the Free Software Foundation, either version 3 of the License, or
> (at your option) any later version.
>
> This program is distributed in the hope that it will be useful,
> but WITHOUT ANY WARRANTY; without even the implied warranty of
> MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
> GNU General Public License for more details.
>
> You should have received a copy of the GNU General Public License
> along with this program.  If not, see <http://www.gnu.org/licenses/>.
