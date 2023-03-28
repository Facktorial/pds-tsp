template <template <typename, typename> typename Monad, typename TLeft, typename TRight>
Monad<TLeft, TRight> mreturn(const TLeft& x)
{
	return x;
}

template <template <typename, typename> typename Monad, typename TLeft, typename TRight>
Monad<TLeft, TRight> mreturn(const TRight& x)
{
	return x;
}

//template <template <typename, typename> typename Monad, typename TLeft, typename TRight>
//auto Do(TRight x) { return mreturn<Monad, TLeft, TRight>(x); }

template<
	template <typename, typename> typename Monad,
	typename TLeft,
	typename TRight,
	typename F
>
auto operator>>=(const Monad<TLeft, TRight>& m, F f)
	-> decltype(f(std::declval<const TRight>()));

