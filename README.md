## Description

Nematode.farm is a simple WASM and SDL2 based game utilizing simulations of the C. elegans nematode as the 'opponent' AI.

This project started as a visualizer/development tool to complement my work on the [nanotode](https://github.com/nategri/nanotode) C. elegans framework and the [nematoduino](https://github.com/nategri/nematoduino) robot (in fact the worms in nematode.farm follow almost exactly the same motion logic as the robot).

But when I realized I could also turn it into a dumb little game, I thought, "Why the heck not?!" And so, here we are.

## Objective

Assuming the role of the mighty Teal Worm, your objective as a player is to push all the wandering red and blue worms into their respective pens before the timer runs down. 

Compete with your friends! Compete with yourself! Just chill out and watch the damn worms! It's all up to you.

## Controls

Arrow keys move the player worm forward and backward, and steer it left and right. Escape exits to the title screen.

## Worm Behavior

"So, what are the AI worms *doing*?" is probably one of the first questions that come to mind here.

Well, most of the time the worms are having their "chemotaxis" neurons stimulated ("chemotaxis" is a fancy worm-science word for "wiggle around and look for food"), so by default they're on kind of a random walk around the screen.

But whenever they bump into a barrier, or another worm, they get their "nose touch" neurons stimulated, which should cause them to reverse direction quickly. Graphically, whenever you see a worm's "nose" light up, its nose touch neurons are being fired.

If you'd like more details on how I interpret the signals from the simulated nervous system of the worm into motion, check out the README on the [nematoduino](https://github.com/nategri/nematoduino) GitHub.

## Visualization Key

The title screen of nematode.farm gives you the option to press [V] to display a readout of the muscle/motion state of the worm on that screen.

<p align="center"><img width=500 src="/images/vis_key.jpeg"></p>
<p align="center"><b>Figure 1.</b><br><i>The visualization shows the state of each muscle in the worm, as well as the left and right components contributing to its motion, which are entirely analogous to that of the nematoduino 2-wheeled robot</i>.</p>

## Closely Related Projects by This Author

#### [nanotode](https://github.com/nategri/nanotode)
A framework for running lightweight simulations of the C. elegans nematode's nervous system. Forms the core of the simulation seen in nematode.farm.


#### [nematoduino](https://github.com/nategri/nematoduino)
An Arduino UNO-compatible robotic simulation of the C. elegans nematode. Also employs nanotode as the core of the C. elegans simulation.

## Acknowledgements and References

#### The OpenWorm Project
[http://openworm.org/](http://openworm.org/)  
*Notes: The reason any of this happened; source data for my neural net comes from their GitHub*

#### Busbice, T. 
*Article: Extending the C. Elegans Connectome to Robotics*  
[https://goo.gl/pxavvY](https://goo.gl/pxavvY)  
*Notes: The project that got me excited about C. elegans in the first place!*

#### Busbice, T., Garrett, G., Churchill G.
*GitHub: GoPiGo C. elegans Connectome Code*  
[https://github.com/Connectome/GoPiGo](https://github.com/Connectome/GoPiGo)  
*Notes: The jumping-off point for my work*

#### Meroni, C.
*YouTube video: C. elegans brain and body simulation*
[https://www.youtube.com/watch?v=WAYeeMcu1tY](https://www.youtube.com/watch?v=WAYeeMcu1tY)  
*Notes: Inspired me to make my own similar project. The idea to show muscle states was wonderful! Thank you, sir!*
