#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include "abcg.hpp"
#include "model.hpp"
#include "trackball.hpp"

#include <random>

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  int m_viewportWidth{};
  int m_viewportHeight{};
 
  static const int m_numStars{700};
  
  std::array<glm::vec3, m_numStars> m_starPositions;
 
  Model m_earth;
  Model m_gummy;
  Model m_star;

  TrackBall m_trackBallLight;

  glm::mat4 m_modelMatrix{1.0f};
  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};

  // Shaders
  std::vector<const char*> m_shaderNames{
      "normalmapping", "texture", "blinnphong", "phong",
      "gouraud",       "normal",  "depth"};
      
  std::vector<GLuint> m_programs;
  int m_currentProgramIndex{};

  // Mapping mode
  // 0: triplanar; 1: cylindrical; 2: spherical;
  int m_mappingMode{};

  // Light and material properties
  glm::vec4 m_lightDir{-1.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  
  // Gummy, Earth and stars models
  glm::vec4 m_Ka_Gummy{};
  glm::vec4 m_Kd_Gummy{};
  glm::vec4 m_Ks_Gummy{};
  float m_shininess_Gummy{};
  
  glm::vec4 m_Ka_Earth{};
  glm::vec4 m_Kd_Earth{};
  glm::vec4 m_Ks_Earth{};
  float m_shininess_Earth{};
  
  glm::vec3 m_Ka_Star{1.0f, 1.0f, 1.0f};
  glm::vec3 m_Kd_Star{1.0f, 1.0f, 1.0f};
  glm::vec3 m_Ks_Star{1.0f, 1.0f, 1.0f};
  float m_shininess_Star{1};
  
  // vars of gummy bear
  float positionGummy{1.0f};
  float controlGummy{1.0f};
  float fallGummy{0.0f};
  bool enabled{true};
  bool down{false};
  
  //vars of earth
  float profundEarth{-20.0f};
  float controlEarth{1.0f};
  
  //speed of objects
  float speed{1.0f};
  
  //degree of objects
  float degree{180.0f};
  
  std::default_random_engine m_randomEngine;
  
  void random_color(glm::vec4 *color);
  void randomizeStar(glm::vec3 &position);
  void loadModel_Gummy(std::string_view path);
  void loadModel_Earth(std::string_view path);
  void loadModel_Star(std::string_view path);
  void update();
};

#endif
