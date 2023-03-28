#pragma once

#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <optional>
#include <numeric>
#include <limits>
#include <cassert>
#include <memory>
#include <utility>


#define WATCH(x) std::cout << (#x) << ": " << (x) << '\n'
#define WATCH_TYPE(x) std::cout << (#x) << ": " << typeid(x).name() << '\n'
#define PRINT(s, x) std::cout << (s) << ": " << (x) << '\n';
#define FOR_PRINT(s, items) for (auto x : (items)) { PRINT(s, x) }
#define FOR_DICT_PRINT(s, items) for (auto x : (items)) { PRINT(x.first, x.second) }
#define CAST(type, object) static_cast<type>(object)
#define NL std::cout << '\n'
#define ONE_WORD(vec, del) std::accumulate(vec.begin(), vec.end(), Str(""), [](Str acc, auto word){ return acc + del + std::to_string(word); })
#define ONE_WORD_SP(vec, del1, del2) std::accumulate(vec.begin(), vec.end(), Str(""), [](Str acc, auto word){ return acc + del1 + std::to_string(word) + del2; })
#define NESTED_ONE_WORD(vec, del, del_inner) std::accumulate(vec.begin(), vec.end(), Str(""), [](Str acc, auto word){ return acc + del + ONE_WORD(word, del_inner); })
#define NESTED_2_FORLOOP(N, M, block) for (size_t j = 0; j < N; j++) for (size_t i; i < M; i++) { block }


using Mint = std::optional<int>;
using Str = std::string;
using MStr = std::optional<Str>;
using SVec = std::vector<Str>;
using MSVec = std::optional<SVec>;

using Row = std::vector<size_t>;
using DistanceMatrix = std::vector<Row>;
using MDistanceMatrix = std::optional<std::vector<Row>>;
using Path = std::string_view;
using Town = size_t;
using Distance = size_t;
using Towns = std::vector<Town>;
using MeasuredTravel = std::pair<size_t, Distance>;

[[ maybe_unused ]] const size_t SIMD_SIZE_8 = 8;
[[ maybe_unused ]] const size_t SIMD_SIZE_4 = 4;

