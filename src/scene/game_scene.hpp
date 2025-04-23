#pragma once
#include "scene.hpp"

struct PosColorVertex {
    float x;
    float y;
    float z;
    uint32_t abgr;
};

class GameScene : public Scene {
public:
    GameScene();

    ~GameScene() override;

    void on_load() override;

    void on_enter() override;

    void on_leave() override;

    void on_unload() override;

    void update(double deltaTime) override;

    void render(Renderer *renderer) override;

private:
    bgfx::VertexBufferHandle vertex_buffer;
    bgfx::IndexBufferHandle index_buffer;
    bgfx::ProgramHandle program;

    std::vector<PosColorVertex> vertices;
    std::vector<uint16_t> indices;

    float rotationX;
    float rotationY;
    float distance;

    void create_cube();

    void create_shaders();
};
