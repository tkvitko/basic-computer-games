"""
KING

A strategy game where the player is the king.

Ported to Python by Martin Thoma in 2022
"""

import sys
from dataclasses import dataclass
from random import randint, random

FOREST_LAND = 1000
INITIAL_LAND = FOREST_LAND + 1000
COST_OF_LIVING = 100
COST_OF_FUNERAL = 9
YEARS_IN_TERM = 8
POLLUTION_CONTROL_FACTOR = 25


def ask_int(prompt) -> int:
    while True:
        try:
            return int(input(prompt))
        except ValueError:
            continue


@dataclass
class GameState:
    rallods: int = -1
    countrymen: int = -1
    land: int = INITIAL_LAND
    foreign_workers: int = 0
    years_in_office: int = 0

    # previous year stats
    crop_loss_last_year: int = 0

    # current year stats
    died_contrymen: int = 0
    pollution_deaths: int = 0
    population_change: int = 0

    # current year - market situation (in rallods per square mile)
    planting_cost: int = -1
    land_buy_price: int = -1

    tourism_earnings: int = 0

    def set_market_conditions(self) -> None:
        self.land_buy_price = randint(95, 105)
        self.planting_cost = randint(10, 15)

    @property
    def farmland(self) -> int:
        return self.land - FOREST_LAND

    @property
    def settled_people(self) -> int:
        return self.countrymen - self.population_change

    def sell_land(self, amount: int) -> None:
        assert amount <= self.farmland
        self.land -= amount
        self.rallods += self.land_buy_price * amount

    def distribute_rallods(self, distribute: int) -> None:
        self.rallods -= distribute

    def spend_pollution_control(self, spend: int) -> None:
        self.rallods -= spend

    def plant(self, sq_to_plant: int) -> None:
        self.rallods -= sq_to_plant * self.planting_cost

    def print_status(self) -> None:
        print(f"\n\nВ казне {self.rallods} роллодов.")
        print(f"{int(self.countrymen)} жителей, ", end="")
        if self.foreign_workers > 0:
            print(f"{int(self.foreign_workers)} иностранных рабочих, ", end="")
        print(f"{self.land} квадратных миль земли, из них {self.farmland} квадратных миль сельскохозяйственных земель и {FOREST_LAND} квадратных миль леса")
        print(
            f"Стоимость земли под промышленность в этом году {self.land_buy_price} "
            "роллодов за квадратную милю."
        )
        print(
            f"Стоимость земли под сельское хозяйство в этом году {self.planting_cost} "
            "роллодов за квадратную милю.\n"
        )

    def handle_deaths(
            self, distributed_rallods: int, pollution_control_spendings: int
    ) -> None:
        starved_countrymen = max(
            0, int(self.countrymen - distributed_rallods / COST_OF_LIVING)
        )

        if starved_countrymen > 0:
            print(f"{starved_countrymen} жителей умерло от голода")

        self.pollution_deaths = int(random() * (INITIAL_LAND - self.land))
        if pollution_control_spendings >= POLLUTION_CONTROL_FACTOR:
            self.pollution_deaths = int(
                self.pollution_deaths
                / (pollution_control_spendings / POLLUTION_CONTROL_FACTOR)
            )
        if self.pollution_deaths > 0:
            print(
                f"{self.pollution_deaths} жителей умерло из-за углеродного "
                f"и пылевого загрязнения"
            )

        self.died_contrymen = starved_countrymen + self.pollution_deaths
        if self.died_contrymen > 0:
            funeral_cost = self.died_contrymen * COST_OF_FUNERAL
            print(f"   Пришлось потратить {funeral_cost} роллодов на ")
            print("похороны.")
            self.rallods -= funeral_cost
            if self.rallods < 0:
                print("   Недостаточно средств для покрытия затрат - земля была продана")
                self.land += int(self.rallods / self.land_buy_price)
                self.rallods = 0
            self.countrymen -= self.died_contrymen

    def handle_tourist_trade(self) -> None:
        V1 = int(self.settled_people * 22 + random() * 500)
        V2 = int((INITIAL_LAND - self.land) * 15)
        tourist_trade_earnings = 0
        if V1 > V2:
            tourist_trade_earnings = V1 - V2
        print(f" Вы заработали {tourist_trade_earnings} роллодов на туристах.")
        if V2 != 0 and not (V1 - V2 >= self.tourism_earnings):
            print("   поток туристов уменьшился, потому что ", end="")
            reason = randint(0, 10)
            if reason <= 2:
                print("в озерах стало меньше рыбы из-за загрязнения воды")
            elif reason <= 4:
                print("в небе стало меньше птиц из-за загрязнения воздуха")
            elif reason <= 6:
                print("минеральные ванны портятся из-за загрязнения воды")
            elif reason <= 8:
                print("из-за неприятного смога не понежиться на солнышке")
            else:
                print("отели потеряли популярность из-за сильного смога")

        # NOTE: The following two lines had a bug in the original game:
        self.tourism_earnings = abs(int(V1 - V2))
        self.rallods += self.tourism_earnings

    def handle_harvest(self, planted_sq: int) -> None:
        crop_loss = int((INITIAL_LAND - self.land) * ((random() + 1.5) / 2))
        if self.foreign_workers != 0:
            print(f"С {planted_sq} квадратных миль засеянной земли")
        if planted_sq <= crop_loss:
            crop_loss = planted_sq
        harvested = int(planted_sq - crop_loss)
        print(f" вы собрали {harvested} квадратных миль урожая.")
        unlucky_harvesting_worse = crop_loss - self.crop_loss_last_year
        if crop_loss != 0:
            print("   (Причина - ", end="")
            if unlucky_harvesting_worse > 2:
                print("возросшее ", end="")
            print("загрязнение воздуха и воды из-за иностранной промышленности.)")
        revenue = int((planted_sq - crop_loss) * (self.land_buy_price / 2))
        print(f"получив {revenue} роллодов.")
        self.crop_loss_last_year = crop_loss
        self.rallods += revenue

    def handle_foreign_workers(
            self,
            sm_sell_to_industry: int,
            distributed_rallods: int,
            polltion_control_spendings: int,
    ) -> None:
        foreign_workers_influx = 0
        if sm_sell_to_industry != 0:
            foreign_workers_influx = int(
                sm_sell_to_industry + (random() * 10) - (random() * 20)
            )
            if self.foreign_workers <= 0:
                foreign_workers_influx = foreign_workers_influx + 20
            print(f"{foreign_workers_influx} иностранных рабочих приехало в страну")

        surplus_distributed = distributed_rallods / COST_OF_LIVING - self.countrymen
        population_change = int(
            (surplus_distributed / 10)
            + (polltion_control_spendings / POLLUTION_CONTROL_FACTOR)
            - ((INITIAL_LAND - self.land) / 50)
            - (self.died_contrymen / 2)
        )
        print(f"{abs(population_change)} новых жителей ", end="")
        if population_change < 0:
            print("покинуло остров", end="")
        else:
            print("прибыло на остров", end="")
        print(" ")
        self.countrymen += population_change
        self.foreign_workers += int(foreign_workers_influx)

    def handle_too_many_deaths(self) -> None:
        print(f"\n\n\n{self.died_contrymen} жителей умерло за год!!!!!")
        print("\n\n\nИ-за такого ужасного управления вас не только")
        print("лишили должности и сняли с занимаемого поста, ")
        message = randint(0, 10)
        if message <= 3:
            print("но и выбили вам левый глаз!")
        if message <= 6:
            print("еще вы заработали очень плохую репутацию.")
        if message <= 10:
            print("но и назвали предателем родины.")
        sys.exit()

    def handle_third_died(self) -> None:
        print()
        print()
        print("Больше трети населения умерло с тех пор как вы")
        print("были избраны. Население (оставшееся)")
        print("ненавидит вас.")
        self.end_game()

    def handle_money_mismanagement(self) -> None:
        print()
        print("В казне остались не потраченные деньги.")
        print("В результате некоторые жители умерли от голода.")
        print("Население в ярости.")
        print("Вы должны или уйти в отставку, или покончить собой.")
        print("Выбор за вами.")
        print("Если выберете второе, просьба выключить компьютер")
        print("перед тем, как приступите.")
        sys.exit()

    def handle_too_many_foreigners(self) -> None:
        print("\n\nЧисло иностранных рабочих превысило число жителей")
        print("Будучи в меньшинстве, они восстали и")
        print("захватили власть в стране.")
        self.end_game()

    def end_game(self) -> None:
        if random() <= 0.5:
            print("Вас убили.")
        else:
            print("Вы были сняты с поста и сейчас")
            print("находитесь в тюрьме.")
        sys.exit()

    def handle_congratulations(self) -> None:
        print("\n\nПоздравляем!!!!!!!!!!!!!!!!!!")
        print(f"Вы успешно завершили ваш {YEARS_IN_TERM}-летний срок")
        print("на занимаемой должности. Конечно, вам чрезвычайно повезло.")
        print("Тем не менее, это огромное достижение. Прощайте и удачи!")
        print("Она, вероятно, вам пригодится, если вы из тех людей, которые")
        print("играют в эту игру.")
        sys.exit()


def print_header() -> None:
    print(" " * 34 + "KING")
    print(" " * 15 + "CREATIVE COMPUTING  MORRISTOWN, NEW JERSEY")
    print(" " * 15 + "Переведено на русский язык @taraskvitko")
    print(" " * 15 + "Версия 1.1 \n\n\n")


def print_instructions() -> None:
    print(
        f"""\n\n\nПоздравляем! Вас только что избрали премьер-министром Сетац
Детину - маленького коммунистического острова размеров 30 на 70 миль. Ваша задача -
управлять бюджетом страны и распределять деньги из общественной казны.
Денежная единица - роллод, и каждому жителю нужно {COST_OF_LIVING}
роллодов в год чтобы выжить. Доход поступает за счет
 сельского хозяйства и туристов, посещающих ваши великолепные
леса для охоты, рыбалки и просто прогулок. Часть вашей земли - сельскохозяйственная,
но она так же богата полезными ископаемыми и может быть продана иностранной промышленности,
которая привезет своих собственных рабочих. Каждая квадратная миля сельскохозяйственной
земли приносит урожая на 10-15 роллодов в год.
Ваша цель - завершить ваш {YEARS_IN_TERM}-летний срок правления.
Удачи!\n\n"""
    )


def ask_how_many_sq_to_plant(state: GameState) -> int:
    while True:
        sq = ask_int("Сколько квадратных миль земли вы хотите засеять? ")
        if sq < 0:
            continue
        elif sq > 2 * state.countrymen:
            print("   Увы, каждый житель может засеять только 2 квадратные мили.")
        elif sq > state.farmland:
            print(
                f"   Увы, у вас есть только {state.farmland} "
                "квадратных миль сельскохозяйственных земель."
            )
        elif sq * state.planting_cost > state.rallods:
            print(
                f"   Подумайте еще раз. У вас осталось лишь {state.rallods} роллодов "
                "в казне."
            )
        else:
            return sq


def ask_pollution_control(state: GameState) -> int:
    while True:
        rallods = ask_int(
            "Сколько роллодов вы хотите потратить на контроль загрязнений? "
        )
        if rallods > state.rallods:
            print(f"   Подумайте еще раз. У вас осталось лишь {state.rallods} роллодов.")
        elif rallods < 0:
            continue
        else:
            return rallods


def ask_sell_to_industry(state: GameState) -> int:
    had_first_err = False
    first = """(Иностранная промышленность согласна покупать только сельскохозяйственные земли,  так как
лес им экономически невыгоден из-за деревьев, верхнего слоя почты и т.д.)"""
    err = f"""***  Подумайте ещё раз. У вас есть всего {state.farmland} квадратных миль сельскохозяйственной земли."""
    while True:
        sm = input("Сколько квадратных миль земли вы хотите продать под промышленность? ")
        try:
            sm_sell = int(sm)
        except ValueError:
            if not had_first_err:
                print(first)
                had_first_err = True
            print(err)
            continue
        if sm_sell > state.farmland:
            print(err)
        elif sm_sell < 0:
            continue
        else:
            return sm_sell


def ask_distribute_rallods(state: GameState) -> int:
    while True:
        rallods = ask_int(
            "Сколько роллодов вы отдадите своим жителям? "
        )
        if rallods < 0:
            continue
        elif rallods > state.rallods:
            print(
                f"   Подумайте ещё раз. У вас есть всего {state.rallods} роллодов в казне"
            )
        else:
            return rallods


def resume() -> GameState:
    while True:
        years = ask_int("Сколько лет вы занимали пост перед тем, как покинули его? ")
        if years < 0:
            sys.exit()
        if years >= YEARS_IN_TERM:
            print(f"   Окей, вы занимали пост всего {YEARS_IN_TERM} лет.")
        else:
            break
    treasury = ask_int("Сколько у вас в казне? ")
    if treasury < 0:
        sys.exit()
    countrymen = ask_int("Сколько жителей? ")
    if countrymen < 0:
        sys.exit()
    workers = ask_int("Сколько рабочих? ")
    if workers < 0:
        sys.exit()
    while True:
        land = ask_int("Сколько квадратных миль земли? ")
        if land < 0:
            sys.exit()
        if land > INITIAL_LAND:
            farm_land = INITIAL_LAND - FOREST_LAND
            print(f"   Окей, вы начинаете с {farm_land:,} квадратных миль сельскохозяйственных земель")
            print(f"   и {FOREST_LAND:,} квадратных миль леса.")
        if land > FOREST_LAND:
            break
    return GameState(
        rallods=treasury,
        countrymen=countrymen,
        foreign_workers=workers,
        years_in_office=years,
    )


def main() -> None:
    print_header()
    want_instructions = input("Показать инструкцию? ")
    if want_instructions != "нет":
        print_instructions()

    want_resume = input("Хотите продолжить остановленную игру? ")
    if want_resume == "да":
        state = resume()
    else:
        state = GameState(
            rallods=randint(59000, 61000),
            countrymen=randint(490, 510),
            planting_cost=randint(10, 15),
        )

    while True:
        state.set_market_conditions()
        state.print_status()

        # Users actions
        sm_sell_to_industry = ask_sell_to_industry(state)
        state.sell_land(sm_sell_to_industry)

        distributed_rallods = ask_distribute_rallods(state)
        state.distribute_rallods(distributed_rallods)

        planted_sq = ask_how_many_sq_to_plant(state)
        state.plant(planted_sq)
        polltion_control_spendings = ask_pollution_control(state)
        state.spend_pollution_control(polltion_control_spendings)

        # Run the year
        state.handle_deaths(distributed_rallods, polltion_control_spendings)
        state.handle_foreign_workers(
            sm_sell_to_industry, distributed_rallods, polltion_control_spendings
        )
        state.handle_harvest(planted_sq)
        state.handle_tourist_trade()

        if state.died_contrymen > 200:
            state.handle_too_many_deaths()
        if state.countrymen < 343:
            state.handle_third_died()
        elif (
                state.rallods / 100
        ) > 5 and state.died_contrymen - state.pollution_deaths >= 2:
            state.handle_money_mismanagement()
        if state.foreign_workers > state.countrymen:
            state.handle_too_many_foreigners()
        elif YEARS_IN_TERM - 1 == state.years_in_office:
            state.handle_congratulations()
        else:
            state.years_in_office += 1
            state.died_contrymen = 0


if __name__ == "__main__":
    main()
