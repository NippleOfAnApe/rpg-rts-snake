# My RPG Snake strategy in real time

### Here I will be documenting all the changes about how to write a full snake game with modern days graphics and features using raylib
___
## Patch 1
![GIF](resources/snake1.gif)
<img src="resources/snake1.gif" alt="img" align="center" width="75%">

+ Textures for background and foreground
+ 2 types of fruits
+ Zoom in/out
+ Camera follows player but clamps to edges when near walls
+ Project is divided into smaller files
## Patch 2
![GIF](resources/snake2.gif)
<img src="resources/snake2.gif" alt="img" align="left" width="50%">
+ Smoother update timing
+ Circles insted of squares
+ Snakes follows mouse pointer
+ No cells
+ Different color body parts
+ Render from tail to head e.g. head will be on top
---
### **TODO**
- [x]~~image bg outside the map~~
- [x]~~find good grass texture png files~~
- different pickups:
    + [x]~~regular plus point~~
    + [x]~~bonus 10 point~~
    + [x]~~many items at once~~
    + liited food (+ point, -tail)
    + bonus food to spawn in smaller area
    + make bonus food animated
    + boost resource collection or speed (changing screen tint)
- [x]make a head differen color and on top of a body
- [x]different color body parts
- a dropoff location where you can cut your snake but get bonus points
- add sfx & music
- make walls and shoot points to burst them or enemies
- [x]~~split project structure into different files + headers~~
- add options to choose from 3-5 different snake swith different speed and maybe later stats
- animations
- textures for snake
- easings for snake tail and camera
- ~~diagonal movement~~
- ~~arrow on top of a player that will be registering move buttons and show the final directions where it will turn.~~
- [x]get rid off cells
- [x]circles instead of squares
- [x]move snake with a mouse pointer
- control moves with left/right, movement is constant
- OR limit the turn angle
- custom mouse pointer
- make snake a dynamic array insted of static