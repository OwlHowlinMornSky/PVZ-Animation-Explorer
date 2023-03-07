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
