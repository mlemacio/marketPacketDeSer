#ifndef BIMAP_H
#define BIMAP_H

#include <initializer_list>
#include <functional>
#include <optional>
#include <unordered_map>

template <typename A, typename B>
class bimap_t
{
    using bipair_t = std::pair<A, B>;
    using bipair_ref = const std::pair<A, B> *;

public:
    bimap_t(std::vector<bipair_t> &&l)
        : pairs(std::move(l))
    {
        for (const auto &bp : pairs)
        {
            A_key_to_pair.emplace(bp.first, &bp);
            B_key_to_pair.emplace(bp.second, &bp);
        }
    }

    std::optional<B> find(const A &A_key)
    {
        std::cout << A_key_to_pair[A_key]->first << std::endl;
        ifA_key_to_pair.find(A_key)
        return B();
    }

    std::optional<A> find(const B &B_key)
    {

        return A();
    }

private:
    std::vector<bipair_t> pairs;

    std::unordered_map<A, bipair_ref> A_key_to_pair;
    std::unordered_map<B, bipair_ref> B_key_to_pair;
};

#endif // BIMAP_H