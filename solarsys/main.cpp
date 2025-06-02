#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Mini Solar System", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Sun shaders (glow/pulse)
    const char* sunVertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        uniform mat4 uTransform;
        out vec2 vPos;
        void main() {
            vPos = aPos;
            gl_Position = uTransform * vec4(aPos, 0.0, 1.0);
        }
    )";
    const char* sunFragmentShaderSource = R"(
        #version 330 core
        in vec2 vPos;
        out vec4 FragColor;
        uniform float uTime;
        void main() {
            float dist = length(vPos);
            float pulse = 0.8 + 0.2 * sin(uTime * 2.0);
            float core = smoothstep(0.0, 0.13 * pulse, 0.13 * pulse - dist);
            float glow = smoothstep(0.14 * pulse, 0.38 * pulse, dist);
            float halo = smoothstep(0.38 * pulse, 0.55 * pulse, dist);
            vec3 sunColor = mix(vec3(1.0, 1.0, 0.0), vec3(1.0, 0.8, 0.2), dist * 2.0);
            vec3 glowColor = vec3(1.0, 0.9, 0.3);
            vec3 haloColor = vec3(1.0, 0.95, 0.6);
            vec3 finalColor = sunColor * core + glowColor * glow * 0.7 + haloColor * halo * 0.3;
            float alpha = core + glow * 0.7 + halo * 0.3;
            FragColor = vec4(finalColor, alpha);
        }
    )";

    // Planet/Moon shaders (radial gradient)
    const char* planetVertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        uniform mat4 uTransform;
        out vec2 vPos;
        void main() {
            vPos = aPos;
            gl_Position = uTransform * vec4(aPos, 0.0, 1.0);
        }
    )";
    const char* planetFragmentShaderSource = R"(
        #version 330 core
        in vec2 vPos;
        out vec4 FragColor;
        uniform vec3 uColor;
        void main() {
            float dist = length(vPos);
            float grad = 1.0 - smoothstep(0.0, 1.0, dist);
            vec3 color = mix(uColor * 0.5, uColor, grad);
            FragColor = vec4(color, 1.0);
        }
    )";

    // Compile and link Sun shader program
    GLuint sunVS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sunVS, 1, &sunVertexShaderSource, nullptr);
    glCompileShader(sunVS);
    GLuint sunFS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sunFS, 1, &sunFragmentShaderSource, nullptr);
    glCompileShader(sunFS);
    GLuint sunProgram = glCreateProgram();
    glAttachShader(sunProgram, sunVS);
    glAttachShader(sunProgram, sunFS);
    glLinkProgram(sunProgram);
    glDeleteShader(sunVS);
    glDeleteShader(sunFS);

    // Compile and link Planet/Moon shader program
    GLuint planetVS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(planetVS, 1, &planetVertexShaderSource, nullptr);
    glCompileShader(planetVS);
    GLuint planetFS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(planetFS, 1, &planetFragmentShaderSource, nullptr);
    glCompileShader(planetFS);
    GLuint planetProgram = glCreateProgram();
    glAttachShader(planetProgram, planetVS);
    glAttachShader(planetProgram, planetFS);
    glLinkProgram(planetProgram);
    glDeleteShader(planetVS);
    glDeleteShader(planetFS);

    // Uniform locations
    GLint sun_uTransformLoc = glGetUniformLocation(sunProgram, "uTransform");
    GLint sun_uTimeLoc = glGetUniformLocation(sunProgram, "uTime");
    GLint planet_uTransformLoc = glGetUniformLocation(planetProgram, "uTransform");
    GLint planet_uColorLoc = glGetUniformLocation(planetProgram, "uColor");

    // Generate circle vertex data (triangle fan)
    const int numSegments = 100;
    float radius = 0.15f;
    float circleVertices[2 * (numSegments + 2)];
    circleVertices[0] = 0.0f; // Center x
    circleVertices[1] = 0.0f; // Center y
    for (int i = 0; i <= numSegments; ++i) {
        float angle = 2.0f * 3.1415926f * float(i) / float(numSegments);
        circleVertices[2 * (i + 1)] = radius * cosf(angle);
        circleVertices[2 * (i + 1) + 1] = radius * sinf(angle);
    }

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Helper: multiply 4x4 matrices (column-major)
    auto mat4_mult = [](const float* a, const float* b, float* out) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                out[j * 4 + i] = 0.0f;
                for (int k = 0; k < 4; ++k)
                    out[j * 4 + i] += a[k * 4 + i] * b[j * 4 + k];
            }
        }
    };
    // Helper: 2D rotation matrix (around Z)
    auto mat4_rotation_z = [](float angle, float* m) {
        float c = cosf(angle), s = sinf(angle);
        m[0] = c;   m[1] = s;   m[2] = 0; m[3] = 0;
        m[4] = -s;  m[5] = c;   m[6] = 0; m[7] = 0;
        m[8] = 0;   m[9] = 0;   m[10]= 1; m[11]= 0;
        m[12]= 0;   m[13]= 0;   m[14]= 0; m[15]= 1;
    };
    // Helper: scale matrix
    auto mat4_scale = [](float s, float* m) {
        m[0] = s; m[1] = 0; m[2] = 0; m[3] = 0;
        m[4] = 0; m[5] = s; m[6] = 0; m[7] = 0;
        m[8] = 0; m[9] = 0; m[10]= 1; m[11]= 0;
        m[12]= 0; m[13]= 0; m[14]= 0; m[15]= 1;
    };
    // Helper: translation matrix
    auto mat4_translate = [](float x, float y, float* m) {
        m[0] = 1; m[1] = 0; m[2] = 0; m[3] = 0;
        m[4] = 0; m[5] = 1; m[6] = 0; m[7] = 0;
        m[8] = 0; m[9] = 0; m[10]= 1; m[11]= 0;
        m[12]= x; m[13]= y; m[14]= 0; m[15]= 1;
    };

    // Camera state
    struct CameraState {
        float angle = 0.0f;
        float zoom = 1.0f;
    } cam = {};
    double lastX = 0.0, lastY = 0.0;
    bool dragging = false;

    // Mouse callbacks
    glfwSetWindowUserPointer(window, &cam);
    glfwSetScrollCallback(window, [](GLFWwindow* win, double xoffset, double yoffset) {
        CameraState* camPtr = (CameraState*)glfwGetWindowUserPointer(win);
        if (yoffset > 0)
            camPtr->zoom *= 1.1f;
        else
            camPtr->zoom *= 0.9f;
        if (camPtr->zoom < 0.2f) camPtr->zoom = 0.2f;
        if (camPtr->zoom > 5.0f) camPtr->zoom = 5.0f;
    });

    // Helper: view matrix (rotation + zoom)
    auto mat4_view = [](float angle, float zoom, float* m) {
        float c = cosf(angle), s = sinf(angle);
        m[0] = c * zoom; m[1] = s * zoom; m[2] = 0; m[3] = 0;
        m[4] = -s * zoom; m[5] = c * zoom; m[6] = 0; m[7] = 0;
        m[8] = 0; m[9] = 0; m[10] = 1; m[11] = 0;
        m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
    };

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAO);
        float time = (float)glfwGetTime();

        // Handle mouse drag for camera rotation
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            if (!dragging) { dragging = true; lastX = x; }
            float dx = (float)(x - lastX);
            cam.angle += dx * 0.005f;
            lastX = x;
        } else {
            dragging = false;
        }

        float view[16];
        mat4_view(cam.angle, cam.zoom, view);

        // Draw Sun (center, with glow/pulse)
        float sunTransform[16] = {
            1.0f, 0,    0, 0,
            0,    1.0f, 0, 0,
            0,    0,    1, 0,
            0.0f, 0.0f, 0, 1
        };
        float sunView[16];
        mat4_mult(view, sunTransform, sunView);
        glUseProgram(sunProgram);
        glUniformMatrix4fv(sun_uTransformLoc, 1, GL_FALSE, sunView);
        glUniform1f(sun_uTimeLoc, time);
        glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments + 2);

        // Draw Planet 1 (orbits Sun, rotates)
        float planet1Dist = 0.5f;
        float planet1Angle = time * 0.7f;
        float planet1X = cosf(planet1Angle) * planet1Dist;
        float planet1Y = sinf(planet1Angle) * planet1Dist;
        float planet1RotAngle = time * 2.0f; // self-rotation
        float planet1Scale[16], planet1Rot[16], planet1Trans[16], planet1SR[16], planet1SRT[16], planet1View[16];
        mat4_scale(0.3f, planet1Scale);
        mat4_rotation_z(planet1RotAngle, planet1Rot);
        mat4_translate(planet1X, planet1Y, planet1Trans);
        mat4_mult(planet1Rot, planet1Scale, planet1SR);
        mat4_mult(planet1Trans, planet1SR, planet1SRT);
        mat4_mult(view, planet1SRT, planet1View);
        glUseProgram(planetProgram);
        glUniformMatrix4fv(planet_uTransformLoc, 1, GL_FALSE, planet1View);
        glUniform3f(planet_uColorLoc, 0.2f, 0.6f, 1.0f); // Blue
        glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments + 2);

        // Draw Planet 2 (orbits Sun, rotates)
        float planet2Dist = 0.8f;
        float planet2Angle = time * 1.2f;
        float planet2X = cosf(planet2Angle) * planet2Dist;
        float planet2Y = sinf(planet2Angle) * planet2Dist;
        float planet2RotAngle = time * 1.5f;
        float planet2Scale[16], planet2Rot[16], planet2Trans[16], planet2SR[16], planet2SRT[16], planet2View[16];
        mat4_scale(0.18f, planet2Scale);
        mat4_rotation_z(planet2RotAngle, planet2Rot);
        mat4_translate(planet2X, planet2Y, planet2Trans);
        mat4_mult(planet2Rot, planet2Scale, planet2SR);
        mat4_mult(planet2Trans, planet2SR, planet2SRT);
        mat4_mult(view, planet2SRT, planet2View);
        glUniformMatrix4fv(planet_uTransformLoc, 1, GL_FALSE, planet2View);
        glUniform3f(planet_uColorLoc, 0.8f, 0.4f, 0.1f); // Orange
        glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments + 2);

        // Draw Moon (orbits Planet 1, rotates)
        float moonDist = 0.18f;
        float moonAngle = time * 2.5f;
        float moonX = planet1X + cosf(moonAngle) * moonDist;
        float moonY = planet1Y + sinf(moonAngle) * moonDist;
        float moonRotAngle = time * 3.0f;
        float moonScale[16], moonRot[16], moonTrans[16], moonSR[16], moonSRT[16], moonView[16];
        mat4_scale(0.08f, moonScale);
        mat4_rotation_z(moonRotAngle, moonRot);
        mat4_translate(moonX, moonY, moonTrans);
        mat4_mult(moonRot, moonScale, moonSR);
        mat4_mult(moonTrans, moonSR, moonSRT);
        mat4_mult(view, moonSRT, moonView);
        glUniformMatrix4fv(planet_uTransformLoc, 1, GL_FALSE, moonView);
        glUniform3f(planet_uColorLoc, 0.9f, 0.9f, 0.9f); // Light gray
        glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments + 2);

        glBindVertexArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(sunProgram);
    glDeleteProgram(planetProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
} 