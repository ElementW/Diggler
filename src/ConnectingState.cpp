#include "ConnectingState.hpp"

#include <sstream>
#include <thread>

#include <glm/gtc/matrix_transform.hpp>

#include "Game.hpp"
#include "GameState.hpp"
#include "GlobalProperties.hpp"
#include "LocalPlayer.hpp"
#include "network/msgtypes/PlayerJoin.hpp"
#include "render/Renderer.hpp"
#include "ui/Manager.hpp"
#include "ui/Text.hpp"
#include "util/Log.hpp"

// TODO: move elsewhere
#include "scripting/lua/State.hpp"

namespace Diggler {

using Util::Log;
using namespace Util::Logging::LogLevels;

static const char *TAG = "ConnectingState";

ConnectingState::ConnectingState(GameWindow *W, const std::string &servHost, int servPort) :
  W(W),
  m_serverHost(servHost),
  m_serverPort(servPort) {
  glfwSetInputMode(*W, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  setupUI();
}

ConnectingState::~ConnectingState() {
}

void ConnectingState::setupUI() {
  txtConnecting = W->G->UIM->addManual<UI::Text>("Connecting");
  txtDot = W->G->UIM->addManual<UI::Text>(".");
  updateViewport();
}

void ConnectingState::updateViewport() {
}

void ConnectingState::onResize(int w, int h) {
  updateViewport();
}

void ConnectingState::run() {
  std::string &serverHost = m_serverHost;
  int serverPort = m_serverPort;
  bool finished = false, success = false;
  Game *const G = W->G;
  std::string failureStr;
  std::thread networkThread = std::thread([G, &success, &finished, &serverHost, serverPort, &failureStr]() {
    try {
      G->H.create();
      G->NS = &G->H.connect(serverHost, serverPort, 5000);
      success = true;
    } catch (const std::exception &e) {
      success = false;
      failureStr = e.what();
    }
    finished = true;
  });
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glm::mat4 mat;

  UI::Text::Size sz = txtConnecting->getSize();
  const glm::mat4 textMat = glm::scale(glm::translate(*G->UIM->PM, glm::vec3(W->getW()/2-sz.x,
      W->getH()/2, 0.f)), glm::vec3(2.f, 2.f, 1.f));
  while (!finished && !W->shouldClose()) { // Infinite loop \o/
    const double T = glfwGetTime();
    G->updateTime(T);

    G->R->beginFrame();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    txtConnecting->render(textMat);
    for (int i=0; i < 6; ++i) {
      const float t = T * 3 + 0.3f * i;
      mat = glm::scale(glm::translate(*G->UIM->PM,
        glm::vec3(W->getW()/2 - 1 + std::sin(t)*sz.x, W->getH()/2-sz.y, 0.f)),
        glm::vec3(2.f, 2.f, 1.f));
      txtDot->render(mat);
    }

    G->R->endFrame();

    glfwSwapBuffers(*W);
    glfwPollEvents();
  }
  if (W->shouldClose())
    W->setVisible(false);
  networkThread.join();

  if (W->shouldClose())
    return;
  if (!success) {
    W->showMessage("Could not connect to server", failureStr);
    return;
  }

  LocalPlayer &LP = *G->LP;
  LP.name = GlobalProperties::PlayerName;

  Net::MsgTypes::PlayerJoinRequest pjr;
  pjr.name = G->LP->name;
  Net::OutMessage join; pjr.writeToMsg(join);
  G->H.send(*G->NS, join, Net::Tfer::Rel, Net::Channels::Base);

  Net::InMessage m_msg;
  bool received = G->H.recv(m_msg, 5000);
  if (!received) {
    W->showMessage("Connected but got no response", "after 5 seconds");
    return;
  }
  bool msgGood = false;
  if (m_msg.getType() == Net::MessageType::PlayerJoin) {
    using PJS = Net::MsgTypes::PlayerJoinSubtype;
    switch (m_msg.getSubtype<PJS>()) {
    case PJS::Success:
      msgGood = true;
      G->U = new Universe(G, true);
      LP.sessId = m_msg.readU32();
      LP.W = G->U->createWorld(m_msg.readI16());
      break;
    case PJS::Failure: {
      // TODO be able to display a custom message
      W->showMessage("Disconnected", "while joining");
    } return;
    default:
      break;
    }
  }
  if (!msgGood) {
    std::ostringstream sstm;
    sstm << "Type: " << static_cast<int>(m_msg.getType()) <<
      " Subtype: " << static_cast<int>(m_msg.getSubtype());
    W->showMessage("Received unexpected packet", sstm.str());
    return;
  }

  // TODO: move elsewhere
  G->LS->initialize();
  const std::string gameLuaRuntimePath(getAssetsDirectory() + "/lua");
  G->LS->setGameLuaRuntimePath(gameLuaRuntimePath);
  G->LS->dofile(gameLuaRuntimePath + "/Diggler.lua");

  Log(Info, TAG) << "Joined as " << LP.name << '/' << LP.sessId;

  W->setNextState(std::make_unique<GameState>(W));
}

}
