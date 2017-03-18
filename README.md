# Particles
The ability to paint meshes as a result of particle collisions adds detail to and leaves a lasting impact on a scene. With the use of screen space collision detection and generating a mapping between the coordinates of mesh vertices in screen space to texture coordinate space, paint results can be written to a texture and used when rendering the mesh. Hundreds of meshes can be painted in real-time, independent of the amount of particle collisions, making the method suitable for real-time graphics applications such as 3D games.