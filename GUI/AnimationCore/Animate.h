/**
* @file    Animate.h
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

#pragma once

#include "AnimData.h"

#include <SFML/Graphics.hpp>

namespace ohms {
namespace pvzanim {

class Animate final : public sf::Drawable {
	friend class AnimData;

protected:
	Animate(const AnimData& data, bool linearFrameInsertion) :
		m_dataRef(data),
		m_linearFI(linearFrameInsertion),
		m_playInfo(data.m_trackArray.size()),
		m_timeScale(1.0f)
	{
	}

public:
	~Animate() {}

	bool setAnimation(const std::string& name);

	void update(float dt);

	void nextFrame(size_t df);

	void setTimeScale(float nv) {
		m_timeScale = nv;
	}

	const std::string& getFrameInfoString() const;

	void renderToFrames(size_t totalFrameInterval); // Frame Count = Frame Interval + 1;

protected:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

protected:
	struct TrackFragData {
		/**
		 * @brief 纹理，updateFrame 时更新
		*/
		sf::Texture texture;
		/**
		 * @brief 纹理大小，随 texture 更新
		*/
		sf::Vector2u texSize;
		/**
		 * @brief 四个顶点
		*/
		sf::Vertex vertex[4];
		/**
		 * @brief 是否为空
		*/
		bool isEmpty = false;
	}; // struct TrackFragData

	struct PlayInfo {
		size_t trackCount;
		AnimTrack* ctrlTrackRef;
		TrackFragData* fragments;
		size_t trackOffset;
		size_t trackLength;
		size_t lastFramePoint;
		float timePoint;

		PlayInfo(size_t cnt) :
			trackCount(cnt),
			ctrlTrackRef(nullptr),
			fragments(new TrackFragData[cnt]),
			trackOffset(0),
			trackLength(0),
			lastFramePoint(0),
			timePoint(0.0f)
		{}

		~PlayInfo() {
			delete[] fragments;
		}
	}; // struct PlayInfo

protected:
	bool m_linearFI;
	const AnimData& m_dataRef;
	PlayInfo m_playInfo;
	float m_timeScale;

	mutable std::string m_frameInfo;

}; // class Animate

} // namespace pvzanim
} // namespace ohms
