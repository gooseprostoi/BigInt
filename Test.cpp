#define CATCH_CONFIG_MAIN

#include <iostream>
#include "catch.hpp"
#include "BigInt.hpp"



TEST_CASE("TEST 1 (Adding)") {
    BigInt x_1({1});
    BigInt x_2({1});

    REQUIRE((x_1 + x_2).PrintPow() == "2");
    std::cout << "checkpoint 1:1\n";

//////////////////////////////////////////////////////////////////////////////////////////
    x_1 = BigInt({U32MAX - 1});
    x_2 = BigInt({1});

    REQUIRE((x_1 + x_2).PrintPow() == "0 + 1 * 2^(32)");
    std::cout << "checkpoint 1:2\n";

//////////////////////////////////////////////////////////////////////////////////////////
    x_1 = BigInt({U32MAX - 1, U32MAX - 1, U32MAX - 1, U32MAX - 1, U32MAX - 1, U32MAX - 1, U32MAX - 1});
    x_2 = BigInt({1});

    REQUIRE((x_1 + x_2).PrintPow() == 
    "0 + 0 * 2^(32) + 0 * 2^(64) + 0 * 2^(96) + 0 * 2^(128) + 0 * 2^(160) + 0 * 2^(192) + 1 * 2^(224)");
    std::cout << "checkpoint 1:3\n";

//////////////////////////////////////////////////////////////////////////////////////////
    x_1 = BigInt({0, 0, 1});
    x_2 = BigInt({1});
    -x_2;

    REQUIRE((x_1 + x_2).PrintBinary() == 
    "01111111111111111111111111111111111111111111111111111111111111111"); // 0 because positive
    std::cout << "checkpoint 1:4\n";

//////////////////////////////////////////////////////////////////////////////////////////
    x_1 = BigInt({1,2,3,4,5,6,7});
    x_2 = BigInt({7,6,5,4,3,2,1});

    REQUIRE((x_1 + x_2).PrintPow() == 
    "8 + 8 * 2^(32) + 8 * 2^(64) + 8 * 2^(96) + 8 * 2^(128) + 8 * 2^(160) + 8 * 2^(192)");
    std::cout << "checkpoint 1:5\n";

//////////////////////////////////////////////////////////////////////////////////////////
    x_1 = BigInt({U32MAX - 1,U32MAX - 1});
    x_2 = BigInt({U32MAX - 1,U32MAX - 1,U32MAX - 1});

    REQUIRE((x_1 + x_2) == 
    BigInt({U32MAX - 2, U32MAX - 1, 0, 1}));
    std::cout << "checkpoint 1:6\n";

//////////////////////////////////////////////////////////////////////////////////////////
    std::vector<u64> v_1 = std::vector<u64>(100, 0);
    v_1.push_back(1);

    std::vector<u64> v_2 = std::vector<u64>(100, 0);
    v_2.push_back(U32MAX - 1);

    x_1 = BigInt(v_1);
    x_2 = BigInt(v_2);

    std::vector<u64> v_3 = std::vector<u64>(101, 0);
    v_3.push_back(1);

    BigInt x_3(v_3);

    REQUIRE((x_1 + x_2) == x_3);
    std::cout << "checkpoint 1:7\n";
}



TEST_CASE("TEST 2 (Substracting)") {
    BigInt x_1;
    BigInt x_2;
    BigInt x_3;
    std::vector<u64> v_1;
    std::vector<u64> v_2;
    std::vector<u64> v_3;

//////////////////////////////////////////////////////////////////////////////////////////   
    v_1 = {1};
    v_2 = {1};
    v_3 = {0};
    x_1 = BigInt(v_1);
    x_2 = BigInt(v_2);
    x_3 = BigInt(v_3);

    REQUIRE((x_1 - x_2) == x_3);
    std::cout << "checkpoint 2:1\n";
//////////////////////////////////////////////////////////////////////////////////////////   
    v_1 = {0};
    v_2 = {1};
    v_3 = {1};
    x_1 = BigInt(v_1);
    x_2 = BigInt(v_2);
    x_3 = BigInt(v_3, 1);

    REQUIRE((x_1 - x_2) == x_3);
    std::cout << "checkpoint 2:2\n";
//////////////////////////////////////////////////////////////////////////////////////////   
    v_1 = {0, 1};
    v_2 = {1};
    v_3 = {U32MAX - 1};
    x_1 = BigInt(v_1);
    x_2 = BigInt(v_2);
    x_3 = BigInt(v_3);

    REQUIRE((x_1 - x_2) == x_3);
    std::cout << "checkpoint 2:3\n";
}


TEST_CASE("Test 3 (Multiplication)") {
    BigInt x_1;
    BigInt x_2;
    BigInt x_3;
    std::vector<u64> v_1;
    std::vector<u64> v_2;
    std::vector<u64> v_3;

//////////////////////////////////////////////////////////////////////////////////////////   
    v_1 = {1};
    v_2 = {1};
    v_3 = {1};
    x_1 = BigInt(v_1);
    x_2 = BigInt(v_2);
    x_3 = BigInt(v_3);

    REQUIRE((x_1 * x_2) == x_3);
    std::cout << "checkpoint 3:1\n";
    //////////////////////////////////////////////////////////////////////////////////////////   
    v_1 = {21};
    v_2 = {12};
    x_1 = BigInt(v_1, 1);
    x_2 = BigInt(v_2);

    REQUIRE((x_1 * x_2).PrintPow() == "-(252)");
    std::cout << "checkpoint 3:2\n";
//////////////////////////////////////////////////////////////////////////////////////////   
    v_1 = {1};
    v_2 = {1};
    x_1 = BigInt(v_1, 1);
    x_2 = BigInt(v_2);

    REQUIRE((x_1 * x_2).PrintPow() == "-(1)");
    std::cout << "checkpoint 3:3\n";
//////////////////////////////////////////////////////////////////////////////////////////   
    v_1 = {U32MAX - 1};
    v_2 = {U32MAX - 1};
    x_1 = BigInt(v_1);
    x_2 = BigInt(v_2);

    REQUIRE((x_1 * x_2).PrintPow() == "1 + 4294967294 * 2^(32)");
    std::cout << "checkpoint 3:4\n";
//////////////////////////////////////////////////////////////////////////////////////////   
    v_1 = {0, 1};
    v_2 = {0, 0, 1};
    x_1 = BigInt(v_1);
    x_2 = BigInt(v_2);

    REQUIRE((x_1 * x_2).PrintPow() == "0 + 0 * 2^(32) + 0 * 2^(64) + 1 * 2^(96)");
    std::cout << "checkpoint 3:5\n";
//////////////////////////////////////////////////////////////////////////////////////////   
    v_1 = {U32MAX - 1, U32MAX - 1, U32MAX - 1, U32MAX - 1, U32MAX - 1};
    v_2 = {0};
    x_1 = BigInt(v_1);
    x_2 = BigInt(v_2);

    REQUIRE((x_1 * x_2).PrintPow() == "0");
    std::cout << "checkpoint 3:6\n";
}


TEST_CASE("Test 3 (Comparison operations)") {
    BigInt x_1;
    BigInt x_2;
    BigInt x_3;
    std::vector<u64> v_1;
    std::vector<u64> v_2;
    std::vector<u64> v_3;

//////////////////////////////////////////////////////////////////////////////////////////   
    v_1 = {0};
    v_2 = {0};
    x_1 = BigInt(v_1);
    x_2 = BigInt(v_2);

    REQUIRE(x_1 == x_2);
    std::cout << "checkpoint 4:1\n";
//////////////////////////////////////////////////////////////////////////////////////////   
    REQUIRE(x_1 >= x_2);
    std::cout << "checkpoint 4:2\n";
//////////////////////////////////////////////////////////////////////////////////////////   
    REQUIRE(x_1 <= x_2);
    std::cout << "checkpoint 4:3\n";
//////////////////////////////////////////////////////////////////////////////////////////   
    REQUIRE(!(x_1 > x_2));
    std::cout << "checkpoint 4:4\n";
//////////////////////////////////////////////////////////////////////////////////////////   
    v_1 = {U32MAX - 1};
    v_2 = {0, 1};
    x_1 = BigInt(v_1);
    x_2 = BigInt(v_2);

    REQUIRE(x_1 < x_2);
    std::cout << "checkpoint 4:5\n";
//////////////////////////////////////////////////////////////////////////////////////////   
    v_1 = {1};
    v_2 = {2};
    x_1 = BigInt(v_1);
    x_2 = BigInt(v_2, 1);

    REQUIRE(x_1 > x_2);
    std::cout << "checkpoint 4:6\n";
}