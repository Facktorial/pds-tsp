#include "EitherMonad/include/EitherMonad.h"
#include "Logger/LoggerHandler.h"  

[[ maybe_unused ]] const size_t MAX_FACK = 20;


APPLICATION_ERROR(AttemptToDivideByZero, "Attempt to divide by zero", 1, GET_EXIT::True)
APPLICATION_ERROR(BadIndexScenario,"Access not existing index",2, GET_EXIT::True)
APPLICATION_ERROR(TestError,"TEST",2, GET_EXIT::False)


using Str = std::string;
using MStr = std::optional<Str>;
using SVec = std::vector<Str>;

using FakInt = long long unsigned int;


enum class LoggerStatus { MsgLog, StageChange, LoadThing, CreateThing, Error };


template <typename Stream>
constexpr auto get_logger_gun(const LoggerStatus& status)
{
	std::unique_ptr<Command_var<Stream>> fire_obj;

    switch (status)
    {
        case LoggerStatus::MsgLog:
        {
            fire_obj = std::make_unique<FireMsgLog<Stream>>(); break;
        }
        case LoggerStatus::StageChange:
        {
            fire_obj = std::make_unique<FireStageChangeLog<Stream>>(); break;
        }
        case LoggerStatus::LoadThing:
        {
            fire_obj = std::make_unique<FireFileLoadedLog<Stream>>(); break;
        }
        case LoggerStatus::CreateThing:
        {
            fire_obj = std::make_unique<FireObjectCreatedLog<Stream>>(); break;
        }
		case LoggerStatus::Error:
		{
            fire_obj = std::make_unique<FireErrorLog<Stream>>(); break;	
		}
	}

	return fire_obj;
}

template <typename Stream>
void log_action(
    const LoggerHandler<Stream>& logger,
    const Either<Error, Str>& x,
    LoggerStatus status
)
{
    if (!x.isRight())
    {
		FireErrorLog<Stream>().execute(
			logger, Message( (*fromLeft(x)).toStr(), Location() )
		);
		return;
	}
	
	auto fire_obj = get_logger_gun<Stream>(status);
	fire_obj->execute(logger, Message( *fromRight(x), Location() ));
}

template <typename Stream>
void log_action(
    const LoggerHandler<Stream>& logger,
    const Either<Error, SVec>& x,
    LoggerStatus status
)
{
    if (!x.isRight())
    {
		FireErrorLog<Stream>().execute(
			logger, Data( (*fromLeft(x)).toStr(), Location() )
		);
		return;
	}
	
	auto vec = *fromRight(x);
	SVec sliced (vec.begin() + 1, vec.end());

	auto fire_obj = get_logger_gun<Stream>(status);
	fire_obj->execute(logger, Data(vec[0], sliced, Location() ));
}

template <int N>
struct Facktorial { static const FakInt value = N * Facktorial<N-1>::value; };

template <>
struct Facktorial<0> { static const FakInt value = 1; };

template <int... N>
constexpr auto generate_facktorials(std::integer_sequence<int, N...>)
{
	return std::array<FakInt, sizeof...(N)> { Facktorial<N>::value... };
}

template <int Size>
constexpr auto facktorials()
{
	return generate_facktorials(std::make_integer_sequence<int, Size + 1>{});
}

