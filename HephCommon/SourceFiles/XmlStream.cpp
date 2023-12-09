#include "XmlStream.h"
#include "StreamHelpers.h"
#include "HephException.h"

#define IS_NOT_END_OF_FILE (this->file.GetOffset() < this->file.FileSize())

namespace HephCommon
{
	XmlAttribute::XmlAttribute() : name(L""), value(L"") {}
	XmlElement::XmlElement() : name(L""), value(L"") {}
	XmlStream::XmlStream(const StringBuffer& filePath) : useIndentation(true), useNewLine(true), stringType(StringType::ASCII)
	{
		if (File::FileExists(filePath))
		{
			this->file.Open(filePath, FileOpenMode::Read | FileOpenMode::Write);
		}
		else
		{
			this->file.Open(filePath, FileOpenMode::Read | FileOpenMode::Overwrite);
		}
	}
	XmlElement XmlStream::Read()
	{
		XmlElement rootElement;

		this->file.SetOffset(0);

		if (StreamHelpers::FindString(this->file, this->stringType, L"encoding=\"", nullptr))
		{
			StringBuffer encoding;
			StreamHelpers::FindString(this->file, this->stringType, L"\"", &encoding);
			this->stringType = (encoding == "UTF-8") ? StringType::ASCII : StringType::Wide;
		}
		this->file.SetOffset(0);

		if (!StreamHelpers::FindString(this->file, this->stringType, L"?>", nullptr))
		{
			RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "XmlStream::Read", "Invalid file."));
		}

		if (!StreamHelpers::FindString(this->file, this->stringType, L"<", nullptr))
		{
			return rootElement;
		}

		if (this->ReadName(rootElement) != L'>')
		{
			this->ReadAttributes(rootElement);
		}

		this->ReadValue(rootElement);

		return rootElement;
	}
	void XmlStream::Write(const StringBuffer& xmlString)
	{
		const StringBuffer& filePath = this->file.FilePath();

		this->file.Close(); // erase the existing content
		this->file.Open(filePath, FileOpenMode::Read | FileOpenMode::Overwrite);
		this->file.WriteFromBuffer(xmlString.Begin(), StreamHelpers::CharSize(xmlString.GetStringType()), xmlString.Size());
		this->file.Flush();
	}
	void XmlStream::Write(const XmlElement& rootElement)
	{
		this->Write(this->ToXmlString(rootElement));
	}
	StringBuffer XmlStream::ToXmlString(const XmlElement& rootElement) const
	{
		const StringBuffer newLineStr = this->useNewLine ? "\r\n" : "";

		StringBuffer xmlString;
		if (this->stringType == StringType::ASCII)
		{
			xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
		}
		else
		{
			xmlString = L"<?xml version=\"1.0\" encoding=\"UTF-16\"?>";
		}

		this->WriteElement(rootElement, xmlString, "", newLineStr);

		return xmlString;
	}
	void XmlStream::ReadAttributes(XmlElement& element) const
	{
		wchar_t c;
		do
		{
			c = this->SkipEmptySpaces();
			if (c == L'>')
			{
				break;
			}

			XmlAttribute attribute;
			attribute.name = c;

			if (!StreamHelpers::FindString(this->file, this->stringType, L"=", &attribute.name))
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "XmlStream::Read", "Invalid file."));
			}
			StreamHelpers::NextChar(this->file, this->stringType); // skip the first "
			if (!StreamHelpers::FindString(this->file, this->stringType, L"\"", &attribute.value))
			{
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HephException::ec_fail, "XmlStream::Read", "Invalid file."));
			}

			element.attributes.push_back(attribute);
		} while (c != L'>' && IS_NOT_END_OF_FILE);
	}
	wchar_t XmlStream::ReadName(XmlElement& element) const
	{
		wchar_t c = StreamHelpers::NextChar(this->file, this->stringType);
		do
		{
			element.name += c;
			c = StreamHelpers::NextChar(this->file, this->stringType);
		} while (c != L' ' && c != L'\t' && c != L'\r' && c != L'\n' && c != L'>' && IS_NOT_END_OF_FILE);
		return c;
	}
	void XmlStream::ReadValue(XmlElement& element) const
	{
		wchar_t c = this->SkipEmptySpaces();
		if (c == L'<')
		{
			while (IS_NOT_END_OF_FILE)
			{
				XmlElement childElement;
				c = this->ReadName(childElement);
				if (childElement.name == (L"/" + element.name))
				{
					break;
				}
				if (c != L'>')
				{
					this->ReadAttributes(childElement);
				}
				this->ReadValue(childElement);
				element.children.push_back(childElement);
				c = this->SkipEmptySpaces();
			}
		}
		else
		{
			element.value = c;
			StreamHelpers::FindString(this->file, this->stringType, L"</" + element.name + L">", &element.value);
		}
	}
	void XmlStream::WriteElement(const XmlElement& element, StringBuffer& xmlString, const StringBuffer& indentStr, const StringBuffer& newLineStr) const
	{
		xmlString += newLineStr + indentStr + "<" + element.name;
		for (size_t i = 0; i < element.attributes.size(); i++)
		{
			xmlString += " " + element.attributes[i].name + "=\"" + element.attributes[i].value + "\"";
		}
		xmlString += ">";

		if (element.children.size() == 0)
		{
			xmlString += element.value;
			xmlString += "</" + element.name + ">";
		}
		else
		{
			for (size_t i = 0; i < element.children.size(); i++)
			{
				this->WriteElement(element.children[i], xmlString, indentStr + (this->useIndentation ? "\t" : ""), newLineStr);
			}
			xmlString += newLineStr + indentStr + "</" + element.name + ">";
		}

	}
	wchar_t XmlStream::SkipEmptySpaces() const
	{
		wchar_t c;
		do
		{
			c = StreamHelpers::NextChar(this->file, this->stringType);
		} while ((c == L' ' || c == L'\t' || c == L'\r' || c == L'\n') && IS_NOT_END_OF_FILE);
		return c;
	}
}