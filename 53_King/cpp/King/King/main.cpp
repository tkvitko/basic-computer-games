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
    
    // константы
    const short initial_land = 2000;
    const short cost_of_living = 100;
    const short cost_of_funeral = 9;
    const short pollution_control_factor = 25;
    
    // время
    unsigned short years = 0;  // лет правления
    
    // финансы
    int balance = 0;  // количество денег в казне
    
    // люди
    short countrymen = 0;  // количество жителей
    short foreigners = 0; // количество иностранных рабочих
    
    // земли
    unsigned short total_land = initial_land;  // общая площадь земли
    unsigned short forest_land = 1000; // площадь леса

    // параметры текущего года
    short died_count = 0;                  // общее количество погибших жителей
    short died_because_of_pollution = 0;   // количество погибших жителей по причине загрязнения
    short population_change = 0;                    // изменение населения
    
    // экономика текущего года
    unsigned int cost_of_planting_land = 0;       // стоимость засева земли
    unsigned int price_of_selling_land = 0;       // цена продажи земли
    unsigned int imcome_from_tourism = 0;         // доход от туризма
    unsigned short settled = this->countrymen - this->population_change; // оседлые жители
    
    // решения текущего года
    short sold_square = 0;
    int distributed_money = 0;
    short planted_square = 0;
    int money_spent_for_pollution_control = 0;
    
    // экономика предыдущего года
    short last_year_lost_farm_land = 0;     // потери урожая
    int last_year_tourists_revenue = 0;   // заработок на туристах
    
    // приватные методы для внутренней работы
    template <class T>
    T _get_random_int_from_range(const T min_value, const T max_value) {
        std::default_random_engine generator;
        std::uniform_int_distribution<T> distribution(min_value, max_value);
        return distribution(generator);
    };
    
    double _get_random_float_from_zero_to_one() {
        std::default_random_engine generator;
        std::uniform_int_distribution<double> distribution(0.0, 1.0);
        return distribution(generator);
    }
    
    int _request_int_value(const std::string question_text) {
        // задает вопрос и считывает числовой ответ; возвращает его или -1, если ответ не числовой
        int answer = -1;
        std::cout << question_text << " ";
        std::cin >> answer;
        return answer;
    }
    
    bool _sell_farm_land_to_industry(const int square) {
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
    
    void _set_prices_for_land() {
        // установка цен на продажу земли и посев на земле
        this->cost_of_planting_land = this->_get_random_int_from_range(10, 15);
        this->price_of_selling_land = this->_get_random_int_from_range(95, 105);
    }

    
public:
    // время правления
    unsigned short get_years() { return this->years; }
    unsigned short increase_years() { return ++this->years; }
    
    // финансы
    long get_money() {return this->balance; }
    long add_money(int amount) {return this->balance += amount; }
    long spend_money(const int amount) {
        if (amount <= this->balance) {
            return this->balance -= amount;
        } return -1;
    }
    
    // земли
    unsigned short get_total_land_square() { return this->total_land;  }
    unsigned short get_forest_land_square() { return this->forest_land;  }
    unsigned short get_farm_land_square() { return this->total_land - this->forest_land;  }
    
    // население
    unsigned short get_countryman_count() { return this->countrymen;  }
    
    // публичные методы для работы с данными объекта класса
    
    void print_state() {
        std::cout << "В казне " << this->balance << " ролодов" << std::endl;
        std::cout << "В стране проживает " << this->countrymen << " жителей";
        if (this->foreigners > 0) {
            std::cout << " и " << this->foreigners << " иностранных рабочих" << std::endl;
        } else {
            std::cout << std::endl;
        };
        std::cout << "У нас " << this->total_land << " квадратных миль земли: " << this->get_farm_land_square() << " сельхоз и " << this->forest_land << " леса" << std::endl;
        std::cout << "В этом году промышленность готова платить " << this->price_of_selling_land << " роллодов за квадратную милю, а стоимость засева одной квадратной мили равна " << this->cost_of_planting_land << std::endl ;
    }
    
    void init_new_year() {
        this->years += 1;
        if (this->years == 1) {
            this->balance = this->_get_random_int_from_range(59000, 61000);
            this->countrymen = this->_get_random_int_from_range(490, 510);
        };
        this->_set_prices_for_land();
    }
    
    void sell_land_to_industry() {
        // процесс продажи земли иностранной промышленности
        short square_to_sell = 0;
        while (true) {
            square_to_sell = this->_request_int_value("Сколько квадратных миль земли вы хотите продать под промышленность? ");
            if (square_to_sell < this->get_farm_land_square()) {
                break;
            } else {
                std::cout << "Подумайте ещё раз. У вас есть всего " << this->get_farm_land_square() << " квадратных миль сельскохозяйственной земли." << std::endl;
            }
        };
        this->_sell_farm_land_to_industry(square_to_sell);
        this->sold_square = square_to_sell;
    }
    
    void distribute_money_to_countryman() {
        // процесс распределения денег по жителям
        int money_to_distribute = 0;
        while (true) {
            money_to_distribute = this->_request_int_value("Сколько роллодов вы отдадите своим жителям? ");
            if (money_to_distribute < this->balance) {
                break;
            } else {
                std::cout << "Подумайте ещё раз. У вас есть всего " << this->balance << " роллодов в казне" << std::endl;
            }
        };
        this->spend_money(money_to_distribute);
        this->distributed_money = money_to_distribute;
    }
    
    void plant_farm_land() {
        // процесс засева сельхоз земель
        short square_to_plant = 0;
        while (true) {
            square_to_plant = this->_request_int_value("Сколько квадратных миль земли вы хотите засеять? ");
            if (square_to_plant > this->get_farm_land_square()) {
                std::cout << "Увы, у вас есть только" << this->get_farm_land_square() << "квадратных миль сельскохозяйственных земель" << std::endl;
            } else if (square_to_plant > this->get_countryman_count()) {
                std::cout << "Увы, каждый житель может засеять только 2 квадратные мили" << std::endl;
            } else if (square_to_plant * this->cost_of_planting_land > this->balance) {
                std::cout << "Подумайте еще раз. У вас осталось лишь " << this->balance << " роллодов в казне" << std::endl;
            } else {
                break;
            };
        }
        this->spend_money(square_to_plant * this->cost_of_planting_land);
        this->planted_square = square_to_plant;
    }
    
    void pollution_control() {
        // процесс вкладывания денег в контроль загрязнений
        int money_for_pollution_control = 0;
        while (true) {
            money_for_pollution_control = this->_request_int_value("Сколько роллодов вы хотите потратить на контроль загрязнений? ");
            if (money_for_pollution_control > this->balance) {
                std::cout << "Подумайте еще раз. У вас осталось лишь " << this->balance << " роллодов в казне" << std::endl;
            } else {
                break;
            };
            this->spend_money(money_for_pollution_control);
            this->money_spent_for_pollution_control = money_for_pollution_control;
        }
    }
    
    void count_deaths() {
        // подсчет погибших
        
        // погибшие от голода
        short starved = this->countrymen - this->distributed_money / this->cost_of_living;
        if (starved > 0) {
            std::cout << starved << " жителей умерло от голода" << std::endl;
        };
        
        // погибшие от загрязнений
        this->died_because_of_pollution = static_cast<int>(this->_get_random_float_from_zero_to_one() * (this->initial_land - this->total_land));
        if (this->money_spent_for_pollution_control >= this->pollution_control_factor) {
            this->died_because_of_pollution = static_cast<int>(this->died_because_of_pollution / (this->money_spent_for_pollution_control / this->pollution_control_factor));
        }
        if (this->died_because_of_pollution > 0) {
            std::cout << this->died_because_of_pollution << " жителей погибло из-за углеродного и пылевого загрязнения" << std::endl;
        }
            
        // затраты на похороны
        this->died_count = starved + this->died_because_of_pollution;
        if (this->died_count > 0) {
            long funeral_cost = this->died_count * this->cost_of_funeral;
            std::cout << "Пришлось потратить " << funeral_cost << " роллодов на похороны" << std::endl;
            this->balance -= funeral_cost;  // вычитание из казны
            if (this->balance < 0) {
                std::cout << "В казне не хватает денег на похороны, придется продать часть земли" << std::endl;
                this->total_land += this->balance / this->price_of_selling_land;    // компенсация отрицательного баланса за счет продажи земли
                this->balance = 0;
            }
            this->countrymen -= this->died_count;
        }
    }
    
    void count_people() {
        // подсчет изменения населения
        
        // инорстранные рабочие
        short foreigners_change = 0;
        if (this->sold_square != 0) {
            foreigners_change = static_cast<int>(this->sold_square + this->_get_random_float_from_zero_to_one() * 10 - this->_get_random_float_from_zero_to_one() * 20);
            if (this->foreigners < 0) {
                foreigners_change += 20;
            }
            std::cout << foreigners_change << " иностранных рабочих приехало в страну"  << std::endl;
        }
        
        // новые жители
        long over_disctibuted_money = this->distributed_money / this->cost_of_living - this->countrymen;
        short countrymen_change = static_cast<int>(over_disctibuted_money / 10
                                                   + this->money_spent_for_pollution_control / this->pollution_control_factor
                                                   - ((this->initial_land - this->total_land) / 50 )
                                                   - this->died_count / 2 );
        std::cout << "Население изменилось: " << countrymen_change << " человек";
        if (countrymen_change < 0) {
            std::cout << " покинуло страну" << std::endl;
        } else {
            std::cout << " прибыло в страну" << std::endl;
        }
        this->countrymen += countrymen_change;
        this->foreigners += foreigners_change;
    }
    
    void count_harvest() {
        // подсчет урожая
        
        // потери урожая из-за загрязнений
        short lost_farm_land = static_cast<int>((this->initial_land - this->total_land) * (this->_get_random_float_from_zero_to_one() + 1.5) / 2);
        if (lost_farm_land > this->planted_square) {
            lost_farm_land = this->planted_square;
        }
        short harvested = this->planted_square - lost_farm_land;
        std::cout << "С " << this->planted_square << " квадратных миль засеянной земли вы собрали " << harvested << " квадратных миль урожая" << std::endl;
        
        // причины потери урожая
        short harvesting_worse_trend = lost_farm_land - this->last_year_lost_farm_land;
        if (lost_farm_land != 0) {
            std::cout << "Причина - ";
            if (harvesting_worse_trend > 2) {
                std::cout << "возросшее ";
            };
            std::cout << "загрязнение воздуха и воды из-за иностранной промышленности." << std::endl;
        }
        
        // учет дохода с урожая
        short revelue = static_cast<int>((this->planted_square - lost_farm_land) * this->price_of_selling_land / 2);
        this->balance += revelue;
        std::cout << "Вы заработали на урожае " << revelue << " роллодов" << std::endl;
        this->last_year_lost_farm_land = lost_farm_land;
    }
    
    void count_tourists() {
        // подсчет доходов с туристов
        
        short koef_1 = static_cast<short>(this->settled * 22 + this->_get_random_float_from_zero_to_one() * 500);
        short koef_2 = static_cast<short>(( this->initial_land - this->total_land) * 15);
        short revenue = 0;
        
        if (koef_1 > koef_2) {
            revenue = koef_1 - koef_2;
        }
        std::cout << "Вы заработали " << revenue << " роллодов на туристах" << std::endl;
        if (koef_2 != 0 && !(koef_1 - koef_2 >= this->last_year_tourists_revenue)) {
            std::cout << "Поток туристов уменьшился, потому что ";
            short reason = this->_get_random_int_from_range(0, 10);
            if (reason <= 2) {
                std::cout << "в озерах стало меньше рыбы из-за загрязнения воды";
            } else if (reason <= 4) {
                std::cout << "в небе стало меньше птиц из-за загрязнения воздуха";
            } else if (reason <= 6) {
                std::cout << "минеральные ванны портятся из-за загрязнения воды";
            } else if (reason <= 8) {
                std::cout << "из-за неприятного смога не понежиться на солнышке";
            } else {
                std::cout << "отели потеряли популярность из-за сильного смога";
            }
            std::cout << std::endl;
        }
        this->last_year_tourists_revenue = abs(koef_1 - koef_2);
        this->balance += revenue;
    }
    
    void process_year() {
        this->count_deaths();
        this->count_people();
        this->count_harvest();
        this->count_tourists();
    }
    
    void get_gamer_decisions() {
        this->sell_land_to_industry();
        this->distribute_money_to_countryman();
        this->plant_farm_land();
        this->pollution_control();
    }
    
    bool get_year_results() {
        // подведение результатов года
        
        // слишком много погибших
        if (this->died_count > 200) {
            std::cout << this->died_count << " жителей умерло за год! И-за такого ужасного управления вас не только лишили должности и сняли с занимаемого поста, ";
            short reason = this->_get_random_int_from_range(0, 10);
            if (reason <= 3) {
                std::cout << "но и выбили вам левый глаз!";
            } else if (reason <= 6) {
                std::cout << "еще вы заработали очень плохую репутацию.";
            } else {
                std::cout << "но и назвали предателем родины.";
            }
            std::cout << std::endl;
            return true;
        }
        
        // слишком мало жителей
        if (this->countrymen < 343) {
            std::cout << "Больше трети населения умерло с тех пор как вы были избраны. Население (оставшееся) ненавидит вас." << std::endl;
            return true;
        }
        
        // слишком много денег осталось в казне
        if (this->balance / 100 > 5 && this->died_count - this->died_because_of_pollution >= 2) {
            std::cout << "В казне остались не потраченные деньги. В результате некоторые жители умерли от голода. Население в ярости. Вы должны или уйти в отставку, или покончить собой. Выбор за вами. Если выберете второе, просьба выключить компьютер перед тем, как приступите." << std::endl;
            return true;
        }
        
        // слишком много иностранцев
        if (this->foreigners > this->countrymen) {
            std::cout << "Число иностранных рабочих превысило число жителей. Будучи в меньшинстве, они восстали и захватили власть в стране" << std::endl;
            return true;
        }
        
        // победа в игре
        if (this->years == 7) {
            std::cout << "Поздравляем! Вы успешно завершили ваш 8-летний срок на занимаемой должности. Конечно, вам чрезвычайно повезло. Тем не менее, это огромное достижение. Прощайте и удачи! Она, вероятно, вам пригодится, если вы из тех людей, которые играют в эту игру.";
            return true;
        }
        
        // старт следующего года
        this->years += 1;
        this->died_count = 0;
        return false;
    }
};

int main(int argc, const char * argv[]) {
    
    GameState game = GameState();
    
    while (true) {
        game.init_new_year();
        game.print_state();
        game.get_gamer_decisions();
        game.process_year();
        bool is_game_over = game.get_year_results();
//        game.print_state();
        if (is_game_over) {
            return 0;
        }
    }
}
