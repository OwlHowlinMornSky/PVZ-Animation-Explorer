/**
* @file    AnimData.cpp
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

#include "AnimData.h"

#include "Animate.h"

namespace ohms {
namespace pvzanim {

void AnimData::clear() {
	for (AnimTrack* i : m_trackArray) {
		delete i;
	}
	m_trackArray.clear();
	m_trackNameTable.clear();
	m_frameCount = 0;
	m_fps = 0;
	m_available = false;
	return;
}

bool AnimData::load(const std::string& filepath) {
	clear();

	char linet[1024] = { 0 };
	std::string line;
	XmlTag tag;
	size_t l_trackReadCount = 0, off = 0;
	bool l_hasReadFPS = false;

	FILE* fp;
	fopen_s(&fp, filepath.c_str(), "r");

	if (fp == 0) {
		clear();
		return false;
	}

	bool keepGoingOn = true;
	while (keepGoingOn) {
		if (fscanf_s(fp, "%s", linet, 1024) == EOF)
			break;

		line.assign(linet);
		off = tag.read(line, 0);
		if (off == 0) {
			clear();
			return false;
		}

		if (tag.isStart()) {
			if (tag.getType() == "fps") {
				if (l_hasReadFPS) {
					clear();
					return false;
				}
				l_hasReadFPS = true;

				size_t flag = line.find_first_of('<', off);
				m_fps = std::atoi(line.substr(off, flag - off).c_str());

				off = tag.read(line, flag);
				if (off == 0) {
					clear();
					return false;
				}
				if (!(!tag.isStart() && tag.getType() == "fps")) {
					clear();
					return false;
				}
			}
			else if (tag.getType() == "track") {
				AnimTrack* tt = new AnimTrack;
				if (tt->readTrack(fp)) {
					++l_trackReadCount;
					m_trackArray.push_back(tt);
					m_trackNameTable.emplace(tt->m_name, m_trackArray.size() - 1);
				}
				else {
					delete tt;
					clear();
					return false;
				}
			}
		}
		else {
			clear();
			return false;
		}
	}

	if (!l_hasReadFPS)
		m_fps = 12;
	if (m_trackArray.size() != l_trackReadCount) {
		clear();
		return false;
	}
	if (l_trackReadCount == 0) {
		clear();
		return false;
	}

	m_frameCount = m_trackArray[0]->m_frameArray.size();

	size_t pathFlag = filepath.find_last_of("\\/");
	if (pathFlag == std::string::npos) {
		m_parentPath.clear();
		m_fileName = filepath;
	}
	else {
		m_parentPath = filepath.substr(0, pathFlag + 1);
		m_fileName = filepath.substr(pathFlag + 1);
	}

	m_available = true;
	return true;
}

Animate* AnimData::create(bool linearFI) {
	if (!m_available)
		return nullptr;
	return new Animate(*this, linearFI);

}

void AnimData::listTrack() const {
	printf_s("File \'%s\' at \'%s\':\n", m_fileName.c_str(), m_parentPath.c_str());
	for (size_t i = 0, n = m_trackArray.size(); i < n; ++i) {
		printf_s("%02zd: %s,\n", i, m_trackArray[i]->m_name.c_str());
	}
	printf_s("Total: %zd. FPS: %d. Frame count: %zd.\n\n",
			 m_trackArray.size(),
			 m_fps,
			 m_trackArray[0]->m_frameArray.size());
	return;
}

void AnimData::listControlTrack() const {
	printf_s("File \'%s\' at \'%s\':\n", m_fileName.c_str(), m_parentPath.c_str());
	size_t i = 0;
	for (AnimTrack* ii : m_trackArray) {
		if (ii->m_name.find("anim_") != std::string::npos) {
			printf_s("%02zd: %s,\n", i, ii->m_name.c_str());
			++i;
		}
	}
	printf_s("Total: %zd. FPS: %d. Frame count: %zd.\n\n",
			 i,
			 m_fps,
			 m_trackArray[0]->m_frameArray.size());
	return;

}

} // namespace pvzanim
} // namespace ohms
