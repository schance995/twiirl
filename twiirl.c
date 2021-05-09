/*
 * Twiirl
 *
 * Written By:
 * Skylar Chan
 * 
 *  wiiuse
 *
 *  Written By:
 *      Michael Laforest    < para >
 *      Email: < thepara (--AT--) g m a i l [--DOT--] com >
 *
 *  Copyright 2006-2007
 *
 *  This file is part of wiiuse.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  $Header$
 *
 */

/**
 *  @file
 *
 *  @brief Twiirl.
 *
 *  This file is the Twiirl Source Code.
 */

#include <stdio.h>                      /* for printf */
/* #include <string.h> */
#include <stdbool.h> 

#include "wiiuse.h"                     /* for wiimote_t, classic_ctrl_t, etc */

#ifndef WIIUSE_WIN32
#include <unistd.h>                     /* for usleep */
#endif

#define MAX_WIIMOTES 4
#define TIMEOUT 5

#define NUM_BUTTONS 10


void handle_ctrl_status(struct wiimote_t* wm);
// hex values are integers, macro substitution
// a fake dictionary
const int buttons[NUM_BUTTONS] = {WIIMOTE_BUTTON_A, WIIMOTE_BUTTON_B, WIIMOTE_BUTTON_UP, WIIMOTE_BUTTON_DOWN, WIIMOTE_BUTTON_LEFT, WIIMOTE_BUTTON_RIGHT, WIIMOTE_BUTTON_PLUS, WIIMOTE_BUTTON_MINUS, WIIMOTE_BUTTON_ONE, WIIMOTE_BUTTON_TWO}; // , WIIMOTE_BUTTON_HOME}; // omitted due to special function
const char *labels[NUM_BUTTONS] = {"A", "B", "Up", "Down", "Left", "Right", "Plus", "Minus", "One", "Two"}; // , "Home"}; // omitted due to special function

// home is treated separately because apparently is_just_pressed can only report that the button was pressed once per loop

bool motion_on = false;

/**
 *  @brief Callback that handles an event.
 *
 *  @param wm       Pointer to a wiimote_t structure.
 *
 *  This function is called automatically by the wiiuse library when an
 *  event occurs on the specified wiimote.
 */
void handle_event(struct wiimote_t* wm) {
    /* printf("\n\n--- Wiimote %i ---\n", wm->unid); */
    /* if a button is pressed, report it */
    
    for (int i = 0; i < NUM_BUTTONS; i ++) {
//        if (IS_HELD(wm, buttons[i])) {
//            printf("%s held\n", labels[i]);
//        } else
        if (IS_JUST_PRESSED(wm, buttons[i])) {
            printf("%s\n", labels[i]);
        }
    }

    // toggle motion
    if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_HOME)) {
        printf("Home\n");
//        handle_ctrl_status(wm);
        motion_on = !motion_on;
        if (motion_on) {
            wiiuse_motion_sensing(wm, 1);
            if (WIIUSE_USING_EXP(wm)) {
                wiiuse_set_motion_plus(wm, 2);    // nunchuck pass-through
            } else {
                wiiuse_set_motion_plus(wm, 1);    // standalone
            }
        } else {
            wiiuse_motion_sensing(wm, 0);
            wiiuse_set_motion_plus(wm, 0); // off
        }
    }
    
    /* if the accelerometer is turned on then print angles */
    if (WIIUSE_USING_ACC(wm)) {
        // ranges from -180 to 180 degrees
        printf("%f %f %f %f %f", wm->orient.roll, wm->orient.a_roll, wm->orient.pitch, wm->orient.a_pitch, wm->orient.yaw);
        // only prints when there are changes


        if (wm->exp.type == EXP_MOTION_PLUS || wm->exp.type == EXP_MOTION_PLUS_NUNCHUK) {
            printf(" ");
            printf("%f %f %f",
                   wm->exp.mp.angle_rate_gyro.pitch,
                   wm->exp.mp.angle_rate_gyro.roll,
                   wm->exp.mp.angle_rate_gyro.yaw);
        }
	printf("\n");
    }
    // prints continuously
    
    // output to parse is roll, absolute roll, pitch, absolute pitch, yaw, gryo pitch, gyro roll, gyro yaw
}


/**
 *  @brief Callback that handles a controller status event.
 *
 *  @param wm               Pointer to a wiimote_t structure.
 *  @param attachment       Is there an attachment? (1 for yes, 0 for no)
 *  @param speaker          Is the speaker enabled? (1 for yes, 0 for no)
 *  @param ir               Is the IR support enabled? (1 for yes, 0 for no)
 *  @param led              What LEDs are lit.
 *  @param battery_level    Battery level, between 0.0 (0%) and 1.0 (100%).
 *
 *  This occurs when either the controller status changed
 *  or the controller status was requested explicitly by
 *  wiiuse_status().
 *
 *  One reason the status can change is if the nunchuk was
 *  inserted or removed from the expansion port.
 */
void handle_ctrl_status(struct wiimote_t* wm) {
    printf("\n\n--- CONTROLLER STATUS [wiimote %i] ---\n", wm->unid);

    printf("attachment:      %i\n", wm->exp.type);
    printf("speaker:         %i\n", WIIUSE_USING_SPEAKER(wm));
    printf("ir:              %i\n", WIIUSE_USING_IR(wm));
    printf("leds:            %i %i %i %i\n", WIIUSE_IS_LED_SET(wm, 1), WIIUSE_IS_LED_SET(wm, 2), WIIUSE_IS_LED_SET(wm, 3), WIIUSE_IS_LED_SET(wm, 4));
    printf("battery:         %f %%\n", wm->battery_level);
}


/**
 *  @brief Callback that handles a disconnection event.
 *
 *  @param wm               Pointer to a wiimote_t structure.
 *
 *  This can happen if the POWER button is pressed, or
 *  if the connection is interrupted.
 */
void handle_disconnect(wiimote* wm) {
    printf("\n\n--- DISCONNECTED [wiimote id %i] ---\n", wm->unid);
}

short any_wiimote_connected(wiimote** wm, int wiimotes) {
    int i;
    if (!wm) {
        return 0;
    }

    for (i = 0; i < wiimotes; i++) {
        if (wm[i] && WIIMOTE_IS_CONNECTED(wm[i])) {
            return 1;
        }
    }

    return 0;
}


/**
 *  @brief main()
 *
 *  Connect to up to two wiimotes and print any events
 *  that occur on either device.
 */
int main(int argc, char** argv) {
    wiimote** wiimotes;
    int found, connected;

    /*
     *  Initialize an array of wiimote objects.
     *
     *  The parameter is the number of wiimotes I want to create.
     */
    // printf("Thanks to WiiUse for their GPL3 library\n");
    wiimotes =  wiiuse_init(MAX_WIIMOTES);

    /*
     *  Find wiimote devices
     *
     *  Now we need to find some wiimotes.
     *  Give the function the wiimote array we created, and tell it there
     *  are MAX_WIIMOTES wiimotes we are interested in.
     *
     *  Set the timeout to be 5 seconds.
     *
     *  This will return the number of actual wiimotes that are in discovery mode.
     */
    printf("You are running Twiirl v0\n"
           "Please connect the Wiimote to Twiirl\n");
    found = wiiuse_find(wiimotes, MAX_WIIMOTES, TIMEOUT);
    if (!found) {
        printf("No wiimotes found.\n"
                       "Make sure to press the pairing button during Twiirl's startup.\n");
        return 1;
    }

    /*
     *  Connect to the wiimotes
     *
     *  Now that we found some wiimotes, connect to them.
     *  Give the function the wiimote array and the number
     *  of wiimote devices we found.
     *
     *  This will return the number of established connections to the found wiimotes.
     */
    connected = wiiuse_connect(wiimotes, MAX_WIIMOTES);
    if (connected) {
        printf("Connected to %i wiimotes (of %i found).\n", connected, found);
    } else {
        printf("Failed to connect to any wiimote.\n");
        return 1;
    }

    /*
     *  Now set the LEDs and rumble for a second so it's easy
     *  to tell which wiimotes are connected (just like the wii does).
     */
    wiiuse_set_leds(wiimotes[0], WIIMOTE_LED_1);
    wiiuse_set_leds(wiimotes[1], WIIMOTE_LED_2);
    wiiuse_set_leds(wiimotes[2], WIIMOTE_LED_3);
    wiiuse_set_leds(wiimotes[3], WIIMOTE_LED_4);
        for (int i=0; i<MAX_WIIMOTES; i++) {
            wiiuse_rumble(wiimotes[i], 1);
        }

        for (int i=0; i<MAX_WIIMOTES; i++) {
            wiiuse_rumble(wiimotes[i], 0);
        }

    printf("\nControls:\n");
    printf("\tPress the home button to start and stop motion reporting\n");
    printf("\n\n");

    /*
     *  Maybe I'm interested in the battery power of the 0th
     *  wiimote.  This should be WIIMOTE_ID_1 but to be sure
     *  you can get the wiimote associated with WIIMOTE_ID_1
     *  using the wiiuse_get_by_id() function.
     *
     *  A status request will return other things too, like
     *  if any expansions are plugged into the wiimote or
     *  what LEDs are lit.
     */
    wiiuse_status(wiimotes[0]);

    /*
     *  This is the main loop
     *
     *  wiiuse_poll() needs to be called with the wiimote array
     *  and the number of wiimote structures in that array
     *  (it doesn't matter if some of those wiimotes are not used
     *  or are not connected).
     *
     *  This function will set the event flag for each wiimote
     *  when the wiimote has things to report.
     */

    while (any_wiimote_connected(wiimotes, MAX_WIIMOTES)) {
        // occurs every time
        /*
        for (int i = 0; i < MAX_WIIMOTES; ++i) {
                switch (wiimotes[i]->event) {
                    case WIIUSE_EVENT:
                        handle_always(wiimotes[i]);
                        break;
                    default:
                        break;
                }
        }
        */
        if (wiiuse_poll(wiimotes, MAX_WIIMOTES)) {
            /*
             *  This happens if something happened on any wiimote.
             *  So go through each one and check if anything happened.
             */
            for (int i = 0; i < MAX_WIIMOTES; ++i) {
                switch (wiimotes[i]->event) {
                    case WIIUSE_EVENT:
                        /* a generic event occurred */
                        handle_event(wiimotes[i]);
                        break;

                    case WIIUSE_STATUS:
                        /* a status event occurred */
                        handle_ctrl_status(wiimotes[i]);
                        break;

                    case WIIUSE_DISCONNECT:
                    case WIIUSE_UNEXPECTED_DISCONNECT:
                        /* the wiimote disconnected */
                        handle_disconnect(wiimotes[i]);
                        break;

                    case WIIUSE_MOTION_PLUS_ACTIVATED:
                        /* printf("Motion+ was activated\n"); */
                        break;

                    case WIIUSE_MOTION_PLUS_REMOVED:
                        /* some expansion was removed */
                        handle_ctrl_status(wiimotes[i]);
                        printf("An expansion was removed.\n");
                        break;

                    default:
                        break;
                }
            }
        }
    }


    /*
     *  Disconnect the wiimotes
     */
    printf("Stopping Twiirl / Wiiuse...\n");
    wiiuse_cleanup(wiimotes, MAX_WIIMOTES);

    return 0;
}
