#include "../test_utils.hpp"
#include "../variant_test_utils.hpp"
#include <scelta/visitation.hpp>

TEST_MAIN()
{
    using namespace test;

    with_all_variant_implementations(   //
        alternatives<int, float, char>, //
        [](auto make)                   //
        {
            {
                auto f = scelta::overload(              //
                    [](int x) { EXPECT_OP(x, ==, 1); }, //
                    [](float) { EXPECT(false); },       //
                    [](char) { EXPECT(false); });

                auto v = make(1);
                scelta::visit(f, v);
            }

            {
                auto f = scelta::overload(                  //
                    [](int) { EXPECT(false); },             //
                    [](float x) { EXPECT_OP(x, ==, 2.f); }, //
                    [](char) { EXPECT(false); });

                auto v = make(2.f);
                scelta::visit(f, v);
            }

            {
                auto f = scelta::overload(        //
                    [](int) { EXPECT(false); },   //
                    [](float) { EXPECT(false); }, //
                    [](char x) { EXPECT_OP(x, ==, 'c'); });

                auto v = make('c');
                scelta::visit(f, v);
            }
        });
}