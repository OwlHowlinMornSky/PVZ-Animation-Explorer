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
