#!/usr/bin/env python3

'''
Author - Skylar Chan
Copyright 2021

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
'''

from pyo import *
from subprocess import Popen, PIPE
import sys
import os
import math

# get path of twiirl executable, defaults to twiirl.exe in same directory if environment variable is not provided
PATH_TO_TWIIRL_X = os.getenv('PATH_TO_TWIIRL_X', os.path.abspath(os.path.join(os.path.dirname(__file__), 'twiirl.exe')))

# boot the server
s = Server(audio='jack', buffersize=1024).boot()
# lower volume
s.amp = 0.1

# default time
t = 0.3

# sigto produces linear shift in parameter
saw_f = SigTo(value = 0, time = t)
saw_detune = SigTo(value = 0.5, time = t)
sin_f = SigTo(value = 0, time = t)
square_f = SigTo(value = 0, time = t)
square_sharp = SigTo(value = 0.5, time = t)
# hard to hear balance
# saw_balance = SigTo(value = 0.5, time = t)

triangle_f = SigTo(value = 0, time = t)
triangle_sharp = SigTo(value = 0.5, time = t)

# the sounds
sin = Sine(freq = [sin_f*3/2, sin_f])
saw = SuperSaw(freq = [saw_f*4/3, saw_f], detune = saw_detune) # , bal = saw_balance)
square = LFO(type=2, freq = [square_f, square_f*6/5], sharp = square_sharp)
triangle = LFO(type=3, freq = [triangle_f, triangle_f*7/4], sharp = triangle_sharp)

def make_melody():
    wav = SquareTable()
    env = CosTable([(0,0), (100,1), (500,.3), (8191,0)])
    met = Metro(.125, 12).play()
    amp = TrigEnv(met, table=env, dur=1)
    pit = TrigXnoiseMidi(met, dist='loopseg', x1=20, scale=1, mrange=(48,84))
    osc = Osc(table=wav, freq=pit, mul=amp)
    return osc

melody = make_melody()

chaos_chaos = SigTo(value = 0.7, time = t)
chaos_add = SigTo(value = 500, time = t)
strange = ChenLee(0.005, chaos=chaos_chaos, stereo=True, mul=250, add=chaos_add)
chaos = LFO(type=4, freq = strange)

def togglesound(sound, name):
    if sound.isOutputting():
        print('Stopping', name)
        sound.stop()
    else:
        print('Starting', name)
        sound.out()

# can adjust by any arbitrary function
def adjustdelay(fun):
    if saw.isOutputting():
        saw_f.time = fun(saw_f.time)
        print('Saw frequency delay', saw_f.time)
        saw_detune.time = fun(saw_detune.time)
        print('Saw detune delay', saw_detune.time)
    if sin.isOutputting():
        sin_f.time = fun(sin_f.time)
        print('Sin frequency delay', sin_f.time)
    if square.isOutputting():
        square_f.time = fun(square_f.time)
        print('Square frequency delay', square_f.time)
        square_sharp.time = fun(square_sharp.time)
        print('Square sharp delay', square_sharp.time)
    if triangle.isOutputting():
        triangle_f.time = fun(triangle_f.time)
        print('Triangle frequency delay', triangle_f.time)
        triangle_sharp.time = fun(triangle_sharp.time)
        print('Triangle sharp delay', triangle_sharp.time)
    if melody.isOutputting():
        melody.mul.setDur(fun(melody.mul.dur))
        print('Melody delay', melody.mul.dur)
    if chaos.isOutputting():
        chaos_chaos.time = fun(chaos_chaos.time)
        print('Chaos time delay', chaos_chaos.time)
        chaos_add.time = fun(chaos_add.time)
        print('Chaos add delay', chaos_add.time)

def A():
    togglesound(melody, 'melody')
def B():
    togglesound(chaos, 'chaos')
def Up():
    togglesound(sin, 'sine')
def Down():
    togglesound(saw, 'saw')
def Left():
    togglesound(square, 'square')
def Right():
    togglesound(triangle, 'triangle')
def Plus():
    # limit volume
    s.amp = min(1, s.amp*1.25)
    print('Server volume', s.amp)
def Minus():
    s.amp = s.amp/1.25
    print('Server volume', s.amp)
def One():
    adjustdelay(lambda t: min(10, t*1.25))
def Two():
    adjustdelay(lambda t: max(0.01, t / 1.25))

commands={"A": A, "B": B, "Up": Up, "Down": Down, "Left": Left, "Right": Right, "Plus": Plus, "Minus": Minus, "One": One, "Two": Two} # no home command

# graceful exit by stopping server
def stop_twiirl(p, code=None):
    p.wait()
    if not isinstance(code, int):
        code = p.returncode
    print('Stopping Twiirl / Pyo...')
    s.shutdown()
    print('Thanks for using Twiirl!')
    sys.exit(code)

# start the server

s.start()

# if the wiimote does not connect then twiirl will stop
# also makes running twiirl a lot easier
# stdout = PIPE means that if python quits then twiirl also quits
# bufsize = 1 forces unbuffered input, aka process each line individually
p = Popen([PATH_TO_TWIIRL_X], stdout=PIPE, bufsize=1, text=True)
print('Please connect the Wii remote to Twiirl in 30 seconds')
try:
    frq = 0
    for line in p.stdout:
        try:
            floats = [float(i) for i in line.split()]
            # pad array to size 8, in case there is no wii motion plus
            # this is the case for windows, if pairing is unsuccessful
            floats += [0] * (8 - len(floats))
            # mroll/mpitch/myaw are similar to acceleration but not exactly the same
            roll, aroll, pitch, apitch, yaw, mroll, mpitch, myaw = floats

            # rotate wiimote in axis of 1&2 buttons
            # sound smoothly rotates at the -180/180 boundary: use abs(pitch) or abs(roll)
            frq = abs(pitch)/180*1000+500
            # save cpu by checking if sound is outputting first

            if saw.isOutputting():
                saw_detune.value = abs(roll)/180
                saw_f.value = frq
            # saw_balance.value = abs(roll+pitch)/360

            # steering wheel motion, as in mariokart
            frq += myaw*5 # pitch and myaw together
            if sin.isOutputting():
                sin_f.value = frq

            if square.isOutputting():
                square_f.value = abs(mroll)*0.4+abs(mpitch)*0.4+abs(myaw)*0.4
                square_sharp.value = abs(roll)/180

            if triangle.isOutputting():
                # scale motion, where sound dips at the -180/180 boundary
                # rotate wiimote in axis of +&- buttons
                triangle_f.value = (roll + 180)/360*1000 + 500
                triangle_sharp.value = (abs(roll + pitch)/360 + 1)/2

            if melody.isOutputting():
                vals = [int(abs(roll)), int(abs(pitch))]
                melody.freq.setRange(min(vals), max(vals))

            if chaos.isOutputting():
                chaos_add.value = abs(roll+pitch+yaw) + abs(mroll) + abs(mpitch) - 20
                chaos_chaos.value = 1/(10*abs(myaw)+0.001)

        except (TypeError, ValueError): # line is not series of numbers
            cmd = commands.get(line.strip())
            if cmd:
                cmd()
            else:
                print(line)

except KeyboardInterrupt:
    stop_twiirl(p, 0)

stop_twiirl(p)
