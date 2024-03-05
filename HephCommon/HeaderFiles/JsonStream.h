#pragma once
#include "HephCommonShared.h"
#include "StreamStatus.h"
#include "File.h"
#include "HephException.h"
#include <vector>

#define JSON_ELEMENT_ROOT L"ROOT"
#define JSON_ELEMENT_OBJECT L"OBJ "
#define JSON_ELEMENT_ARRAY L"ARRY"
#define JSON_ELEMENT_ARRAY_ELEMENT L"AREL"

namespace HephCommon
{
	struct JsonElement;
	typedef std::vector<JsonElement> JsonElementVector;

	struct JsonElement final
	{
		StringBuffer name;
		StringBuffer value;
		JsonElementVector children;
		JsonElement();
	};

	class JsonStream final
	{
	private:
		File file;
	public:
		bool useIndentation;
		bool useNewLine;
		StringType stringType;
	public:
		JsonStream(const StringBuffer& filePath);
		JsonStream(const JsonStream&) = delete;
		JsonStream(JsonStream&& rhs) noexcept;
		JsonStream& operator=(const JsonStream&) = delete;
		JsonStream& operator=(JsonStream&& rhs) noexcept;
		JsonElement Read() const;
		void Write(const StringBuffer& jsonString);
		void Write(const JsonElement& rootElement);
		StringBuffer ToJsonString(const JsonElement& rootElement) const;
	private:
		JsonElement ReadElement(JsonElement* pParent) const;
		bool ReadElementValue(JsonElement& element) const;
		void WriteElement(const JsonElement& element, StringBuffer& jsonString, const StringBuffer& indentStr, const StringBuffer& newLineStr, bool isLastChild) const;
	};
}