#include "JsonStream.h"
#include "StreamHelpers.h"
#include <cctype>

#define IS_END_OF_FILE (this->file.GetOffset() == this->file.FileSize())

namespace HephCommon
{
	JsonElement::JsonElement() : name(L""), value(L""), children(JsonElementVector()) {}
	JsonStream::JsonStream(const StringBuffer& filePath) : useIndentation(true), useNewLine(true), stringType(StringType::ASCII)
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
	JsonStream::JsonStream(JsonStream&& rhs) noexcept : file(std::move(rhs.file)), useIndentation(rhs.useIndentation), useNewLine(rhs.useNewLine), stringType(rhs.stringType) {}
	JsonStream& JsonStream::operator=(JsonStream&& rhs) noexcept 
	{
		this->file = std::move(rhs.file);
		this->useIndentation = rhs.useIndentation;
		this->useNewLine = rhs.useNewLine;
		this->stringType = rhs.stringType;
		return *this;
	}
	JsonElement JsonStream::Read() const
	{
		JsonElement rootElement;
		rootElement.name = JSON_ELEMENT_ROOT;
		rootElement.value = JSON_ELEMENT_ROOT;

		this->file.SetOffset(0);
		if (!StreamHelpers::FindChar(this->file, this->stringType, L'{', nullptr))
		{
			return rootElement;
		}

		do
		{
			rootElement.children.push_back(std::move(this->ReadElement(nullptr)));
		} while (!IS_END_OF_FILE && StreamHelpers::FindChar(this->file, this->stringType, L',', nullptr));

		return rootElement;
	}
	void JsonStream::Write(const StringBuffer& jsonString)
	{
		const StringBuffer& filePath = this->file.FilePath();

		this->file.Close(); // erase the existing content
		this->file.Open(filePath, FileOpenMode::Read | FileOpenMode::Overwrite);
		this->file.WriteFromBuffer(jsonString.Begin(), 1u, jsonString.ByteSize());
		this->file.Flush();
	}
	void JsonStream::Write(const JsonElement& rootElement)
	{
		this->Write(this->ToJsonString(rootElement));
	}
	StringBuffer JsonStream::ToJsonString(const JsonElement& rootElement) const
	{
		const StringBuffer indentStr = this->useIndentation ? "\t" : "";
		const StringBuffer newLineStr = this->useNewLine ? "\r\n" : "";

		StringBuffer jsonString = '{' + newLineStr;
		jsonString.SetStringType(this->stringType);

		for (size_t i = 0; i < rootElement.children.size(); i++)
		{
			this->WriteElement(rootElement.children[i], jsonString, indentStr, newLineStr, i == (rootElement.children.size() - 1));
		}

		jsonString += '}';

		return jsonString;
	}
	JsonElement JsonStream::ReadElement(JsonElement* pParent) const
	{
		JsonElement element;
		element.name = L"";
		element.value = L"";

		// Get the element name
		if (!StreamHelpers::FindChar(this->file, this->stringType, L'"', nullptr))
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "JsonStream::Read", "Failed to read the json element. File might be corrupted."));
			return element;
		}
		if (!StreamHelpers::FindChar(this->file, this->stringType, L'"', &element.name))
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "JsonStream::Read", "Failed to read the json element. File might be corrupted."));
			return element;
		}
		if (!StreamHelpers::FindChar(this->file, this->stringType, L':', nullptr))
		{
			RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "JsonStream::Read", "Failed to read the json element. File might be corrupted."));
			return element;
		}

		this->ReadElementValue(element);

		return element;
	}
	bool JsonStream::ReadElementValue(JsonElement& element) const
	{
		wchar_t c;
		do
		{
			c = StreamHelpers::NextChar(this->file, this->stringType);
		} while (c == L' ' || c == L'\t' || c == L'\r' || c == L'\n'); // Get to the value

		if (c == L'"')
		{
			if (!StreamHelpers::FindChar(this->file, this->stringType, L'"', &element.value))
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "JsonStream::Read", "Failed to read the json element. File might be corrupted."));
				return false;
			}
		}
		else if (c == L'{')
		{
			element.value = JSON_ELEMENT_OBJECT;
			do
			{
				element.children.push_back(std::move(this->ReadElement(&element)));
				do
				{
					c = StreamHelpers::NextChar(this->file, this->stringType);
				} while (c != L',' && c != L'}'); // find the next element or the end of the object
			} while (c != L'}');
		}
		else if (c == L'[')
		{
			element.value = JSON_ELEMENT_ARRAY;
			do
			{
				JsonElement arrayElement;
				arrayElement.name = JSON_ELEMENT_ARRAY_ELEMENT;
				arrayElement.value = L"";

				if (!this->ReadElementValue(arrayElement))
				{
					return false;
				}
				element.children.push_back(std::move(arrayElement));

				do
				{
					c = StreamHelpers::NextChar(this->file, this->stringType);
				} while (c != L',' && c != L']'); // find the next element or the end of the array
			} while (c != L']');
		}
		else // a number
		{
			do
			{
				if (iswdigit(c) || c == L'.')
				{
					element.value += c;
				}
				c = StreamHelpers::NextChar(this->file, this->stringType);
			} while (c != L',' && c != L'}' && c != L']');
			this->file.DecreaseOffset(1);
		}

		return true;
	}
	void JsonStream::WriteElement(const JsonElement& element, StringBuffer& jsonString, const StringBuffer& indentStr, const StringBuffer& newLineStr, bool isLastChild) const
	{
		jsonString += indentStr;

		if (!element.name.CompareContent(JSON_ELEMENT_ARRAY_ELEMENT))
		{
			jsonString += '"' + element.name + "\": ";
		}

		if (element.value.CompareContent(JSON_ELEMENT_OBJECT))
		{
			jsonString += '{' + newLineStr;
			for (size_t i = 0; i < element.children.size(); i++)
			{
				this->WriteElement(element.children[i], jsonString, indentStr + (this->useIndentation ? "\t" : ""), newLineStr, i == (element.children.size() - 1));
			}
			jsonString += indentStr;
			jsonString += isLastChild ? "}" : "},";
			jsonString += newLineStr;
		}
		else if (element.value.CompareContent(JSON_ELEMENT_ARRAY))
		{
			jsonString += '[' + newLineStr;
			for (size_t i = 0; i < element.children.size(); i++)
			{
				this->WriteElement(element.children[i], jsonString, indentStr + (this->useIndentation ? "\t" : ""), newLineStr, i == (element.children.size() - 1));
			}
			jsonString += indentStr;
			jsonString += isLastChild ? "]" : "],";
			jsonString += newLineStr;
		}
		else
		{
			if (element.value.IsNumber())
			{
				if (isLastChild)
				{
					jsonString += element.value + newLineStr;
				}
				else
				{
					jsonString += element.value + "," + newLineStr;
				}
			}
			else
			{
				if (isLastChild)
				{
					jsonString += '"' + element.value + '"' + newLineStr;
				}
				else
				{
					jsonString += '"' + element.value + "\"," + newLineStr;
				}
			}
		}
	}
}