#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <array>
#include <map>
#include <queue>
#include <chrono>
#include <functional>

#include "mediator.h"
#include "utils.cpp"
#include "single_thread_strategies.cpp"
#include "multi_thread_strategies.cpp"


const size_t SMALLEST_SIZE = 4;
[[ maybe_unused ]] const auto MINI_4 = "../data_assets/ulysses4.txt";
[[ maybe_unused ]] const auto MINI_5 = "../data_assets/ulysses5.txt";
[[ maybe_unused ]] const auto MINI_6 = "../data_assets/ulysses6.txt";
[[ maybe_unused ]] const auto MINI_7 = "../data_assets/ulysses7.txt";
[[ maybe_unused ]] const auto MINI = "../data_assets/ulysses8.txt";
[[ maybe_unused ]] const auto MINI_9 = "../data_assets/ulysses9.txt";
[[ maybe_unused ]] const auto MINI_10 = "../data_assets/ulysses10.txt";
[[ maybe_unused ]] const auto MINI_11 = "../data_assets/ulysses11.txt";
[[ maybe_unused ]] const auto MINI_12 = "../data_assets/ulysses12.txt";
[[ maybe_unused ]] const auto SMALL_13 = "../data_assets/ulysses13.txt";
[[ maybe_unused ]] const auto SMALL = "../data_assets/ulysses16.txt";
[[ maybe_unused ]] const auto BIGGER = "../data_assets/ulysses22.txt";

const size_t THREADS_COUNT = 16;


static MDistanceMatrix read_data(Path path)
{
	std::ifstream fs ( path.data() );
	if (!fs.is_open()) { return {}; }
	
	size_t num;
	Str line;
	
	std::getline(fs, line);
	std::stringstream s { line };
	s >> num;
	
	DistanceMatrix table ( num, Row(num) );
	
	size_t j = 0;
	while (std::getline(fs, line))
	{
		size_t i = 0;
		std::stringstream ss { line };
		while (ss >> num) { table[j][i++] = num; }
		++j;
	}
	
	return { table };
}

template <typename Fn>
MeasuredTravel measure_duration(const DistanceMatrix& graph, Fn method)
{
	auto start = std::chrono::steady_clock::now();
	
	auto result = method(graph);
	
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::steady_clock::now() - start
	).count();
	
	return std::make_pair( duration, result );
}

int main()
{
	LoggerHandler<std::ostream> logger (std::cout);
	logger.begin("\tWelcome to PDS project\n\tTake on TSP DVO0226 Edition\n");
	
	// auto table = read_data(MINI_4);
	// assert(table);
	// auto result = dumb_brute_forceTSP(*table);
	// WATCH(ONE_WORD(result, "\t"));

	//auto inputs = { MINI_4, MINI_5, MINI_6 };
	auto inputs = { MINI_4, MINI_5, MINI_6, MINI_7, MINI, MINI_9, MINI_10, MINI_11, MINI_12};
	std::map<Str, std::function<Distance(const DistanceMatrix&)>> methods = {
		// Single threaded
		{ "Dumb Brutal force", dumb_brute_forceTSP },
		{ "Brutal force", brute_forceTSP },
		{ "Slightly Brutal force", brute_slightlyoptimized_forceTSP	},
		{ "Brutal SIMD 8", simd_brute_forceTSP<SIMD_SIZE_8> },
		{ "Brutal SIMD 4", simd_brute_forceTSP<SIMD_SIZE_4> },
		// Multithreaded
		{ "Brutal Multithreaded " + std::to_string (THREADS_COUNT/4), threads_brute_forceTSP<THREADS_COUNT/4> },
		{ "Brutal Multithreaded " + std::to_string(THREADS_COUNT / 2), threads_brute_forceTSP<THREADS_COUNT/2> },
		{ "Brutal Multithreaded " + std::to_string(THREADS_COUNT), threads_brute_forceTSP<THREADS_COUNT> },
		{ "Brunch&Bound Multi" + std::to_string (THREADS_COUNT/4), threads_branch_n_bound_TSP<THREADS_COUNT/4> },
		{ "Brunch&Bound Multi " + std::to_string(THREADS_COUNT / 2), threads_branch_n_bound_TSP<THREADS_COUNT/2> },
		{ "Brunch&Bound Multi " + std::to_string(THREADS_COUNT), threads_branch_n_bound_TSP<THREADS_COUNT> },
		//{ "Brutal OPENMPI", threads_openmpi<THREADS_COUNT> },
		// cuda
		// META make them multithread
		// genetic
		// particle swarm optimization (pso)
		// ants
	};
	
	log_action(logger, { "About to mesasure: " }, LoggerStatus::StageChange);
	
	std::map<Str, std::vector<MeasuredTravel>> results;
	for (const auto& file : inputs)
	{
		auto table = read_data(file);
		assert(table);
	
		log_action(logger, { file }, LoggerStatus::CreateThing);
	
		for (auto [name, method] : methods)
		{
			results[name].push_back(measure_duration(*table, method));
		}
	}

	auto cols = std::vector<size_t>(inputs.size());
	std::iota(cols.begin(), cols.end(), SMALLEST_SIZE);
	log_action(
		logger, { "datasets:\t\t" + ONE_WORD(cols, "\t") }, LoggerStatus::CreateThing
	);

	for (auto [method_name, rs] : results)
	{
		std::vector<size_t> tms;
		std::vector<Distance> res;
		std::vector<int> scalling;

		for (auto [time, result] : rs)
		{
			tms.push_back(time);
			res.push_back(result);
		}

		scalling.push_back(1.0);
		for (auto idx = 1; idx < tms.size(); idx++)
		{
			scalling.push_back(tms[idx]/tms[idx - 1]);
		}

		log_action(logger, {
			SVec {
				method_name,
				"Result:\t\t" + ONE_WORD(res, "\t"),
				"Duration:\t" + ONE_WORD(tms, "\t"),
				"Scalling:\t" + ONE_WORD_SP(scalling, "\t", "x")
			}
		}, LoggerStatus::LoadThing);	
	}
	
	return 0;
}
