#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include "streets_singleton.h"
#include "streets_singleton_exception.h"


using namespace streets_service;
// Test singleton class
class test_singleton_impl : public streets_singleton<test_singleton_impl,  int, std::string>{
    friend class streets_singleton<test_singleton_impl, int, std::string >;
    private:
        // Double property
        int test_int_property;
        // String property
        std::string test_string_property;
        // Parameterized Constructor 
        test_singleton_impl(int init_param1, std::string init_param2): test_int_property(init_param1), test_string_property(init_param2) {};
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
    ASSERT_THROW(test_singleton_impl::get_singleton(), streets_singleton_exception);
    test_singleton_impl::create(1, "TEST");
    ASSERT_EQ(test_singleton_impl::get_singleton().get_test_string_property(), "TEST");
    ASSERT_EQ(test_singleton_impl::get_singleton().get_test_int_property(), 1);
    
    test_singleton_impl::get_singleton().do_some_update();

    ASSERT_EQ(test_singleton_impl::get_singleton().get_test_string_property(), "TESTTEST");
    ASSERT_EQ(test_singleton_impl::get_singleton().get_test_int_property(), 2);

    test_singleton_impl::get_singleton().do_some_update();

    ASSERT_EQ(test_singleton_impl::get_singleton().get_test_string_property(), "TESTTESTTEST");
    ASSERT_EQ(test_singleton_impl::get_singleton().get_test_int_property(), 4);
};

TEST(test_streets_singleton, test_recreate_singleton) {
    // Used a different singleton since unit testing revealed that the first singleton was not deconstructed by the time of this 
    // unit test.
    streets_singleton<int>::create();
    try {
        streets_singleton<int>::create();
    }catch (const streets_singleton_exception &e ) {
        ASSERT_STREQ("Singleton has already been created!", e.what());
    }
}

   
