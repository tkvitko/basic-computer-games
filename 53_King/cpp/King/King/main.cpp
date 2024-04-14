//
//  main.cpp
//  King
//
//  Created by tkvitko on 14.04.2024.
//

#include <iostream>
#include <random>


class NotEnoughLand : public std::exception {
public:
    const char* what() const noexcept override {
        return "Not enough land";
    };
};


class GameState {
private:
    
    // время
    unsigned short years = 0;  // лет правления
    
    // финансы
    long balance = 0;  // количество денег в казне
    
    // люди
    unsigned short countymen = 0;  // количество жителей
    unsigned short foreigners = 0; // количество иностранных рабочих
    
    // земли
    unsigned short total_land = 2000;  // общая площадь земли
    unsigned short forest_land = 1000; // площадь леса
    
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
    
    // приватные методы для внутренней работы
    short get_random_from_range(short min_value, short max_value) {
        std::default_random_engine generator;
        std::uniform_int_distribution<short> distribution(min_value, max_value);
        return distribution(generator);
    }
    
public:
    // время правления
    unsigned short get_years() { return this->years; }
    unsigned short increase_years() { return ++this->years; }
    
    // финансы
    long get_money() {return this->balance; }
    long add_money(int amount) {return this->balance += amount; }
    long spend_money(int amount) {
        if (amount <= this->balance) {
            return this->balance -= amount;
        } return -1;
    }
    
    // земли
    unsigned short get_total_land_square() { return this->total_land;  }
    unsigned short get_forest_land_square() { return this->forest_land;  }
    unsigned short get_farm_land_square() { return this->total_land - this->forest_land;  }
    
    // публичные методы для работы с данными объекта класса
    
    void set_prices_for_land() {
        // установка цен на продажу земли и посев на земле
        this->cost_of_planting_land = this->get_random_from_range(10, 15);
        this->price_of_selling_land = this->get_random_from_range(95, 105);
    }
    
    bool sell_farm_land_to_industry(int square) {
        /*
         продажа сельхоз земли иностранной промышленности
         true если продажа успешна, false если не успешна (не хватает земли)
         */
        if (square <= this->get_farm_land_square()) {
            this->total_land -= square;
            this->add_money(this->price_of_selling_land * square);
            return true;
        } return false;
    }
    
    void print_state() {
        std::cout << "В казне " << this->balance << " ролодов" << std::endl;
        std::cout << "В стране проживает " << this->countymen << " жителей";
        if (this->foreigners > 0) {
            std::cout << " и " << this->foreigners << " иностранных рабочих" << std::endl;
        } else {
            std::cout << std::endl;
        };
        std::cout << "У нас " << this->total_land << " квадратных миль земли: " << this->get_farm_land_square() << " сельхоз и " << this->forest_land << " леса" << std::endl;
        std::cout << "В этом году промышленность готова платить " << this->price_of_selling_land << " роллодов за квадратную милю, а стоимость засева одной квадратной мили равна " << this->cost_of_planting_land << std::endl ;
    }
};

int main(int argc, const char * argv[]) {
    
    GameState game_state = GameState();
//    std::cout << "Years: " << game_state.increase_years() << std::endl;
//    std::cout << "Money: " << game_state.get_money() << std::endl;
//    std::cout << "Money: " << game_state.spend_money(100) << std::endl;
//    std::cout << "Money: " << game_state.get_money() << std::endl;
    game_state.print_state();
    return 0;
}
