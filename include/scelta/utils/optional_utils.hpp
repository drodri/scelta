// Copyright (c) 2017 Vittorio Romeo
// MIT License |  https://opensource.org/licenses/MIT
// http://vittorioromeo.info | vittorio.romeo@outlook.com

#pragma once

#include "../meta/forward_like.hpp"
#include "../traits/adt/valid.hpp"
#include "./access_optional.hpp"
#include "./assert.hpp"
#include "./returns.hpp"

namespace scelta
{
    struct nullopt_t
    {
    };

    inline constexpr nullopt_t nullopt{};
}

namespace scelta::impl
{
    // clang-format off
    template <typename... Ts>
    constexpr auto to_nullopt(Ts&&...)
        SCELTA_RETURNS(
            ::scelta::nullopt
        )

    template <typename Visitor>
    constexpr auto visit_optional(Visitor&& visitor)
        SCELTA_RETURNS(
            FWD(visitor)()
        )

    // Invokes the continuation `c` with the result of `f(<"unpacked" o>)`.
    template <typename F, typename Continuation, typename Optional>
    constexpr auto call_with_optional(F&& f, Continuation&& c, Optional&& o)
        SCELTA_RETURNS(
            o ? FWD(c)(FWD(f)(meta::forward_like<Optional>(::scelta::impl::access_optional(o))))
              : FWD(c)(FWD(f)(to_nullopt()))
        )

    // TODO: noexcept incorrect
    template <typename Visitor, typename Optional, typename... Optionals>
    constexpr auto visit_optional(
        Visitor&& visitor, Optional&& o, Optionals&&... os) ->decltype(auto)
        // SCELTA_NOEXCEPT_AND_TRT(FWD(visitor)(to_nullopt(), to_nullopt(os)...))
    {
        return
            SCELTA_CONSTEXPR_ASSERT(
                ::scelta::traits::adt::valid_v<std::decay_t<Optional>>(o, os...)
            ),
            call_with_optional(
            // Binds the first "unpacked" optional to the visitor.
            // Returns a the next visitor with the bound value, which will
            // be passed to the continuation function.
            [&](auto&& x) noexcept
            {
                return [&](auto&&... xs) SCELTA_RETURNS(
                    FWD(visitor)(FWD(x), FWD(xs)...)
                );
            },

            // Continuation: recurse upon the newly bound visitor.
            [&](auto&& bound_visitor) SCELTA_RETURNS(
                visit_optional(FWD(bound_visitor), FWD(os)...)
            ),
            FWD(o)
        );
    }
    // clang-format on
}
