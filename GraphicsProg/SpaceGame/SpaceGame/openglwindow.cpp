#include "openglwindow.hpp"

#include <imgui.h>

#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.set(static_cast<size_t>(Input::Space));
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.set(static_cast<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_gameData.m_input.set(static_cast<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.set(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.set(static_cast<size_t>(Input::Right));
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Space));
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Right));
  }
}

void OpenGLWindow::initializeGL() {
  ImGuiIO &io{ImGui::GetIO()};
  auto filename{getAssetsPath() + "Inconsolata-Black.ttf"};
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

  // Create program to render the objects
  m_objectsProgram = createProgramFromFile(getAssetsPath() + "objects.vert",
                                           getAssetsPath() + "objects.frag");

  abcg::glClearColor(0, 0, 0, 1);

#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  m_gameData.m_state = State::Menu;
}

void OpenGLWindow::restart() {
  m_gameData.m_state = State::Playing;
  m_ship.count = 3;

  m_ship.initializeGL(m_objectsProgram);
  m_balls.initializeGL(m_objectsProgram, m_ship.count);
}

void OpenGLWindow::update() {
  const float deltaTime{static_cast<float>(getDeltaTime())};

  // Wait 5 seconds before restarting
  if (m_gameData.m_state != State::Playing &&
      m_restartWaitTimer.elapsed() > 5) {
    restart();
    return;
  }

  m_ship.update(m_gameData, deltaTime);
  m_balls.update(deltaTime, m_ship.count);

  if (m_gameData.m_state == State::Playing) {
    checkCollisions();
    checkWinCondition();
  }
}

void OpenGLWindow::paintGL() {
  update();
  abcg::glClearColor(0.2, 0.2, 0.2, 1);

  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  m_balls.paintGL();
  m_ship.paintGL(m_gameData);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  {
    const auto size{ImVec2(325, 85)};
    const auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                               (m_viewportHeight - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs};

    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    if (m_gameData.m_state == State::Win) {
      ImGui::Text("  You Win!");
    }

    if (m_gameData.m_state == State::Menu) {
      ImGui::Text("Press SPACE");
      if (m_gameData.m_input[static_cast<size_t>(Input::Space)]) {
        restart();
      }
    }

    ImGui::PopFont();
    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  abcg::glDeleteProgram(m_objectsProgram);

  m_balls.terminateGL();
  m_ship.terminateGL();
}

void OpenGLWindow::checkCollisions() {
  // Check collision between ship and balls
  for (auto &ball : m_balls.m_balls) {
    auto ballTranslation{ball.m_translation};
    auto distance{glm::distance(m_ship.m_translation, ballTranslation)};

    if (distance < m_ship.m_scale * 0.7f + ball.m_scale * 0.85f) {
      m_ship.count -= 1;
      for (auto &ball_hit : m_balls.m_balls) {
        ball_hit.m_hit = true;
      }
      std::uniform_real_distribution<float> m_randomDist{-1.0f, 1.0f};
      std::generate_n(std::back_inserter(m_balls.m_balls), m_ship.count, [&]() {
        const glm::vec2 offset{m_randomDist(m_randomEngine),
                               m_randomDist(m_randomEngine)};
        return m_balls.createBall(offset, ball.m_scale);
      });
    }
  }
  m_balls.m_balls.remove_if([](const Balls::Ball &a) { return a.m_hit; });
}

void OpenGLWindow::checkWinCondition() {
  if (m_ship.count == 0) {
    m_gameData.m_state = State::Win;
    m_restartWaitTimer.restart();
  }
}