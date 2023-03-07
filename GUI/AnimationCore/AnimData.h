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
