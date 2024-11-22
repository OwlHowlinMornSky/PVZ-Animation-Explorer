/**
* @file    Animate.cpp
* @author  Tyler Parret True (OwlHowlinMornSky) <mysteryworldgod@outlook.com>
*
* @section LICENSE
*
*                   GNU AFFERO GENERAL PUBLIC LICENSE
*                      Version 3, 19 November 2007
*
*    Copyright (c) 2023  Tyler Parret True
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

namespace ohms {
namespace pvzanim {

bool Animate::setAnimation(const std::string& name) {
	if (!m_dataRef.m_available)
		return false;

	if (m_playInfo.ctrlTrackRef && name == m_playInfo.ctrlTrackRef->m_name) {
		m_playInfo.timePoint = 0.0f;
		update(0.0f);
		return true;
	}

	auto it = m_dataRef.m_trackNameTable.find(name);

	if (it == m_dataRef.m_trackNameTable.end()) {
		printf_s("Animaiton: Cannot find track: \'%s\'.\n\n", name.c_str());
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
			printf_s("Animaiton: This track cannot be used as control track: \'%s\'.\n\n", name.c_str());
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
				ii.texture.loadFromFile(m_dataRef.m_parentPath + tf.imName + ".png");
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

	printf_s("Animation: Set track: \'%s\'. Length: %zd.\n\n", name.c_str(), m_playInfo.trackLength);

	return true;
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
					ii.texture.loadFromFile(m_dataRef.m_parentPath + tf.imName + ".png");
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
			m_dataRef.m_trackArray[i]->getFrame(m_playInfo.lastFramePoint + m_playInfo.trackOffset + 1);

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

const std::string& Animate::getFrameInfoString() const {
	char tmp[64];
	if (m_linearFI)
		sprintf_s(tmp, 64, "Frame: %04.1f / %02zd.0", m_playInfo.timePoint * m_dataRef.m_fps, m_playInfo.trackLength);
	else
		sprintf_s(tmp, 64, "Frame: %02zd / %02zd", m_playInfo.lastFramePoint, m_playInfo.trackLength);
	m_frameInfo.assign(tmp);
	return m_frameInfo;
}

void Animate::renderToFrames(size_t totalFrameInterval) {
	if (!m_playInfo.ctrlTrackRef)
		return;

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
			if (m_playInfo.fragments[i].isEmpty) continue;
			for (int k = 0; k < 4; ++k) {
				auto& vp = m_playInfo.fragments[i].vertex[k].position;
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
	unsigned int width = (int)(std::ceilf(max.x - min.x) + 0.5f) + 200;
	unsigned int height = (int)(std::ceilf(max.y - min.y) + 0.5f) + 200;
	rtex.create(width, height);

	sf::Transform transform;
	transform.translate(-min.x + 100, -min.y + 100);

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
			if (m_playInfo.fragments[i].isEmpty) continue;
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

void Animate::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (!m_playInfo.ctrlTrackRef)
		return;

	sf::VertexArray va;
	va.setPrimitiveType(sf::Quads);

	for (size_t i = 0, n = m_playInfo.trackCount; i < n; ++i) {
		if (m_playInfo.fragments[i].isEmpty) continue;
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

} // namespace pvzanim
} // namespace ohms
