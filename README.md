# glReadPixels_example
A example on how to use glReadPixels.

Here, we use the [glfw](http://www.glfw.org) library, but any type of [graphics library](https://www.khronos.org/opengl/wiki/Related_toolkits_and_APIs) that supports creating an OpenGL context can be used.

```cpp
typedef struct
{
    u_char head[12];
    u_short dx /* Width */, dy /* Height */, head2;
    u_char pic[768 * 1024 * 10][3];
} typetga;
typetga tga;

char captureName[256];
u_long captureNo;
u_char tgahead[12] = {0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void capture(GLFWwindow* window)
{
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight); /* Get size, store into specified variables  */
    
    /* Prepare the targa header */
    memcpy(tga.head, tgahead, 12);
    tga.dx = screenWidth;
    tga.dy = screenHeight;
    tga.head2 = 0x2018;
    
    /* Store pixels into tga.pic */
    glReadPixels(0, 0, screenWidth, screenHeight, GL_RGB, GL_UNSIGNED_BYTE, tga.pic[0]);
    
    /* Store "Capture_%04lu.tga" + captureNo into captureName, increase frame count */
    sprintf(captureName, "Capture_%04lu.tga" /* 'lu' for unsigned long */, captureNo); captureNo++;
    
    /* Write file */
    FILE* cc = fopen(captureName, "wb");
    fwrite(&tga, 1, (18 + 3 * screenWidth * screenHeight), cc);
    fclose(cc);
}
```

The rendered lossless sequence of images can then be scaled up with something like [ffmpeg](https://www.ffmpeg.org):

`ffmpeg -r 60 -f image2 -filter_complex scale=iw*3:ih*3 -i ./Capture_%04d.tga -vcodec libx264 -crf 0 -pix_fmt yuv420p encode.mp4`

A test encoding is supplied, alt. a [youtube video](https://www.youtube.com/watch?v=5WKc3bgXXVg).

Link against:
- GLFW
- GLEW
- OpenGL.framework (macOS)
