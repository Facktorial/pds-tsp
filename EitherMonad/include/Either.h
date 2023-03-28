#pragma once
#include <optional>
#include <vector>


template <typename L, typename R> 
struct EithersData
{
	std::vector<L> lefts;
	std::vector<R> rights;
};

template <typename L, typename R>
class Either
{
	std::optional<L> m_left;
	std::optional<R> m_right;

	template <typename L_, typename R_> friend Either<L_, R_> Left(L_ x);
	template <typename L_, typename R_> friend Either<L_, R_> Right(R_ x);

	template <typename L_, typename R_> friend 
	std::optional<L_> fromLeft(const Either<L_, R_>& e);
	template <typename L_, typename R_> friend
	std::optional<R_> fromRight(const Either<L_, R_>& e);

public:
	Either(const L& l) : m_left(l) {}
	Either(const R& r) : m_right(r) {}
	Either(L&& l) : m_left(l) {}
	Either(R&& r) : m_right(r) {}

	using RighType = R;
	using LeftType = L;

	// warning: definition of implicit copy constructor for ... is deprecated because it has a user-provided destructor [-Wdeprecated-copy-with-user-provided-dtor]
	// ~Either() { if ( isLeft() ) { (*m_left).~L(); } else { (*m_right).~R(); } }

	bool isLeft() const { return m_left.has_value(); }
	bool isRight() const { return m_right.has_value(); }

	//L Left() const { return m_left.get(); }
	//R Right() const { return m_right.get(); }
};


template <typename L, typename R> Either<L, R> Left(L x) { return { x }; }
template <typename L, typename R> Either<L, R> Right(R x) {	return { x }; }

template <typename L, typename R>
std::optional<L> fromLeft(const Either<L, R>& e) { return e.m_left; }
template <typename L, typename R>
std::optional<R> fromRight(const Either<L, R>& e) {	return e.m_right; }
 
template <typename LF, typename RF, typename L, typename R>
auto either(LF lFn, RF rFn, Either<L, R> ei)
{
	if ( ei.isLeft() ) { return lFn(*fromLeft(ei)); } 
	else { return rFn(*fromRight(ei)); } 
}

template <typename L, typename R>
using EitherVec = std::vector<Either<L, R>>;

template <typename L, typename R>
std::vector<L> lefts(EitherVec<L, R> vec)
{
	std::vector<L> ret_vec;
	for (auto item : vec)
	{
		auto x = fromLeft(item);
		if (x) { ret_vec.push_back(*x); } 
	}
	return ret_vec;
}

template <typename L, typename R>
std::vector<R> rights(EitherVec<L, R> vec)
{
	std::vector<R> ret_vec;
	for (auto item : vec)
	{
		auto x = fromRight(item);
		if (x) { ret_vec.push_back(*x); } 
	}
	return ret_vec;
}

template <typename L, typename R>
EithersData<L, R> partitionEithers(EitherVec<L, R> vec)
{
	return { lefts(vec), rights(vec) };
}

