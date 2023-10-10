#pragma once
#include "HephCommonFramework.h"
#include "File.h"
#include "StringBuffer.h"
#include <vector>

namespace HephCommon
{
	struct XmlAttribute;
	struct XmlElement;

	typedef std::vector<XmlAttribute> XmlAttributeVector;
	typedef std::vector<XmlElement> XmlElementVector;
	
	struct XmlAttribute final
	{
		StringBuffer name;
		StringBuffer value;
		XmlAttribute();
	};

	struct XmlElement final
	{
		StringBuffer name;
		StringBuffer value;
		XmlAttributeVector attributes;
		XmlElementVector children;
		XmlElement();
	};

	class XmlStream final
	{
	private:
		File file;
	public:
		bool useIndentation;
		bool useNewLine;
		StringType stringType;
	public:
		XmlStream(const StringBuffer& filePath);
		XmlStream(const XmlStream&) = delete;
		XmlStream& operator=(const XmlStream&) = delete;
		XmlElement Read();
		void Write(const StringBuffer& xmlString);
		void Write(const XmlElement& rootElement);
		StringBuffer ToXmlString(const XmlElement& rootElement) const;
	private:
		wchar_t ReadName(XmlElement& element) const;
		void ReadAttributes(XmlElement& element) const;
		void ReadValue(XmlElement& element) const;
		void WriteElement(const XmlElement& element, StringBuffer& xmlString, const StringBuffer& indentStr, const StringBuffer& newLineStr) const;
		wchar_t SkipEmptySpaces() const;
	};
}