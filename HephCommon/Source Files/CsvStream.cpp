#include "CsvStream.h"
#include "StreamHelpers.h"
#include "HephException.h"

#define IS_NOT_END_OF_FILE (this->file.GetOffset() < this->file.FileSize())

namespace HephCommon
{
	CsvStream::CsvStream(const StringBuffer& filePath) : stringType(StringType::ASCII)
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
	CsvRecordVector CsvStream::Read() const
	{
		CsvRecordVector recordVector;
		bool isInQuotation = false;
		wchar_t c = L'\0';

		this->file.SetOffset(0);

		while (IS_NOT_END_OF_FILE)
		{
			CsvRecord record;

			do
			{
				StringBuffer field = L"";

				c =StreamHelpers::NextChar(this->file, this->stringType);
				if (c == L'"')
				{
					do {
						c =StreamHelpers::NextChar(this->file, this->stringType);
						do
						{
							field += c;
							c =StreamHelpers::NextChar(this->file, this->stringType);
						} while (c != L'"' && IS_NOT_END_OF_FILE);
						if (IS_NOT_END_OF_FILE)
						{
							c =StreamHelpers::NextChar(this->file, this->stringType);
						}
						if (c == L'"')
						{
							field += c;
						}
					} while (c != L',' && c != L'\n' && IS_NOT_END_OF_FILE);
					if (!IS_NOT_END_OF_FILE && c != L',' && c != L'\n') // last character is at the EOF, include it.
					{
						field += c;
					}
				}
				else
				{
					do
					{
						field += c;
						c =StreamHelpers::NextChar(this->file, this->stringType);
					} while (c != L',' && c != L'\n' && IS_NOT_END_OF_FILE);
					if (!IS_NOT_END_OF_FILE && c != L',' && c != L'\n')
					{
						field += c;
					}
				}

				field.SetStringType(this->stringType);
				if ('\r'_b.CompareContent(field.c_at(field.Size() - 1)))
				{
					field.RemoveAt(field.Size() - 1);
				}
				record.push_back(field);
			} while (c != L'\n' && IS_NOT_END_OF_FILE);

			recordVector.push_back(record);
		}

		return recordVector;
	}
	void CsvStream::Write(const StringBuffer& csvString)
	{
		const StringBuffer& filePath = this->file.FilePath();

		this->file.Close(); // erase the existing content
		this->file.Open(filePath, FileOpenMode::Read | FileOpenMode::Overwrite);
		this->file.WriteFromBuffer(csvString.Begin(), 1u, csvString.ByteSize());
		this->file.Flush();
	}
	void CsvStream::Write(const CsvRecordVector& recordVector)
	{
		this->Write(this->ToCsvString(recordVector));
	}
	StringBuffer CsvStream::ToCsvString(const CsvRecordVector& recordVector) const
	{
		StringBuffer csvString;
		for (size_t i = 0; i < recordVector.size(); i++)
		{
			const CsvRecord& record = recordVector[i];
			for (size_t j = 0; (j + 1) < record.size(); j++)
			{
				if (record[j].Contains('"') || record[j].Contains(','))
				{
					StringBuffer field = record[j];
					size_t offset = 0;
					while ((offset = field.Find('"', offset)) != StringBuffer::npos)
					{
						field.Insert(offset, '"');
						offset += 2;
					}
					csvString += '"' + field + "\",";
				}
				else
				{
					csvString += record[j] + ',';
				}
			}
			csvString += record[record.size() - 1] + '\n';
		}
		return csvString;
	}
}