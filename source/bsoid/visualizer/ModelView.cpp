#include "bsoid/visualizer/ModelView.hpp"
#include "bsoid/ShaderPaths.hpp"
#include "bsoid/global/LayoutLocations.glsl"

#include <atlas/utils/GUI.hpp>
#include <atlas/core/Enum.hpp>

#if defined ATLAS_DEBUG
#define ATHENA_DEBUG_CONTOURS 1 
#endif

enum class ShaderNames : int
{
    Lattice = 0,
    Wireframe,
    Mesh
};

namespace gl = atlas::gl;
namespace math = atlas::math;

namespace bsoid
{
    namespace visualizer
    {
        ModelView::ModelView(polygonizer::Bsoid&& soid) :
            mSoid(std::move(soid)),
            mLatticeData(GL_ARRAY_BUFFER),
            mLatticeIndices(GL_ELEMENT_ARRAY_BUFFER),
            mLatticeNumIndices(0),
            mMeshData(GL_ARRAY_BUFFER),
            mMeshIndices(GL_ELEMENT_ARRAY_BUFFER),
            mMeshNumIndices(0),
            mMCData(GL_ARRAY_BUFFER),
            mMCIndices(GL_ELEMENT_ARRAY_BUFFER),
            mMCNumIndices(0),
            mShowLattices(false),
            mShowMesh(false),
            mShowMCMesh(false),
            mHasMC(false),
            mRenderMode(0),
            mSelectedSlice(0)
        {
            initShaders();
        }

        ModelView::ModelView(polygonizer::Bsoid&& soid, 
            polygonizer::MarchingCubes&& mc) :
            mSoid(std::move(soid)),
            mMC(std::move(mc)),
            mLatticeData(GL_ARRAY_BUFFER),
            mLatticeIndices(GL_ELEMENT_ARRAY_BUFFER),
            mLatticeNumIndices(0),
            mMeshData(GL_ARRAY_BUFFER),
            mMeshIndices(GL_ELEMENT_ARRAY_BUFFER),
            mMeshNumIndices(0),
            mMCData(GL_ARRAY_BUFFER),
            mMCIndices(GL_ELEMENT_ARRAY_BUFFER),
            mMCNumIndices(0),
            mShowLattices(false),
            mShowMesh(false),
            mShowMCMesh(false),
            mHasMC(true),
            mRenderMode(0),
            mSelectedSlice(0)
        {
            initShaders();
        }

        std::string ModelView::getModelName() const
        {
            return mSoid.getName();
        }

        void ModelView::renderGeometry()
        {
            using atlas::core::enumToUnderlyingType;
            for (auto& shader : mShaders)
            {
                shader.hotReloadShaders();
                if (!shader.shaderProgramValid())
                {
                    return;
                }
            }

            if (mShowLattices)
            {
                auto latticeIndex = enumToUnderlyingType(ShaderNames::Lattice);
                mShaders[latticeIndex].enableShaders();

                glUniformMatrix4fv(mUniforms["lattice_model"], 1, GL_FALSE,
                    &mModel[0][0]);

                mLatticeVao.bindVertexArray();
                mLatticeIndices.bindBuffer();
                if (mSelectedSlice == 0)
                {
                    glDrawElements(GL_LINES, (GLsizei)mLatticeNumIndices,
                        GL_UNSIGNED_INT, gl::bufferOffset<GLuint>(0));
                }
                else
                {
                    glDrawElements(GL_LINES, (GLsizei)mLatticeNumIndices,
                        GL_UNSIGNED_INT, gl::bufferOffset<GLuint>(0));
                }
                mLatticeIndices.unBindBuffer();
                mLatticeVao.unBindVertexArray();

                mShaders[latticeIndex].disableShaders();
            }

            if (mShowMesh)
            {
                mMeshVao.bindVertexArray();
                mMeshIndices.bindBuffer();

                if (mRenderMode == 0)
                {
                    auto meshIndex = enumToUnderlyingType(ShaderNames::Wireframe);
                    mShaders[meshIndex].enableShaders();
                    auto var = mUniforms["wireframe_renderMode"];

                    glUniformMatrix4fv(mUniforms["wireframe_model"], 1, GL_FALSE,
                        &mModel[0][0]);


                    glUniform1i(var, 0);
                    glDrawArrays(GL_POINTS, 0, mMeshNumVertices);

                    mShaders[meshIndex].disableShaders();
                }
                else if (mRenderMode == 1)
                {
                    auto meshIndex = enumToUnderlyingType(ShaderNames::Wireframe);
                    mShaders[meshIndex].enableShaders();
                    auto var = mUniforms["wireframe_renderMode"];

                    glUniformMatrix4fv(mUniforms["wireframe_model"], 1, GL_FALSE,
                        &mModel[0][0]);

                    glUniform1i(var, 0);
                    glDrawArrays(GL_POINTS, 0, mMeshNumVertices);

                    glUniform1i(var, 1);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glDrawElements(GL_TRIANGLES, (GLsizei)mMeshNumIndices,
                        GL_UNSIGNED_INT, gl::bufferOffset<GLuint>(0));
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

                    mShaders[meshIndex].disableShaders();
                }
                else
                {
                    auto meshIndex = enumToUnderlyingType(ShaderNames::Mesh);
                    mShaders[meshIndex].enableShaders();
                    auto var = mUniforms["mesh_renderMode"];

                    glUniformMatrix4fv(mUniforms["mesh_model"], 1, GL_FALSE,
                        &mModel[0][0]);
                    glUniform1i(var, mRenderMode);
                    mMeshVao.bindVertexArray();
                    mMeshIndices.bindBuffer();
                    glDrawElements(GL_TRIANGLES, (GLsizei)mMeshNumIndices,
                        GL_UNSIGNED_INT, gl::bufferOffset<GLuint>(0));
                    mShaders[meshIndex].disableShaders();
                }

                mMeshIndices.unBindBuffer();
                mMeshVao.unBindVertexArray();
            }

            if (mShowMCMesh)
            {
                mMCVao.bindVertexArray();
                mMCIndices.bindBuffer();

                if (mRenderMode == 0)
                {
                    auto meshIndex = enumToUnderlyingType(ShaderNames::Wireframe);
                    mShaders[meshIndex].enableShaders();
                    auto var = mUniforms["wireframe_renderMode"];

                    glUniformMatrix4fv(mUniforms["wireframe_model"], 1, GL_FALSE,
                        &mModel[0][0]);


                    glUniform1i(var, 0);
                    glDrawArrays(GL_POINTS, 0, mMCNumVertices);

                    mShaders[meshIndex].disableShaders();
                }
                else if (mRenderMode == 1)
                {
                    auto meshIndex = enumToUnderlyingType(ShaderNames::Wireframe);
                    mShaders[meshIndex].enableShaders();
                    auto var = mUniforms["wireframe_renderMode"];

                    glUniformMatrix4fv(mUniforms["wireframe_model"], 1, GL_FALSE,
                        &mModel[0][0]);

                    glUniform1i(var, 0);
                    glDrawArrays(GL_POINTS, 0, mMCNumVertices);

                    glUniform1i(var, 1);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glDrawElements(GL_TRIANGLES, (GLsizei)mMCNumIndices,
                        GL_UNSIGNED_INT, gl::bufferOffset<GLuint>(0));
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

                    mShaders[meshIndex].disableShaders();
                }
                else
                {
                    auto meshIndex = enumToUnderlyingType(ShaderNames::Mesh);
                    mShaders[meshIndex].enableShaders();
                    auto var = mUniforms["mesh_renderMode"];

                    glUniformMatrix4fv(mUniforms["mesh_model"], 1, GL_FALSE,
                        &mModel[0][0]);
                    glUniform1i(var, mRenderMode);
                    mMCVao.bindVertexArray();
                    mMCIndices.bindBuffer();
                    glDrawElements(GL_TRIANGLES, (GLsizei)mMCNumIndices,
                        GL_UNSIGNED_INT, gl::bufferOffset<GLuint>(0));
                    mShaders[meshIndex].disableShaders();
                }

                mMCIndices.unBindBuffer();
                mMCVao.unBindVertexArray();
            }
        }

        void ModelView::drawGui()
        {
            // Polygonizer controls window.
            ImGui::SetNextWindowSize(ImVec2(470, 400), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Polygonizer Controls");

            ImGui::Dummy(ImVec2(0, 10));
            ImGui::Text("Generation controls");
            ImGui::Separator();
            if (ImGui::Button("Construct Lattice"))
            {
                constructLattices();
            }

            if (ImGui::Button("Construct mesh"))
            {
                constructMesh();
            }

            if (mHasMC)
            {
                if (ImGui::Button("Construct MC mesh"))
                {
                        constructMCMesh();
                }
            }

            if (ImGui::Button("Save mesh"))
            {
                mSoid.saveMesh();
            }

            ImGui::Dummy(ImVec2(0, 10));
            ImGui::Text("Visualization Options");
            ImGui::Separator();
            ImGui::Checkbox("Show lattices", &mShowLattices);
            ImGui::Checkbox("Show mesh", &mShowMesh);
            ImGui::Checkbox("Show MC mesh", &mShowMCMesh);

            ImGui::Dummy(ImVec2(0, 10));
            ImGui::Text("Log");
            ImGui::Separator();
            ImGui::BeginChild("Log", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()),
                false, ImGuiWindowFlags_AlwaysHorizontalScrollbar);
            ImGui::TextWrapped(mSoid.getLog().c_str());
            ImGui::EndChild();
            ImGui::End();

            // Render controls.
            ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Render Controls");

            std::vector<const char*> renderNames = { "Vertices", "Wireframe", 
                "Shaded", "Wireframe on Shaded", "Normals" };
            ImGui::Combo("Render mode", &mRenderMode, renderNames.data(),
                ((int)renderNames.size()));
            ImGui::End();
        }

        void ModelView::initShaders()
        {
            using atlas::core::enumToUnderlyingType;

            // Load the lattice shaders first.
            std::vector<gl::ShaderUnit> latticeShaders
            {
                { std::string(ShaderDirectory) +
                "bsoid/visualizer/Lattice.vs.glsl", GL_VERTEX_SHADER },
                { std::string(ShaderDirectory) +
                "bsoid/visualizer/Lattice.fs.glsl", GL_FRAGMENT_SHADER }
            };

            std::vector<gl::ShaderUnit> wireframeShaders
            {
                { std::string(ShaderDirectory) +
                "bsoid/visualizer/Wireframe.vs.glsl", GL_VERTEX_SHADER },
                { std::string(ShaderDirectory) +
                "bsoid/visualizer/Wireframe.fs.glsl", GL_FRAGMENT_SHADER }
            };

            // Finally the mesh shaders.
            std::vector<gl::ShaderUnit> meshShaders
            {
                { std::string(ShaderDirectory) +
                "bsoid/visualizer/Mesh.vs.glsl", GL_VERTEX_SHADER },
                { std::string(ShaderDirectory) +
                "bsoid/visualizer/Mesh.gs.glsl", GL_GEOMETRY_SHADER },
                { std::string(ShaderDirectory) +
                "bsoid/visualizer/Mesh.fs.glsl", GL_FRAGMENT_SHADER} 
            };

            mShaders.push_back(gl::Shader(latticeShaders));
            mShaders.push_back(gl::Shader(wireframeShaders));
            mShaders.push_back(gl::Shader(meshShaders));

            for (auto& shader : mShaders)
            {
                shader.setShaderIncludeDir(ShaderDirectory);
                shader.compileShaders();
                shader.linkShaders();
            }

            // Grab the lattice uniforms.
            auto latticeIndex = enumToUnderlyingType(ShaderNames::Lattice);
            auto var = mShaders[latticeIndex].getUniformVariable("model");
            mUniforms.insert(UniformKey("lattice_model", var));

            auto wireframeIndex = enumToUnderlyingType(ShaderNames::Wireframe);
            var = mShaders[wireframeIndex].getUniformVariable("model");
            mUniforms.insert(UniformKey("wireframe_model", var));
            var = mShaders[wireframeIndex].getUniformVariable("renderMode");
            mUniforms.insert(UniformKey("wireframe_renderMode", var));

            // Finally the mesh uniforms.
            auto meshIndex = enumToUnderlyingType(ShaderNames::Mesh);
            var = mShaders[meshIndex].getUniformVariable("model");
            mUniforms.insert(UniformKey("mesh_model", var));

            var = mShaders[meshIndex].getUniformVariable("renderMode");
            mUniforms.insert(UniformKey("mesh_renderMode", var));

            for (auto& shader : mShaders)
            {
                shader.disableShaders();
            }
        }

        void ModelView::constructLattices()
        {
            if (!mSoid.getLattice().vertices.empty())
            {
                return;
            }

            namespace gl = atlas::gl;
            namespace math = atlas::math;

            mSoid.constructLattice();

            auto verts = mSoid.getLattice().vertices;
            auto idx = mSoid.getLattice().indices;
            mLatticeNumIndices = idx.size();

            if (mLatticeNumIndices == 0)
            {
                return;
            }

            mLatticeVao.bindVertexArray();
            mLatticeData.bindBuffer();
            mLatticeData.bufferData(
                gl::size<math::Point>(verts.size()), verts.data(),
                GL_STATIC_DRAW);
            mLatticeData.vertexAttribPointer(VERTICES_LAYOUT_LOCATION, 3,
                GL_FLOAT, GL_FALSE, 0, gl::bufferOffset<float>(0));
            mLatticeVao.enableVertexAttribArray(VERTICES_LAYOUT_LOCATION);

            mLatticeIndices.bindBuffer();
            mLatticeIndices.bufferData(
                gl::size<GLuint>(idx.size()), idx.data(), GL_STATIC_DRAW);

            mLatticeIndices.unBindBuffer();
            mLatticeData.unBindBuffer();
            mLatticeVao.unBindVertexArray();
        }

        void ModelView::constructMesh()
        {
            // Some condition here.
            namespace gl = atlas::gl;
            namespace math = atlas::math;

            mSoid.constructMesh();

            // Update the contour buffers.
            std::vector<atlas::math::Point> verts;
            std::vector<std::uint32_t> idx;

            // Now grab the mesh data.
            verts = mSoid.getMesh().vertices();
            auto normals = mSoid.getMesh().normals();
            idx = mSoid.getMesh().indices();

            mMeshNumVertices = verts.size();
            mMeshNumIndices = idx.size();

            std::vector<float> data;
            for (std::size_t i = 0; i < verts.size(); ++i)
            {
                data.push_back(verts[i].x);
                data.push_back(verts[i].y);
                data.push_back(verts[i].z);

                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }

            mMeshVao.bindVertexArray();
            mMeshData.bindBuffer();
            mMeshData.bufferData(gl::size<float>(data.size()), data.data(),
                GL_STATIC_DRAW);
            mMeshData.vertexAttribPointer(VERTICES_LAYOUT_LOCATION, 3,
                GL_FLOAT, GL_FALSE, gl::stride<float>(6), 
                gl::bufferOffset<float>(0));
            mMeshData.vertexAttribPointer(NORMALS_LAYOUT_LOCATION, 3,
                GL_FLOAT, GL_FALSE, gl::stride<float>(6), 
                gl::bufferOffset<float>(3));
            mMeshVao.enableVertexAttribArray(VERTICES_LAYOUT_LOCATION);
            mMeshVao.enableVertexAttribArray(NORMALS_LAYOUT_LOCATION);

            mMeshIndices.bindBuffer();
            mMeshIndices.bufferData(
                gl::size<GLuint>(idx.size()), idx.data(), GL_STATIC_DRAW);

            mMeshIndices.unBindBuffer();
            mMeshData.unBindBuffer();
            mMeshVao.unBindVertexArray();
        }

        void ModelView::constructMCMesh()
        {
            mMC.polygonize();

            auto verts = mMC.getMesh().vertices();
            auto normals = mMC.getMesh().normals();
            auto idx = mMC.getMesh().indices();

            mMCNumVertices = verts.size();
            mMCNumIndices = idx.size();

            std::vector<float> data;
            for (std::size_t i = 0; i < verts.size(); ++i)
            {
                data.push_back(verts[i].x);
                data.push_back(verts[i].y);
                data.push_back(verts[i].z);

                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }

            mMCVao.bindVertexArray();
            mMCData.bindBuffer();
            mMCData.bufferData(gl::size<float>(data.size()), data.data(),
                GL_STATIC_DRAW);
            mMCData.vertexAttribPointer(VERTICES_LAYOUT_LOCATION, 3,
                GL_FLOAT, GL_FALSE, gl::stride<float>(6),
                gl::bufferOffset<float>(0));
            mMCData.vertexAttribPointer(NORMALS_LAYOUT_LOCATION, 3,
                GL_FLOAT, GL_FALSE, gl::stride<float>(6), 
                gl::bufferOffset<float>(3));
            mMCVao.enableVertexAttribArray(VERTICES_LAYOUT_LOCATION);
            mMCVao.enableVertexAttribArray(NORMALS_LAYOUT_LOCATION);

            mMCIndices.bindBuffer();
            mMCIndices.bufferData(
                gl::size<GLuint>(idx.size()), idx.data(), GL_STATIC_DRAW);

            mMCIndices.unBindBuffer();
            mMCData.unBindBuffer();
            mMCVao.unBindVertexArray();
        }
    }
}