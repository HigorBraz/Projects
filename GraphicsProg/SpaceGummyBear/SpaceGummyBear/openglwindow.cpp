#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "imfilebrowser.h"

void OpenGLWindow::handleEvent(SDL_Event &event) {
  glm::ivec2 mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  if (event.type == SDL_MOUSEMOTION) {
    m_trackBallLight.mouseMove(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      m_trackBallLight.mousePress(mousePosition);
    }
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      m_trackBallLight.mouseRelease(mousePosition);
    }
  }

  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_SPACE) {
      const float deltaTime{static_cast<float>(getDeltaTime())};

      // Update stars
      for (const auto index : iter::range(m_numStars)) {
        auto &position{m_starPositions.at(index)};

        // X coordinate increases by speed deltaTime
        position.x += deltaTime * 10.0f * speed;

        // If this star is out the camera, select a new random
        // position and move it back to - 20
        if (position.x > 21.0f) {
          randomizeStar(position);
          position.x = -20.0f;
        }
      }

      // update gummy and earth
      positionGummy += 0.02f * controlGummy * speed;
      profundEarth += 0.02f * controlEarth * speed;
      degree += 0.5f * speed;
      fallGummy += -0.02 * speed;
    }

    // randomize color of gummy
    if (event.key.keysym.sym == SDLK_a) {
      random_color(&m_Ka_Gummy);
      random_color(&m_Kd_Gummy);
      random_color(&m_Ks_Gummy);
    }

    // randomize color of light
    if (event.key.keysym.sym == SDLK_s) {
      random_color(&m_Ia);
      random_color(&m_Id);
      random_color(&m_Is);
    }

    // set light to white
    if (event.key.keysym.sym == SDLK_x) {
      m_Ia = glm::vec4(1.0f);
      m_Id = glm::vec4(1.0f);
      m_Is = glm::vec4(1.0f);
    }
  }
}

void OpenGLWindow::initializeGL() {
  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);

  // Create programs
  for (const auto &name : m_shaderNames) {
    const auto path{getAssetsPath() + "shaders/" + name};
    const auto program{createProgramFromFile(path + ".vert", path + ".frag")};
    m_programs.push_back(program);
  }

  // Load default models
  loadModel_Gummy(getAssetsPath() + "gummy.obj");
  loadModel_Earth(getAssetsPath() + "Earth 2K.obj");
  loadModel_Star(getAssetsPath() + "box.obj");
  m_mappingMode = 0;

  // get random positions for stars
  for (const auto index : iter::range(m_numStars)) {
    auto &position{m_starPositions.at(index)};

    randomizeStar(position);
  }
}

void OpenGLWindow::random_color(glm::vec4 *color) {
  std::uniform_real_distribution<float> rd(0.0f, 1.0f);
  std::uniform_real_distribution<float> gd(0.0f, 1.0f);
  std::uniform_real_distribution<float> bd(0.0f, 1.0f);

  *color = glm::vec4(rd(m_randomEngine), gd(m_randomEngine), bd(m_randomEngine),
                     1.0f);
}

// randommize stars position
void OpenGLWindow::randomizeStar(glm::vec3 &position) {
  // Get random position
  // x and y coordinates in the range [-20, 20]
  // z coordinates in the range [-80, -20]
  std::uniform_real_distribution<float> distPosXY(-20.0f, 20.0f);
  std::uniform_real_distribution<float> distPosZ(-80.0f, -20.0f);

  position = glm::vec3(distPosXY(m_randomEngine), distPosXY(m_randomEngine),
                       distPosZ(m_randomEngine));
}

// function for load gummy model
void OpenGLWindow::loadModel_Gummy(std::string_view path) {
  m_gummy.terminateGL();

  m_gummy.loadDiffuseTexture(getAssetsPath() + "maps/pattern.png");
  m_gummy.loadNormalTexture(getAssetsPath() + "maps/pattern_normal.png");
  m_gummy.loadObj(path);
  m_gummy.setupVAO(m_programs.at(m_currentProgramIndex));

  m_Ka_Gummy = m_gummy.getKa();
  m_Kd_Gummy = m_gummy.getKd();
  m_Ks_Gummy = m_gummy.getKs();
  m_shininess_Gummy = m_gummy.getShininess();
}

// function for load earth model
void OpenGLWindow::loadModel_Earth(std::string_view path) {
  m_earth.terminateGL();

  m_earth.loadDiffuseTexture(getAssetsPath() + "maps/earth.png");
  m_earth.loadNormalTexture(getAssetsPath() + "maps/pattern_normal.png");
  m_earth.loadObj(path);
  m_earth.setupVAO(m_programs.at(m_currentProgramIndex));

  m_Ka_Earth = m_earth.getKa();
  m_Kd_Earth = m_earth.getKd();
  m_Ks_Earth = m_earth.getKs();
  m_shininess_Earth = m_earth.getShininess();
}

// function for load star model
void OpenGLWindow::loadModel_Star(std::string_view path) {
  m_star.terminateGL();

  m_star.loadObj(path);
  m_star.setupVAO(m_programs.at(m_currentProgramIndex));
}

void OpenGLWindow::paintGL() {
  update();

  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  // Use currently selected program
  const auto program{m_programs.at(m_currentProgramIndex)};
  abcg::glUseProgram(program);

  // Get location of uniform variables
  const GLint viewMatrixLoc{abcg::glGetUniformLocation(program, "viewMatrix")};
  const GLint projMatrixLoc{abcg::glGetUniformLocation(program, "projMatrix")};
  const GLint modelMatrixLoc{
      abcg::glGetUniformLocation(program, "modelMatrix")};
  const GLint normalMatrixLoc{
      abcg::glGetUniformLocation(program, "normalMatrix")};
  const GLint lightDirLoc{
      abcg::glGetUniformLocation(program, "lightDirWorldSpace")};
  const GLint shininessLoc{abcg::glGetUniformLocation(program, "shininess")};
  const GLint IaLoc{abcg::glGetUniformLocation(program, "Ia")};
  const GLint IdLoc{abcg::glGetUniformLocation(program, "Id")};
  const GLint IsLoc{abcg::glGetUniformLocation(program, "Is")};
  const GLint KaLoc{abcg::glGetUniformLocation(program, "Ka")};
  const GLint KdLoc{abcg::glGetUniformLocation(program, "Kd")};
  const GLint KsLoc{abcg::glGetUniformLocation(program, "Ks")};
  const GLint diffuseTexLoc{abcg::glGetUniformLocation(program, "diffuseTex")};
  const GLint normalTexLoc{abcg::glGetUniformLocation(program, "normalTex")};
  const GLint mappingModeLoc{
      abcg::glGetUniformLocation(program, "mappingMode")};

  // Set uniform variables used by every scene object
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform1i(diffuseTexLoc, 0);
  abcg::glUniform1i(normalTexLoc, 1);
  abcg::glUniform1i(mappingModeLoc, m_mappingMode);

  const auto lightDirRotated{m_trackBallLight.getRotation() * m_lightDir};
  abcg::glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
  abcg::glUniform4fv(IaLoc, 1, &m_Ia.x);
  abcg::glUniform4fv(IdLoc, 1, &m_Id.x);
  abcg::glUniform4fv(IsLoc, 1, &m_Is.x);

  const auto modelViewMatrix{glm::mat3(m_viewMatrix * m_modelMatrix)};
  glm::mat3 normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  glm::mat4 modelMatrix{1.0f};

  // render earth
  abcg::glUniform1f(shininessLoc, m_shininess_Earth);
  abcg::glUniform4fv(KaLoc, 1, &m_Ka_Earth.x);
  abcg::glUniform4fv(KdLoc, 1, &m_Kd_Earth.x);
  abcg::glUniform4fv(KsLoc, 1, &m_Ks_Earth.x);

  modelMatrix =
      glm::translate(modelMatrix, glm::vec3(-2.0f, 2.0f, profundEarth));
  modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f));
  modelMatrix =
      glm::rotate(modelMatrix, glm::radians(degree), glm::vec3(1, 1, 1));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

  m_earth.render(m_earth.getNumTriangles());

  // render gummy
  // if enabled render just one gummy bear
  if (enabled) {
    glm::mat4 modelMatrix{1.0f};
    abcg::glUniform1f(shininessLoc, m_shininess_Gummy);
    abcg::glUniform4fv(KaLoc, 1, &m_Ka_Gummy.x);
    abcg::glUniform4fv(KdLoc, 1, &m_Kd_Gummy.x);
    abcg::glUniform4fv(KsLoc, 1, &m_Ks_Gummy.x);

    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f));

    if (down) {
      modelMatrix =
          glm::translate(modelMatrix, glm::vec3(1.0f, fallGummy, -10.0f));
      modelMatrix =
          glm::rotate(modelMatrix, glm::radians(degree), glm::vec3(1, 1, 1));
    }

    else {
      modelMatrix =
          glm::translate(modelMatrix, glm::vec3(positionGummy, -1.0f, -10.0f));
      modelMatrix =
          glm::rotate(modelMatrix, glm::radians(degree), glm::vec3(0, 1, 0));
    }

    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

    m_gummy.render(m_gummy.getNumTriangles());
  }

  // if not render two
  else {
    for (int i = 0; i < 2; i++) {
      if (i == 0) {
        glm::mat4 modelMatrix{1.0f};
        abcg::glUniform1f(shininessLoc, m_shininess_Gummy);
        abcg::glUniform4fv(KaLoc, 1, &m_Ka_Gummy.x);
        abcg::glUniform4fv(KdLoc, 1, &m_Kd_Gummy.x);
        abcg::glUniform4fv(KsLoc, 1, &m_Ks_Gummy.x);

        modelMatrix =
            glm::translate(modelMatrix, glm::vec3(-1.5f, -1.0f, -10.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f));
        modelMatrix =
            glm::rotate(modelMatrix, glm::radians(degree), glm::vec3(0, 1, 1));

        abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE,
                                 &modelMatrix[0][0]);

        m_gummy.render(m_gummy.getNumTriangles());
      }

      else {
        glm::mat4 modelMatrix{1.0f};
        abcg::glUniform1f(shininessLoc, m_shininess_Gummy);
        abcg::glUniform4fv(KaLoc, 1, &m_Ka_Gummy.x);
        abcg::glUniform4fv(KdLoc, 1, &m_Kd_Gummy.x);
        abcg::glUniform4fv(KsLoc, 1, &m_Ks_Gummy.x);

        modelMatrix =
            glm::translate(modelMatrix, glm::vec3(1.0f, -1.0f, -10.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f));
        modelMatrix =
            glm::rotate(modelMatrix, glm::radians(degree), glm::vec3(1, 1, 0));

        abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE,
                                 &modelMatrix[0][0]);

        m_gummy.render(m_gummy.getNumTriangles());
      }
    }
  }

  // render each star
  for (const auto index : iter::range(m_numStars)) {
    const auto &position{m_starPositions.at(index)};

    abcg::glUniform1f(shininessLoc, m_shininess_Star);
    abcg::glUniform4fv(KaLoc, 1, &m_Ka_Star.x);
    abcg::glUniform4fv(KdLoc, 1, &m_Kd_Star.x);
    abcg::glUniform4fv(KsLoc, 1, &m_Ks_Star.x);

    glm::mat4 modelMatrix{1.0f};

    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));

    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

    m_star.render(m_star.getNumTriangles());
  }

  abcg::glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  // File browser for textures
  static ImGui::FileBrowser fileDialogDiffuseMap;
  fileDialogDiffuseMap.SetTitle("Load Diffuse Map");
  fileDialogDiffuseMap.SetTypeFilters({".jpg", ".png"});
  fileDialogDiffuseMap.SetWindowSize(m_viewportWidth * 0.8f,
                                     m_viewportHeight * 0.8f);

  // File browser for normal maps
  static ImGui::FileBrowser fileDialogNormalMap;
  fileDialogNormalMap.SetTitle("Load Normal Map");
  fileDialogNormalMap.SetTypeFilters({".jpg", ".png"});
  fileDialogNormalMap.SetWindowSize(m_viewportWidth * 0.8f,
                                    m_viewportHeight * 0.8f);

// Only in WebGL
#if defined(__EMSCRIPTEN__)
  fileDialogDiffuseMap.SetPwd(getAssetsPath() + "/maps");
  fileDialogNormalMap.SetPwd(getAssetsPath() + "/maps");
#endif

  {
    auto widgetSize{ImVec2(222, 200)};

    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 5, 5));
    ImGui::SetNextWindowSize(widgetSize);
    auto flags{ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration};

    ImGui::Begin("Widget window", nullptr, flags);

    // Menu
    {
      if (enabled) {
        ImGui::Button("two bears", ImVec2(-1, 20));
        if (ImGui::IsItemClicked()) {
          enabled = false;
        }

        ImGui::Checkbox("fall", &down);
      } else {
        ImGui::Button("one bear", ImVec2(-1, 20));
        if (ImGui::IsItemClicked()) {
          enabled = true;
        }
      }

      if (!down) fallGummy = 0.0f;

      bool loadDiffMap{};
      bool loadNormalMap{};
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
          ImGui::MenuItem("Load Diffuse Map...", nullptr, &loadDiffMap);
          ImGui::MenuItem("Load Normal Map...", nullptr, &loadNormalMap);
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }
      if (loadDiffMap) fileDialogDiffuseMap.Open();
      if (loadNormalMap) fileDialogNormalMap.Open();
    }

    // Projection combo box
    {
      static std::size_t currentIndex{};
      std::vector<std::string> comboItems{"Perspective", "Orthographic"};

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("Projection",
                            comboItems.at(currentIndex).c_str())) {
        for (auto index : iter::range(comboItems.size())) {
          const bool isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;
          if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      const auto aspect{static_cast<float>(m_viewportWidth) /
                        static_cast<float>(m_viewportHeight)};
      if (currentIndex == 0) {
        m_projMatrix =
            glm::perspective(glm::radians(30.0f), aspect, 0.01f, 100.0f);
      } else {
        m_projMatrix = glm::ortho(-20.0f * aspect, 20.0f * aspect, -20.0f,
                                  20.0f, 0.01f, 100.0f);
      }
    }

    // Shader combo box
    {
      static std::size_t currentIndex{};

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("Shader", m_shaderNames.at(currentIndex))) {
        for (auto index : iter::range(m_shaderNames.size())) {
          const bool isSelected{currentIndex == index};
          if (ImGui::Selectable(m_shaderNames.at(index), isSelected))
            currentIndex = index;
          if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      // Set up VAO if shader program has changed
      if (static_cast<int>(currentIndex) != m_currentProgramIndex) {
        m_currentProgramIndex = currentIndex;
        m_gummy.setupVAO(m_programs.at(m_currentProgramIndex));
        m_earth.setupVAO(m_programs.at(m_currentProgramIndex));
        m_star.setupVAO(m_programs.at(m_currentProgramIndex));
      }
    }

    // UV mapping box
    {
      std::vector<std::string> comboItems{"Triplanar", "Cylindrical",
                                          "Spherical"};

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("UV mapping",
                            comboItems.at(m_mappingMode).c_str())) {
        for (auto index : iter::range(comboItems.size())) {
          const bool isSelected{m_mappingMode == static_cast<int>(index)};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            m_mappingMode = index;
          if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();
    }

    // a slider for speed of objects
    ImGui::SliderFloat("speed", &speed, 1.0f, 5.0f, "%.0f");

    ImGui::End();
  }

  fileDialogDiffuseMap.Display();
  if (fileDialogDiffuseMap.HasSelected()) {
    m_earth.loadDiffuseTexture(fileDialogDiffuseMap.GetSelected().string());
    fileDialogDiffuseMap.ClearSelected();
  }

  fileDialogNormalMap.Display();
  if (fileDialogNormalMap.HasSelected()) {
    m_earth.loadNormalTexture(fileDialogNormalMap.GetSelected().string());
    fileDialogNormalMap.ClearSelected();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_trackBallLight.resizeViewport(width, height);
}

void OpenGLWindow::terminateGL() {
  m_gummy.terminateGL();
  m_earth.terminateGL();
  m_star.terminateGL();

  for (const auto &program : m_programs) {
    abcg::glDeleteProgram(program);
  }
}

void OpenGLWindow::update() {
  // position z of earth
  if (profundEarth >= -10.0f) {
    controlEarth = -1.0f;
  }

  else if (profundEarth <= -20.0f) {
    controlEarth = 1.0f;
  }

  // position x of gummy bear
  if (positionGummy >= 2.0f) {
    controlGummy = -1.0f;
  }

  else if (positionGummy <= 0.0f) {
    controlGummy = 1.0f;
  }

  // turn back gummy bear for up the camera
  if (fallGummy < -3.5) fallGummy = 3.5;

  // update the shininess of earth
  m_shininess_Earth = 0.4f + (profundEarth * -1) - 10;
}