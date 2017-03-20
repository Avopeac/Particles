# Particles
The ability to paint meshes as a result of particle collisions adds detail to and leaves a lasting impact on a scene. With the use of screen space collision detection and generating a mapping between the coordinates of mesh vertices in screen space to texture coordinate space, paint results can be written to a texture and used when rendering the mesh. Hundreds of meshes can be painted in real-time, independent of the amount of particle collisions, making the method suitable for real-time graphics applications such as 3D games.

To get the example running (on Windows 10 64-bit):

1. Build with Visual Studio (2015).
2. Copy folders "Textures", "Models" and "Shaders" to the build directory.
3. Copy DLLs inside "Binaries" folder to the build directory.
4. Run executable.

Things shown in the example:
* Pressing SPACE will cause a blood effect to emit in front of the camera, painting intersecting meshes.
* Two spline interpolated fire effects roam across the floor in the Sponza scene.

Quirks and drawbacks:
* Camera is poorly implemented, making it feel like there is frame rate drops from time to time.
* Painting on objects with bad triangle / UV ratio will cause stretching of the applied paint.
* Painting on objects with triangles sharing UV coordinates will cause mirroring of the applied paint.
