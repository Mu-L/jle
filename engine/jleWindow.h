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

    unsigned int addWindowResizeCallback(std::function<void(unsigned int, unsigned int)> callback);

    void removeWindowResizeCallback(unsigned int callback_id);

    void executeResizeCallbacks(int w, int h);

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

    std::map<unsigned int, std::function<void(unsigned int, unsigned int)>>
        windowResizedCallbacks;
};
