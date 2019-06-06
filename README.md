# GAME ENGINE 
Its a single threaded engine which supports DirectX 11 and OpenGL apis for rendering

# BUILD STEPS
1. Clone or download the repo
2. Open .sln file using visual studio 2019
3. Open the property page for the My Game project (set the project as start up if not automatically set)
4. For all configs and platforms, go to debugging tab 
  * Change **Working Directory** to **$(GameDir)**
5. Build **BuildAllAssets**
6. Run **MyGame**

# CONTROLS
WASD to move the third person character
c/v to change camera from third person to fly cam
TFGH to rotate the fly cam and arrow keys to move the fly cam
` to access debug menu
arrow keys to move up and down in options and debug menu
debug menu has fps, network type and slider to adjust backgroung music and effects volume
arrow keys and enter should work in debug menu


THE FPS IS SO LOW CAUSE I BRUTE FORCED COLLISION CHECKING
