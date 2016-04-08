#FLIP 2D Particle Simulation

Author:         Austin Brennan  
University:     Clemson University  
Course:         2D Fluid Simulation  
Professor:      Dr. Jerry Tessendorf  
Due Date:       4/7/2016  


##Purpose:
The purpose of this assignment is to get a better understanding of how a
fluid solver with implicit particles (FLIP) approach to fluid simulation / particle
dynamics works.


##Pitfalls:
This project has some major problems that I was not able to resolve. I will hopefully resolve them
one day, but for now I need to move onto other things.


##Build
`$> ./build`

##Usage:
`$> ./FLIP_fluid_simulator [-update_method (LF or S for leap frog or sixth)] [-output_path output_path]
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
