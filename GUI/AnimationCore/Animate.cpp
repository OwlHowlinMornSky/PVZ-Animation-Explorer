/**
* @file    Animate.cpp
* @author  Tyler Parret True (OwlHowlinMornSky) <mysteryworldgod@outlook.com>
*
* @section LICENSE
*
*    Copyright (c) 2023-2025  Tyler Parret True
*
*                   GNU AFFERO GENERAL PUBLIC LICENSE
*                      Version 3, 19 November 2007
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
*/

#include "Animate.h"
#include <iostream>
#include <format>

namespace ohms {
namespace pvzanim {

Animate::Animate(const AnimData& data, bool linearFrameInsertion) :
	m_dataRef(data),
	m_linearFI(linearFrameInsertion),
	m_playInfo(data.m_trackArray.size()),
	m_timeScale(1.0f) {}

bool Animate::setAnimation(const std::string& name) {
	if (!m_dataRef.m_available)
		return false;

	if (m_playInfo.ctrlTrackRef && (size_t)m_playInfo.ctrlTrackRef != 1 && name == m_playInfo.ctrlTrackRef->m_name) {
		m_playInfo.timePoint = 0.0f;
		update(0.0f);
		return true;
	}

	auto it = m_dataRef.m_trackNameTable.find(name);

	if (it == m_dataRef.m_trackNameTable.end()) {
		std::cerr << "Animaiton: Cannot find track: \'" << name << "\'.\n\n";
		return false;
	}

	m_playInfo.ctrlTrackRef = m_dataRef.m_trackArray[it->second];


	m_playInfo.trackOffset = 0;

	while ((m_playInfo.trackOffset < m_dataRef.m_frameCount) &&
		m_playInfo.ctrlTrackRef->getFrame(m_playInfo.trackOffset).isEmpty)
		++m_playInfo.trackOffset;

	m_playInfo.trackLength = m_playInfo.trackOffset;

	while ((m_playInfo.trackLength < m_dataRef.m_frameCount) &&
		!m_playInfo.ctrlTrackRef->getFrame(m_playInfo.trackLength).isEmpty)
		++m_playInfo.trackLength;

	size_t checkFlag = m_playInfo.trackLength;

	while (checkFlag < m_dataRef.m_frameCount) {
		if (!m_playInfo.ctrlTrackRef->getFrame(checkFlag).isEmpty) {
			std::cerr << "Animaiton: This track cannot be used as control track: \'" << name << "\'.\n\n";
			return false;
		}
		++checkFlag;
	}

	m_playInfo.trackLength = m_playInfo.trackLength - m_playInfo.trackOffset - 1;

	m_playInfo.timePoint = 0.0f;

	m_playInfo.lastFramePoint = m_playInfo.trackLength;

	for (size_t i = 0; i < m_playInfo.trackCount; ++i) {
		TrackFragData& ii = m_playInfo.fragments[i];

		for (size_t j = 0; j <= m_playInfo.trackOffset; ++j) {
			const FrameData& tf = m_dataRef.m_trackArray[i]->getFrame(j);

			ii.isEmpty = tf.isEmpty;
			if (tf.imName != "NULL") {
				ii.texture.loadFromFile((m_dataRef.m_parentPath / (tf.imName + ".png")).string());
				ii.texture.setSmooth(true);
				ii.texture.generateMipmap();
				ii.texSize = ii.texture.getSize();
			}
		}

		const FrameData& thisFrame = m_dataRef.m_trackArray[i]->getFrame(m_playInfo.trackOffset);

		float lpx, lpy, lkx, lky, lsx, lsy, lap, coskx, sinkx, cosky, sinky;

		lpx = thisFrame.px;
		lpy = thisFrame.py;
		lkx = thisFrame.kx * 3.1415926535897932f / 180.0f;
		lky = thisFrame.ky * 3.1415926535897932f / 180.0f;
		lsx = thisFrame.sx;
		lsy = thisFrame.sy;
		lap = thisFrame.a * 256.0f;

		int16_t lALPHA = static_cast<int16_t>(lap);
		if (lALPHA > 255) lALPHA = 255;
		else if (lALPHA < 0) lALPHA = 0;

		coskx = cos(lkx);
		sinkx = sin(lkx);
		cosky = cos(lky);
		sinky = sin(lky);

		ii.vertex[0].position = ii.vertex[0].texCoords = { 0.0f, 0.0f };
		ii.vertex[1].position = ii.vertex[1].texCoords = { (float)ii.texSize.x, 0.0f };
		ii.vertex[2].position = ii.vertex[2].texCoords = { 0.0f, (float)ii.texSize.y };
		ii.vertex[3].position = ii.vertex[3].texCoords = { (float)ii.texSize.x, (float)ii.texSize.y };

		for (unsigned char i = 0; i < 4; ++i) {
			ii.vertex[i].position.x *= lsx;
			ii.vertex[i].position.y *= lsy;
		}

		for (unsigned char i = 0; i < 4; ++i) {
			ii.vertex[i].position = { ii.vertex[i].position.x * coskx - ii.vertex[i].position.y * sinky,
				ii.vertex[i].position.x * sinkx + ii.vertex[i].position.y * cosky };
		}

		for (unsigned char i = 0; i < 4; ++i) {
			ii.vertex[i].position += { lpx, lpy };
			ii.vertex[i].color = { 255, 255, 255, static_cast<uint8_t>(lALPHA) };
		}
	}

	std::cerr << "Animation: Set track: \'" << name << "\'. Length: " << m_playInfo.trackLength << ".\n\n";

	return true;
}

bool Animate::setEntireTrack() {
	m_playInfo.timePoint = 0.0f;
	m_playInfo.trackOffset = 0;
	m_playInfo.trackLength = m_dataRef.m_frameCount - 1;
	m_playInfo.ctrlTrackRef = (ohms::pvzanim::AnimTrack*)0x1;
	m_playInfo.lastFramePoint = m_dataRef.m_frameCount;

	for (size_t i = 0; i < m_playInfo.trackCount; ++i) {
		TrackFragData& ii = m_playInfo.fragments[i];

		for (size_t j = 0; j <= m_playInfo.trackOffset; ++j) {
			const FrameData& tf = m_dataRef.m_trackArray[i]->getFrame(j);

			ii.isEmpty = tf.isEmpty;
			if (tf.imName != "NULL") {
				ii.texture.loadFromFile((m_dataRef.m_parentPath / (tf.imName + ".png")).string());
				ii.texture.setSmooth(true);
				ii.texture.generateMipmap();
				ii.texSize = ii.texture.getSize();
			}
		}

		const FrameData& thisFrame = m_dataRef.m_trackArray[i]->getFrame(m_playInfo.trackOffset);

		float lpx, lpy, lkx, lky, lsx, lsy, lap, coskx, sinkx, cosky, sinky;

		lpx = thisFrame.px;
		lpy = thisFrame.py;
		lkx = thisFrame.kx * 3.1415926535897932f / 180.0f;
		lky = thisFrame.ky * 3.1415926535897932f / 180.0f;
		lsx = thisFrame.sx;
		lsy = thisFrame.sy;
		lap = thisFrame.a * 256.0f;

		int16_t lALPHA = static_cast<int16_t>(lap);
		if (lALPHA > 255) lALPHA = 255;
		else if (lALPHA < 0) lALPHA = 0;

		coskx = cos(lkx);
		sinkx = sin(lkx);
		cosky = cos(lky);
		sinky = sin(lky);

		ii.vertex[0].position = ii.vertex[0].texCoords = { 0.0f, 0.0f };
		ii.vertex[1].position = ii.vertex[1].texCoords = { (float)ii.texSize.x, 0.0f };
		ii.vertex[2].position = ii.vertex[2].texCoords = { 0.0f, (float)ii.texSize.y };
		ii.vertex[3].position = ii.vertex[3].texCoords = { (float)ii.texSize.x, (float)ii.texSize.y };

		for (unsigned char i = 0; i < 4; ++i) {
			ii.vertex[i].position.x *= lsx;
			ii.vertex[i].position.y *= lsy;
		}

		for (unsigned char i = 0; i < 4; ++i) {
			ii.vertex[i].position = { ii.vertex[i].position.x * coskx - ii.vertex[i].position.y * sinky,
				ii.vertex[i].position.x * sinkx + ii.vertex[i].position.y * cosky };
		}

		for (unsigned char i = 0; i < 4; ++i) {
			ii.vertex[i].position += { lpx, lpy };
			ii.vertex[i].color = { 255, 255, 255, static_cast<uint8_t>(lALPHA) };
		}
	}

	std::cerr << "Animation: Set entire track. Length: " << m_playInfo.trackLength << ".\n\n";

	return true;
}

void Animate::stop() {
	if (m_playInfo.ctrlTrackRef) {
		m_playInfo.timePoint = 0.0f;
		update(0.0f);
		m_playInfo.ctrlTrackRef = nullptr;
	}
}

void Animate::update(float dt) {
	if (!m_playInfo.ctrlTrackRef)
		return;
	dt *= m_timeScale;

	m_playInfo.timePoint += dt;

	while (m_playInfo.timePoint >= static_cast<float>(m_playInfo.trackLength) / m_dataRef.m_fps)
		m_playInfo.timePoint -= static_cast<float>(m_playInfo.trackLength) / m_dataRef.m_fps;

	size_t framePoint = static_cast<size_t>(m_playInfo.timePoint * m_dataRef.m_fps);
	float mixtime = m_playInfo.timePoint * m_dataRef.m_fps - framePoint;

	if (framePoint != m_playInfo.lastFramePoint) {
		for (size_t i = 0; i < m_playInfo.trackCount; ++i) {
			TrackFragData& ii = m_playInfo.fragments[i];

			for (size_t j = m_playInfo.lastFramePoint + 1,
				n = framePoint + (j > framePoint ? m_playInfo.trackLength : 0);
				j <= n; ++j) {
				const FrameData& tf =
					m_dataRef.m_trackArray[i]->getFrame(m_playInfo.trackOffset + j % m_playInfo.trackLength);

				if (tf.imName != "NULL") {
					ii.texture.loadFromFile((m_dataRef.m_parentPath / (tf.imName + ".png")).string());
					ii.texture.setSmooth(true);
					ii.texture.generateMipmap();
					ii.texSize = ii.texture.getSize();
				}
			}
			ii.isEmpty = m_dataRef.m_trackArray[i]->getFrame(m_playInfo.trackOffset + framePoint).isEmpty;
		}
		m_playInfo.lastFramePoint = framePoint % m_playInfo.trackLength;
	}

	for (size_t i = 0, n = m_playInfo.trackCount; i < n; ++i) {
		TrackFragData& ii = m_playInfo.fragments[i];

		const FrameData& thisFrame =
			m_dataRef.m_trackArray[i]->getFrame(m_playInfo.lastFramePoint + m_playInfo.trackOffset);
		const FrameData& nextFrame =
			m_dataRef.m_trackArray[i]->getFrame(
				(m_playInfo.lastFramePoint + m_playInfo.trackOffset + 1 < m_dataRef.m_trackArray[i]->m_frameArray.size()) ?
				m_playInfo.lastFramePoint + m_playInfo.trackOffset + 1 :
				m_playInfo.trackOffset
			);

		float lpx, lpy, lkx, lky, lsx, lsy, lap, coskx, sinkx, cosky, sinky;

		if (m_linearFI && !nextFrame.isEmpty) {
			lpx = thisFrame.px * (1.0f - mixtime) + nextFrame.px * mixtime;
			lpy = thisFrame.py * (1.0f - mixtime) + nextFrame.py * mixtime;
			if (thisFrame.kx - nextFrame.kx > 180.0f) {
				lkx = (thisFrame.kx * (1.0f - mixtime) + (nextFrame.kx + 360.0f) * mixtime) * 3.1415926535897932f / 180.0f;
			}
			else {
				lkx = (thisFrame.kx * (1.0f - mixtime) + nextFrame.kx * mixtime) * 3.1415926535897932f / 180.0f;
			}
			if (thisFrame.ky - nextFrame.ky > 180.0f) {
				lky = (thisFrame.ky * (1.0f - mixtime) + (nextFrame.ky + 360.0f) * mixtime) * 3.1415926535897932f / 180.0f;
			}
			else {
				lky = (thisFrame.ky * (1.0f - mixtime) + nextFrame.ky * mixtime) * 3.1415926535897932f / 180.0f;
			}
			lsx = thisFrame.sx * (1.0f - mixtime) + nextFrame.sx * mixtime;
			lsy = thisFrame.sy * (1.0f - mixtime) + nextFrame.sy * mixtime;
			lap = (thisFrame.a * (1.0f - mixtime) + nextFrame.a * mixtime) * 256.0f;
		}
		else {
			lpx = thisFrame.px;
			lpy = thisFrame.py;
			lkx = thisFrame.kx * 3.1415926535897932f / 180.0f;
			lky = thisFrame.ky * 3.1415926535897932f / 180.0f;
			lsx = thisFrame.sx;
			lsy = thisFrame.sy;
			lap = thisFrame.a * 256.0f;
		}
		int16_t lALPHA = static_cast<int16_t>(lap);
		if (lALPHA > 255) lALPHA = 255;
		else if (lALPHA < 0) lALPHA = 0;

		coskx = cos(lkx);
		sinkx = sin(lkx);
		cosky = cos(lky);
		sinky = sin(lky);

		ii.vertex[0].position = ii.vertex[0].texCoords = { 0.0f, 0.0f };
		ii.vertex[1].position = ii.vertex[1].texCoords = { (float)ii.texSize.x, 0.0f };
		ii.vertex[2].position = ii.vertex[2].texCoords = { 0.0f, (float)ii.texSize.y };
		ii.vertex[3].position = ii.vertex[3].texCoords = { (float)ii.texSize.x, (float)ii.texSize.y };

		for (unsigned char i = 0; i < 4; ++i) {
			ii.vertex[i].position.x *= lsx;
			ii.vertex[i].position.y *= lsy;
		}

		for (unsigned char i = 0; i < 4; ++i) {
			ii.vertex[i].position = { ii.vertex[i].position.x * coskx - ii.vertex[i].position.y * sinky,
				ii.vertex[i].position.x * sinkx + ii.vertex[i].position.y * cosky };
		}

		for (unsigned char i = 0; i < 4; ++i) {
			ii.vertex[i].position += { lpx, lpy };
			ii.vertex[i].color = { 255, 255, 255, static_cast<uint8_t>(lALPHA) };
		}

	}


	return;
}

void Animate::setTimeScale(float nv) {
	m_timeScale = nv;
}

const std::string& Animate::getFrameInfoString() const {
	if (m_linearFI)
		m_frameInfo = std::format("Frame: {:04.1f} / {:02}.0", m_playInfo.timePoint * m_dataRef.m_fps, m_playInfo.trackLength);
	else
		m_frameInfo = std::format("Frame: {:02} / {:02}", m_playInfo.lastFramePoint, m_playInfo.trackLength);
	return m_frameInfo;
}

void Animate::renderToFrames(size_t totalFrameInterval, float scale) {
	if (!m_playInfo.ctrlTrackRef)
		return;

	sf::Transform transform;
	transform.scale(scale, scale);

	sf::Vector2f min, max;
	min.x = INFINITY;
	min.y = INFINITY;
	max.x = -INFINITY;
	max.y = -INFINITY;

	float totalTime = m_playInfo.trackLength * 1.0f / m_dataRef.m_fps;

	for (int j = 0; j <= totalFrameInterval; ++j) {
		float t = totalTime * j / totalFrameInterval;
		if (t > totalTime)
			t = totalTime;
		m_playInfo.timePoint = t;
		update(0.0f);
		for (size_t i = 0, n = m_playInfo.trackCount; i < n; ++i) {
			if (m_playInfo.fragments[i].isEmpty || m_playInfo.fragments[i].disabled) continue;
			for (int k = 0; k < 4; ++k) {
				auto vp = transform.transformPoint(m_playInfo.fragments[i].vertex[k].position);
				if (min.x > vp.x) {
					min.x = vp.x;
				}
				if (min.y > vp.y) {
					min.y = vp.y;
				}
				if (max.x < vp.x) {
					max.x = vp.x;
				}
				if (max.y < vp.y) {
					max.y = vp.y;
				}
			}
		}
	}

	sf::RenderTexture rtex;
	unsigned int width = (int)(std::ceilf(max.x - min.x) + 0.5f + scale * 10);
	unsigned int height = (int)(std::ceilf(max.y - min.y) + 0.5f + scale * 10);
	rtex.create(width, height);

	transform = transform.Identity;
	transform.translate(-min.x + scale * 5, -min.y + scale * 5.0f);
	transform.scale(scale, scale);
	sf::RenderStates states;
	states.transform = transform;

	sf::VertexArray va;
	va.setPrimitiveType(sf::Quads);

	for (int j = 0; j <= totalFrameInterval; ++j) {
		float t = totalTime * j / totalFrameInterval;
		if (t > totalTime)
			t = totalTime;

		m_playInfo.timePoint = t;
		update(0.0f);

		rtex.clear(sf::Color::Transparent);
		for (size_t i = 0, n = m_playInfo.trackCount; i < n; ++i) {
			if (m_playInfo.fragments[i].isEmpty || m_playInfo.fragments[i].disabled) continue;
			va.clear();
			va.append(m_playInfo.fragments[i].vertex[0]);
			va.append(m_playInfo.fragments[i].vertex[1]);
			va.append(m_playInfo.fragments[i].vertex[3]);
			va.append(m_playInfo.fragments[i].vertex[2]);
			states.texture = &m_playInfo.fragments[i].texture;
			rtex.draw(va, states);
		}
		rtex.display();

		rtex.getTexture().copyToImage().saveToFile(m_playInfo.ctrlTrackRef->m_name + std::to_string(j) + ".png");
	}

	return;
}

void Animate::setFragmentDisabled(const std::string& name, bool disabled) {
	if (!m_dataRef.m_available)
		return;

	auto it = m_dataRef.m_trackNameTable.find(name);

	if (it == m_dataRef.m_trackNameTable.end()) {
		std::cerr << "Animaiton: No track: \'" << name << "\'.\n\n";
		return;
	}

	m_playInfo.fragments[it->second].disabled = disabled;

}

void Animate::enableAllFragments() {
	for (size_t i = 0, n = m_playInfo.trackCount; i < n; ++i)
		m_playInfo.fragments[i].disabled = false;
}

void Animate::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (!m_playInfo.ctrlTrackRef)
		return;

	sf::VertexArray va;
	va.setPrimitiveType(sf::Quads);

	for (size_t i = 0, n = m_playInfo.trackCount; i < n; ++i) {
		if (m_playInfo.fragments[i].isEmpty || m_playInfo.fragments[i].disabled) continue;
		va.clear();
		va.append(m_playInfo.fragments[i].vertex[0]);
		va.append(m_playInfo.fragments[i].vertex[1]);
		va.append(m_playInfo.fragments[i].vertex[3]);
		va.append(m_playInfo.fragments[i].vertex[2]);
		states.texture = &m_playInfo.fragments[i].texture;
		target.draw(va, states);
	}
	return;

}

Animate::PlayInfo::PlayInfo(size_t cnt) :
	trackCount(cnt),
	ctrlTrackRef(nullptr),
	fragments(new TrackFragData[cnt]),
	trackOffset(0),
	trackLength(0),
	lastFramePoint(0),
	timePoint(0.0f) {}

Animate::PlayInfo::~PlayInfo() {
	delete[] fragments;
}

} // namespace pvzanim
} // namespace ohms
