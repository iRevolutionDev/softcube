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
    SC_LOG_INFO("Loading Game scene resources");
    create_cube();
    create_shaders();
    SC_LOG_INFO("Game scene resources loaded successfully");
}

void GameScene::on_enter() {
    SC_LOG_INFO("Entering Game scene");
}

void GameScene::on_leave() {
    SC_LOG_INFO("Leaving Game scene");
}

void GameScene::on_unload() {
    SC_LOG_INFO("Unloading Game scene resources");

    if (bgfx::isValid(vertex_buffer)) {
        bgfx::destroy(vertex_buffer);
        SC_LOG_DEBUG("Destroyed vertex buffer");
    }

    if (bgfx::isValid(index_buffer)) {
        bgfx::destroy(index_buffer);
        SC_LOG_DEBUG("Destroyed index buffer");
    }

    if (bgfx::isValid(program)) {
        bgfx::destroy(program);
        SC_LOG_DEBUG("Destroyed shader program");
    }

    SC_LOG_INFO("Game scene resources unloaded");
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

    ImGui::Begin("Controls"); {
    }
    ImGui::End();
}

void GameScene::create_cube() {
    SC_LOG_DEBUG("Creating cube geometry");

    bgfx::VertexLayout layout = {};
    layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();

    vertex_buffer = bgfx::createVertexBuffer(
        bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)),
        layout);

    if (!bgfx::isValid(vertex_buffer)) {
        SC_LOG_ERROR("Failed to create vertex buffer");
        return;
    }

    index_buffer = bgfx::createIndexBuffer(
        bgfx::makeRef(s_cubeIndices, sizeof(s_cubeIndices)));

    if (!bgfx::isValid(index_buffer)) {
        SC_LOG_ERROR("Failed to create index buffer");
        return;
    }

    SC_LOG_DEBUG("Cube geometry created successfully: {} vertices, {} indices",
                 sizeof(s_cubeVertices)/sizeof(PosColorVertex),
                 sizeof(s_cubeIndices)/sizeof(uint16_t));
}

void GameScene::create_shaders() {
    SC_LOG_DEBUG("Creating shader program");

    auto renderer_type = bgfx::getRendererType();
    SC_LOG_DEBUG("Using renderer type: {}", bgfx::getRendererName(renderer_type));

    const auto vs = bgfx::createEmbeddedShader(
        &k_simple_vs,
        renderer_type,
        "v_simple");

    if (!bgfx::isValid(vs)) {
        SC_LOG_ERROR("Failed to create vertex shader 'v_simple'");
        return;
    }

    const auto fs = bgfx::createEmbeddedShader(
        &k_simple_fs,
        renderer_type,
        "f_simple");

    if (!bgfx::isValid(fs)) {
        SC_LOG_ERROR("Failed to create fragment shader 'f_simple'");
        bgfx::destroy(vs);
        return;
    }

    program = createProgram(vs, fs, true);

    if (!bgfx::isValid(program)) {
        SC_LOG_ERROR("Failed to create shader program");
        return;
    }

    SC_LOG_DEBUG("Shader program created successfully");
}
