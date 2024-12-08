/**
*              PVZ Animation Explorer
*
*    Copyright (c) 2023-2024  Tyler Parret True
*
* @section LICENSE
*
*                   GNU AFFERO GENERAL PUBLIC LICENSE
*                      Version 3, 19 November 2007
*
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Affero General Public License as published
*    by the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*
* @author  Tyler Parret True (OwlHowlinMornSky) <mysteryworldgod@outlook.com>
*/

#include "AnimationCore/Animate.h"

#include "Win32OHMS.h"

sf::RenderWindow app;
sf::Clock sfclock;
ohms::pvzanim::Animate* test;
sf::Transform animTrans;
sf::Text textFrame;
sf::Text textTrans;
sf::Text textPos;
sf::Text textTimeScale;
sf::Font fontMono;
bool running;
float scaleRatio;
float rotateRatio;
uint8_t timeScale;
sf::Texture texBackgnd;
sf::Vector2u viewSize;

void initialize_animTrans();
void initialize_font_text();
void initialize_callback();
void initialize_window();
void initialize_texture_coord();

void myEnterSYSLOOP();
void myExitSYSLOOP();
void myIdle();

void update_textTrans();
void update_textPos(float, float);
void update_textTimeScale();
void update_view();

int main() {
	initialize_callback();
	initialize_font_text();
	initialize_animTrans();
	initialize_texture_coord();
	update_textTrans();
	update_textPos(INFINITY, 0.0f);
	timeScale = 10;
	update_textTimeScale();

	ohms::pvzanim::AnimData testdata;
	testdata.load("assets/Zombie.reanim");
	//testdata.load("assets/Zombie_polevaulter.reanim");
	//testdata.load("assets/Zombie_bungi.reanim");
	//testdata.load("assets/Zombie_pogo.reanim");
	//testdata.load("assets/Zombie_balloon.reanim");
	//testdata.load("assets/Zombie_gargantuar.reanim");
	//testdata.load("assets/Cabbagepult.reanim");
	//testdata.load("assets/Blover.reanim");
	//testdata.load("assets/CrazyDave.reanim");
	//testdata.load("assets/Sunflower.reanim");
	//testdata.load("assets/Chomper.reanim");
	//testdata.load("assets/PeaShooter.reanim");
	//testdata.load("assets/CherryBomb.reanim");
	//testdata.load("assets/DoomShroom.reanim");
	//testdata.load("assets/FirePea.reanim");
	//testdata.load("assets/SelectorScreen.reanim");
	//testdata.load("assets/Sun.reanim");
	//testdata.load("assets/PotatoMine.reanim");
	//testdata.load("assets/Squash.reanim");
	//testdata.load("assets/SunShroom.reanim");
	//testdata.load("assets/Wallnut.reanim");
	//testdata.load("assets/Tallnut.reanim");
	//testdata.load("assets/Z.reanim");
	//testdata.load("assets/Hammer.reanim");
	//testdata.load("assets/ZenGarden_phonograph.reanim");
	//testdata.load("assets/ZenGarden_wateringcan.reanim");
	//testdata.load("assets/Zombie_disco.reanim");
	//testdata.load("assets/Zombie_jackson.reanim");
	//testdata.load("assets/Zombie_jackbox.reanim");
	//testdata.load("assets/Zombie_snorkle.reanim");

	testdata.listControlTrack();
	printf_s("LIST:\n\n");
	testdata.listTrack();

	test = testdata.create(true);
	//if (!test->setAnimation("anim_eat")) {
	//if (!test->setAnimation("anim_swim")) {
	//if (!test->setAnimation("anim_run")) {
	//if (!test->setAnimation("anim_pogo")) {
	//if (!test->setAnimation("anim_shooting")) {
	//if (!test->setAnimation("anim_loop")) {
	if (!test->setAnimation("anim_idle")) {
	//if (!test->setAnimation("anim_walk")) {
	//if (!test->setAnimation("anim_smash")) {
	//if (!test->setAnimation("anim_armed")) {
	//if (!test->setAnimation("anim_explode")) {
	//if (!test->setAnimation("anim_sleep")) {
	//if (!test->setAnimation("anim_full_idle")) {
	//if (!test->setAnimation("anim_head_idle")) {
	//if (!test->setAnimation("anim_idle_handing")) {
	//if (!test->setAnimation("anim_blink")) {
	//if (!test->setAnimation("anim_grab")) {
	//if (!test->setAnimation("anim_enterup")) {
		return 0;
	}

	// Normal
	{
		test->setFragmentDisabled("Zombie_mustache", true);
		//test->setFragmentDisabled("anim_tongue", true);
		//test->setFragmentDisabled("anim_hair", true);

		test->setFragmentDisabled("anim_bucket", true);
		test->setFragmentDisabled("anim_cone", true);
	}
	// Screen Door
	{
		// On
		test->setFragmentDisabled("anim_screendoor", true);
		test->setFragmentDisabled("Zombie_innerarm_screendoor", true);
		test->setFragmentDisabled("Zombie_innerarm_screendoor_hand", true);
		test->setFragmentDisabled("Zombie_outerarm_screendoor", true);

		// Off
		/*test->setFragmentDisabled("anim_innerarm1", true);
		test->setFragmentDisabled("anim_innerarm2", true);
		test->setFragmentDisabled("anim_innerarm3", true);
		test->setFragmentDisabled("Zombie_outerarm_lower", true);
		test->setFragmentDisabled("Zombie_outerarm_upper", true);
		test->setFragmentDisabled("Zombie_outerarm_hand", true);*/
	}
	test->setFragmentDisabled("Zombie_flaghand", true);

	test->setFragmentDisabled("Zombie_duckytube", true);
	test->setFragmentDisabled("Zombie_whitewater", true);
	test->setFragmentDisabled("Zombie_whitewater2", true);

	//test->renderToFrames(60, 3.0f);
	//return 0;

	initialize_window();

	// Message Pump
	MSG msg = { 0 };
	running = true;
	sfclock.restart();
	while (running) {
		if (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else {
			myIdle();
		}
	}
	app.close();

	return 0;
}

void myIdle() {
	static sf::Vector2f l_mouseLastPos;
	static bool l_pressing = false;

	sf::Event evt;
	while (app.pollEvent(evt)) {
		switch (evt.type) {
		case sf::Event::Closed:
			running = false;
			break;
		case sf::Event::Resized:
			app.setView(sf::View({ 0, 0, (float)evt.size.width, (float)evt.size.height }));
			update_view();
			break;
		case sf::Event::KeyPressed:
			switch (evt.key.code) {
			case sf::Keyboard::R:
				initialize_animTrans();
				break;
			case sf::Keyboard::Left:
				if (timeScale > 0)
					--timeScale;
				test->setTimeScale(timeScale / 10.0f);
				update_textTimeScale();
				break;
			case sf::Keyboard::Right:
				if (timeScale < 100)
					++timeScale;
				test->setTimeScale(timeScale / 10.0f);
				update_textTimeScale();
				break;
			default:;
			}
			break;
		case sf::Event::MouseWheelScrolled:
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
				sf::Vector2f mPos((float)evt.mouseWheelScroll.x, (float)evt.mouseWheelScroll.y);
				mPos = animTrans.getInverse().transformPoint(mPos);
				float ratio = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ? 2.0f : 1.0f;
				ratio = -3.0f * ratio * evt.mouseWheelScroll.delta;
				animTrans.rotate(ratio, mPos);
				rotateRatio -= ratio;
				if (rotateRatio <= -0.0f) rotateRatio += 360.0f;
				rotateRatio = std::fmodf(rotateRatio, 360.0f);
			}
			else {
				sf::Vector2f mPos((float)evt.mouseWheelScroll.x, (float)evt.mouseWheelScroll.y);
				mPos = animTrans.getInverse().transformPoint(mPos);
				float ratio = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ? 1.21f : 1.1f;
				ratio = std::powf(ratio, evt.mouseWheelScroll.delta);
				float fratio = scaleRatio * ratio;
				if (fratio > 10000.0f) {
					fratio = 10000.0f;
					ratio = 10000.0f / scaleRatio;
				}
				else if (fratio < 5.0f) {
					fratio = 5.0f;
					ratio = 5.0f / scaleRatio;
				}
				animTrans.scale(sf::Vector2f(ratio, ratio), mPos);
				scaleRatio = fratio;
			}
			update_textTrans();
			break;
		}
		case sf::Event::MouseButtonPressed:
			if (evt.mouseButton.button == sf::Mouse::Left) {
				l_mouseLastPos.x = (float)evt.mouseButton.x;
				l_mouseLastPos.y = (float)evt.mouseButton.y;
				l_pressing = true;
			}
			break;
		case sf::Event::MouseButtonReleased:
			if (evt.mouseButton.button == sf::Mouse::Left) {
				l_pressing = false;
			}
			break;
		case sf::Event::MouseMoved:
		{
			sf::Vector2f mPos((float)evt.mouseMove.x, (float)evt.mouseMove.y);
			sf::Transform IV = animTrans.getInverse();
			sf::Vector2f truePos(IV.transformPoint(mPos));
			if (l_pressing) {
				animTrans.translate(truePos - IV.transformPoint(l_mouseLastPos));
				l_mouseLastPos = mPos;
			}
			update_textPos(truePos.x, truePos.y);
			break;
		}
		case sf::Event::MouseLeft:
			update_textPos(INFINITY, 0.0f);
			break;
		default:;
		}
	}

	float dt = sfclock.restart().asSeconds();
	if (timeScale > 0)
		test->update(dt);

	textFrame.setString(test->getFrameInfoString());

	app.clear(sf::Color(0xCC, 0xCC, 0xCC));

	{
		sf::Vector2f p[4];
		p[0] = { 0.0f, 0.0f };
		p[1] = { (float)viewSize.x, 0.0f };
		p[2] = { (float)viewSize.x, (float)viewSize.y };
		p[3] = { 0.0f, (float)viewSize.y };
		sf::VertexArray va;
		for (int i = 0; i < 4; ++i) {
			sf::Vertex vertex;
			vertex.position = p[i];
			vertex.texCoords = animTrans.getInverse().transformPoint(p[i]) + sf::Vector2f(0.5f, 0.5f);
			va.append(vertex);
		}
		va.setPrimitiveType(sf::Quads);
		if (scaleRatio >= 500.0f)
			texBackgnd.setSmooth(false);
		else
			texBackgnd.setSmooth(true);
		app.draw(va, sf::RenderStates(&texBackgnd));
	}

	app.draw(*test, { animTrans });

	app.draw(textFrame);
	app.draw(textTrans);
	app.draw(textTimeScale);
	app.draw(textPos);

	return app.display();
}

void update_view() {
	viewSize = app.getSize();
}

void update_textTimeScale() {
	char tmp[64];
	sprintf_s(tmp, 64, "Speed: %.0f%%.", timeScale / 0.1f);
	return textTimeScale.setString(tmp);
}

void update_textTrans() {
	wchar_t tmp[64];
	swprintf_s(tmp, 64, L"Scale: %.1f%%.\nRotate: %.1f°.", scaleRatio, rotateRatio);
	return textTrans.setString(tmp);
}

void update_textPos(float px, float py) {
	if (px == INFINITY && py == 0.0f)
		return textPos.setString("x: -, y:-.");
	char tmp[64];
	sprintf_s(tmp, 64, "x: %.2f, y: %.2f.", px, py);
	return textPos.setString(tmp);
}

void myEnterSYSLOOP() {
	return app.setFramerateLimit(0);
}

void myExitSYSLOOP() {
	return app.setFramerateLimit(60);
}

void initialize_animTrans() {
	animTrans = sf::Transform::Identity;
	animTrans.translate(100.0f, 120.0f);
	scaleRatio = 100.0f;
	rotateRatio = 0.0f;
	return update_textTrans();
}

void initialize_font_text() {
	fontMono.loadFromFile("CascadiaMono-Regular.otf");
	textFrame.setFont(fontMono);
	textFrame.setFillColor(sf::Color::White);
	textFrame.setCharacterSize(24);
	textFrame.setPosition(12.0f, 7.0f);
	textFrame.setOutlineColor(sf::Color::Black);
	textFrame.setOutlineThickness(2.5f);

	textTrans.setFont(fontMono);
	textTrans.setFillColor(sf::Color::White);
	textTrans.setCharacterSize(18);
	textTrans.setPosition(14.0f, 36.0f);
	textTrans.setOutlineColor(sf::Color::Black);
	textTrans.setOutlineThickness(2.0f);

	textTimeScale.setFont(fontMono);
	textTimeScale.setFillColor(sf::Color::White);
	textTimeScale.setCharacterSize(18);
	textTimeScale.setPosition(14.0f, 78.0f);
	textTimeScale.setOutlineColor(sf::Color::Black);
	textTimeScale.setOutlineThickness(2.0f);

	textPos.setFont(fontMono);
	textPos.setFillColor(sf::Color::White);
	textPos.setCharacterSize(18);
	textPos.setPosition(14.0f, 104.0f);
	textPos.setOutlineColor(sf::Color::Black);
	textPos.setOutlineThickness(2.0f);

	return;
}

void initialize_callback() {
	OnIdle = myIdle;
	OnEnterSYSLOOP = myEnterSYSLOOP;
	OnExitSYSLOOP = myExitSYSLOOP;
	return;
}

void initialize_window() {
	HINSTANCE hInst = GetModuleHandleW(NULL);
	if (!MyRegistClass(hInst)) {
		MessageBoxW(0, L"Regist Class: Failed!", L"Error", MB_ICONERROR);
		throw std::exception();
	}
	HWND hWnd0 = CreateWindowW(L"WINDOWS_OHMS_CLASS_PVZANIMATION", L"PVZ Animation",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		0, 0, hInst, 0);
	if (!hWnd0) {
		MessageBoxW(NULL, L"Create Window: Failed!", L"Error", MB_ICONERROR);
		throw std::exception();
	}

	app.create(hWnd0, sf::ContextSettings(0, 0, 8));
	app.setSize({ 800, 600 });
	app.setView(sf::View({ 0, 0, 800.0f, 600.0f }));

	update_view();
	return myExitSYSLOOP();
}

void initialize_texture_coord() {
	sf::Image tmp;
	tmp.create(100, 100, sf::Color(0, 0, 0, 0));
	for (int i = 1; i < 10; ++i) {
		for (int j = 0; j < 100; ++j) {
			tmp.setPixel(i * 10, j, sf::Color(0, 0, 0, 0x22));
			tmp.setPixel(j, i * 10, sf::Color(0, 0, 0, 0x22));
		}
	}
	for (int i = 0; i < 100; ++i) {
		tmp.setPixel(50, i, sf::Color(0, 0, 0, 0x55));
		tmp.setPixel(i, 50, sf::Color(0, 0, 0, 0x55));
	}
	for (int i = 0; i < 100; ++i) {
		tmp.setPixel(0, i, sf::Color(0, 0, 0, 0xcc));
		tmp.setPixel(i, 0, sf::Color(0, 0, 0, 0xcc));
	}
	texBackgnd.loadFromImage(tmp);
	texBackgnd.setRepeated(true);
	texBackgnd.generateMipmap();
	return;
}
