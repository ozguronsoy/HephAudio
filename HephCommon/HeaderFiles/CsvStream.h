#pragma once
#include "HephCommonFramework.h"
#include "StreamStatus.h"
#include "StringBuffer.h"
#include "File.h"
#include <vector>

namespace HephCommon
{
	typedef std::vector<StringBuffer> CsvRecord;
	typedef std::vector<CsvRecord> CsvRecordVector;
	class CsvStream final
	{
	private:
		File file;
	public:
		StringType stringType;
	public:
		CsvStream(const StringBuffer& filePath);
		CsvStream(const CsvStream&) = delete;
		CsvStream(CsvStream&& rhs) noexcept;
		CsvStream& operator=(const CsvStream&) = delete;
		CsvStream& operator=(CsvStream&& rhs) noexcept;
		CsvRecordVector Read() const;
		void Write(const StringBuffer& csvString);
		void Write(const CsvRecordVector& recordVector);
		StringBuffer ToCsvString(const CsvRecordVector& recordVector) const;
	};
}