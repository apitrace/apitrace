/*
 * glDrawRangeElements with client-memory vertex arrays, where the declared
 * range (end) goes beyond the highest index actually referenced.
 *
 * Drivers honour end and may read vertices up to it, so the traced arrays
 * must cover end + 1 vertices, or replaying them can read past the end of the
 * replayed memory.
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

    // Only 3 vertices are used, but 4 are within the range.
    static const float vertices[4][3] = {
        { -0.9f, -0.9f, 0.0f },
        {  0.9f, -0.9f, 0.0f },
        {  0.0f,  0.9f, 0.0f },
        {  0.0f,  0.0f, 0.0f }
    };

    static const GLuint indices[3] = { 0, 1, 2 };

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawRangeElements(GL_TRIANGLES, 0, 3, 3, GL_UNSIGNED_INT, indices);

    glfwSwapBuffers(window);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
