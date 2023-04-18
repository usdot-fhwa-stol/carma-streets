#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include "streets_singleton.h"
#include "streets_singleton_exception.h"


using namespace streets_service;

struct instance_struct {
    int prop1;
    std::string prop2;
    instance_struct(int _param1, const std::string _param2): prop1(_param1), prop2(_param2){};
    void do_some_update(){
            prop1 = prop1 * 2;
            prop2 += "TEST";
    };
};
// Test singleton class
class test_singleton_impl : public streets_singleton<instance_struct,  int, std::string>{
    friend class streets_singleton<instance_struct, int, std::string >;


};
TEST(test_streets_singleton, test_streets_singleton_scope)
{
    ASSERT_THROW(test_singleton_impl::get_singleton(), streets_singleton_exception);
    test_singleton_impl::create(1, "TEST");
    ASSERT_EQ(test_singleton_impl::get_singleton().prop2, "TEST");
    ASSERT_EQ(test_singleton_impl::get_singleton().prop1, 1);
    
    test_singleton_impl::get_singleton().do_some_update();

    ASSERT_EQ(test_singleton_impl::get_singleton().prop2, "TESTTEST");
    ASSERT_EQ(test_singleton_impl::get_singleton().prop1, 2);

    test_singleton_impl::get_singleton().do_some_update();

    ASSERT_EQ(test_singleton_impl::get_singleton().prop2, "TESTTESTTEST");
    ASSERT_EQ(test_singleton_impl::get_singleton().prop1, 4);
};

TEST(test_streets_singleton, test_recreate_singleton) {
    // Used a different singleton since unit testing revealed that the first singleton was not deconstructed by the time of this 
    // unit test.
    test_singleton_impl::create(1, "TEST");
    ASSERT_EQ(test_singleton_impl::get_singleton().prop2, "TEST");
    ASSERT_EQ(test_singleton_impl::get_singleton().prop1, 1);
    SPDLOG_INFO("Attempting to test recreate functionality");
    // Recreate
    test_singleton_impl::create(2, "RECREATE");
    ASSERT_EQ(test_singleton_impl::get_singleton().prop2, "RECREATE");
    ASSERT_EQ(test_singleton_impl::get_singleton().prop1, 2);

    
   
}

   
