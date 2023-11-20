//
// pch.h
//

#pragma once
#include<iostream>
#include "gtest/gtest.h"

int add(int a, int b) {
    return a + b;
}

TEST(GeneralTest, BasicAssertion)
{
    //FAIL() << "Fatal Failure";
    //std::cerr << "Fatal Failure" << std::endl; 
    //exit(-1);

    ASSERT_TRUE(add(1, 2) == 3);
    ASSERT_FALSE(add(1, 2) == 2);
}

TEST(GeneralTest, BinaryComparison) {


    EXPECT_EQ(add(1, 2), add(2, 1));
    EXPECT_GT(add(1, 2), 1);
    EXPECT_NE(std::string("a"), std::string("b"));
    EXPECT_EQ(std::string("abc"), std::string("abc"));
    // when compare std::string and c string, it is string comparison.
    // It use overloaded string comparison operator.
    EXPECT_EQ(std::string("a"), "a");
    EXPECT_NE(std::string("a"), "b");

    {
        const char* cstr_1 = "abc";
        const char* cstr_2 = "abc";
        // c string comparison is to compare addresses,cstr_1 addresses and cstr_2 addresses are equal
        EXPECT_EQ(cstr_1, cstr_2);
    }
    {
        const char* cstr_1 = "abc";
        const std::string stdstr = "abc";
        const char* cstr_2 = stdstr.c_str();
        // cstr_1 addresses and cstr_2 addresses are not equal
        EXPECT_NE(cstr_1, cstr_2);
        EXPECT_STREQ(cstr_1, cstr_2);
    }
}

TEST(GeneralTest, StringComparison)
{
    const char* cstr_1 = "abc";
    const std::string stdstr_1 = "abc";
    const char* cstr_2 = stdstr_1.c_str();
    const std::string stdstr_2 = "AbC";
    const char* cstr_3 = stdstr_2.c_str();

    ASSERT_STREQ(cstr_1, cstr_2);
    ASSERT_STRCASEEQ(cstr_2, cstr_3);
}

TEST(GeneralTest, ThrowAssertion)
{
    const auto throw_error_lamba = []() {
        throw std::runtime_error("woops!");
    };
    //statement throws an exception of the given type
    ASSERT_THROW(throw_error_lamba(), std::runtime_error);
    ASSERT_ANY_THROW(throw_error_lamba());
    ASSERT_NO_THROW({ int a = 1; });
}

TEST(GeneralTest, FloatAssertion)
{
    ASSERT_FLOAT_EQ(1.0000001f, 1.0f);
    // ASSERT_DOUBLE_EQ(1.0000001, 1.0); //error,this is 1.0000001000000001 compared to 1 

     // not exceed absolute error 
    ASSERT_NEAR(1.009f, 1.0f, 0.01f);
    // predicate judgment
    EXPECT_PRED_FORMAT2(::testing::FloatLE, 1.0000001f, 1.0f);
}


TEST(GeneralTest, ClassTraits)
{
    //class A
    //{
    //public:
    //    int x;
    //};
    //class B
    //{
    //public:
    //    int y;
    //};
    //::testing::StaticAssertTypeEq<double, int>();
}


// This class has a private member we want to test.  We will test it
// both in a TEST and in a TEST_F.
class Foo {
public:
    Foo() {}

private:
    int Bar() const { return 1; }

    // Declares the friend tests that can access the private member
    // Bar().
    FRIEND_TEST(FRIEND_TEST_Test, TEST);
    FRIEND_TEST(FRIEND_TEST_Test2, TEST_F);
};

// Tests that the FRIEND_TEST declaration allows a TEST to access a
// class's private members.  This should compile.
TEST(FRIEND_TEST_Test, TEST) {
    ASSERT_EQ(1, Foo().Bar());
}

// The fixture needed to test using FRIEND_TEST with TEST_F.
class FRIEND_TEST_Test2 : public ::testing::Test {
protected:
    Foo foo;
};

// Tests that the FRIEND_TEST declaration allows a TEST_F to access a
// class's private members.  This should compile.
TEST_F(FRIEND_TEST_Test2, TEST_F) {
    ASSERT_EQ(1, foo.Bar());
}

::testing::AssertionResult IsEven(int n) {
    if ((n % 2) == 0)
        return ::testing::AssertionSuccess() << n << " is even";
    else
        return ::testing::AssertionFailure() << n << " is odd";
}

TEST(TestAssertResult, Check) {
    EXPECT_FALSE(IsEven(1));
    EXPECT_TRUE(IsEven(0));
}



class TimedTest : public testing::Test
{
protected:
    // 开始计时，如果别的夹具继承TimedTest，需要显式地调用SetUp和TearDown。
    void SetUp() override {
        start_time_ = time(NULL);
    }

    // check if the test was too slow.  
    void TearDown() override {
        // Gets the time when the test finishes    
        const time_t end_time = time(NULL);
        // Asserts that the test took no more than ~5 seconds.      
        EXPECT_TRUE(end_time - start_time_ <= 5) << "The test took too long.";
    }
    time_t start_time_;
};

TEST(TimedTest, Timer) {
    time_t start_time_ = time(NULL);
    while (true)
    {
        time_t end_time = time(NULL);
        if (end_time - start_time_ >5)
            break;
    }
}

class MyEnvironment : public ::testing::Environment {
public:
    ~MyEnvironment() override {}

    // Override this to define how to set up the environment.
    void SetUp() override {}

    // Override this to define how to tear down the environment.
    void TearDown() override {}
};



int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    // GTest own the pointer.
    testing::Environment* my = testing::AddGlobalTestEnvironment(new MyEnvironment);
    return RUN_ALL_TESTS();
}
class IsEvenTest : public testing::TestWithParam<int> {
};

TEST_P(IsEvenTest, HandleTrue)
{
    EXPECT_TRUE(IsEven(GetParam()));
}

INSTANTIATE_TEST_CASE_P(InstantiationName, IsEvenTest, testing::Range(0,11,1), testing::PrintToStringParamName());