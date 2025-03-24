#include "nlohmann/json.hpp"
#include <cadmium/modeling/celldevs/grid/coupled.hpp>
#include <cadmium/simulation/logger/csv.hpp>
#include <cadmium/simulation/root_coordinator.hpp>
#include <chrono>
#include <fstream>
#include <string>
#include "include/playerCell.hpp"

using namespace cadmium::celldevs;
using namespace cadmium;

std::shared_ptr<GridCell<playerState, double>> addGridCell(const coordinates & cellId, const std::shared_ptr<const GridCellConfig<playerState, double>>& cellConfig) {
	auto cellModel = cellConfig->cellModel;

	if (cellModel == "player") {
		return std::make_shared<player>(cellId, cellConfig);
	} else {
		throw std::bad_typeid();
	}
}

int main(int argc, char ** argv) {
    if (argc < 2) {
		std::cout << "Program used with wrong parameters. The program must be invoked as follows:";
		std::cout << argv[0] << " SCENARIO_CONFIG.json [MAX_SIMULATION_TIME (default: 500)]" << std::endl;
		return -1;
	}
	std::string configFilePath = argv[1];
	double simTime = (argc > 2)? std::stod(argv[2]) : 500;

    auto model = std::make_shared<GridCellDEVSCoupled<playerState, double>>("player", addGridCell, configFilePath);
	model->buildModel();

    auto rootCoordinator = RootCoordinator(model);
	rootCoordinator.setLogger<CSVLogger>("grid_log.csv", ";");
	
	rootCoordinator.start();
	rootCoordinator.simulate(simTime);
	rootCoordinator.stop();
}