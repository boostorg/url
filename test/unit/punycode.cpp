//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

// Test that header file is self-contained.
#include <boost/url/detail/punycode.hpp>

#include <boost/url/string.hpp>
#include "test_suite.hpp"
#include <string>

namespace boost {
namespace urls {
namespace detail {

class punycode_test
{
public:
    test_suite::log_type log;

    static
    std::u32string
    decode(string_view s)
    {
        std::u32string result;
        result.resize(1024);
        std::size_t len = result.size();
        puny::decode(
            s.data(),
            s.size(),
            &result[0],
            &len);
        result.resize(len);
        return result;
    }

    static
    std::string
    encode(std::u32string s)
    {
        std::string result;
        result.resize(
            puny::encoded_size(
                s.data(),
                s.data() + s.size()));
        puny::encode(
            &result[0],
            s.data(),
            s.data() + s.size());
        return result;
    }

    static
    std::string
    to_utf8(std::u32string const& in)
    {
        std::size_t len;
        std::copy(in.begin(), in.end(),
            utf8_count(len));
        std::string out;
        out.resize(len);
        std::copy(in.begin(), in.end(),
            utf8_output(&out[0]));
        return out;
    }

    static
    std::u32string
    from_utf8(std::string const& in)
    {
        std::size_t len;
        std::copy(
            utf8_input(
                in.data(),
                in.data() + in.size()),
            utf8_input(
                in.data() + in.size()),
            utf32_count(len));
        std::u32string out;
        out.resize(len);
        std::copy(
            utf8_input(
                in.data(),
                in.data() + in.size()),
            utf8_input(
                in.data() + in.size()),
            &out[0]);
        return out;
    }

    //------------------------------------------------------

    template<class F>
    void
    test_set(F const& f)
    {
        f("bcher-kva",
            { 'b', 0xFC, 'c', 'h', 'e', 'r' });
            
        // (A) Arabic (Egyptian):
        f("egbpdaj6bu4bxfgehfvwxn", {
        0x0644, 0x064A, 0x0647, 0x0645, 0x0627, 0x0628, 0x062A, 0x0643, 0x0644,
        0x0645, 0x0648, 0x0634, 0x0639, 0x0631, 0x0628, 0x064A, 0x061F
            });

        // (B) Chinese (simplified):
        f("ihqwcrb4cv8a8dqg056pqjye", {
        0x4ED6, 0x4EEC, 0x4E3A, 0x4EC0, 0x4E48, 0x4E0D, 0x8BF4, 0x4E2D, 0x6587
            });

        // (C) Chinese (traditional):
        f("ihqwctvzc91f659drss3x8bo0yb", {
        0x4ED6, 0x5011, 0x7232, 0x4EC0, 0x9EBD, 0x4E0D, 0x8AAA, 0x4E2D, 0x6587
            });
        // (D) Czech: Pro<ccaron>prost<ecaron>nemluv<iacute><ccaron>esky
        f("proprostnemluvesky-uyb24dma41a", {
        0x0070, 0x0072, 0x006F, 0x010D, 0x0070, 0x0072, 0x006F, 0x0073, 0x0074,
        0x011B, 0x006E, 0x0065, 0x006D, 0x006C, 0x0075, 0x0076, 0x00ED, 0x010D,
        0x0065, 0x0073, 0x006B, 0x0079
            });

        // (E) Hebrew:
        f("4dbcagdahymbxekheh6e0a7fei0b", {
        0x05DC, 0x05DE, 0x05D4, 0x05D4, 0x05DD, 0x05E4, 0x05E9, 0x05D5, 0x05D8,
        0x05DC, 0x05D0, 0x05DE, 0x05D3, 0x05D1, 0x05E8, 0x05D9, 0x05DD, 0x05E2,
        0x05D1, 0x05E8, 0x05D9, 0x05EA
            });

        // (F) Hindi (Devanagari):
        f("i1baa7eci9glrd9b2ae1bj0hfcgg6iyaf8o0a1dig0cd", {
        0x092F, 0x0939, 0x0932, 0x094B, 0x0917, 0x0939, 0x093F, 0x0928, 0x094D,
        0x0926, 0x0940, 0x0915, 0x094D, 0x092F, 0x094B, 0x0902, 0x0928, 0x0939,
        0x0940, 0x0902, 0x092C, 0x094B, 0x0932, 0x0938, 0x0915, 0x0924, 0x0947,
        0x0939, 0x0948, 0x0902
            });

        // (G) Japanese (kanji and hiragana):
        f("n8jok5ay5dzabd5bym9f0cm5685rrjetr6pdxa", {
        0x306A, 0x305C, 0x307F, 0x3093, 0x306A, 0x65E5, 0x672C, 0x8A9E, 0x3092,
        0x8A71, 0x3057, 0x3066, 0x304F, 0x308C, 0x306A, 0x3044, 0x306E, 0x304B
            });

        // (H) Korean (Hangul syllables):
        f("989aomsvi5e83db1d2a355cv1e0vak1dwrv93d5xbh15a0dt30a5jpsd879ccm6fea98c", {
        0xC138, 0xACC4, 0xC758, 0xBAA8, 0xB4E0, 0xC0AC, 0xB78C, 0xB4E4, 0xC774,
        0xD55C, 0xAD6D, 0xC5B4, 0xB97C, 0xC774, 0xD574, 0xD55C, 0xB2E4, 0xBA74,
        0xC5BC, 0xB9C8, 0xB098, 0xC88B, 0xC744, 0xAE4C
            });

        // (I) Russian (Cyrillic):
        f("b1abfaaepdrnnbgefbadotcwatmq2g4l", {
        0x043F, 0x043E, 0x0447, 0x0435, 0x043C, 0x0443, 0x0436, 0x0435, 0x043E,
        0x043D, 0x0438, 0x043D, 0x0435, 0x0433, 0x043E, 0x0432, 0x043E, 0x0440,
        0x044F, 0x0442, 0x043F, 0x043E, 0x0440, 0x0443, 0x0441, 0x0441, 0x043A,
        0x0438
            });

        // (J) Spanish: Porqu<eacute>nopuedensimplementehablarenEspa<ntilde>ol
        f("porqunopuedensimplementehablarenEspaol-fmd56a", {
        0x0070, 0x006F, 0x0072, 0x0071, 0x0075, 0x00E9, 0x006E, 0x006F, 0x0070,
        0x0075, 0x0065, 0x0064, 0x0065, 0x006E, 0x0073, 0x0069, 0x006D, 0x0070,
        0x006C, 0x0065, 0x006D, 0x0065, 0x006E, 0x0074, 0x0065, 0x0068, 0x0061,
        0x0062, 0x006C, 0x0061, 0x0072, 0x0065, 0x006E, 0x0045, 0x0073, 0x0070,
        0x0061, 0x00F1, 0x006F, 0x006C
            });

        // (K) Vietnamese:
        f("TisaohkhngthchnitingVit-kjcr8268qyxafd2f1b9g", {
        0x0054, 0x1EA1, 0x0069, 0x0073, 0x0061, 0x006F, 0x0068, 0x1ECD, 0x006B,
        0x0068, 0x00F4, 0x006E, 0x0067, 0x0074, 0x0068, 0x1EC3, 0x0063, 0x0068,
        0x1EC9, 0x006E, 0x00F3, 0x0069, 0x0074, 0x0069, 0x1EBF, 0x006E, 0x0067,
        0x0056, 0x0069, 0x1EC7, 0x0074
            });

        //
        // Japanese
        //

        // (L) 3<nen>B<gumi><kinpachi><sensei>
        f("3B-ww4c5e180e575a65lsy2b", {
        0x0033, 0x5E74, 0x0042, 0x7D44, 0x91D1, 0x516B, 0x5148, 0x751F,
            });

        // (M) <amuro><namie>-with-SUPER-MONKEYS
        f("-with-SUPER-MONKEYS-pc58ag80a8qai00g7n9n", {
        0x5B89, 0x5BA4, 0x5948, 0x7F8E, 0x6075, 0x002D, 0x0077, 0x0069, 0x0074,
        0x0068, 0x002D, 0x0053, 0x0055, 0x0050, 0x0045, 0x0052, 0x002D, 0x004D,
        0x004F, 0x004E, 0x004B, 0x0045, 0x0059, 0x0053
            });

        // (N) Hello-Another-Way-<sorezore><no><basho>
        f("Hello-Another-Way--fc4qua05auwb3674vfr0b", {
        0x0048, 0x0065, 0x006C, 0x006C, 0x006F, 0x002D, 0x0041, 0x006E, 0x006F,
        0x0074, 0x0068, 0x0065, 0x0072, 0x002D, 0x0057, 0x0061, 0x0079, 0x002D,
        0x305D, 0x308C, 0x305E, 0x308C, 0x306E, 0x5834, 0x6240
            });

        // (O) <hitotsu><yane><no><shita>2
        f("2-u9tlzr9756bt3uc0v", {
        0x3072, 0x3068, 0x3064, 0x5C4B, 0x6839, 0x306E, 0x4E0B, 0x0032
            });

        // (P) Maji<de>Koi<suru>5<byou><mae>
        f("MajiKoi5-783gue6qz075azm5e", {
        0x004D, 0x0061, 0x006A, 0x0069, 0x3067, 0x004B, 0x006F, 0x0069, 0x3059,
        0x308B, 0x0035, 0x79D2, 0x524D
            });

        // (Q) <pafii>de<runba>
        f("de-jg4avhby1noc0d", {
        0x30D1, 0x30D5, 0x30A3, 0x30FC, 0x0064, 0x0065, 0x30EB, 0x30F3, 0x30D0
            });

        // (R) <sono><supiido><de>
        f("d9juau41awczczp", {
        0x305D, 0x306E, 0x30B9, 0x30D4, 0x30FC, 0x30C9, 0x3067
            });
    }

    void
    doTestSet()
    {
        auto const check =
            [&](std::string a, std::u32string u)
        {
            BOOST_TEST(decode(a) == u);
            BOOST_TEST(encode(u) == a);
            BOOST_TEST(encode(decode(a)) == a);
            BOOST_TEST(decode(encode(u)) == u);
            BOOST_TEST(from_utf8(to_utf8(u)) == u);

            {
                auto const u8 = to_utf8(u);
                std::string a2;
                a2.resize(
                    puny::encoded_size(
                        utf8_input(
                            u8.data(),
                            u8.data() + u8.size()),
                        utf8_input(
                            u8.data() + u8.size())));
                puny::encode(
                    &a2[0],
                    utf8_input(
                        u8.data(),
                        u8.data() + u8.size()),
                    utf8_input(
                        u8.data() + u8.size()));
                BOOST_TEST(a2 == a);
            }
        };
        test_set(check);
    }

    void
    run()
    {
        doTestSet();
    }
};

TEST_SUITE(
    punycode_test,
    "boost.url.punycode");

} // detail
} // urls
} // boost
