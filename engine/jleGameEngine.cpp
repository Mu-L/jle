// Copyright (c) 2022. Johan Lind

#include "jleGameEngine.h"

#include "jleRenderingInternalAPIInterface.h"

#include "jleExplicitInclude.h"
#include "jleMouseInputInternal.h"

#include <plog/Log.h>

jleGameEngine::jleGameEngine(std::shared_ptr<jleGameSettings> gs)
    : gameSettings{gs}, jleCore{gs} {
    gEngine = this;
    SetGameDimsPixels(gs->framebufferSettings.fixedAxis,
                      gs->framebufferSettings.fixedAxisPixels);
}

jleGameEngine::~jleGameEngine() { gEngine = nullptr; }

void jleGameEngine::SetGameDimsPixels(FIXED_AXIS fa, unsigned int pixels) {
    fixed_axis = fa;
    gameDimsPixels = pixels;
}

std::pair<unsigned int, unsigned int> jleGameEngine::GetFramebufferDimensions(
    unsigned int windowWidth, unsigned int windowHeight) {
    if (fixed_axis == FIXED_AXIS::height) {
        float aspect =
            static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

        unsigned int w = static_cast<unsigned int>(gameDimsPixels * aspect);
        return std::make_pair(w, gameDimsPixels);
    }
    else if (fixed_axis == FIXED_AXIS::width) {
        float aspect =
            static_cast<float>(windowHeight) / static_cast<float>(windowWidth);

        unsigned int h = static_cast<unsigned int>(gameDimsPixels * aspect);
        return std::make_pair(gameDimsPixels, h);
    }

    return std::make_pair(windowWidth, windowHeight);
}

void jleGameEngine::StartGame() {
    if (!gameCreator) {
        LOG_WARNING << "Game has not been set! Use SetGame<jleGameDerived>() "
                       "before starting the game.";
        return;
    }
    game = gameCreator->CreateGame();
    game->Start();

    game->mMainCamera.mCameraWidth = framebuffer_main->GetWidth();
    game->mMainCamera.mCameraHeight = framebuffer_main->GetHeight();
}

void jleGameEngine::RestartGame() {
    game.reset();
    mTimerManager.ClearTimers();
    StartGame();
}

void jleGameEngine::KillGame() {
    mTimerManager.ClearTimers();
    game.reset();
}

void jleGameEngine::HaltGame() {
    // TODO: Halt timers
    gameHalted = true;
}

void jleGameEngine::UnhaltGame() {
    // TODO: Unhalt timers
    gameHalted = false;
}

void jleGameEngine::ExecuteNextFrame() {
    LOG_VERBOSE << "Next frame dt: " << status->GetDeltaFrameTime();
    auto gameHaltedTemp = gameHalted;
    gameHalted = false;
    Update(status->GetDeltaFrameTime());
    ((jleRenderingInternalAPIInterface *)rendering.get())
        ->Render(*framebuffer_main, GetGameRef().mMainCamera);
    gameHalted = gameHaltedTemp;
}

bool jleGameEngine::IsGameKilled() {
    if (game) {
        return false;
    }
    return true;
}

bool jleGameEngine::IsGameHalted() { return gameHalted; }

jleGame& jleGameEngine::GetGameRef() { return *game; }

void jleGameEngine::Start() {
    auto dims =
        GetFramebufferDimensions(core_settings->windowSettings.windowWidth,
                                 core_settings->windowSettings.windowHeight);
    framebuffer_main =
        renderingFactory->CreateFramebuffer(dims.first, dims.second);

    const auto& internalInputMouse =
        std::static_pointer_cast<jleMouseInputInternal>(
            jleCore::core->input->mouse);
    internalInputMouse->SetPixelatedScreenSize(dims.first, dims.second);
    internalInputMouse->SetScreenSize(
        core_settings->windowSettings.windowWidth,
        core_settings->windowSettings.windowHeight);

    fullscreen_renderer = renderingFactory->CreateFullscreenRendering();

    window->AddWindowResizeCallback(
        std::bind(&jleGameEngine::FramebufferResizeEvent,
                  this,
                  std::placeholders::_1,
                  std::placeholders::_2));

    // framebuffer_main->ResizeFramebuffer(200, 200);

    LOG_INFO << "Starting the game engine";

    StartGame();
}

void jleGameEngine::FramebufferResizeEvent(unsigned int width,
                                           unsigned int height) {
    auto dims = GetFramebufferDimensions(width, height);
    framebuffer_main->ResizeFramebuffer(dims.first, dims.second);

    const auto& internalInputMouse =
        std::static_pointer_cast<jleMouseInputInternal>(
            jleCore::core->input->mouse);
    internalInputMouse->SetPixelatedScreenSize(dims.first, dims.second);
    internalInputMouse->SetScreenSize(width, height);

    game->mMainCamera.mCameraWidth = dims.first;
    game->mMainCamera.mCameraHeight = dims.second;
}

void jleGameEngine::Update(float dt) {
    JLE_SCOPE_PROFILE(jleGameEngine::Update)
    if (!gameHalted && game) {
        game->Update(dt);
        game->UpdateActiveScenes(dt);
    }
}

void jleGameEngine::Render() {
    JLE_SCOPE_PROFILE(jleGameEngine::Render)
    if (!gameHalted && game) {
        ((jleRenderingInternalAPIInterface *)rendering.get())
            ->Render(*framebuffer_main.get(), GetGameRef().mMainCamera);
        ((jleRenderingInternalAPIInterface *)rendering.get())
            ->ClearBuffersForNextFrame();
        fullscreen_renderer->RenderFramebufferFullscreen(
            *framebuffer_main.get(),
            window->GetWindowWidth(),
            window->GetWindowHeight());
    }
}

void jleGameEngine::Exiting() { KillGame(); }
