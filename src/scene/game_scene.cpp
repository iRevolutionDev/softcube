#include "common.hpp"
#include "game_scene.hpp"
#include "core/renderer.hpp"
#include "utils/shader_utils.hpp"
#include "core/shaders.hpp"

static PosColorVertex s_cubeVertices[] = {
    {-1.0f, 1.0f, 1.0f, 0xff000000},
    {1.0f, 1.0f, 1.0f, 0xff0000ff},
    {-1.0f, -1.0f, 1.0f, 0xff00ff00},
    {1.0f, -1.0f, 1.0f, 0xff00ffff},
    {-1.0f, 1.0f, -1.0f, 0xffff0000},
    {1.0f, 1.0f, -1.0f, 0xffff00ff},
    {-1.0f, -1.0f, -1.0f, 0xffffff00},
    {1.0f, -1.0f, -1.0f, 0xffffffff},
};

static const uint16_t s_cubeIndices[] = {
    0, 1, 2, // front
    1, 3, 2,
    4, 6, 5, // back
    5, 6, 7,
    0, 2, 4, // left
    4, 2, 6,
    1, 5, 3, // right
    5, 7, 3,
    0, 4, 1, // top
    4, 5, 1,
    2, 3, 6, // bottom
    6, 3, 7,
};

GameScene::GameScene() : Scene("Game"),
                         vertex_buffer(BGFX_INVALID_HANDLE),
                         index_buffer(BGFX_INVALID_HANDLE),
                         program(BGFX_INVALID_HANDLE),
                         rotationX(0),
                         rotationY(0),
                         distance(5.f) {
}

GameScene::~GameScene() = default;

void GameScene::on_load() {
    create_cube();
    create_shaders();
}

void GameScene::on_enter() {
}

void GameScene::on_leave() {
}

void GameScene::on_unload() {
}

void GameScene::update(double deltaTime) {
    rotationX += 0.001f;
    rotationY += 0.002f;
}

void GameScene::render(Renderer *renderer) {
    float view[16];
    bx::mtxLookAt(view,
                  {0.0f, 0.0f, distance},
                  {0.0f, 1.0f, 0.0f},
                  {0.0f, 0.0f, 0.0f});

    float projection[16];
    bx::mtxProj(projection,
                60.0f,
                renderer->get_width() / renderer->get_height(),
                0.1f,
                100.0f,
                bgfx::getCaps()->homogeneousDepth);

    bgfx::setViewRect(0, 0, 0, renderer->get_width(), renderer->get_height());
    bgfx::setViewTransform(0, view, projection);

    float mtx[16];
    bx::mtxRotateXY(mtx, rotationX, rotationY);

    bgfx::setTransform(mtx);

    bgfx::setVertexBuffer(0, vertex_buffer);
    bgfx::setIndexBuffer(index_buffer);

    bgfx::setState(BGFX_STATE_DEFAULT);

    bgfx::submit(0, program);
}

void GameScene::create_cube() {
    bgfx::VertexLayout layout = {};
    layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();

    vertex_buffer = bgfx::createVertexBuffer(
        bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)),
        layout);

    index_buffer = bgfx::createIndexBuffer(
        bgfx::makeRef(s_cubeIndices, sizeof(s_cubeIndices)));
}

void GameScene::create_shaders() {
    const auto vs = bgfx::createEmbeddedShader(
        &k_simple_vs,
        bgfx::getRendererType(),
        "v_simple");
    const auto fs = bgfx::createEmbeddedShader(
        &k_simple_fs,
        bgfx::getRendererType(),
        "f_simple");

    program = createProgram(vs, fs, true);
}
