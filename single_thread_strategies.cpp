#include "mediator.h"
#include <immintrin.h>


[[ maybe_unused ]] const size_t REGISTER_COUNT = 8;
[[ maybe_unused ]] const size_t REGISTER_SIZE = 256;


static void print_m128i(__m128i v)
{
	alignas( REGISTER_SIZE/REGISTER_COUNT ) int tmp[REGISTER_COUNT/2];
	_mm_storeu_si128((__m128i*) tmp, v);
	
	std::cout << "{";
	for (size_t i = 0; i < REGISTER_COUNT/2; i++)
	{
		std::cout << tmp[i];
		if (i < (REGISTER_COUNT/2 - 1)) { std::cout << ", "; }
	}
	std::cout << "}\n";
}

static void print_m256i(__m256i v)
{
	alignas( REGISTER_SIZE/REGISTER_COUNT ) int tmp[REGISTER_COUNT];
	_mm256_storeu_si256((__m256i*) tmp, v);
	
	std::cout << "{";
	for (size_t i = 0; i < REGISTER_COUNT; i++)
	{
		std::cout << tmp[i];
		if (i < (REGISTER_COUNT - 1)) { std::cout << ", "; }
	}
	std::cout << "}\n";
}

template <size_t SIMD_SIZE>
static Distance simd_brute_forceTSP(const DistanceMatrix& graph)
{
	const auto size = graph.size();
	Distance result { std::numeric_limits<Town>::max() };
	//Towns towns(size + 1);
	std::vector<int> towns(size + 1);
	std::iota(towns.begin(), towns.end(), 0);
	towns[size] = towns[0];
	
	const size_t simd_end = size - (size % SIMD_SIZE);
	
	auto flatt_graph = std::vector<int> (size * size);
	auto it = flatt_graph.begin();
	for (const auto& line : graph) { 
		it = std::transform(line.begin(), line.end(), it, [](const auto& elem) {
			return CAST(int32_t, elem);
		});
	}
	
	__m256i size_vec = _mm256_set1_epi32(size);
	__m128i size_vec_4 = _mm_set1_epi32(size);
	
	do
	{
		size_t acc = 0;
	
		size_t i = 0;
		if (simd_end > 0)
		{
			for (; i < simd_end; i+= SIMD_SIZE)
			{
				if constexpr (SIMD_SIZE == SIMD_SIZE_4)
				{
					__m128i prevs = _mm_loadu_si128((__m128i*)& towns[i]);
					__m128i nexts = nexts = _mm_loadu_si128((__m128i*)& towns[i + 1]);

					__m128i index1 = _mm_mullo_epi32(prevs, size_vec_4);
					__m128i index2 = _mm_add_epi32(index1, nexts);

					__m128i dists = _mm_i32gather_epi32(
							(__m128i*)& flatt_graph[0],
							index2,
							sizeof(int32_t)
					);

					__m128i sum = _mm_hadd_epi32(dists, dists);
					sum = _mm_hadd_epi32(sum, sum);
	
					int sum_result = _mm_extract_epi32(sum, 0);
					acc += CAST(size_t, sum_result);
				}
				else if (SIMD_SIZE == 8)
				{
					__m256i prevs = _mm256_loadu_si256((__m256i*)& towns[i]);
					__m256i nexts = _mm256_loadu_si256((__m256i*)& towns[i + 1]);

					__m256i index1 = _mm256_mullo_epi32(prevs, size_vec);
					__m256i index2 = _mm256_add_epi32(index1, nexts);

					__m256i dists = _mm256_i32gather_epi32(
							(__m256i*)& flatt_graph[0],
							index2,
							sizeof(int32_t)
					);
	
					__m256i sum = _mm256_hadd_epi32(dists, dists);
					sum = _mm256_hadd_epi32(sum, sum);
	
					int sum_result = _mm256_extract_epi32(sum, 0) + _mm256_extract_epi32(sum, 4);
	
					acc += CAST(size_t, sum_result);
				}
				else { break; }

				if (acc > CAST(size_t, result)) { i = simd_end; break; }
			}
		}


		// Scalar loop for remaining elements
		if (i < size)
		{
			for (; i < size; i++) { acc += graph[towns[i + 1]][CAST(size_t, towns[i])]; }
		}
		
		result = std::min(result, acc);
	
	} while (std::next_permutation(towns.begin() + 1, towns.end() - 1));

	return result;
}

static Distance dumb_brute_forceTSP(const DistanceMatrix& graph)
{
	const auto size = graph.size();
	Distance result { std::numeric_limits<Town>::max() };
	Towns towns (size, 0);
	std::iota(towns.begin(), towns.end(), 0);
	
	do {
		size_t acc = 0;
	
		const auto first = towns[0];
		auto prev = first;
		for (size_t i = 1; i < size; i++)
		{
			acc += graph[prev][towns[i]];
			prev = towns[i];
		}
		acc += graph[prev][first];
	
		result = std::min(result, acc);
	
	} while (std::next_permutation(towns.begin(), towns.end()));
	
	return result;
}

static Distance brute_forceTSP(const DistanceMatrix& graph)
{
	const auto size = graph.size();
	Distance result { std::numeric_limits<Town>::max() };
	Towns towns (size, 0);
	std::iota(towns.begin(), towns.end(), 0);
	
	do {
		size_t acc = 0;
	
		const auto first = towns[0];
		auto prev = first;
		for (size_t i = 1; i < size; i++)
		{
			acc += graph[prev][towns[i]];
			prev = towns[i];
		}
		acc += graph[prev][first];
	
		result = std::min(result, acc);
	
	} while (std::next_permutation(towns.begin() + 1, towns.end()));
	
	return result;
}

static Distance brute_slightlyoptimized_forceTSP(const DistanceMatrix& graph)
{
	const auto size = graph.size();
	Distance result { std::numeric_limits<Town>::max() };
	Towns towns (size, 0);
	std::iota(towns.begin(), towns.end(), 0);
	
	//size_t op_count = 0;
	do {
		size_t acc = 0;
	
		const auto first = towns[0];
		auto prev = first;
		for (size_t i = 1; i < size; i++)
		{
			acc += graph[prev][towns[i]];
			prev = towns[i];
			if (acc > result) { break; }
		}
		acc += graph[prev][first];
	
		result = std::min(result, acc);
	
		//++op_count;
	} while (std::next_permutation(towns.begin() + 1, towns.end()));
//WATCH(op_count);	
	return result;
}

