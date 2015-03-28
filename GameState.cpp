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

GameState::GameState(GameWindow *W, const std::string &servHost, int servPort)
	: W(W), m_serverHost(servHost), m_serverPort(servPort), bloom(*W->G) {
	G = W->G;
	int w = W->getW(),
		h = W->getH();

	// Initialized in setupUI
	UI.EM = nullptr;
	m_chatBox = nullptr;

	float coords[6*3*2*3] = {
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f, 
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		
		1.0f,-1.0f, 1.0f, 
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f, 
		1.0f, 1.0f,-1.0f, 
		1.0f,-1.0f,-1.0f, 
		-1.0f,-1.0f,-1.0f,
		
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f, 
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f, 
		1.0f, 1.0f, 1.0f, 
		1.0f,-1.0f,-1.0f, 
		1.0f, 1.0f,-1.0f, 
		
		1.0f,-1.0f,-1.0f, 
		1.0f, 1.0f, 1.0f, 
		1.0f,-1.0f, 1.0f, 
		1.0f, 1.0f, 1.0f, 
		1.0f, 1.0f,-1.0f, 
		-1.0f, 1.0f,-1.0f,
		
		1.0f, 1.0f, 1.0f, 
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f, 
	};
	m_highlightBox.vbo.setData(coords, 6*3*2*3);
	m_highlightBox.program = G->PM->getProgram(PM_3D);
	m_highlightBox.att_coord = m_highlightBox.program->att("coord");
	m_highlightBox.uni_unicolor = m_highlightBox.program->uni("unicolor");
	m_highlightBox.uni_mvp = m_highlightBox.program->uni("mvp");

	m_3dFbo = new FBO(w, h, Texture::PixelFormat::RGB, true);
	m_3dRenderVBO = new VBO();
	m_clouds = new Clouds(G, 32, 32, 4);
	//m_sky = new Skybox(G, getAssetPath("alpine"));
	m_3dFboRenderer = G->PM->getProgram(PM_2D | PM_TEXTURED); //getSpecialProgram("effect3dRender");
	m_3dFboRenderer_coord = m_3dFboRenderer->att("coord");
	m_3dFboRenderer_texcoord = m_3dFboRenderer->att("texcoord");
	m_3dFboRenderer_mvp = m_3dFboRenderer->uni("mvp");

	Coord2DTex renderQuad[6] = {
		{0, 0, 0, 0},
		{1, 0, 1, 0},
		{0, 1, 0, 1},

		{1, 1, 1, 1},
		{0, 1, 0, 1},
		{1, 0, 1, 0}
	};
	m_3dRenderVBO->setData(renderQuad, 6*sizeof(Coord2DTex));

	m_crossHair.tex = new Texture(getAssetPath("crosshair.png"), Texture::PixelFormat::RGBA);
	UI.headerBg.color = glm::vec4(0, 0, 0, .5f);

	//"\f0H\f1e\f2l\f3l\f4l\f5o \f6d\f7e\f8m\f9b\faa\fbz\fcz\fde\fes\ff,\n\f0ye see,it werks purrfektly :D\n(and also; it's optimized)"

	m_mouseLocked = false;
	nextNetUpdate = 0;
}

GameState::Bloom::Bloom(Game &G) {
	enable = true;
	scale = 4;

	extractor.fbo = new FBO(G.GW->getW()/scale, G.GW->getH()/scale, Texture::PixelFormat::RGBA);
	extractor.fbo->tex->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);
	extractor.prog = G.PM->getSpecialProgram("bloomExtractor");
	extractor.att_coord = extractor.prog->att("coord");
	extractor.att_texcoord = extractor.prog->att("texcoord");
	extractor.uni_mvp = extractor.prog->uni("mvp");

	renderer.fbo = new FBO(G.GW->getW()/scale, G.GW->getH()/scale, Texture::PixelFormat::RGBA);
	renderer.fbo->tex->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);
	renderer.prog = G.PM->getSpecialProgram("bloom");
	renderer.att_coord = renderer.prog->att("coord");
	renderer.att_texcoord = renderer.prog->att("texcoord");
	renderer.uni_mvp = renderer.prog->uni("mvp");
	renderer.uni_pixshift = renderer.prog->uni("pixshift");
}

GameState::Bloom::~Bloom() {
	delete extractor.fbo;
	delete renderer.fbo;
}

GameState::BuilderGun::BuilderGun() {
	tex = new Texture(getAssetPath("tools", "tex_tool_build.png"), Texture::PixelFormat::RGBA);
	for (uint i=0; i < sizeof(Blocks::TypeInfos)/sizeof(*Blocks::TypeInfos); ++i) {
		const Blocks::TypeInfo &inf = Blocks::TypeInfos[i];
		// TODO change to player's team
		if (inf.teamCanBuild & Blocks::TeamRed) {
			blockTexs.emplace_back(inf.type, new Texture(getAssetPath("icons", inf.icon), Texture::PixelFormat::RGB));
		}
	}
	select(1);
}

GameState::BuilderGun::~BuilderGun() {
	delete tex;
	for (auto tuple : blockTexs)
		delete std::get<1>(tuple);
}

void GameState::BuilderGun::select(int idx) {
	int max = blockTexs.size();
	if (idx < 0)
		idx = max-1;
	index = idx % max;
	auto tuple = blockTexs.at(index);
	currentBlock = std::get<0>(tuple);
	currentBlockTex = std::get<1>(tuple);
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
	delete m_clouds;
	delete m_crossHair.tex;
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
			bloom.enable = !bloom.enable;
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
	case GLFW_KEY_F7:
		if (action == GLFW_PRESS) {
			G->RP->wavingLiquids = !G->RP->wavingLiquids;
			G->SC->onRenderPropertiesChanged();
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
	if (!m_mouseLocked && action == GLFW_PRESS && !isEscapeToggled) {
		lockMouse();
	}
	
	if (action == GLFW_PRESS) {
		glm::ivec3 pointed, face;
		if (G->LP->raytracePointed(32, &pointed, &face, .1f)) {
			Net::OutMessage msg(Net::MessageType::MapUpdate);
			if (key == GLFW_MOUSE_BUTTON_LEFT) {
				msg.writeU16(pointed.x);
				msg.writeU16(pointed.y);
				msg.writeU16(pointed.z);
				msg.writeU8((uint8)BlockType::Air);
			} else {
				msg.writeU16(face.x);
				msg.writeU16(face.y);
				msg.writeU16(face.z);
				msg.writeU8((uint8)m_builderGun.currentBlock);
			}
			sendMsg(msg, Net::Tfer::Rel, Net::Channels::MapUpdate);
		}
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

	G->LP->angle = angles.x;
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
	if (y < 0)
		m_builderGun.select(m_builderGun.index-1);
	if (y > 0)
		m_builderGun.select(m_builderGun.index+1);
}

void GameState::updateViewport() {
	int w = W->getW(), h = W->getH();
	Diggler::UI::Manager &UIM = *G->UIM;
	glViewport(0, 0, w, h);
	G->LP->camera.setPersp((float)M_PI/180*75.0f, (float)w / h, 0.1f, 32.0f);
	m_3dFbo->resize(w, h);
	bloom.extractor.fbo->resize(w/bloom.scale, h/bloom.scale);
	//bloom.extractor.fbo->tex->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);
	bloom.renderer.fbo->resize(w/bloom.scale, h/bloom.scale);
	//bloom.renderer.fb->tex->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);

	{ int tw = 5*UIM.Scale, th = 5*UIM.Scale;
		m_crossHair.mat = glm::scale(glm::translate(*UIM.PM,
			glm::vec3((w-tw)/2, (h-th)/2, 0)),
			glm::vec3(tw, tw, 0));
	}

	{ int scale = 3, tw = 120*scale, th = 126*scale;
		m_builderGun.matGun = glm::scale(glm::translate(*UIM.PM,
			glm::vec3((w-tw)/2, -46*scale, 0)),
			glm::vec3(tw, th, 0));
		int iw = 39*scale, ih = 21*scale;
		m_builderGun.matIcon = glm::scale(glm::translate(*UIM.PM,
			glm::vec3((w-iw)/2-3*scale, 9*scale, 0)),
			glm::vec3(iw, ih, 0));
	}

	int lineHeight = G->F->getHeight()*UIM.Scale;
	char str[15]; std::snprintf(str, 15, "Loot: %d/%d", G->LP->ore, Player::getMaxOre(G->LP->playerclass));
	UI.Ore->setText(std::string(str));
	UI.Ore->setPos(2*UIM.Scale, h-lineHeight);

	UI.Loot->setText("Loot: $0");
	UI.Loot->setPos(w/6, h-lineHeight);

	UI.Weight->setText("Weight: 0");
	UI.Weight->setPos(w/3, h-lineHeight);

	UI.TeamOre->setText("Team ore: 0");
	UI.TeamOre->setPos(w/2, h-lineHeight);

	UI.RedCash->setText("\f4Red: $0");
	UI.RedCash->setPos((w*6)/8, h-lineHeight);

	UI.BlueCash->setText("\fbBlue: $0");
	UI.BlueCash->setPos((w*7)/8, h-lineHeight);

	UI.FPS->setPos(16, 16);
	UI.Altitude->setText("Altitude: XX");
	UI.Altitude->setPos(w-16-UI.Altitude->getSize().x, 16);
	UI.lastAltitude = INT_MAX;

	UI.headerBg.mat = glm::scale(glm::translate(*UIM.PM,
		glm::vec3(0, h-lineHeight, 0)),
		glm::vec3(w, lineHeight, 0));

	UI.DebugInfo->setPos(0, h-(lineHeight+UI.DebugInfo->getSize().y));

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
	while (!W->shouldClose()) {
		if (!processNetwork()) return;

		T = glfwGetTime(); deltaT = T - lastT;
		G->Time = T;
		if (T > fpsT) {
			char str[10]; std::sprintf(str, "FPS: %d", frames);
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
				msg.writeFloat(LP->angle);
				sendMsg(msg, Net::Tfer::Unrel, Net::Channels::Movement);
				nextNetUpdate = T+0.25;
			}
			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if (bloom.enable) {
				m_3dFbo->bind();
				glClearColor(0.0, 0.0, 0.0, 1.0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}

			LP->update(deltaT);
			if (LP->position.y < -32) {
				LP->setDead(true, Player::DeathReason::Void, true);
			}

			glm::mat4 m_transform = LP->getPVMatrix();

			/*** 3D PART ***/
			glm::mat4 cloudmat = glm::scale(glm::translate(m_transform, glm::vec3(0.f, G->SC->getChunksY()*CY+4, 0.f)), glm::vec3(G->SC->getChunksX()*CX, 1, G->SC->getChunksZ()*CZ));
			m_clouds->render(cloudmat);

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);

			G->SC->render(m_transform);
			for (Player &p : G->players) {
				p.update(deltaT);
				if (G->LP->camera.frustum.sphereInFrustum(p.position, 2))
					p.render(m_transform);
			}

			// TODO: move
			glm::ivec3 pointed, face;
			// TODO: replace harcoded 32 viewdistance
			if (G->LP->raytracePointed(32, &pointed, &face, .05f)) {
				m_highlightBox.program->bind();
				glEnableVertexAttribArray(m_highlightBox.att_coord);
				m_highlightBox.vbo.bind();
				glVertexAttribPointer(m_highlightBox.att_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
				
				glUniform4f(m_highlightBox.uni_unicolor, 1.f, 1.f, 1.f, .1f);
				glUniformMatrix4fv(m_highlightBox.uni_mvp, 1, GL_FALSE, glm::value_ptr(
					glm::scale(glm::translate(m_transform, glm::vec3(pointed)+glm::vec3(.5f)), glm::vec3(0.5f*1.03f))));
				glDrawArrays(GL_TRIANGLES, 0, 6*2*3);
				
				/*glUniform4f(m_highlightBox.uni_unicolor, 0.f, 0.f, 1.f, .2f);
				glUniformMatrix4fv(m_highlightBox.uni_mvp, 1, GL_FALSE, glm::value_ptr(
					glm::scale(glm::translate(m_transform, glm::vec3(face)+glm::vec3(.5f)), glm::vec3(0.40f+ sin(G->Time*4)*0.01f ))));
				glDrawArrays(GL_TRIANGLES, 0, 6*2*3);*/
				
				glDisableVertexAttribArray(m_highlightBox.att_coord);
			}

			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			LP->render(m_transform);

			if (bloom.enable) {
				m_3dFbo->unbind();
				G->UIM->drawFullTexV(*m_3dFbo->tex);

				m_3dFbo->tex->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);
				bloom.extractor.fbo->bind();
				glViewport(0, 0, W->getW()/bloom.scale, W->getH()/bloom.scale);
				glClearColor(0.f, 0.f, 0.f, 0.f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				bloom.extractor.prog->bind();
				glEnableVertexAttribArray(bloom.extractor.att_coord);
				glEnableVertexAttribArray(bloom.extractor.att_texcoord);
				m_3dRenderVBO->bind();
				glUniformMatrix4fv(bloom.extractor.uni_mvp, 1, GL_FALSE, glm::value_ptr(*G->GW->UIM.PM1));
				glVertexAttribPointer(bloom.extractor.att_coord, 2, GL_INT, GL_FALSE, sizeof(Coord2DTex), 0);
				glVertexAttribPointer(bloom.extractor.att_texcoord, 2, GL_BYTE, GL_FALSE, sizeof(Coord2DTex), (GLvoid*)offsetof(Coord2DTex, u));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisableVertexAttribArray(bloom.extractor.att_texcoord);
				glDisableVertexAttribArray(bloom.extractor.att_coord);
				m_3dFbo->tex->setFiltering(Texture::Filter::Nearest, Texture::Filter::Nearest);

				bloom.renderer.fbo->bind();
				glClearColor(0.f, 0.f, 0.f, 0.f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				bloom.extractor.fbo->tex->bind();
				bloom.renderer.prog->bind();
				glEnableVertexAttribArray(bloom.renderer.att_coord);
				glEnableVertexAttribArray(bloom.renderer.att_texcoord);
				m_3dRenderVBO->bind();
				glUniformMatrix4fv(bloom.renderer.uni_mvp, 1, GL_FALSE, glm::value_ptr(*G->GW->UIM.PM1));
				glUniform2f(bloom.renderer.uni_pixshift, 1.f/(W->getW()/bloom.scale), 1.f/(W->getH()/bloom.scale));
				glVertexAttribPointer(bloom.renderer.att_coord, 2, GL_INT, GL_FALSE, sizeof(Coord2DTex), 0);
				glVertexAttribPointer(bloom.renderer.att_texcoord, 2, GL_BYTE, GL_FALSE, sizeof(Coord2DTex), (GLvoid*)offsetof(Coord2DTex, u));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisableVertexAttribArray(bloom.renderer.att_texcoord);
				glDisableVertexAttribArray(bloom.renderer.att_coord);
				bloom.renderer.fbo->unbind();

				// render to real surface
				glViewport(0, 0, W->getW(), W->getH());
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				bloom.extractor.fbo->tex->bind();
				bloom.extractor.fbo->tex->setFiltering(Texture::Filter::Linear, Texture::Filter::Linear);
				bloom.renderer.prog->bind();
				glEnableVertexAttribArray(bloom.renderer.att_coord);
				glEnableVertexAttribArray(bloom.renderer.att_texcoord);
				m_3dRenderVBO->bind();
				glUniformMatrix4fv(bloom.renderer.uni_mvp, 1, GL_FALSE, glm::value_ptr(*G->GW->UIM.PM1));
				glVertexAttribPointer(bloom.renderer.att_coord, 2, GL_INT, GL_FALSE, sizeof(Coord2DTex), 0);
				glVertexAttribPointer(bloom.renderer.att_texcoord, 2, GL_BYTE, GL_FALSE, sizeof(Coord2DTex), (GLvoid*)offsetof(Coord2DTex, u));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glDisableVertexAttribArray(bloom.renderer.att_texcoord);
				glDisableVertexAttribArray(bloom.renderer.att_coord);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}

			/*** 2D PART ***/
			G->UIM->drawFullRect(glm::vec4(1.f, 0.f, 0.f, 1-G->LP->health));
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
	LocalPlayer &LP = *G->LP;
	int altitude = LP.position.y;
	if (altitude != UI.lastAltitude) {
		char str[15]; std::snprintf(str, 15, "Altitude: %d", altitude);
		UI.lastAltitude = altitude;
		UI.Altitude->setText(std::string(str));
	}
	if (showDebugInfo) {
		int verts = 0;
		const static glm::vec3 cShift(Chunk::MidX, Chunk::MidY, Chunk::MidZ);
		for (int x = 0; x < G->SC->getChunksX(); x++)
			for (int y = 0; y < G->SC->getChunksY(); y++)
				for (int z = 0; z < G->SC->getChunksZ(); z++)
					if (G->SC->getChunk(x, y, z))
						if (G->LP->camera.frustum.sphereInFrustum(glm::vec3(x * CX, y * CY, z * CZ) + cShift, Chunk::CullSphereRadius)) {
							verts += G->SC->getChunk(x, y, z)->vertices;
						}
		verts /= 3;
		std::ostringstream oss;
		oss << std::setprecision(3) <<
			"HP: " << LP.health << std::endl <<
			"x: " << LP.position.x << std::endl <<
			"y: " << LP.position.y << std::endl <<
			"z: " << LP.position.z << std::endl <<
			"vy: " << LP.velocity.y << std::endl <<
			"rx: " << LP.angle << std::endl <<
			"chunk tris: " << verts;
		UI.DebugInfo->setText(oss.str());
	}
}

void GameState::drawUI() {
	G->UIM->drawRect(UI.headerBg.mat, UI.headerBg.color);
	G->UIM->render();
	m_chatBox->render();

	G->UIM->drawTex(m_crossHair.mat, *m_crossHair.tex);
	// TODO render weapon
	G->UIM->drawTex(m_builderGun.matGun, *m_builderGun.tex);
	G->UIM->drawTex(m_builderGun.matIcon, *m_builderGun.currentBlockTex);
}

bool GameState::processNetwork() {
	while (G->H.recv(m_msg, 0)) {
		switch (m_msg.getType()) {
			case Net::MessageType::Disconnect:
				W->showMessage("Disconnected", "Timed out");
				return false;

			case Net::MessageType::MapTransfer: {
				G->SC->read(m_msg);
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
							plr.angle = m_msg.readFloat();
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
