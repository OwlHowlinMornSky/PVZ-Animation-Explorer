/**
* @file    AnimFrame.h
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

#include "XmlTag.h"

namespace ohms {
namespace pvzanim {

/**
 * @brief ֡���ݵĽṹ�塣
*/
struct FrameData {
	/**
	 * @brief �Ƿ�Ϊ�հ�֡��
	*/
	bool isEmpty;
	/**
	 * @brief ͼ�� X ���ꡣ
	*/
	float px;
	/**
	 * @brief ͼ�� Y ���ꡣ
	*/
	float py;
	/**
	 * @brief X �᷽����б������
	*/
	float kx;
	/**
	 * @brief Y�᷽����б������
	*/
	float ky;
	/**
	 * @brief X �᷽�����Űٷֱȡ�
	*/
	float sx;
	/**
	 * @brief Y �᷽�����Űٷֱȡ�
	*/
	float sy;
	/**
	 * @brief IMAGE_REANIM_ͼ�����ơ�
	*/
	std::string imName;
	/**
	 * @brief Alpha �ٷֱȡ�
	*/
	float a;

	FrameData() :
		isEmpty(false),
		px(0.0f),
		py(0.0f),
		kx(0.0f),
		ky(0.0f),
		sx(1.0f),
		sy(1.0f),
		imName("NULL"),
		a(1.0f)
	{}

	void follow(const FrameData& data) {
		this->isEmpty = data.isEmpty;
		this->px = data.px;
		this->py = data.py;
		this->kx = data.kx;
		this->ky = data.ky;
		this->sx = data.sx;
		this->sy = data.sy;
		this->a = data.a;
	}
};

/**
 * @brief ����һ֡��
*/
class AnimFrame final {
	friend class AnimTrack;
	friend class Animate;

protected:
	AnimFrame() {}

public:
	~AnimFrame() {}

	void follow(const AnimFrame& lastFrame) {
		m_data.follow(lastFrame.m_data);
	}

	/**
	 * @brief ��ȡ һ֡�����ݡ�
	 * @param line һ֡����һ�С�
	 * @return ��ȡ�Ƿ�ɹ���
	*/
	bool readFrame(const std::string& line);

	/**
	 * @brief ȡ ��һ֡�� ���ݡ�
	 * @return 
	*/
	const FrameData& getData() const {
		return m_data;
	}
protected:
	FrameData m_data;
}; // class AnimFrame

} // namespace pvzanim
} // namespace ohms
