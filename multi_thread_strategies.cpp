#include "mediator.h"
#include <thread>
#include <mutex>

#include <cmath>
//#include "mediator.h"
//#include "utils.cpp"

// template <size_t ThreadsCount>
// static Towns threads_openmpi(const DistanceMatrix& graph)
// {
// 	const auto size = graph.size();
// 	auto fcks = facktorials<MAX_FACK>();
// 	assert (size <= MAX_FACK); 
// 	
// 	Towns result (size, std::numeric_limits<Town>::max());
// 	Towns towns (size, 0);
// 	std::iota(towns.begin(), towns.end(), 0);
// 	
// 	size_t wload_sz = CAST(size_t, std::ceil(fcks[size] / CAST(float, ThreadsCount)));
// 
// 	int rank, num_workers;
// 	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
// 	MPI_Comm_size(MPI_COMM_WORLD, &num_workers);
// 
// 	do {
// 		size_t acc = 0;
// 	
// 		const auto first = towns[0];
// 		auto prev = first;
// 		for (size_t i = 1; i < size; i++)
// 		{
// 			acc += graph[prev][towns[i]];
// 			prev = towns[i];
// 			if (acc > result[first]) { break; }
// 		}
// 		acc += graph[prev][first];
// 	
// 		result[first] = std::min(result[first], acc);
// 
// 		//#pragma omp barier
// 	
// 	} while (std::next_permutation(towns.begin(), towns.end()));
// 
// 	return result;
// }
template <typename Tx, typename Ty>
struct Pair
{
	Tx x;
	Ty y;
};

struct LoggThreads
{
	std::vector<std::tuple<std::string, size_t, size_t>> states;
};

static size_t traverse_bnb(
	//Towns towns, size_t base, size_t level, size_t result, const DistanceMatrix& graph, LoggThreads& log)
	Towns towns, size_t base, size_t level, size_t result, const DistanceMatrix& graph)
{
	size_t acc = std::numeric_limits<int>::max();
	if (base >= result) { return acc; }
	//if (level > 2) { return {}; }

	if (towns.size() - level <= 4)
	{
		// check 2 variant:
		using Indices = std::array<Pair<size_t, size_t>, 3>;
		Indices part_1 = {{ {level, level + 1}, {level + 1, level + 2}, {level + 2, 0}}};
		Indices part_2 = {{ {level, level + 2}, {level + 2, level + 1}, {level + 1, 0}}};

		auto acc_a = 0;
		auto acc_b = 0;
		for (auto idx = 0; idx < part_1.size(); idx++)
		{
			acc_a += graph[towns[part_1[idx].x]][towns[part_1[idx].y]];
			acc_b += graph[towns[part_2[idx].x]][towns[part_2[idx].y]];
		}

		//log.states.emplace_back(std::make_tuple(ONE_WORD(towns, "") + std::string("a"), level, base + acc_a));
		return { base + std::min(acc_a, acc_b) };
	}

	const auto first = towns[level];
	for (auto p_count = 0; p_count < towns.size() - level - 2; p_count++)
	{
		std::swap(towns[level + 1], towns[level + p_count + 1]);

		auto prev = towns[level + 1];

		auto ret = traverse_bnb(
			towns,
			base + graph[first][prev],
			level + 1,
			result,
			graph
		);

		std::swap(towns[level + p_count + 1], towns[level + 1]);

		acc = std::min(acc, ret);
	}

	return acc;
}

static void bb_lambda_fn(size_t thread_idx,
		const std::vector<Towns>& workload,
		const DistanceMatrix& graph,
		std::vector<Distance>& results
)
{
	for (auto towns : workload)
	{
		size_t acc = graph[towns[0]][towns[1]];

		auto ret = traverse_bnb(towns, acc, 1, results[thread_idx], graph);
		
		// for (auto [state, lvl, score] : log.states)
		// {
		// 	PRINT(state + " lvl: " + std::to_string(lvl), score);
		// }

		results[thread_idx] = std::min(results[thread_idx], ret);
	}
}


template <size_t ThreadsCount>
static Distance threads_branch_n_bound_TSP(const DistanceMatrix& graph)
{
	const auto size = graph.size();
	const auto n_threads = std::min(ThreadsCount, size);

	std::vector<std::thread> threads (n_threads);
	std::vector<std::vector<Towns>> workload (n_threads);
	std::vector<Distance> results (n_threads, std::numeric_limits<Town>::max());

	Towns towns (size);
	std::iota(towns.begin(), towns.end(), 0);

	auto thread_idx = 0;
	for (auto p_count = 0; p_count < size - 1; p_count++)
	{
		Towns cp_towns (size + 1);
		std::copy(towns.begin(), towns.end(), cp_towns.begin());
		cp_towns[size] = towns[0];

		std::swap(cp_towns[1], cp_towns[cp_towns[p_count + 1]]);

		workload[thread_idx].push_back(cp_towns);

		thread_idx = (thread_idx + 1) % n_threads;
	}

	for (thread_idx = 0; thread_idx < threads.size(); thread_idx++)
	{
		if (workload[thread_idx].size() > 0)
		{
			threads[thread_idx] = std::thread([thread_idx, &workload, &graph, &results]()
			{
				return bb_lambda_fn(thread_idx, workload[thread_idx], graph, results);
			});			
		}
		else
		{
			threads.erase(threads.begin() + thread_idx);
			workload.erase(workload.begin() + thread_idx);
			results.erase(results.begin() + thread_idx);
		}
	}
	for (size_t i = 0; i < threads.size(); i++) { threads[i].join(); }
	return std::accumulate(
		results.begin(), results.end(),	std::numeric_limits<size_t>::max(),
		[](auto acc, auto b) -> Distance { return std::min(acc, b); }
	);
}

static void lambda_fn(size_t thread_idx,
		const std::vector<Towns>& workload,
		const DistanceMatrix& graph,
		std::vector<Distance>& results
)
{
	for (auto const& towns : workload)
	{
		size_t acc = 0;

		const auto first = towns[0];
		auto prev = first;
		for (size_t i = 1; i < graph.size(); i++)
		{
			acc += graph[prev][towns[i]];
			prev = towns[i];
			//if (acc > results[thread_idx]) { break; }
		}
		acc += graph[prev][first];

		if (acc != 0u) { results[thread_idx] = std::min(results[thread_idx], acc); }
	}
}

template <size_t ThreadsCount>
static Distance threads_brute_forceTSP(const DistanceMatrix& graph)
{
	const auto size = graph.size();
	
	auto fcks = facktorials<MAX_FACK>();
	assert (size <= MAX_FACK); 
	
	const auto n_threads = std::min(ThreadsCount, size);
	const auto wload_sz = CAST(size_t, std::ceil(fcks[size-1]/CAST(float, n_threads)));

	std::vector<std::thread> threads(n_threads);

	std::vector<std::vector<Towns>> workload(n_threads);
	for (auto& vec : workload) { vec = std::vector(wload_sz, Towns(size + 1)); }

	std::vector<Distance> results (n_threads, std::numeric_limits<Town>::max());

	Towns towns (size);
	std::iota(towns.begin(), towns.end(), 0);

	size_t thread_idx = 0;
	size_t row = 0;
	do {
		auto& vec = workload[thread_idx][row];
		std::copy(towns.begin(), towns.end(), vec.begin());
		vec[size] = towns[0];

		if (thread_idx == n_threads - 1) { ++row; }
		thread_idx = (thread_idx + 1) % n_threads;
	
	} while (std::next_permutation(towns.begin() + 1, towns.end()));

	// remove empty routes "00...00"
	for (; thread_idx < n_threads; thread_idx++) { workload[thread_idx].pop_back(); }

	for (thread_idx = 0; thread_idx < threads.size(); thread_idx++)
	//for (thread_idx = 0; thread_idx < 1; thread_idx++)
	{
		threads[thread_idx] = std::thread([thread_idx, &workload, &graph, &results]() {
			return lambda_fn(thread_idx, workload[thread_idx], graph, results);
		});
	}
	for (size_t i = 0; i < threads.size(); i++) { threads[i].join(); }
	//for (size_t i = 0; i < 1; i++) { threads[i].join(); }

	return std::accumulate(
		results.begin(), results.end(),	std::numeric_limits<size_t>::max(),
		[](auto acc, auto b) -> Distance { return std::min(acc, b); }
	);
}

