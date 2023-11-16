/**
* @file    AnimTrack.cpp
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
#include "AnimTrack.h"

namespace ohms {
namespace pvzanim {

void AnimTrack::clear() {
	for (AnimFrame* i : m_frameArray) {
		delete i;
	}
	m_frameArray.clear();
	return;
}

bool AnimTrack::readTrack(FILE*& fp) {
	clear();

	char linet[1024] = { 0 };
	std::string line;
	XmlTag tag;
	bool l_hasReadName = false;
	size_t l_frameReadCount = 0, off = 0;

	bool keepGoingOn = true;
	while (keepGoingOn) {
		fscanf_s(fp, "%s", linet, 1024);
		line.assign(linet);
		off = tag.read(line, 0);
		if (off == 0) {
			clear();
			return false;
		}

		if (tag.isStart()) {
			if (tag.getType() == "name") {
				if (l_hasReadName) {
					clear();
					return false;
				}
				l_hasReadName = true;

				size_t flag = line.find_first_of('<', off);
				m_name = line.substr(off, flag - off);

				off = tag.read(line, flag);
				if (off == 0) {
					clear();
					return false;
				}
				if (!(!tag.isStart() && tag.getType() == "name")) {
					clear();
					return false;
				}
			}
			else if (tag.getType() == "t") {
				AnimFrame* ft = new AnimFrame;

				if (m_frameArray.size())
					ft->follow(*(m_frameArray[m_frameArray.size() - 1]));

				if (ft->readFrame(line)) {
					++l_frameReadCount;
					m_frameArray.push_back(ft);
				}
				else {
					delete ft;
					clear();
					return false;
				}
			}
		}
		else if (tag.getType() == "track") break;
		else {
			clear();
			return false;
		}
	}

	if (!l_hasReadName) {
		clear();
		return false;
	}
	if (m_frameArray.size() != l_frameReadCount) {
		clear();
		return false;
	}
	
    return true;
}

} // namespace pvzanim
} // namespace ohms
