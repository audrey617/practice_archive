// FILENAME: yl6274_hw9_main.cpp
// PROGRAMMER: Yunqi 
// DATE: 6/8/2021
// COMPILER: Clang++
// PURPOSE: Initialize the world with 5 doodlebugs and 100 ants. After each time step, prompt the user to press Enter to move to the next time step. 
// You should see a cyclical pattern between the population of predators and prey, although random perturbations may lead to the elimination of one or both species.

#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unordered_set>

struct Coordinate {
	int x = 0;
	int y = 0;

	friend bool operator==(const Coordinate& lhs, const Coordinate& rhs) {
		return lhs.x == rhs.x && lhs.y == rhs.y;
	}
};

class Organism;

struct CoordinateWithOrganism {
	Coordinate c;
	Organism* o = nullptr;
};

class Organism {
protected:
	int _steps = 0;
	int _generation = 0;
	Coordinate _locationxy;

public:
	virtual Coordinate getCoordinate() { return _locationxy; }
	virtual void setCoordinate(Coordinate newlocation) { _locationxy = newlocation; }
	virtual int getGeneration() { return _generation; }
	virtual void setGeneration(int num) { _generation = num; }
	virtual char getsymbol() = 0;
	virtual Coordinate move(std::vector<CoordinateWithOrganism> validAdjacentCell) = 0;
	virtual Coordinate breed(std::vector<Coordinate> validAdjacentEmptycell) = 0;
	virtual ~Organism() {};
};

class Ant :public Organism {
public:
	// constructor
	Ant(Coordinate location) { _locationxy = location; }
	// functions
	virtual char getsymbol() override { return 'o'; }
	virtual Coordinate move(std::vector<CoordinateWithOrganism> validAdjacentCell) override;
	virtual Coordinate breed(std::vector<Coordinate> validAdjacentEmptycell) override;
	// Destructor
	virtual ~Ant() {};
};

class Doodlebugs :public Organism {
private:
	int _laststepswithfood = 0;
public:
	// constructor
	Doodlebugs(Coordinate location) { _locationxy = location; }
	// functions
	virtual char getsymbol() override { return  'X'; }
	virtual Coordinate move(std::vector<CoordinateWithOrganism> validAdjacentAntcell) override;
	virtual Coordinate breed(std::vector<Coordinate> validAdjacentEmptycell) override;
	bool starve();
	// Destructor
	virtual ~Doodlebugs() {};
};


class World {
private:
	std::vector< std::vector<Organism*> > vec;
	std::unordered_set<Organism*> ant_dict;
	std::unordered_set<Organism*> bug_dict;
	int rows;
	int cols;

public:
	// Constructor
	World(int row, int col);

	// Display the map
	void displayWorld();

	// Display the map
	int hackCountBugs() {
		int bugCount = 0;
		for (int i = 0; i < vec.size(); i++) {
			for (int j = 0; j < vec[i].size(); j++) {
				if (vec[i][j] != nullptr) {
					if (vec[i][j]->getsymbol() == 'X') {
						++bugCount;
					}
				}
			}
		}
		return bugCount;
	}

	// Return ant dict 
	std::unordered_set<Organism*> getAnt_dict() { return ant_dict; }

	// Return bug dict 
	std::unordered_set<Organism*> getBug_dict() { return bug_dict; }

	// Check whether the spot is empty. GetContents return Organism *.
	Organism* getContent(Coordinate location);

	// Check whether the spot is within the map range
	bool isnotValidSpot(int x, int y);

	// Check adjecent spots 
	std::vector<Coordinate> getValidAdjacentEmptycell(Coordinate location);

	// Check whether adjecent cell has ant
	std::vector<Coordinate> getValidAdjacentAntcell(Coordinate location);

	//  Remove the organism
	void removeInWorld(Organism* thisinsect);

	// Real Move function
	void moveInsectInWorld(Organism* insect, Coordinate direction);

	// Generate an ant/doodlebug in a random location in the map
	void generateInsectRandomLocation(char type);

	// Generate an ant/doodlebug in an adjacent location in the map
	void generateInsectAdjacent(Organism* insect, Coordinate location, char type);
};



Coordinate pickRandomCell(std::vector<Coordinate> cells) {
	if (cells.empty()) {
		return Coordinate{ -1, -1 };
	}
	int pick = rand() % cells.size();
	return cells.at(pick);
}

CoordinateWithOrganism pickRandomCell(std::vector<CoordinateWithOrganism> cells) {
	if (cells.empty()) {
		return CoordinateWithOrganism{ .c = { -1, -1 }, .o = nullptr };
	}
	int pick = rand() % cells.size();
	return cells.at(pick);
}

Coordinate Ant::move(std::vector<CoordinateWithOrganism> validAdjacentEmptycell) {
	_steps++;
	return pickRandomCell(validAdjacentEmptycell).c;
}

Coordinate Ant::breed(std::vector<Coordinate> validAdjacentEmptycell) {
	if (_steps % 3 == 0) {

		if (validAdjacentEmptycell.empty()) {
			return Coordinate{ -1,-1 };
		}

		int pick = rand() % validAdjacentEmptycell.size();
		// std::cout << "Creating new organism at: " << validAdjacentEmptycell.at(pick).x << ", " << validAdjacentEmptycell.at(pick).y << " from " << getCoordinate().x << ", " << getCoordinate().y << std::endl;
		return validAdjacentEmptycell.at(pick);
	}
	return Coordinate{ -1,-1 };
}

Coordinate Doodlebugs::move(std::vector<CoordinateWithOrganism> validAdjacentCells) {
	_steps++;
	for (const auto& [c, o] : validAdjacentCells) {
		if (o != nullptr && o->getsymbol() == 'o') {
			_laststepswithfood = _steps;
			//std::cout << "Eating organism at: " << c.x << ", " << c.y << " from " << getCoordinate().x << ", " << getCoordinate().y << std::endl;
			return c;
		}
	}
	const auto choice = pickRandomCell(validAdjacentCells).c;
	//std::cout << "Randomly moving to: " << choice.x << ", " << choice.y << " from " << getCoordinate().x << ", " << getCoordinate().y << std::endl;
	return choice;
}


Coordinate Doodlebugs::breed(std::vector<Coordinate> validAdjacentEmptycell) {
	if (_steps % 8 == 0) {

		if (validAdjacentEmptycell.empty()) {
			return Coordinate{ -1,-1 };
		}

		int pick = rand() % validAdjacentEmptycell.size();
		return validAdjacentEmptycell.at(pick);
	}
	return Coordinate{ -1,-1 };
}

bool Doodlebugs::starve() {
	return (_steps - _laststepswithfood > 3);
}

World::World(int row, int col) {
	// Inserting elements into vector
	rows = row;
	cols = col;

	for (int i = 0; i < row; i++) {
		// Vector to store column elements
		std::vector<Organism*> v1;

		for (int j = 0; j < col; j++) {
			v1.push_back(nullptr);
		}

		vec.push_back(v1);
	}

}

// Display the map
void World::displayWorld() {
	for (int i = 0; i < vec.size(); i++) {
		for (int j = 0; j < vec[i].size(); j++) {
			if (vec[i][j] == nullptr) {
				std::cout << '-';
			}
			else {
				std::cout << (vec[i][j]->getsymbol());
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

Organism* World::getContent(Coordinate location) {
	return vec[location.x][location.y];
}

bool World::isnotValidSpot(int x, int y) {
	return (x < 0) || (x >= rows) || (y < 0) || (y >= cols);
}

std::vector<Coordinate> World::getValidAdjacentEmptycell(Coordinate location) {
	std::vector<Coordinate> temp;

	const auto try_push_dest = [this, &temp](Coordinate dest) {
		if (!isnotValidSpot(dest.x, dest.y) && vec[dest.x][dest.y] == nullptr) {
			temp.push_back(dest);
		}
	};
	try_push_dest(Coordinate{ location.x - 1, location.y });
	try_push_dest(Coordinate{ location.x + 1, location.y });
	try_push_dest(Coordinate{ location.x, location.y - 1 });
	try_push_dest(Coordinate{ location.x, location.y + 1 });

	return temp;
}


std::vector<Coordinate> World::getValidAdjacentAntcell(Coordinate location) {
	std::vector<Coordinate> temp;

	const auto try_push_dest = [this, &temp](Coordinate dest) {
		if (!isnotValidSpot(dest.x, dest.y) && vec[dest.x][dest.y] != nullptr && vec[dest.x][dest.y]->getsymbol() == 'o') {
			temp.push_back(dest);
		}
	};
	try_push_dest(Coordinate{ location.x - 1, location.y });
	try_push_dest(Coordinate{ location.x + 1, location.y });
	try_push_dest(Coordinate{ location.x, location.y - 1 });
	try_push_dest(Coordinate{ location.x, location.y + 1 });

	return temp;
}

void World::removeInWorld(Organism* thisinsect) {
	auto coord = thisinsect->getCoordinate();
	vec[coord.x][coord.y] = nullptr; // insect is no longer in the world
	if (thisinsect->getsymbol() == 'o') {
		ant_dict.erase(thisinsect);
	}
	else {
		bug_dict.erase(thisinsect);
	}

	delete thisinsect;
	thisinsect = nullptr;
}

void World::moveInsectInWorld(Organism* insect, Coordinate new_location) {

	if (new_location == Coordinate{ -1, -1 }) {
		return;
	}

	Coordinate curr_location = insect->getCoordinate();

	if (auto target = getContent(new_location); target != nullptr) {
		if (target->getsymbol() == 'o' && insect->getsymbol() == 'X') {
			removeInWorld(vec[new_location.x][new_location.y]); // delete ants
		}
		else {
			return; // fail to move
		}
	}

	vec[new_location.x][new_location.y] = insect; // put insect in the new place
	vec[curr_location.x][curr_location.y] = nullptr;
	insect->setCoordinate(new_location);
}

void World::generateInsectRandomLocation(char type) {
	Coordinate thisinsect;

	do {
		thisinsect.x = rand() % rows;
		thisinsect.y = rand() % cols;
	} while (getContent(thisinsect) != nullptr);

	if (type == 'X') {
		auto bug_ptr = new Doodlebugs(thisinsect);
		vec[thisinsect.x][thisinsect.y] = bug_ptr;
		bug_dict.insert(bug_ptr);
	}

	if (type == 'o') {
		auto ant_ptr = new Ant(thisinsect);
		vec[thisinsect.x][thisinsect.y] = ant_ptr;
		ant_dict.insert(ant_ptr);
	}

}

void World::generateInsectAdjacent(Organism* insect, Coordinate new_location, char type) {
	if (new_location == Coordinate{ -1, -1 }) {
		return;
	}

	int new_generation = insect->getGeneration() + 1;
	if (type == 'X') {
		auto bug_ptr = new Doodlebugs(new_location);
		bug_ptr->setGeneration(new_generation);
		vec[new_location.x][new_location.y] = bug_ptr;
		bug_dict.insert(bug_ptr);
	}

	if (type == 'o') {
		auto ant_ptr = new Ant(new_location);
		ant_ptr->setGeneration(new_generation);
		vec[new_location.x][new_location.y] = ant_ptr;
		ant_dict.insert(ant_ptr);
	}
}

int main() {
	srand(time(nullptr));

	//Initialize the world with 5 doodlebugs and 100 ants.After each time step, prompt the user to press Enter to move to the next time step.
	World gridworld_predators(20,20);
	for (int i = 1; i < 11; i++) {
		gridworld_predators.generateInsectRandomLocation('o');
	}

	for (int i = 1; i < 6; i++) {
		gridworld_predators.generateInsectRandomLocation('X');
	}

	gridworld_predators.displayWorld();
	int stage = 0;
	char answer = 'y';
	
	std::cout << " Current World Stage: " << stage << std::endl;
	std::cout << " Evolve the World  - please enter 'y' or 'n'" << std::endl;
	std::cin >> answer;

	while (answer == 'y') {
		const auto bug_list = gridworld_predators.getBug_dict();
		//for (const auto& e : bug_list) {
		//	std::cout << "DB: " << e << std::endl;
		//}

		if (gridworld_predators.getBug_dict().size() != gridworld_predators.hackCountBugs()) {
			std::cout << "BIG ERROR COUNT MISMATCH: " << gridworld_predators.getBug_dict().size() << " vs " << gridworld_predators.hackCountBugs() << std::endl;
		}

		//std::cout << "Antlion turn:" << std::endl;
		for (const auto& bug : bug_list) {
			std::vector<Coordinate> validAdjacentAntcell = gridworld_predators.getValidAdjacentAntcell(bug->getCoordinate());
			std::vector<CoordinateWithOrganism> coordinate_with_organism;
			if (!validAdjacentAntcell.empty()) {
				for (const auto& found : validAdjacentAntcell) {
					coordinate_with_organism.push_back(CoordinateWithOrganism{ .c = found, .o = gridworld_predators.getContent(found) });
				}
				auto victim = bug->move(coordinate_with_organism);
				//std::cout << "We ate victim " << gridworld_predators.getContent(victim) << std::endl;
				gridworld_predators.moveInsectInWorld(bug, victim);
			}
			else {
				for (const auto& cell : gridworld_predators.getValidAdjacentEmptycell(bug->getCoordinate())) {
					coordinate_with_organism.push_back(CoordinateWithOrganism{ .c = cell, .o = nullptr });
				}
				gridworld_predators.moveInsectInWorld(bug, bug->move(coordinate_with_organism));
			}

			std::vector<Coordinate> validAdjacentEmptycell = gridworld_predators.getValidAdjacentEmptycell(bug->getCoordinate());
			gridworld_predators.generateInsectAdjacent(bug, bug->breed(validAdjacentEmptycell), 'X');
			if (static_cast<Doodlebugs*>(bug)->starve()) {
				//std::cout << "Starvation! : " << bug->getname() << ", " << bug<< std::endl;
				gridworld_predators.removeInWorld(bug);
			}
		}

		if (gridworld_predators.getBug_dict().size() != gridworld_predators.hackCountBugs()) {
			std::cout << "BIG ERROR2 COUNT MISMATCH: " << gridworld_predators.getBug_dict().size() << " vs " << gridworld_predators.hackCountBugs() << std::endl;
		}
		//gridworld_predators.displayWorld();

		const auto ant_list = gridworld_predators.getAnt_dict();
		//for (const auto& [k, v] : ant_list) {
		//	std::cout << "Ant: " << k << ", " << v << std::endl;
		//}
		//std::cout << "Ant turn:" << std::endl;
		for (const auto& ant : ant_list) {
			std::vector<CoordinateWithOrganism> coordinate_with_organism;
			for (const auto& cell : gridworld_predators.getValidAdjacentEmptycell(ant->getCoordinate())) {
				if (auto victim = gridworld_predators.getContent(cell); victim != nullptr) {
					std::cout << "Error! Cell is not actually empty :/. " << ant->getCoordinate().x << ", " << ant->getCoordinate().y << ", victim: " << victim->getCoordinate().x << ", " << victim->getCoordinate().y << std::endl;
				}
				coordinate_with_organism.push_back(CoordinateWithOrganism{ .c = cell, .o = nullptr });
			}
			gridworld_predators.moveInsectInWorld(ant, ant->move(coordinate_with_organism));
			gridworld_predators.generateInsectAdjacent(ant, ant->breed(gridworld_predators.getValidAdjacentEmptycell(ant->getCoordinate())), 'o');
		}

		gridworld_predators.displayWorld();
		stage++;
		std::cout << " Current World Stage: " << stage << std::endl;
		std::cout << " Evolve the World  - please enter 'y' or 'n'" << std::endl;
		std::cin >> answer;
	}
}