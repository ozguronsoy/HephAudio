#include "Exceptions/Exception.h"
#include "Exceptions/ExceptionEventArgs.h"
#include "ConsoleLogger.h"
#include "StringHelpers.h"
#include "Exceptions/ExternalException.h"

namespace Heph
{
	Exception::Exception() { }

	Exception::Exception(const std::string& method, const std::string& message) : std::exception(message.c_str()), method(method) {}

	std::string Exception::GetName() const
	{
		return "Exception";
	}

	const std::string& Exception::GetMethod() const
	{
		return this->method;
	}

	std::string Exception::GetMessage() const
	{
		return std::string(this->what());
	}

	void Exception::Raise(const void* pSender) const
	{
		ExceptionEventArgs args(pSender, *this);
		Exception::OnException.Invoke(&args, nullptr);
		this->AddToExceptions();
	}

	void Exception::AddToExceptions() const
	{
		Exception::GetExceptions().push_back(std::make_unique<Exception>(*this));
	}

	std::vector<std::unique_ptr<Exception>>& Exception::GetExceptions() noexcept
	{
		// had to use singleton to dllexport the thread_local instance.
		static thread_local std::vector<std::unique_ptr<Exception>> instance;
		return instance;
	}

	void Exception::DefaultHandler(const EventParams& params)
	{
#if defined(__ANDROID__)
		const std::string x = "";
#else
		const std::string x = "\n";
#endif

		const Exception& ex = ((ExceptionEventArgs*)params.pArgs)->exception;
		const ExternalException* externalEx = dynamic_cast<const ExternalException*>(&ex);

		if (externalEx != nullptr)
		{
			ConsoleLogger::LogError(x +
				externalEx->GetName() +
				"\nmethod:\t\t\t" + externalEx->GetMethod() + "\nmessage:\t\t" + externalEx->GetMessage() +
				"\nexternal source:\t" + externalEx->GetExternalSource() + "\nexternal message:\t" + externalEx->GetExternalMessage() + "\n"
			);
		}
		else
		{
			ConsoleLogger::LogError(x + ex.GetName() + "\nmethod:\t\t" + ex.GetMethod() + "\nmessage:\t" + ex.GetMessage() + "\n");
		}
	}
}