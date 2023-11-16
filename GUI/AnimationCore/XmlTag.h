/**
* @file    XmlTag.h
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

#include <string>

namespace ohms {

/**
 * @brief XML 标签读取。可以用来 获取标签类型，功能很少，针对的 就是这个项目。
 * 不要 用在其他地方，真正 读取 XML 的开源库 很多。
*/
class XmlTag {
public:
	XmlTag() :
		m_type("NULL"),
		m_isStart(true)
	{}

	void clear() {
		m_type = "NULL";
		m_isStart = true;
		return;
	}

	/**
	 * @brief 读取 XML 标签，从 给定的位置 读起。没有考虑 空格 和 其他符号，必须保证 没有空格。
	 * @param str 字符串。
	 * @param off 偏移。
	 * @return 成功读取 则 返回 标签结束后的 第一个字符的 位置，出错 则 返回 0。
	*/
	size_t read(const std::string& str, size_t off) {
		clear();

		// 不合要求 直接返回
		if (off >= str.length()) 
			return 0;
		if (str[off] != '<')
			return 0;

		++off;
		// 读到 结束标记（我不确定 语法包不包括 反斜线）
		if (str[off] == '/') {
			m_isStart = 0;
			++off;
		}

		size_t off_s = off, off_e;

		// 找到 标签结束点
		off_e = str.find_first_of('>', off_s);
		if (off > str.length())
			return 0;

		// 取出类型
		m_type = str.substr(off_s, off_e - off_s);
		return off_e + 1;
	}

	/**
	 * @brief 获取 标签类型（或者是 名字，我记不清了）。
	 * @return 类型。字符串。
	*/
	const std::string& getType() const {
		return m_type;
	}

	/**
	 * @brief 是否是 开头的标签。也就是 没有斜线。
	 * @return 你知道是啥。
	*/
	bool isStart() const {
		return m_isStart;
	}

protected:

	/**
	 * @brief XML 对象类型（或者是 名字，我记不清了）。
	*/
	std::string m_type;

	/**
	 * @brief XML 标签 不是结束标记。
	*/
	bool m_isStart;

}; // class XmlTag

} // namespace ohms
