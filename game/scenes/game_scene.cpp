#include "game_scene.hpp"
#include "graphics/renderer/renderer.hpp"

namespace game {
    GameScene::GameScene()
        : Scene("GameScene") {
    }

    GameScene::~GameScene() = default;

    void GameScene::on_load() {
        SC_INFO("Loading Game Scene");
    }

    void GameScene::on_enter() {
        SC_INFO("Entering Game Scene");
    }

    void GameScene::on_leave() {
        SC_INFO("Leaving Game Scene");
    }

    void GameScene::on_unload() {
        SC_INFO("Unloading Game Scene");
    }

    void GameScene::update(double delta_time) {
    }

    void GameScene::render(Renderer *renderer) {
        ImGui::Begin("Game Scene");
        ImGui::Text("Hello, world!");
        ImGui::End();
    }
}
