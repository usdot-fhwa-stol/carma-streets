#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include "streets_singleton.h"


using namespace streets_service;
// Test singleton class
class test_singleton_impl : public streets_singleton<test_singleton_impl>{
    friend class streets_singleton<test_singleton_impl>;
    private:
        // Double property
        int test_int_property;
        // String property
        std::string test_string_property;
        // Parameterized Constructor 
        test_singleton_impl(int init_param1=1, std::string init_param2="TEST"): test_int_property(init_param1), test_string_property(init_param2) {};
    public:
        // test
        int const get_test_int_property() {
            return test_int_property;
        }

        std::string const get_test_string_property() {
            return test_string_property;
        }

        void do_some_update(){
            test_int_property = test_int_property * 2;
            test_string_property += "TEST";
        }


};
TEST(test_streets_singleton, test_streets_singleton_scope)
{
    ASSERT_EQ(test_singleton_impl::get_singleton().get_test_string_property(), "TEST");
    ASSERT_EQ(test_singleton_impl::get_singleton().get_test_int_property(), 1);
    
    test_singleton_impl::get_singleton().do_some_update();

    ASSERT_EQ(test_singleton_impl::get_singleton().get_test_string_property(), "TESTTEST");
    ASSERT_EQ(test_singleton_impl::get_singleton().get_test_int_property(), 2);

    test_singleton_impl::get_singleton().do_some_update();

    ASSERT_EQ(test_singleton_impl::get_singleton().get_test_string_property(), "TESTTESTTEST");
    ASSERT_EQ(test_singleton_impl::get_singleton().get_test_int_property(), 4);
};
