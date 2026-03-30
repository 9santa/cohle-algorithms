#include <concepts>

template<typename G>
concept AbelianGroup = requires (G a, G b) {
    { a + b } -> std::same_as<G>; // closure
    { a + (b + a) == (a + b) + a } -> std::convertible_to<bool>; // associativity
    { -a } -> std::same_as<G>;    // inverse
    G{0};                         // identity
    requires (a + b == b + a);    // commutativity
};
