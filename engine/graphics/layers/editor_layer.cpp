#include "editor_layer.hpp"
#include "imgui.h"
#include "ecs/components/basic/name_component.hpp"
#include "ecs/components/basic/tag_component.hpp"
#include "ecs/components/basic/transform_component.hpp"
#include "ecs/components/renderer/mesh_renderer_component.hpp"
#include "ecs/components/renderer/camera_component.hpp"
#include "ecs/components/hierarchy/children_component.hpp"
#include "ecs/components/hierarchy/parent_component.hpp"
#include "ecs/systems/hierarchy/hierarchy_system.hpp"
#include "graphics/renderer/renderer.hpp"

namespace softcube {
    EditorLayer::EditorLayer() = default;

    EditorLayer::~EditorLayer() = default;

    void EditorLayer::init(EcsManager *ecs_manager) {
        m_ecs_manager = ecs_manager;
        SC_INFO("Editor layer initialized");
    }

    void EditorLayer::render() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                ImGui::MenuItem("New Scene", "Ctrl+N");
                ImGui::MenuItem("Open Scene...", "Ctrl+O");
                ImGui::MenuItem("Save Scene", "Ctrl+S");
                ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S");
                ImGui::Separator();
                ImGui::MenuItem("Exit", "Alt+F4");
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit")) {
                ImGui::MenuItem("Undo", "Ctrl+Z");
                ImGui::MenuItem("Redo", "Ctrl+Y");
                ImGui::Separator();
                ImGui::MenuItem("Cut", "Ctrl+X");
                ImGui::MenuItem("Copy", "Ctrl+C");
                ImGui::MenuItem("Paste", "Ctrl+V");
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Hierarchy", nullptr, &m_hierarchy_window_open);
                ImGui::MenuItem("Inspector", nullptr, &m_inspector_window_open);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Entity")) {
                if (ImGui::MenuItem("Create Empty")) {
                    const auto entity = m_ecs_manager->create_entity("New Entity");
                    set_selected_entity(entity);
                }

                if (ImGui::BeginMenu("3D Objects")) {
                    ImGui::MenuItem("Cube");
                    ImGui::MenuItem("Sphere");
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Light")) {
                    ImGui::MenuItem("Directional Light");
                    ImGui::MenuItem("Point Light");
                    ImGui::EndMenu();
                }

                ImGui::MenuItem("Camera");

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpaceWindow", nullptr, window_flags);
        ImGui::PopStyleVar(3);

        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        }

        if (m_hierarchy_window_open) {
            render_hierarchy_panel();
        }

        if (m_inspector_window_open) {
            render_inspector_panel();
        }

        ImGui::End();
    }

    void EditorLayer::set_selected_entity(Entity entity) {
        if (entity && entity.get_handle() != entt::null) {
            if (const auto registry = m_ecs_manager->get_hierarchy_system().get_registry();
                registry && registry->valid(entity.get_handle())) {
                m_selected_entity = entity;
                SC_DEBUG("Selected entity: {} (ID: {})",
                         entity.has_component<component::Name>() ?
                         entity.get_component<component::Name>().name.c_str() : "Unnamed",
                         static_cast<uint32_t>(entity.get_handle()));
                return;
            }
        }

        m_selected_entity = Entity();
    }

    void EditorLayer::render_hierarchy_panel() {
        ImGui::Begin("Hierarchy", &m_hierarchy_window_open);
        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered()) {
            set_selected_entity(Entity());
        }

        if (ImGui::BeginPopupContextWindow(
            nullptr, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
            if (ImGui::MenuItem("Create Empty Entity")) {
                auto entity = m_ecs_manager->create_entity("New Entity");
                set_selected_entity(entity);
            }
            ImGui::EndPopup();
        }

        if (auto registry = m_ecs_manager->get_hierarchy_system().get_registry()) {
            auto draw_entity_node = [this, registry](entt::entity entity_handle, auto &self) -> void {
                if (!registry->valid(entity_handle)) {
                    return;
                }

                Entity entity{entity_handle, registry};
                if (!entity.has_component<component::Name>()) {
                    return;
                }

                const auto &name_component = entity.get_component<component::Name>();

                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
                if (entity_handle == m_selected_entity.get_handle()) {
                    flags |= ImGuiTreeNodeFlags_Selected;
                }

                bool has_children = entity.has_component<component::Children>() &&
                                    !entity.get_component<component::Children>().entities.empty();

                if (!has_children) {
                    flags |= ImGuiTreeNodeFlags_Leaf;
                }

                bool opened = ImGui::TreeNodeEx(
                    reinterpret_cast<void *>(static_cast<uintptr_t>(static_cast<uint32_t>(entity_handle))),
                    flags,
                    "%s", name_component.name.c_str()
                );

                if (ImGui::IsItemClicked()) {
                    set_selected_entity(entity);
                }

                bool entity_deleted = false;
                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::MenuItem("Delete Entity")) {
                        entity_deleted = true;
                    }

                    if (ImGui::MenuItem("Add Child Entity")) {
                        auto child = m_ecs_manager->create_entity("Child");
                        m_ecs_manager->set_parent(child, entity);
                        set_selected_entity(child);
                    }
                    ImGui::EndPopup();
                }

                if (opened) {
                    if (has_children) {
                        const auto &children = entity.get_component<component::Children>();
                        for (auto child_handle: children.entities) {
                            self(child_handle, self);
                        }
                    }
                    ImGui::TreePop();
                }

                if (entity_deleted) {
                    if (m_selected_entity.get_handle() == entity_handle) {
                        set_selected_entity(Entity());
                    }
                    m_ecs_manager->destroy_entity(entity);
                }
            };

            auto view = registry->view<component::Name>(entt::exclude<component::Parent>);
            for (auto entity_handle: view) {
                draw_entity_node(entity_handle, draw_entity_node);
            }
        }

        ImGui::End();
    }

    void EditorLayer::render_inspector_panel() {
        ImGui::Begin("Inspector", &m_inspector_window_open);

        auto registry = m_ecs_manager->get_hierarchy_system().get_registry();
        bool entity_valid = m_selected_entity &&
                            m_selected_entity.get_handle() != entt::null &&
                            registry &&
                            registry->valid(m_selected_entity.get_handle());

        if (entity_valid) {
            if (m_selected_entity.has_component<component::Name>()) {
                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
                ImGui::Text("%s", m_selected_entity.get_component<component::Name>().name.c_str());
                ImGui::PopFont();
            }

            ImGui::Text("Entity ID: %u", static_cast<uint32_t>(m_selected_entity.get_handle()));
            ImGui::Separator();

            render_components(m_selected_entity);

            if (ImGui::Button("Add Component")) {
                ImGui::OpenPopup("AddComponent");
            }

            if (ImGui::BeginPopup("AddComponent")) {
                if (ImGui::MenuItem("Tag")) {
                    if (!m_selected_entity.has_component<component::Tag>()) {
                        m_selected_entity.add_component<component::Tag>("Entity");
                    }
                }

                ImGui::EndPopup();
            }
        } else {
            ImGui::TextDisabled("No entity selected");
        }

        ImGui::End();
    }

    void EditorLayer::render_components(Entity entity) {
        if (entity.has_component<component::Name>()) {
            auto &name = entity.get_component<component::Name>();
            render_name_component(name);
        }

        if (entity.has_component<component::Tag>()) {
            auto &tag = entity.get_component<component::Tag>();
            render_tag_component(tag);
        }

        if (entity.has_component<component::Transform>()) {
            auto &transform = entity.get_component<component::Transform>();
            render_transform_component(transform);
        }

        if (entity.has_component<component::MeshRenderer>()) {
            auto &mesh_renderer = entity.get_component<component::MeshRenderer>();
            render_mesh_renderer_component(mesh_renderer);
        }

        if (entity.has_component<component::Camera>()) {
            auto &camera = entity.get_component<component::Camera>();
            render_camera_component(camera);
        }
    }

    void EditorLayer::render_transform_component(component::Transform &transform) {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            float position[3] = {
                transform.position.x,
                transform.position.y,
                transform.position.z
            };
            if (ImGui::DragFloat3("Position", position, 0.1f)) {
                transform.position = {position[0], position[1], position[2]};
                transform.local_position = transform.position;
                transform.matrix_dirty = true;
            }

            float rotation[3] = {
                transform.rotation.x,
                transform.rotation.y,
                transform.rotation.z
            };
            if (ImGui::DragFloat3("Rotation", rotation, 0.1f)) {
                transform.matrix_dirty = true;
            }

            float scale[3] = {
                transform.scale.x,
                transform.scale.y,
                transform.scale.z
            };
            if (ImGui::DragFloat3("Scale", scale, 0.1f)) {
                transform.scale = {scale[0], scale[1], scale[2]};
                transform.local_scale = transform.scale;
                transform.matrix_dirty = true;
            }
        }
    }

    void EditorLayer::render_mesh_renderer_component(component::MeshRenderer &mesh_renderer) {
        if (ImGui::CollapsingHeader("Mesh Renderer")) {
            float color[4] = {
                mesh_renderer.color.x,
                mesh_renderer.color.y,
                mesh_renderer.color.z,
                mesh_renderer.color.w
            };

            if (ImGui::ColorEdit4("Color", color)) {
                mesh_renderer.color = {color[0], color[1], color[2], color[3]};
            }

            float metallic = mesh_renderer.metallic;
            if (ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f)) {
                mesh_renderer.metallic = metallic;
            }

            float roughness = mesh_renderer.roughness;
            if (ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f)) {
                mesh_renderer.roughness = roughness;
            }

            ImGui::Checkbox("Cast Shadows", &mesh_renderer.cast_shadows);
            ImGui::Checkbox("Receive Shadows", &mesh_renderer.receive_shadows);
            ImGui::Checkbox("Visible", &mesh_renderer.visible);
        }
    }

    void EditorLayer::render_camera_component(component::Camera &camera) const {
        if (ImGui::CollapsingHeader("Camera")) {
            bool is_main = camera.is_main;
            if (ImGui::Checkbox("Main Camera", &is_main)) {
                camera.is_main = is_main;
                if (is_main && m_selected_entity) {
                    m_ecs_manager->set_active_camera(m_selected_entity);
                }
            }

            float fov = camera.fov;
            if (ImGui::SliderFloat("Field of View", &fov, 1.0f, 179.0f)) {
                camera.fov = fov;
                if (m_ecs_manager->get_renderer()) {
                    const float width = m_ecs_manager->get_renderer()->get_width();
                    const float height = m_ecs_manager->get_renderer()->get_height();
                    camera.calculate_projection_matrix(width, height);
                }
            }

            float near_clip = camera.near_clip;
            if (ImGui::DragFloat("Near Clip", &near_clip, 0.01f, 0.001f, camera.far_clip - 0.1f)) {
                camera.near_clip = near_clip;
                if (m_ecs_manager->get_renderer()) {
                    const float width = m_ecs_manager->get_renderer()->get_width();
                    const float height = m_ecs_manager->get_renderer()->get_height();
                    camera.calculate_projection_matrix(width, height);
                }
            }

            float far_clip = camera.far_clip;
            if (ImGui::DragFloat("Far Clip", &far_clip, 1.0f, camera.near_clip + 0.1f, 10000.0f)) {
                camera.far_clip = far_clip;
                if (m_ecs_manager->get_renderer()) {
                    const float width = m_ecs_manager->get_renderer()->get_width();
                    const float height = m_ecs_manager->get_renderer()->get_height();
                    camera.calculate_projection_matrix(width, height);
                }
            }

            bool is_orthographic = camera.is_orthographic;
            if (ImGui::Checkbox("Orthographic", &is_orthographic)) {
                camera.is_orthographic = is_orthographic;
                if (m_ecs_manager->get_renderer()) {
                    const float width = m_ecs_manager->get_renderer()->get_width();
                    const float height = m_ecs_manager->get_renderer()->get_height();
                    camera.calculate_projection_matrix(width, height);
                }
            }

            if (camera.is_orthographic) {
                float ortho_size = camera.ortho_size;
                if (ImGui::DragFloat("Orthographic Size", &ortho_size, 0.1f, 0.1f, 100.0f)) {
                    camera.ortho_size = ortho_size;
                    if (m_ecs_manager->get_renderer()) {
                        const float width = m_ecs_manager->get_renderer()->get_width();
                        const float height = m_ecs_manager->get_renderer()->get_height();
                        camera.calculate_projection_matrix(width, height);
                    }
                }
            }
        }
    }

    void EditorLayer::render_name_component(component::Name &name) {
        if (ImGui::CollapsingHeader("Name", ImGuiTreeNodeFlags_DefaultOpen)) {
            char buffer[256];
            strcpy_s(buffer, name.name.c_str());
            if (ImGui::InputText("##Name", buffer, sizeof(buffer))) {
                name.name = buffer;
            }
        }
    }

    void EditorLayer::render_tag_component(component::Tag &tag) {
        if (ImGui::CollapsingHeader("Tag", ImGuiTreeNodeFlags_DefaultOpen)) {
            char buffer[256];
            strcpy_s(buffer, tag.tag.c_str());
            if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
                tag.tag = buffer;
            }
        }
    }
}
