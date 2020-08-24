#include "render_util.h"

GLFWwindow * gfx_init(size_t window_width, size_t window_height)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(
        window_width, window_height, "OpenGL Window", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // glfwSetMouseButtonCallback(window, RedirectionMouseCallback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    return window;
}