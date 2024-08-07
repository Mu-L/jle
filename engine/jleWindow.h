/*********************************************************************************************
 *                                                                                           *
 *               ,     .     ,                      .   ,--.                                 *
 *               |     |     |                      |   |            o                       *
 *               | ,-. |- -- |    ,-: ,-: ,-: ,-. ,-|   |-   ;-. ,-: . ;-. ,-.               *
 *               | |-' |     |    | | | | | | |-' | |   |    | | | | | | | |-'               *
 *              -' `-' `-'   `--' `-` `-| `-| `-' `-'   `--' ' ' `-| ' ' ' `-'               *
 *                                                                                           *
 *     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~     *
 *          Jet-Lagged Engine (jle) is licenced under GNU General Public License v3.0.       *
 *     The licence can be found here: https://github.com/Mormert/jle/blob/master/LICENSE     *
 *                  Copyright (c) 2020-2024 Johan Lind. All rights reserved.                 *
 *                                                                                           *
 *********************************************************************************************/

#pragma once

#include "jleCommon.h"

#include "jleWindowSettings.h"
#include "jleIncludeGL.h"

#include <map>
#include <memory>

#ifdef __APPLE__
void customizeTitleBarMacOS(GLFWwindow* window);
#endif

struct jleWindowResizeEvent{
    // Main framebuffer dimensions
    int framebufferWidth;
    int framebufferHeight;

    // The physical size of the monitor in millimeters
    int monitorPhysicalSizeWidth;
    int monitorPhysicalSizeHeight;

    float dpiWidth;
    float dpiHeight;

    // The content scale is the ratio between the current DPI and the platform's default DPI
    float contentScaleX;
    float contentScaleY;
};

class jleWindow {
public:
    ~jleWindow();

    static void error_callback(int error, const char *description);

    static void glfwKeyCallback(
        GLFWwindow *window, int key, int scancode, int action, int mods);

    static void glfwScrollCallback(GLFWwindow *window,
                                   double xoffset,
                                   double yoffset);

    static void glfwFramebufferSizeCallback(GLFWwindow *window, int width, int height);

    void settings(WindowSettings &windowSettings);

    void displayCursor(bool enable);

    bool isCursorDisplayed() const;

    void setCursorPosition(int x, int y);

    [[nodiscard]] unsigned int height() const;

    [[nodiscard]] unsigned int width() const;

    void initWindow();

    unsigned int addWindowResizeCallback(std::function<void(const jleWindowResizeEvent& resizeEvent)> callback);

    void removeWindowResizeCallback(unsigned int callback_id);

    void executeResizeCallbacks(const jleWindowResizeEvent& resizeEvent);

    void updateWindow();

    bool windowShouldClose();

    float time();

    bool mouseClick(int button);

    bool key(int key);

    bool keyPressed(int key);

    bool keyReleased(int key);

    float scrollX();

    float scrollY();

    std::pair<int, int> cursor();

    GLFWwindow *
    glfwWindow();

    static GLFWwindow *initGlfwWindow(int width, int height, const char *title);

protected:
    GLFWwindow *_glfwWindow;
    static inline jleWindow *_activeWindow;

    WindowSettings windowSettings;

    float currentScrollX;
    float currentScrollY;

    bool cursorVisible{false};

private:
    inline static bool sPressedKeys[512];
    inline static bool sReleasedKeys[512];

    std::map<unsigned int, std::function<void(const jleWindowResizeEvent& resizeEvent)>>
        windowResizedCallbacks;
};
