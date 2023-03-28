#include <iostream>
#include <typeinfo>

#include "include/Either.h"


int main()
{
	auto x = Left<int, std::string>(5);
	auto y = Right<int, std::string>("5");

	auto e1 = Either<int, std::string>(5);
	auto e2 = Either<int, std::string>("7");

	std::cout << *fromLeft(e1) << '\n';
	std::cout << (fromLeft(e2) ? "1" : "0") << '\n';

	std::cout << *fromRight(e2) <<'\n';
	std::cout << (fromRight(e1) ? "1" : "0") << '\n';

	std::vector<Either<int, std::string>> sample = {
		{ 1 },
		{ "2" },
		{ 3 },
		{ "4" },
		{ 5 },
		{ "6" },
		{ "7" },
		{ "8" },
		{ 14 }
	};

	puts("testing lefts");
	for (auto num : lefts(sample)) { std::cout << num << '\n'; }
	puts("testing rights");
	for (auto str : rights(sample)) { std::cout << str << '\n'; }

	auto vec = partitionEithers<int, std::string>(sample);
	puts("testing partitionEithers lefts");
	for (auto item : vec.lefts) { std::cout << item << '\n'; }
	puts("testing partition rights");
	for (auto item : vec.rights) { std::cout << item << '\n'; }

	puts("testingeither funciton");
	std::cout << either(
			[](int a) -> std::string { return  std::to_string(a) + "--"; },
			[](std::string a) -> std::string { return a + "_"; },
		e1) << '\n';
	std::cout << either(
			[](auto a) -> std::string { return std::to_string(a) + "--"; },
			[](auto a) -> std::string { return a + "_"; },
		e2) << '\n';

	return 0;
}
