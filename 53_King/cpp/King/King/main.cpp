//
//  main.cpp
//  King
//
//  Created by tkvitko on 14.04.2024.
//

#include <iostream>


class GameState {
private:
    
    // время
    unsigned short years = 0;  // лет правления
    
    // финансы
    short balance = 0;  // количество денег в казне
    
    // люди
    unsigned short countymen = 0;  // количество жителей
    unsigned short foreigners = 0; // количество иностранных рабочих
    
    // земли
    unsigned short total_land = 2000;  // общая площадь земли
    unsigned short forest_land = 1000; // площадь леса
    unsigned short farm_land = this->total_land - this->forest_land; // площадь сельхоз земель
    
    // статистика с прошлого года
    unsigned short last_year_harvest_loss = 0; // потери урожая

    // параметры текущего года
    unsigned short died_count = 0;                  // общее количество погибших жителей
    unsigned short died_becaufe_of_pollution = 0;   // количество погибших жителей по причине загрязнения
    short population_change = 0;                    // изменение населения
    
    // экономика текущего года
    unsigned short cost_of_planting_land = 0;       // стоимость засева земли
    unsigned short price_of_selling_land = 0;       // цена продажи земли
    unsigned short imcome_from_tourism = 0;         // доход от туризма
    unsigned short settled = this->countymen - this->population_change; // оседлые жители
    
public:
    // время правления
    unsigned short get_years() { return this->years; }
    unsigned short increase_years() { return ++this->years; }
    
    // финансы
    short get_money() {return this->balance; }
    short add_money(int amount) {return this->balance += amount; }
    short spend_money(int amount) {return this->balance -= amount; }
};

int main(int argc, const char * argv[]) {
    
    GameState game_state = GameState();
    std::cout << "Years: " << game_state.increase_years() << std::endl;
    std::cout << "Money: " << game_state.get_money() << std::endl;
    std::cout << "Money: " << game_state.spend_money(100) << std::endl;
    std::cout << "Money: " << game_state.get_money() << std::endl;
    return 0;
}
