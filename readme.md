The Walking Game
----------------

![](/documentation/user/screenshot1.png)

Our computer graphics lecturer asked us to make a walking simulation using OpenGL. This is an OpenGL 4.3 application using only low level libraries:
1. GLEW for OpenGL loader
2. GLFW for windowing and OpenGL context creation
3. GLM for mathematics.

We didn't use game engine like Unity and whatnot. We made our own engine.

Features include: model loading using OBJ Wavefront, third-person camera, Blinn-Phong shader, walking animation, simple collision handler, nameplate rendering, and a scripting system running on a different thread. 

See documentation/user/main.pdf for more information.

Console commands
----------------

```C++
help
exit
showallnames
createnpc [string:name] [int:model_id]
createnpc [string:name] [int:model_id] [float:x] [float:y] [float:z]
deletenpc [string:name]
setmodel [string:name] [int:model_id]
getmodelinfo [string:name]
setwalkingspeed [string:name] [float:speed]
getwalkingspeed [string:name]
setposition [string:name] [float:x] [float:y] [float:z]
getposition [string:name]
setangle [string:name] [float:angle]
getangle [string:name]
setanimationstate [string:name] [int:state]
getanimationstate [string:name]
takecontrol [string:name]
getcamerainfo
setcamerapos [float:x] [float:y] [float:z]
setcameratarget [float:x] [float:y] [float:z]
setcameraradius [float:radius]
getlightinfo
setlightpos [float:x] [float:y] [float:z]
setlightintensity [float:r] [float:g] [float:b]
setfpslimit [int:fps]
sleep [double:seconds] -- pause (for scripting)
loadscript [string:path] -- the file must be in ASCII encoding (sample script can be found in /binary/testscript.txt)
test -- for testing purposes
```

![](/documentation/user/screenshot2.png)

![](/documentation/user/screenshot3.png)

Control
-------

Input                 | Action
----------------------|----------------------------------
Esc                   |  Exit the program
W, S                  |  Move forward/backward
A, D                  |  Turn left/right
Left, right           |  Pan camera
Up, down              |  Tilt camera
Page down, page up    |  Dolly camera
Mouse wheel           |  Dolly camera
Left click and drag   |  Pan and tilt camera
Right click and drag  |  Pan and tilt camera and turn
