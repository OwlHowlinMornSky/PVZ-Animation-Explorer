/**
* @file    AnimFrame.cpp
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

#include "AnimFrame.h"

namespace ohms {
namespace pvzanim {

bool AnimFrame::readFrame(const std::string& line) {
	std::string tmp;
	XmlTag tag;
	size_t off = 0;
	size_t n = line.length();
	size_t flag = 0;
	uint16_t l_loaded = 0;

	// 取开头
	off = tag.read(line, off);
	if (off == 0 || !tag.isStart() || tag.getType() != "t")
		return false;

	// 循环读取中间的值
	bool keepGoingOn = true;
	while (keepGoingOn) {
		off = tag.read(line, off);
		if (off == 0)
			return false;
		if (off == n && !(!tag.isStart() && tag.getType() == "t"))
			return false;
		if ((!tag.isStart() && tag.getType() == "t") && off != n)
			return false;

		// 一行结束 且 到</t> 则 退出
		if (off == n && (!tag.isStart() && tag.getType() == "t"))
			break;

		// 分类读取
		if (tag.isStart()) {
			if (tag.getType() == "f") {
				if (l_loaded & (1 << 0)) return false;
				l_loaded |= 1 << 0;

				flag = line.find_first_of('<', off);
				m_data.isEmpty = (std::atoi(line.substr(off, flag - off).c_str()) == -1);

				off = tag.read(line, flag);
				if (off == 0) return false;
				if (!(!tag.isStart() && tag.getType() == "f"))
					return false;
			}
			else if (tag.getType() == "x") {
				if (l_loaded & (1 << 1)) return false;
				l_loaded |= 1 << 1;

				flag = line.find_first_of('<', off);
				m_data.px = static_cast<float>(std::atof(line.substr(off, flag - off).c_str()));

				off = tag.read(line, flag);
				if (off == 0) return false;
				if (!(!tag.isStart() && tag.getType() == "x"))
					return false;
			}
			else if (tag.getType() == "y") {
				if (l_loaded & (1 << 2)) return false;
				l_loaded |= 1 << 2;

				flag = line.find_first_of('<', off);
				m_data.py = static_cast<float>(std::atof(line.substr(off, flag - off).c_str()));

				off = tag.read(line, flag);
				if (off == 0) return false;
				if (!(!tag.isStart() && tag.getType() == "y"))
					return false;
			}
			else if (tag.getType() == "kx") {
				if (l_loaded & (1 << 3)) return false;
				l_loaded |= 1 << 3;

				flag = line.find_first_of('<', off);
				m_data.kx = static_cast<float>(std::atof(line.substr(off, flag - off).c_str()));

				off = tag.read(line, flag);
				if (off == 0) return false;
				if (!(!tag.isStart() && tag.getType() == "kx"))
					return false;
			}
			else if (tag.getType() == "ky") {
				if (l_loaded & (1 << 4)) return false;
				l_loaded |= 1 << 4;

				flag = line.find_first_of('<', off);
				m_data.ky = static_cast<float>(std::atof(line.substr(off, flag - off).c_str()));

				off = tag.read(line, flag);
				if (off == 0) return false;
				if (!(!tag.isStart() && tag.getType() == "ky"))
					return false;
			}
			else if (tag.getType() == "sx") {
				if (l_loaded & (1 << 5)) return false;
				l_loaded |= 1 << 5;

				flag = line.find_first_of('<', off);
				m_data.sx = static_cast<float>(std::atof(line.substr(off, flag - off).c_str()));

				off = tag.read(line, flag);
				if (off == 0) return false;
				if (!(!tag.isStart() && tag.getType() == "sx"))
					return false;
			}
			else if (tag.getType() == "sy") {
				if (l_loaded & (1 << 6)) return false;
				l_loaded |= 1 << 6;

				flag = line.find_first_of('<', off);
				m_data.sy = static_cast<float>(std::atof(line.substr(off, flag - off).c_str()));

				off = tag.read(line, flag);
				if (off == 0) return false;
				if (!(!tag.isStart() && tag.getType() == "sy"))
					return false;
			}
			else if (tag.getType() == "i") {
				if (l_loaded & (1 << 7)) return false;
				l_loaded |= 1 << 7;

				flag = line.find_first_of('<', off);
				m_data.imName = line.substr(off, flag - off);
				size_t ep = m_data.imName.find("IMAGE_REANIM_");

				if (ep != std::string::npos)
					m_data.imName.erase(ep, 13);

				off = tag.read(line, flag);
				if (off == 0) return false;
				if (!(!tag.isStart() && tag.getType() == "i"))
					return false;
			}
			else if (tag.getType() == "a") {
				if (l_loaded & (1 << 8)) return false;
				l_loaded |= 1 << 8;

				flag = line.find_first_of('<', off);
				m_data.a = static_cast<float>(std::atof(line.substr(off, flag - off).c_str()));

				off = tag.read(line, flag);
				if (off == 0) return false;
				if (!(!tag.isStart() && tag.getType() == "a"))
					return false;
			}
			else return false;
		}
		else return false;
	}
	return true;
}

} // namespace pvzanim
} // namespace ohms
