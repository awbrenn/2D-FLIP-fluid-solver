#FLIP 2D Particle Simulation

Author:         Austin Brennan
University:     Clemson University
Course:         2D Fluid Simulation
Professor:      Dr. Jerry Tessendorf
Due Date:       4/7/2016


##Purpose:

The purpose of this assignment is to get a better understanding of how
smoothed particle hydrodynmics (SPH) approach to fluid simulation / particle
dynamics works. This program simulates SPH in two dimensions for the


##Extra Features:

I have included several extra features on top of the project requirements.
These include a party mode that can be activated with the command line flag
-party_mode. It can also be toggled on and off with the 'p' key.

I also made the w/a/s/d keys switch the direction of gravity to
up/left/down/right respectively.

Another feature I added is a dampening factor on particle bounces. This is
a scalar multiplier, which reduces or increases the output velocity of a
particle after it collides with a boundary. Try increasing and decreasing
the percentage of energy retained on a bounce with the ,/. keys.

I encourage you to try playing with this features, and see how they effect
the simulation.


##Usage:
`$> ./sph_fluid_simulator [-update_method (LF or S for leap frog or sixth)] [-output_path output_path]
                      [-write_on_start 0 or 1 for off or on]  [-party_mode (0 or 1 for off or on)]`

The flags can be set in any order. And have default values of:
-update_method: S
-output_path: ./output_path
-output
-party_mode: 0 (off)


sph_fluid_simulator keyboard choices
./,      increase/decrease % of energy retained after bounce
p        turn on party mode. Randomizes particle color on collison
w/a/s/d  switches gravity to point up/left/down/right
spacebar paused the simulation. pressing it again un-pauses the simulation
o        toggle writing to output_path
q        exits the program
