/**
* @file    AnimData.h
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

#include "AnimTrack.h"

#include <map>

namespace ohms {
namespace pvzanim {

class Animate;

class AnimData final {
	friend class Animate;
	friend class Exporter;

public:
	AnimData() :
		m_available(false),
		m_fps(12),
		m_frameCount(0)
	{}

	~AnimData() {
		clear();
	}

	void clear();

	bool load(const std::string& filepath);

	Animate* create(bool linearFI = true);

	void listTrack() const;

	void listControlTrack() const;

protected:

	bool m_available;

	/**
	 * @brief 动画 每秒帧数
	*/
	int m_fps;

	/**
	 * @brief 动画 总帧数
	*/
	size_t m_frameCount;
	
	/**
	 * @brief 轨道指针 序列
	*/
	std::vector<AnimTrack*> m_trackArray;
	
	/**
	 * @brief 轨道名称 到 轨道指针 的 映射
	*/
	std::map<std::string, size_t> m_trackNameTable;

	std::string m_parentPath;
	std::string m_fileName;

}; // class AnimData

} // namespace pvzanim
} // namespace ohms
