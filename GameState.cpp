#include "GameState.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <cstdio>
#include <iomanip>
#include <memory>
#include <sstream>
#include <thread>
#include "KeyBinds.hpp"
#include "GlobalProperties.hpp"
#include "Game.hpp"
#include "FBO.hpp"
#include "Clouds.hpp"
#include "Chatbox.hpp"
#include "CaveGenerator.hpp"
#include "Skybox.hpp"
#include "EscMenu.hpp"
#include "Audio.hpp"
#include "network/NetHelper.hpp"

using std::unique_ptr;

namespace Diggler {

const int GameState::BloomScale = 4;

GameState::GameState(GameWindow *W, const std::string &servHost, int servPort)
	: W(W), m_serverHost(servHost), m_serverPort(servPort) {
	G = W->G;
	int w = W->getW(),
		h = W->getH();

	// Initialized in setupUI
	UI.EM = nullptr;
	m_chatBox = nullptr;

	m_3dFbo = new FBO(w, h, Texture::PixelFormat::RGB, true);
	m_3dRenderVBO = new VBO();
	m_clouds = new Clouds(G, 32, 32, 4);
	//m_sky = new Skybox(G, getAssetPath("alpine"));
	m_3dFboRenderer = G->PM->getProgram(PM_2D | PM_TEXTURED); //getSpecialProgram("effect3dRender");
	m_3dFboRenderer_coord = m_3dFboRenderer->att("coord");
	m_3dFboRenderer_texcoord = m_3dFboRenderer->att("texcoord");
	m_3dFboRenderer_mvp = m_3dFboRenderer->uni("mvp");

	m_extractorFbo = new FBO(w/BloomScale, h/BloomScale, Texture::PixelFormat::RGBA);
	m_extractorFbo->tex->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);
	m_bloomExtractorRenderer = G->PM->getSpecialProgram("bloomExtractor");
	m_bloomExtractorRenderer_coord = m_bloomExtractorRenderer->att("coord");
	m_bloomExtractorRenderer_texcoord = m_bloomExtractorRenderer->att("texcoord");
	m_bloomExtractorRenderer_mvp = m_bloomExtractorRenderer->uni("mvp");
	m_bloomFbo = new FBO(w/BloomScale, h/BloomScale, Texture::PixelFormat::RGBA);
	m_bloomFbo->tex->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);
	m_bloomRenderer = G->PM->getSpecialProgram("bloom");
	m_bloomRenderer_coord = m_bloomRenderer->att("coord");
	m_bloomRenderer_texcoord = m_bloomRenderer->att("texcoord");
	m_bloomRenderer_mvp = m_bloomRenderer->uni("mvp");
	m_bloomRenderer_pixshift = m_bloomRenderer->uni("pixshift");

	Coord2DTex renderQuad[6] = {
		{0, 0, 0, 0},
		{1, 0, 1, 0},
		{0, 1, 0, 1},

		{1, 1, 1, 1},
		{0, 1, 0, 1},
		{1, 0, 1, 0}
	};
	m_3dRenderVBO->setData(renderQuad, 6*sizeof(Coord2DTex));

	//"\f0H\f1e\f2l\f3l\f4l\f5o \f6d\f7e\f8m\f9b\faa\fbz\fcz\fde\fes\ff,\n\f0ye see,it werks purrfektly :D\n(and also; it's optimized)"

	m_mouseLocked = false;
	nextNetUpdate = 0;

	enableExtractor = true;
}

void GameState::setupUI() {
	UI.Ore = G->UIM->add<UI::Text>(G->F); UI.Ore->setScale(2, 2);
	UI.Loot = G->UIM->add<UI::Text>(G->F); UI.Loot->setScale(2, 2);
	UI.Weight = G->UIM->add<UI::Text>(G->F); UI.Weight->setScale(2, 2);
	UI.TeamOre = G->UIM->add<UI::Text>(G->F); UI.TeamOre->setScale(2, 2);
	UI.RedCash = G->UIM->add<UI::Text>(G->F); UI.RedCash->setScale(2, 2);
	UI.BlueCash = G->UIM->add<UI::Text>(G->F); UI.BlueCash->setScale(2, 2);
	UI.FPS = G->UIM->add<UI::Text>(G->F); UI.FPS->setScale(2, 2);
	UI.Altitude = G->UIM->add<UI::Text>(G->F); UI.Altitude->setScale(2, 2);
	UI.DebugInfo = G->UIM->add<UI::Text>(G->F); UI.DebugInfo->setVisible(false);
	UI.EM = new EscMenu(G);

	m_chatBox = new Chatbox(G);

	updateViewport();
}

GameState::~GameState() {
	delete UI.EM;
	delete m_3dFbo; delete m_3DRenderVBO; delete m_extractorFbo; delete m_clouds; delete m_bloomFbo;
	delete m_chatBox;
	//delete m_sky;
}

void GameState::onChar(char32 unichar) {
	if (m_chatBox->isChatting()) {
		m_chatBox->handleChar(unichar);
	}
}

void GameState::onKey(int key, int scancode, int action, int mods) {
	if (!(action == GLFW_PRESS || action == GLFW_RELEASE))
		return;

	switch (key) {
	case GLFW_KEY_ESCAPE:
		if (mods & GLFW_MOD_SHIFT)
			glfwSetWindowShouldClose(*G->GW, true);
		break;
	case GLFW_KEY_F1:
		if (action == GLFW_PRESS)
			enableExtractor = !enableExtractor;
		break;
	case GLFW_KEY_F5:
		if (action == GLFW_PRESS) {
			showDebugInfo = !showDebugInfo;
			UI.DebugInfo->setVisible(showDebugInfo);
		}
		break;
	case GLFW_KEY_F6:
		if (action == GLFW_PRESS) {
			unlockMouse();
		}
		break;
	default:
		break;
	}

	if (m_chatBox->isChatting()) {
		switch (key) {
		case GLFW_KEY_ENTER:
			if (action == GLFW_PRESS) {
				std::string str = m_chatBox->getChatString();
				if (str.size() > 0) {
					NetHelper::SendChat(G, str);
				}
				m_chatBox->setIsChatting(false);
			}
			break;
		case GLFW_KEY_ESCAPE:
			if (action == GLFW_PRESS)
				m_chatBox->setIsChatting(false);
			break;
		default:
			m_chatBox->handleKey(key, scancode, action, mods);
			break;
		}
	} else {
		if (key == G->KB->gameMenu && action == GLFW_PRESS) {
			isEscapeToggled = !isEscapeToggled;
			UI.EM->setVisible(isEscapeToggled);
			if (isEscapeToggled)
				unlockMouse();
			else
				lockMouse();
		}
		if (!isEscapeToggled) {
			if (key == G->KB->forward) {
				G->LP->goForward(action == GLFW_PRESS);
			} else if (key == G->KB->backward) {
				G->LP->goBackward(action == GLFW_PRESS);
			} else if (key == G->KB->left) {
				G->LP->goLeft(action == GLFW_PRESS);
			} else if (key == G->KB->right) {
				G->LP->goRight(action == GLFW_PRESS);
			} else if (key == G->KB->jump) {
				if (action == GLFW_PRESS)
					G->LP->jump();
			} else if (key == G->KB->chat) {
				if (action == GLFW_PRESS)
					m_chatBox->setIsChatting(true);
			} else if (key == GLFW_KEY_V) {
				G->LP->setHasNoclip(true);
			} else if (key == GLFW_KEY_B) {
				G->LP->setHasNoclip(false);
			}
			// TODO remove me G->LP->special1();
		}
	}
}

void GameState::lockMouse() {
	glfwSetInputMode(*W, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	double x, y;
	glfwGetCursorPos(*W, &x, &y);
	cX = (int)x; cY = (int)y;
	m_mouseLocked = true;
}

void GameState::unlockMouse() {
	m_mouseLocked = false;
	glfwSetInputMode(*W, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}


void GameState::onMouseButton(int key, int action, int mods) {
	if (key != GLFW_MOUSE_BUTTON_LEFT)
		return;

	if (!m_mouseLocked && action == GLFW_PRESS && !isEscapeToggled) {
		lockMouse();
	}
}

void GameState::onCursorPos(double x, double y) {
	if (!m_mouseLocked)
		return;
	int cx = (int)x, dx = cx-cX, cy = (int)y, dy = cy-cY;
	const float mousespeed = 0.003;

	angles.x -= dx * mousespeed;
	angles.y -= dy * mousespeed;

	if(angles.x < -M_PI)
		angles.x += M_PI * 2;
	else if(angles.x > M_PI)
		angles.x -= M_PI * 2;

	if(angles.y < -M_PI / 2)
		angles.y = -M_PI / 2 + 0.001;
	if(angles.y > M_PI / 2)
		angles.y = M_PI / 2 - 0.001;

	lookat.x = sinf(angles.x) * cosf(angles.y);
	lookat.y = sinf(angles.y);
	lookat.z = cosf(angles.x) * cosf(angles.y);

	G->LP->lookAt(lookat);

	cX = cx; cY = cy;
}

void GameState::onResize(int w, int h) {
	updateViewport();
}

void GameState::onMouseScroll(double x, double y) {

}

void GameState::updateViewport() {
	int w = W->getW(), h = W->getH();
	glViewport(0, 0, w, h);
	G->LP->camera.setPersp((float)M_PI/180*75.0f, (float)w / h, 0.1f, 32.0f);
	m_3dFbo->resize(w, h);
	m_extractorFbo->resize(w/BloomScale, h/BloomScale);
	//m_extractorFbo->tex->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);
	m_bloomFbo->resize(w/BloomScale, h/BloomScale);
	//m_bloomFbo->tex->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);

	char str[15]; std::snprintf(str, 15, "Loot: %d/%d", 0/*G->LP->ore*/, Player::getMaxOre(G->LP->playerclass));
	UI.Ore->setText(std::string(str));
	UI.Ore->setPos(4, h-14);

	UI.Loot->setText("Loot: $0");
	UI.Loot->setPos(w/6, h-14);

	UI.Weight->setText("Weight: 0");
	UI.Weight->setPos(w/3, h-14);

	UI.TeamOre->setText("Team ore: 0");
	UI.TeamOre->setPos(w/2, h-14);

	UI.RedCash->setText("\f4Red: $0");
	UI.RedCash->setPos((w*5)/8, h-14);

	UI.BlueCash->setText("\fbBlue: $0");
	UI.BlueCash->setPos((w*6)/8, h-14);

	UI.FPS->setPos(16, 16);
	UI.Altitude->setText("Altitude: XX");
	UI.Altitude->setPos(w-16-UI.Altitude->getSize().x, 16);
	UI.lastAltitude = INT_MAX;

	UI.DebugInfo->setPos(0, h-(UI.BlueCash->getSize().y+UI.DebugInfo->getSize().y));

	m_chatBox->setPosition(4, 64);
	updateUI();
}

void GameState::sendMsg(Net::OutMessage &msg, Net::Tfer mode, Net::Channels chan) {
	G->H.send(G->NS, msg, mode, chan);
}

void GameState::run() {
	if (connectLoop()) return;

	setupUI();
	gameLoop();
}

bool GameState::connectLoop() {
	std::string &serverHost = m_serverHost;
	int serverPort = m_serverPort;
	bool finished = false, success = false; Game *G = this->G;
	m_networkThread = std::thread([G, &success, &finished, &serverHost, serverPort]() {
		try {
			G->H.create();
			G->NS = G->H.connect(serverHost, serverPort, 5000);
			success = true;
		} catch (const Net::Exception &e) {
			success = false;
		}
		finished = true;
	});
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	ConnectingUI cUI {
		G->UIM->create<UI::Text>(G->F, "Connecting"),
		G->UIM->create<UI::Text>(G->F, ".")
	};
	double T; glm::mat4 mat;

	while (!finished && !glfwWindowShouldClose(*W)) { // Infinite loop \o/
		T = glfwGetTime();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		UI::Text::Size sz = cUI.Connecting->getSize();
		mat = glm::scale(glm::translate(*G->GW->UIM.PM, glm::vec3(W->getW()/2-sz.x, W->getH()/2, 0.f)),
			glm::vec3(2.f, 2.f, 1.f));
		cUI.Connecting->render(mat);
		for (int i=0; i < 6; ++i) {
			mat = glm::scale(glm::translate(*G->GW->UIM.PM,
				glm::vec3(W->getW()/2 - 1 + sin(T*3+0.3*i)*sz.x, W->getH()/2-sz.y, 0.f)),
				glm::vec3(2.f, 2.f, 1.f));
			cUI.Dot->render(mat);
		}

		glfwSwapBuffers(*W);
		glfwPollEvents();
	}
	if (glfwWindowShouldClose(*W))
		glfwHideWindow(*W);
	m_networkThread.join();
	delete cUI.Connecting; delete cUI.Dot;

	if (glfwWindowShouldClose(*W))
		return true;
	if (!success) {
		std::ostringstream oss;
		oss << serverHost << ':' << serverPort << " did not respond";
		W->showMessage("Could not connect to server", oss.str());
		return true;
	}

	G->LP->name = GlobalProperties::PlayerName;

	Net::OutMessage join(Net::MessageType::PlayerJoin);
	std::string strname(G->LP->name);
	join.writeString(strname);
	sendMsg(join, Net::Tfer::Rel);

	bool received = G->H.recv(m_msg, 5000);
	if (!received) {
		W->showMessage("Connected but got no response", "after 5 seconds");
		return true;
	}
	switch (m_msg.getType()) {
		case Net::MessageType::PlayerJoin: {
			G->LP->id = m_msg.readU32();
		} break;
		case Net::MessageType::PlayerQuit: {
			std::string desc = m_msg.readString();
			W->showMessage("Disconnected", desc);
		} return true;
		default: {
			std::ostringstream sstm;
			sstm << "Type: " << (int)m_msg.getType() << " Subtype: " << (int)m_msg.getSubtype();
			W->showMessage("Received weird packet", sstm.str());
		} return true;
	}

	getDebugStream() << "Joined as " << G->LP->name << '/' << G->LP->id << std::endl;
	return false;
}

void GameState::gameLoop() {
	double lastT, deltaT, T, fpsT = 0; int frames = 0;
	LocalPlayer *LP = G->LP;
	LP->position = glm::vec3(-2, 2, -2);
	angles.x = M_PI/4; angles.y = M_PI/4;
	lookat.x = sinf(angles.x) * cosf(angles.y);
	lookat.y = sinf(angles.y);
	lookat.z = cosf(angles.x) * cosf(angles.y);
	LP->lookAt(lookat);
	LP->forceCameraUpdate();
	G->A->update();
	LP->setHasNoclip(true);
	while (!glfwWindowShouldClose(*W)) {
		if (!processNetwork()) return;

		T = glfwGetTime(); deltaT = T - lastT;
		G->Time = T;
		if (T > fpsT) {
			char str[8]; std::sprintf(str, "FPS: %d", frames); //\f
			UI.FPS->setText(std::string(str));
			fpsT = T+1;
			frames = 0;
		}
		
		if (G->LP->isAlive) {
			if (T > nextNetUpdate) {
				Net::OutMessage msg(Net::MessageType::PlayerUpdate, Net::PlayerUpdateType::Move);
				msg.writeVec3(LP->position);
				msg.writeVec3(LP->velocity);
				msg.writeVec3(LP->accel);
				sendMsg(msg, Net::Tfer::Unrel);
				nextNetUpdate = T+0.25;//+1;
			}
			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_3dFbo->bind();
			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			LP->update(deltaT);
			// TODO: disable teleport and kill player
			if (LP->position.y < -32-LP->size.y) {
				LP->position.y = G->SC->getChunksY()*CY+32+LP->size.y;
			}

			glm::mat4 m_transform = LP->getPVMatrix();

			/*** 3D PART ***/
			glEnable(GL_CULL_FACE);

			//m_sky->render(LP->camera.getSkyMatrix());

			glEnable(GL_DEPTH_TEST);

			G->SC->render(m_transform);
			for (Player &p : G->players) {
				p.update(deltaT);
				p.render(m_transform);
			}

			glDisable(GL_CULL_FACE);

			glm::mat4 cloudmat = glm::scale(glm::translate(m_transform, glm::vec3(0.f, (G->SC->getChunksY()*CY/4)+.5f, 0.f)), glm::vec3(G->SC->getChunksX()*CX, 2, G->SC->getChunksZ()*CZ));
			m_clouds->render(cloudmat);

			glDisable(GL_DEPTH_TEST);

			m_3dFbo->unbind();
			G->UIM->drawFullTexV(*m_3dFbo->tex);

			if (0) { //enableExtractor) {
				glViewport(0, 0, W->getW()/BloomScale, W->getH()/BloomScale);
				m_3dFbo->tex->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);
				m_extractorFbo->bind();
				glClearColor(0.f, 0.f, 0.f, 0.f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				m_bloomExtractorRenderer->bind();
				glEnableVertexAttribArray(m_bloomExtractorRenderer_coord);
				glEnableVertexAttribArray(m_bloomExtractorRenderer_texcoord);
				m_3dRenderVBO->bind();
				glUniformMatrix4fv(m_bloomExtractorRenderer_mvp, 1, GL_FALSE, glm::value_ptr(*G->GW->UIM.PM1));
				glVertexAttribPointer(m_bloomExtractorRenderer_coord, 2, GL_INT, GL_FALSE, sizeof(Coord2DTex), 0);
				glVertexAttribPointer(m_bloomExtractorRenderer_texcoord, 2, GL_BYTE, GL_FALSE, sizeof(Coord2DTex), (GLvoid*)offsetof(Coord2DTex, u));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisableVertexAttribArray(m_bloomExtractorRenderer_texcoord);
				glDisableVertexAttribArray(m_bloomExtractorRenderer_coord);
				m_3dFbo->tex->setFiltering(Texture::Filter::Nearest, Texture::Filter::Nearest);

				m_extractorFbo->unbind();

				m_bloomFbo->bind();
				glClearColor(0.f, 0.f, 0.f, 0.f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				m_extractorFbo->tex->bind();
				m_bloomRenderer->bind();
				glEnableVertexAttribArray(m_bloomRenderer_coord);
				glEnableVertexAttribArray(m_bloomRenderer_texcoord);
				m_3dRenderVBO->bind();
				glUniformMatrix4fv(m_bloomRenderer_mvp, 1, GL_FALSE, glm::value_ptr(*G->GW->UIM.PM));
				GLfloat pixshift[2] = { 1.f/(W->getW()/BloomScale), 1.f/(W->getH()/BloomScale) };
				glUniform2fv(m_bloomRenderer_pixshift, 1, pixshift);
				glVertexAttribPointer(m_bloomRenderer_coord, 2, GL_INT, GL_FALSE, sizeof(Coord2DTex), 0);
				glVertexAttribPointer(m_bloomRenderer_texcoord, 2, GL_BYTE, GL_FALSE, sizeof(Coord2DTex), (GLvoid*)offsetof(Coord2DTex, u));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisableVertexAttribArray(m_bloomRenderer_texcoord);
				glDisableVertexAttribArray(m_bloomRenderer_coord);
				m_bloomFbo->unbind();

				// render to real surface
				glViewport(0, 0, W->getW(), W->getH());
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				m_bloomFbo->tex->bind();
				m_bloomFbo->tex->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);
				m_bloomRenderer->bind();
				glEnableVertexAttribArray(m_bloomRenderer_coord);
				glEnableVertexAttribArray(m_bloomRenderer_texcoord);
				m_3dRenderVBO->bind();
				glUniformMatrix4fv(m_bloomRenderer_mvp, 1, GL_FALSE, glm::value_ptr(*G->GW->UIM.PM1));
				glVertexAttribPointer(m_bloomRenderer_coord, 2, GL_INT, GL_FALSE, sizeof(Coord2DTex), 0);
				glVertexAttribPointer(m_bloomRenderer_texcoord, 2, GL_BYTE, GL_FALSE, sizeof(Coord2DTex), (GLvoid*)offsetof(Coord2DTex, u));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisableVertexAttribArray(m_bloomRenderer_texcoord);
				glDisableVertexAttribArray(m_bloomRenderer_coord);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}

			/*** 2D PART ***/
			updateUI();
			drawUI();
		} else {
			if (!G->LP->deathShown) {
				G->LP->deathShown = true;
				G->A->playSound("death");
			}
			if (!G->LP->deathSent) {
				G->LP->deathSent = true;
				Net::OutMessage out(Net::MessageType::PlayerUpdate, Net::PlayerUpdateType::Die);
				out.writeU8((uint8)G->LP->deathReason);
				sendMsg(out, Net::Tfer::Rel, Net::Channels::Life);
			}
			renderDeathScreen();
		}

		if (isEscapeToggled)
			UI.EM->render();

		glfwSwapBuffers(*W);
		glfwPollEvents();

		lastT = T;
		frames++;
	}
	Net::OutMessage quit(Net::MessageType::PlayerQuit);
	sendMsg(quit, Net::Tfer::Rel);
}

void GameState::renderDeathScreen() {
	double red = std::max(1-(G->Time-G->LP->deathTime), 0.0);
	glClearColor(red, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GameState::updateUI() {
	int altitude = G->LP->position.y;
	if (altitude != UI.lastAltitude) {
		char str[15]; std::snprintf(str, 15, "Altitude: %d", altitude);
		UI.lastAltitude = altitude;
		UI.Altitude->setText(std::string(str));
	}
	if (showDebugInfo) {
		std::ostringstream oss;
		oss << std::setprecision(3) <<
			"x: " << G->LP->position.x << std::endl <<
			"y: " << G->LP->position.y << std::endl <<
			"z: " << G->LP->position.z << std::endl <<
			"vy: " << G->LP->velocity.y << std::endl;
		UI.DebugInfo->setText(oss.str());
	}
}

void GameState::drawUI() {
	G->UIM->render();
	m_chatBox->render();

	static _<Texture> tex(getAssetPath("tools", "tex_tool_build.png"), Texture::PixelFormat::RGBA);
	G->UIM->drawTexRect(UI::Element::Area {20, 20, 40, 40}, *tex);
}

bool GameState::processNetwork() {
	while (G->H.recv(m_msg, 0)) {
		switch (m_msg.getType()) {
			case Net::MessageType::Disconnect:
				W->showMessage("Disconnected", "Timed out");
				return false;

			case Net::MessageType::MapTransfer: {
				G->SC->readMsg(m_msg);
			} break;
			case Net::MessageType::Chat: {
				m_chatBox->addChatEntry(m_msg.readString());
			} break;
			case Net::MessageType::PlayerJoin: {
				Player &plr = G->players.add();
				plr.id = m_msg.readU32();
				plr.name = m_msg.readString();
				getDebugStream() << "Player " << plr.name << '(' << plr.id << ") joined the party!" << std::endl;
			} break;
			case Net::MessageType::PlayerQuit: {
				uint32 id = m_msg.readU32();
				try {
					Player &plr = G->players.getById(id);
					getOutputStream() << plr.name << " is gone :(" << std::endl;
					G->players.remove(plr);
				} catch (const std::out_of_range &e) {
					getOutputStream() << "Phantom player #" << id << " disconnected" << std::endl;
				}
			} break;
			case Net::MessageType::PlayerUpdate: {
				uint32 id = m_msg.readU32();
				try {
					Player &plr = G->players.getById(id);
					switch (m_msg.getSubtype()) {
						case Net::PlayerUpdateType::Move: {
							glm::vec3 pos = m_msg.readVec3(),
									  vel = m_msg.readVec3(),
									  acc = m_msg.readVec3();
							plr.setPosVel(pos, vel, acc);
						} break;
						case Net::PlayerUpdateType::Die:
							plr.setDead(false, (Player::DeathReason)m_msg.readU8());
							break;
						case Net::PlayerUpdateType::Respawn:
							plr.setDead(false);
							break;
						default:
							break;
					}
				} catch (const std::out_of_range &e) {
					getOutputStream() << "Invalid player update: #" << id << " is not on server" << std::endl;
				}
			} break;
			case Net::MessageType::MapUpdate: {
				int count  = m_msg.getSize() /
					(sizeof(uint16) * 3 + sizeof(uint8));
				for (int i=0; i < count; ++i) {
					int x = m_msg.readU16(),
						y = m_msg.readU16(),
						z = m_msg.readU16();
					uint8 b = m_msg.readU8();
					G->SC->set(x, y, z, static_cast<BlockType>(b));
				}
			} break;
			case Net::MessageType::Event: {
				switch (m_msg.getSubtype()) {
					case Net::EventType::ExplosivesBlow: {
						glm::vec3 pos = m_msg.readVec3();
						G->A->playSound("explosion", pos);
					} break;
					case Net::PlayerJumpOnPad: {
						uint32 id = m_msg.readU32();
						if (id == G->LP->id) { // Is it a-me?
							G->A->playSound("jumpblock");
						} else {
							Player &plr = G->players.getById(id);
							G->A->playSound("jumpblock", plr.position);
						}
					} break;
				}
			} break;
			default:
				break;
		}
	}
	return true;
}

}
