/**
* @file    XmlTag.h
* @author  Tyler Parret True (OwlHowlinMornSky) <mysteryworldgod@outlook.com>
*
* @section LICENSE
*
*    Copyright (c) 2023-2025  Tyler Parret True
*
*                   GNU AFFERO GENERAL PUBLIC LICENSE
*                      Version 3, 19 November 2007
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
#include "XmlTag.h"

ohms::XmlTag::XmlTag() :
	m_type("NULL"),
	m_isStart(true) {}

void ohms::XmlTag::clear() {
	m_type = "NULL";
	m_isStart = true;
	return;
}

size_t ohms::XmlTag::read(const std::string& str, size_t off) {
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

const std::string& ohms::XmlTag::getType() const {
	return m_type;
}

bool ohms::XmlTag::isStart() const {
	return m_isStart;
}
