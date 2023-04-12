#ifndef ORGANISM__H
#define ORGANISM__H

#include <cstdint>
#include <optional>
#include <tuple>
template <typename species_t, bool can_eat_meat, bool can_eat_plants>
requires std::equality_comparable<species_t>
class Organism {
   private:
    species_t species;
    uint64_t vitality;

   public:
    constexpr Organism(species_t species, uint64_t vitality)
        : species(species), vitality(vitality) {}

    constexpr uint64_t get_vitality() const { return vitality; }
    constexpr const species_t &get_species() const { return species; }
    constexpr bool is_dead() const { return vitality == 0; }
    constexpr void kill() { vitality = 0; }
    template <bool cm, bool cp>
    constexpr Organism make_child(Organism<species_t, cm, cp> organism) {
        return Organism(
            this->species,
            this->vitality / 2 + organism.get_vitality() / 2 +
                (this->vitality % 2 + organism.get_vitality() % 2 == 2));
    }
    template <bool cm, bool cp>
    constexpr void fight(Organism<species_t, cm, cp> &organism) {
        if (this->vitality > organism.get_vitality()) {
            this->vitality += (organism.get_vitality() / 2);
            organism.kill();
        } else if (this->vitality == organism.get_vitality()) {
            this->kill();
            organism.kill();
        }
    }
    template <bool cm, bool cp>
    constexpr void eat(Organism<species_t, cm, cp> &organism) {
        if (!(cm || cp) && can_eat_plants) {
            this->vitality += organism.get_vitality();
            organism.kill();
            return;
        } else if ((cm || cp) && can_eat_meat &&
                   (this->vitality > organism.get_vitality())) {
            this->vitality += organism.get_vitality() / 2;
            organism.kill();
        }
    };
};

template <typename species_t>
using Carnivore = Organism<species_t, true, false>;

template <typename species_t>
using Omnivore = Organism<species_t, true, true>;

template <typename species_t>
using Herbivore = Organism<species_t, false, true>;

template <typename species_t>
using Plant = Organism<species_t, false, false>;

template <typename species_t, bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m,
          bool sp2_eats_p>
requires sp1_eats_m || sp1_eats_p || sp2_eats_m ||
    sp2_eats_p constexpr std::tuple<
        Organism<species_t, sp1_eats_m, sp1_eats_p>,
        Organism<species_t, sp2_eats_m, sp2_eats_p>,
        std::optional<Organism<species_t, sp1_eats_m, sp1_eats_p>>>
    encounter(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1,
              Organism<species_t, sp2_eats_m, sp2_eats_p> organism2) {
    if (organism1.is_dead() || organism2.is_dead()) {
        return {organism1, organism2, std::nullopt};
    }
    if (organism1.get_species() == organism2.get_species() &&
        sp1_eats_m == sp2_eats_m && sp1_eats_p == sp2_eats_p) {
        return {organism1, organism2, organism1.make_child(organism2)};
    }
    if constexpr (sp1_eats_m && sp2_eats_m) {
        organism1.fight(organism2);
        organism2.fight(organism1);
        return {organism1, organism2, std::nullopt};
    }
    organism1.eat(organism2);
    organism2.eat(organism1);
    return {organism1, organism2, std::nullopt};
}
template <typename species_t, bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m,
          bool sp2_eats_p>
constexpr Organism<species_t, sp1_eats_m, sp1_eats_p> operator+(
    Organism<species_t, sp1_eats_m, sp1_eats_p> organism1,
    Organism<species_t, sp2_eats_m, sp2_eats_p> organism2) {
    return std::get<0>(encounter(organism1, organism2));
}

template <typename species_t, bool sp1_eats_m, bool sp1_eats_p,
          typename... Args>
constexpr Organism<species_t, sp1_eats_m, sp1_eats_p> encounter_series(
    Organism<species_t, sp1_eats_m, sp1_eats_p> organism1, Args... args) {
    return (organism1 + ... + args);
}

#endif