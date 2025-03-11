#pragma once

#include "AnimationCore/Animate.h"

class MainWindow {
public:
	MainWindow();
	~MainWindow();

public:
	void init();
	void run();

private:
	void initialize_callback();
	void initialize_font_text();
	void initialize_window();
	void initialize_texture_coord();
	void initialize_animTrans();

	void update_textTrans();
	void update_textPos(float px, float py);
	void update_textTimeScale();
	void update_view();

	void onOpenClose(bool isOpen, std::wstring_view file);
	void onClickAnimTrack(size_t offset);
	void onClickAnimTrackCtrl(size_t offset);

	void myEnterSYSLOOP();
	void myExitSYSLOOP();
	void myIdle();

private:
	sf::RenderWindow app;
	sf::Clock sfclock;
	ohms::pvzanim::AnimData testdata;
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
};

