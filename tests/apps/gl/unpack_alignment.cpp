/*
 * Client-memory pixel uploads with the default GL_UNPACK_ALIGNMENT (4), where
 * the rows are not a multiple of 4 bytes long.
 *
 * The traced blobs must cover the padding between rows (and not just the
 * tightly packed pixels), as drivers are free to read it, so replaying an
 * undersized blob can read past the end of the replayed memory.
 */

#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>


int main(int argc, char** argv)
{
    glfwInit();

    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

    GLFWwindow* window = NULL;
    window = glfwCreateWindow(32, 32, argv[0], NULL, NULL);
    if (!window) {
        return EXIT_SKIP;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        return EXIT_FAILURE;
    }

    // 2x2 RGB: rows are 6 bytes, padded to 8 -> 8 + 6 = 14 bytes.
    static const GLubyte pixels[16] = {
        255, 0, 0,   0, 255, 0,   0, 0,
        0, 0, 255,   255, 255, 255,   0, 0
    };

    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // 1x32 RGB: rows are 3 bytes, padded to 4 -> 31*4 + 3 = 127 bytes.
    static GLubyte column[128];
    glTexImage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, column);

    glDeleteTextures(1, &texture);

    glfwSwapBuffers(window);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
