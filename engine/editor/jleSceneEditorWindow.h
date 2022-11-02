// Copyright (c) 2022. Johan Lind

#pragma once

#include "editor/jleEditorImGuiWindowInterface.h"

#include "jleGameEngine.h"
#include "jleImage.h"

class jleSceneEditorWindow : public iEditorImGuiWindow {
public:
    jleSceneEditorWindow(const std::string &window_name,
                         std::shared_ptr<jleFramebuffer> &framebuffer);

    void update(jleGameEngine &ge) override;

private:
    float _lastGameWindowWidth = 0.f, _lastGameWindowHeight = 0.f;
    std::pair<int32_t, int32_t> _lastCursorPos;
    bool _wasFocused = false;

    std::shared_ptr<jleFramebuffer> _framebuffer;
    jleImage _transformMarkerImage;
    std::shared_ptr<jleTexture> _transformMarkerTexture;
    texturedQuad _texturedQuad;

    jleCameraSimpleFPVController _fpvCamController;
};
