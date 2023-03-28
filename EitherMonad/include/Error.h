#pragma once
#include <string_view>

enum class GET_EXIT { True = true, False = false };

class Error
{
	const std::string_view m_msg;
	size_t m_error_code;
	std::string m_debug_data = "empty";
	GET_EXIT get_exit;

public:
	Error (std::string_view msg, size_t c, GET_EXIT ex)
		: m_msg(msg), m_error_code(c), get_exit(ex) {}

	Error& updateDDate(std::string sv)
	{
		m_debug_data = std::string{ sv };
		return *this;
	} 
	size_t exitCode() { return m_error_code; }
	std::string toStr()
	{
		return std::string(
			"Error with #" + std::to_string(m_error_code)
			+ " occured.\n\tMessage: " + std::string(m_msg.data()) + "\n\twith:\t"
			+ m_debug_data + '\n'
		);
	}

	bool isExit() { return GET_EXIT::True == get_exit; }
};

template <typename Stream>
Stream& error_handle(Stream& ss, Error err)
{
	ss << err.toStr();
	if (err.isExit()) { exit(static_cast<int>(err.exitCode())); }
	return ss;
}

#define APPLICATION_ERROR(error_name, error_desc, code, exit) \
    struct error_name : public Error { \
        error_name() : Error(error_desc, code, exit) { }  \
    };

