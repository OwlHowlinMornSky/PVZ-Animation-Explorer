/**
* @file    AnimTrack.h
* @author  Tyler Parret Zhu (OwlHowlinMornSky) <mysteryworldgod@outlook.com>
*
* @section LICENSE
*
* GNU GENERAL PUBLIC LICENSE
*                       Version 3, 29 June 2007
*
* Copyright (c) 2023  Tyler Parret Zhu
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*
*/

#pragma once

#include "AnimFrame.h"

#include <vector>

namespace ohms {
namespace pvzanim {

class AnimTrack final {
	friend class AnimData;
	friend class Animate;

protected:
	AnimTrack() {}

public:
	~AnimTrack() {
		clear();
	}

	void clear();

	/**
	 * @brief 读取轨道
	 * @return 帧数量
	*/
	bool readTrack(FILE*& fp);

	const FrameData& getFrame(size_t n) {
		if (n >= m_frameArray.size()) {
			throw std::exception();
		}
		return m_frameArray[n]->getData();
	}

protected:
	/**
	 * @brief 轨道的所有帧
	*/
	std::vector<AnimFrame*> m_frameArray;

	/**
	 * @brief 轨道的名称
	*/
	std::string m_name;

}; // class AnimTrack

} // namespace pvzanim
} // namespace ohms
