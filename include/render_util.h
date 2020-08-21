#pragma once

#include <stdlib.h>

// rendering dependencies
#include "glad/glad.h"
#include "GLFW/glfw3.h"

GLFWwindow * gfx_init()
{
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(
        512, 512, "OpenGL Window", NULL, NULL);
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