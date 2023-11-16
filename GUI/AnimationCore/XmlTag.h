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
 * @brief XML ��ǩ��ȡ���������� ��ȡ��ǩ���ͣ����ܺ��٣���Ե� ���������Ŀ��
 * ��Ҫ ���������ط������� ��ȡ XML �Ŀ�Դ�� �ܶࡣ
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
	 * @brief ��ȡ XML ��ǩ���� ������λ�� ����û�п��� �ո� �� �������ţ����뱣֤ û�пո�
	 * @param str �ַ�����
	 * @param off ƫ�ơ�
	 * @return �ɹ���ȡ �� ���� ��ǩ������� ��һ���ַ��� λ�ã����� �� ���� 0��
	*/
	size_t read(const std::string& str, size_t off) {
		clear();

		// ����Ҫ�� ֱ�ӷ���
		if (off >= str.length()) 
			return 0;
		if (str[off] != '<')
			return 0;

		++off;
		// ���� ������ǣ��Ҳ�ȷ�� �﷨�������� ��б�ߣ�
		if (str[off] == '/') {
			m_isStart = 0;
			++off;
		}

		size_t off_s = off, off_e;

		// �ҵ� ��ǩ������
		off_e = str.find_first_of('>', off_s);
		if (off > str.length())
			return 0;

		// ȡ������
		m_type = str.substr(off_s, off_e - off_s);
		return off_e + 1;
	}

	/**
	 * @brief ��ȡ ��ǩ���ͣ������� ���֣��Ҽǲ����ˣ���
	 * @return ���͡��ַ�����
	*/
	const std::string& getType() const {
		return m_type;
	}

	/**
	 * @brief �Ƿ��� ��ͷ�ı�ǩ��Ҳ���� û��б�ߡ�
	 * @return ��֪����ɶ��
	*/
	bool isStart() const {
		return m_isStart;
	}

protected:

	/**
	 * @brief XML �������ͣ������� ���֣��Ҽǲ����ˣ���
	*/
	std::string m_type;

	/**
	 * @brief XML ��ǩ ���ǽ�����ǡ�
	*/
	bool m_isStart;

}; // class XmlTag

} // namespace ohms
