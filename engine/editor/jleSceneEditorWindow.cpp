// Copyright (c) 2022. Johan Lind

#include "jleSceneEditorWindow.h"
#include "editor/jleEditorSceneObjectsWindow.h"
#include "jleEditor.h"

#include "ImGui/imgui.h"

#include "cTransform.h"
#include "glm/common.hpp"
#include "jleQuadRenderingInternalInterface.h"
#include "jleStaticOpenGLState.h"
#include "jleWindowInternalAPIInterface.h"

#include "3rdparty/git_submodules/glfw/include/GLFW/glfw3.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#else

#include "3rdparty/glad/glad.h"

#endif

jleSceneEditorWindow::jleSceneEditorWindow(const std::string& window_name,
                                     std::shared_ptr<jleFramebufferInterface>& framebuffer)
    : iEditorImGuiWindow(window_name),
      mTransformMarkerImage("EditorResources/icons/transform_marker.png") {
    mFramebuffer = framebuffer;
    mTransformMarkerTexture =
        jleCore::core->texture_creator->CreateTextureFromImage(
            mTransformMarkerImage);

    mTexturedQuad.texture = mTransformMarkerTexture;
    mTexturedQuad.width = 128;
    mTexturedQuad.height = 128;
    mTexturedQuad.depth = 1000.f;
    mTexturedQuad.textureX = 0;
    mTexturedQuad.textureY = 0;
}

void jleSceneEditorWindow::Update(jleGameEngine& ge) {
    if (!isOpened) {
        return;
    }

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(window_name.c_str(), &isOpened, flags);

    constexpr float negYOffset = 6;
    constexpr float negXOffset = 6;

    const auto& cursorScreenPos = ImGui::GetCursorScreenPos();
    const auto viewport = ImGui::GetMainViewport();
    const int32_t windowPositionX =
        int32_t(cursorScreenPos.x) - viewport->Pos.x;
    const int32_t windowPositionY =
        int32_t(cursorScreenPos.y) - viewport->Pos.y;

    const auto previousFrameCursorPos = mLastCursorPos;
    mLastCursorPos =
        std::static_pointer_cast<jleWindowInternalAPIInterface>(jleCore::core->window)
            ->GetCursor();
    const int32_t mouseX = mLastCursorPos.first;
    const int32_t mouseY = mLastCursorPos.second;
    const int32_t mouseDeltaX = mouseX - previousFrameCursorPos.first;
    const int32_t mouseDeltaY = mouseY - previousFrameCursorPos.second;

    const auto getPixelatedMousePosX = [&]() -> int32_t {
        const float ratio =
            float(mFramebuffer->GetWidth()) / float(mLastGameWindowWidth);
        return int(ratio * float(mouseX - windowPositionX));
    };

    const auto getPixelatedMousePosY = [&]() -> int32_t {
        const float ratio =
            float(mFramebuffer->GetHeight()) / float(mLastGameWindowHeight);
        return int(ratio * float(mouseY - windowPositionY));
    };

    const auto mouseCoordinateX =
        getPixelatedMousePosX() + jleEditor::mEditorCamera.GetIntX();
    const auto mouseCoordinateY =
        getPixelatedMousePosY() + jleEditor::mEditorCamera.GetIntY();

    static float zoomValue = 1.f;

    if (!(ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset ==
              mLastGameWindowWidth &&
          ImGui::GetWindowHeight() - ImGui::GetCursorStartPos().y -
                  negYOffset ==
              mLastGameWindowHeight)) {
        mLastGameWindowWidth =
            ImGui::GetWindowWidth() - ImGui::GetCursorStartPos().x - negXOffset;
        mLastGameWindowHeight = ImGui::GetWindowHeight() -
                                ImGui::GetCursorStartPos().y - negYOffset;

        auto dims = ge.GetFramebufferDimensions(
            static_cast<unsigned int>(ImGui::GetWindowWidth()),
            static_cast<unsigned int>(ImGui::GetWindowHeight()));
        mFramebuffer->ResizeFramebuffer(dims.first * zoomValue,
                                        dims.second * zoomValue);
    }

    const auto& selectedObject =
        jleEditorSceneObjectsWindow::GetSelectedObject();
    std::shared_ptr<cTransform> transform{nullptr};

    // Render the transform marker only in the editor window
    if (auto object = selectedObject.lock()) {
        transform = object->GetComponent<cTransform>();
        if (transform) {
            mTexturedQuad.x = transform->GetWorldX() - 64.f;
            mTexturedQuad.y = transform->GetWorldY() - 64.f;
            std::vector<TexturedQuad> texturedQuads{mTexturedQuad};
            auto quadRenderer =
                ((jleQuadRenderingInternalInterface *)(jleCore::core->rendering->quads
                                                .get()));
            quadRenderer->Render(*mFramebuffer,
                                 jleEditor::mEditorCamera,
                                 texturedQuads,
                                 {},
                                 false);
        }
    }

    glBindTexture(GL_TEXTURE_2D, (unsigned int)mFramebuffer->GetTexture());
    jleStaticOpenGLState::globalActiveTexture =
        (unsigned int)mFramebuffer->GetTexture();

    // Render the framebuffer as an image
    ImGui::Image((void *)(intptr_t)mFramebuffer->GetTexture(),
                 ImVec2(mLastGameWindowWidth, mLastGameWindowHeight),
                 ImVec2(0, 1),
                 ImVec2(1, 0));

    if (ImGui::IsWindowHovered()) {
        auto dragDelta = ImGui::GetMouseDragDelta(1);
        jleEditor::mEditorCamera.mX += dragDelta.x * 0.001f * zoomValue;
        jleEditor::mEditorCamera.mY += dragDelta.y * 0.001f * zoomValue;

        jleEditor::mEditorCamera.mXNoOffset =
            jleEditor::mEditorCamera.mX + mFramebuffer->GetWidth() * .5;
        jleEditor::mEditorCamera.mYNoOffset =
            jleEditor::mEditorCamera.mY + mFramebuffer->GetHeight() * .5;

        jleEditor::mEditorCamera.ApplyPerspectiveMouseMovementDelta(
            glm::vec2{dragDelta.x, -dragDelta.y});

        if (ImGui::IsKeyDown(ImGuiKey_W)) {
            jleEditor::mEditorCamera.MovePerspectiveForward(
                115.f * ge.status->GetDeltaFrameTime());
        }
        if (ImGui::IsKeyDown(ImGuiKey_S)) {
            jleEditor::mEditorCamera.MovePerspectiveBackward(
                115.f * ge.status->GetDeltaFrameTime());
        }
        if (ImGui::IsKeyDown(ImGuiKey_D)) {
            jleEditor::mEditorCamera.MovePerspectiveRight(
                115.f * ge.status->GetDeltaFrameTime());
        }
        if (ImGui::IsKeyDown(ImGuiKey_A)) {
            jleEditor::mEditorCamera.MovePerspectiveLeft(
                115.f * ge.status->GetDeltaFrameTime());
        }
        if (ImGui::IsKeyDown(ImGuiKey_Space)) {
            jleEditor::mEditorCamera.MovePerspectiveUp(
                115.f * ge.status->GetDeltaFrameTime());
        }
        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
            jleEditor::mEditorCamera.MovePerspectiveDown(
                115.f * ge.status->GetDeltaFrameTime());
        }

        auto currentScroll = jleCore::core->input->mouse->GetScrollY();
        if (currentScroll != 0.f) {
            zoomValue -= currentScroll * 0.1f;
            zoomValue = glm::clamp(zoomValue, 0.25f, 5.f);
            auto dims = ge.GetFramebufferDimensions(
                static_cast<unsigned int>(ImGui::GetWindowWidth()),
                static_cast<unsigned int>(ImGui::GetWindowHeight()));

            const auto oldWidth = mFramebuffer->GetWidth();
            const auto oldHeight = mFramebuffer->GetHeight();
            const auto widthDiff = dims.first * zoomValue - oldWidth;
            const auto heightDiff = dims.second * zoomValue - oldHeight;
            jleEditor::mEditorCamera.mX -= widthDiff * .5f;
            jleEditor::mEditorCamera.mY -= heightDiff * .5f;
            mFramebuffer->ResizeFramebuffer(dims.first * zoomValue,
                                            dims.second * zoomValue);
        }

        static int draggingTransformMarker = 0;
        if (ImGui::IsMouseClicked(0)) {
            if ((mouseCoordinateX >= mTexturedQuad.x &&
                 mouseCoordinateX <= mTexturedQuad.x + 128) &&
                (mouseCoordinateY >= mTexturedQuad.y &&
                 mouseCoordinateY <= mTexturedQuad.y + 128)) {
                LOGV << "Inside AABB " << mouseCoordinateX - mTexturedQuad.x
                     << ' ' << mouseCoordinateY - mTexturedQuad.y;

                constexpr std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>
                    redPart = {217, 87, 99, 255};
                constexpr std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>
                    greenPart = {153, 229, 80, 255};
                constexpr std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>
                    bluePart = {99, 155, 255, 255};

                const auto pixels = mTransformMarkerImage.GetPixelAtLocation(
                    mouseCoordinateX - mTexturedQuad.x,
                    mouseCoordinateY - mTexturedQuad.y);

                if (pixels == redPart) {
                    draggingTransformMarker = 1;
                }
                else if (pixels == greenPart) {
                    draggingTransformMarker = 2;
                }
                else if (pixels == bluePart) {
                    draggingTransformMarker = 3;
                }
            }
        }

        if (ImGui::IsMouseReleased(0) && draggingTransformMarker) {
            draggingTransformMarker = 0;
        }

        if (ImGui::IsMouseDragging(0)) {
            if (draggingTransformMarker == 1) {
                transform->SetWorldPosition(mouseCoordinateX,
                                            mouseCoordinateY,
                                            transform->GetWorldDepth());
            }
            else if (draggingTransformMarker == 2) {
                transform->SetWorldPositionX(mouseCoordinateX);
            }
            else if (draggingTransformMarker == 3) {
                transform->SetWorldPositionY(mouseCoordinateY);
            }
        }

        if (!draggingTransformMarker && ImGui::IsMouseClicked(0)) {
            // Select closest object from mouse click
            auto& game = ((jleGameEngine *)jleCore::core)->GetGameRef();
            const auto& scenes = game.GetActiveScenesRef();

            std::unordered_map<std::shared_ptr<cTransform>,
                               std::shared_ptr<jleObject>>
                transformsMap;
            for (auto& scene : scenes) {
                for (auto& object : scene->GetSceneObjects()) {
                    auto objectsTransform = object->GetComponent<cTransform>();
                    if (objectsTransform) {
                        transformsMap.insert(
                            std::make_pair(objectsTransform, object));
                    }
                }
            }

            std::shared_ptr<cTransform> closestTransform = nullptr;
            std::shared_ptr<jleObject> selectedObject = nullptr;
            for (auto& transformPair : transformsMap) {
                const auto& transform = transformPair.first;
                const auto& object = transformPair.second;
                if (closestTransform == nullptr) {
                    closestTransform = transform;
                    selectedObject = object;
                }
                else if ((pow(transform->GetWorldX() - mouseCoordinateX, 2) +
                          pow(transform->GetWorldY() - mouseCoordinateY, 2)) <
                         (pow(closestTransform->GetWorldX() - mouseCoordinateX,
                              2)) +
                             pow(closestTransform->GetWorldY() -
                                     mouseCoordinateY,
                                 2)) {
                    closestTransform = transform;
                    selectedObject = object;
                }
            }

            if (selectedObject) {
                jleEditorSceneObjectsWindow::SetSelectedObject(selectedObject);
            }
        }
    }

    ImGui::End();
}
