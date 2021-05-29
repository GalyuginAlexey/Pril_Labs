#include <gtest/gtest.h>
#include "config/config.h"
#include "database/database.h"
#include "database/person.h"
#include <Poco/Data/SessionFactory.h>
using Poco::Data::Session;
using Poco::Data::Statement;
using namespace Poco::Data::Keywords;

class TestApp : public ::testing::Test {
protected:
    TestApp() {
        Config::get().host() = "127.0.0.1";
        Config::get().database() = "lab1_Gal";
        Config::get().port() = "8080";
        Config::get().login() = "lab1";
        Config::get().password() = "12345";
    }
    ~TestApp() {
        Poco::Data::Session session = database::Database::get().create_session();
        Statement reset_data(session);
        reset_data << "DELETE FROM Person WHERE id > 100000", now;
        Statement reset_increment(session);
        reset_increment << "ALTER TABLE Person AUTO_INCREMENT=100001", now;
    }
     void SetUp() {}
     void TearDown() {}

protected:
};

TEST_F(TestApp, TestPerson) {

    database::Person person;

    //POST tests
    person.login() = "123";
    person.first_name() = "Jack";
    person.last_name() = "London";
    person.age() = 34;
    testing::internal::CaptureStdout();
    person.save_to_mysql();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "inserted:100001\n");

    person.login() = "456";
    person.first_name() = "Tom";
    person.last_name() = "Sawyer";
    testing::internal::CaptureStdout();
    person.save_to_mysql();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "inserted:100002\n");

    person.login() = "789";
    person.first_name() = "Greg";
    person.last_name() = "Makeev";
    person.age() = 31;
    testing::internal::CaptureStdout();
    person.save_to_mysql();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "inserted:100003\n");

    person.login() = "509";
    person.first_name() = "Andrey";
    person.last_name() = "Saveliev";
    person.age() = 27;
    testing::internal::CaptureStdout();
    person.save_to_mysql();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "inserted:100004\n");

    person.login() = "919";
    person.first_name() = "Pavel";
    person.last_name() = "Petrov";
    person.age() = 18;
    testing::internal::CaptureStdout();
    person.save_to_mysql();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "inserted:100005\n");

    //GET tests
    database::Person login_result1 = database::Person::read_by_login("123");
    ASSERT_EQ(login_result1.get_first_name(), "Jack");
    ASSERT_EQ(login_result1.get_last_name(), "London");
    ASSERT_EQ(login_result1.get_age(), 34);

    database::Person login_result2 = database::Person::read_by_login("456");
    ASSERT_EQ(login_result2.get_first_name(), "Tom");
    ASSERT_EQ(login_result2.get_last_name(), "Sawyer");
    ASSERT_EQ(login_result2.get_age(), 34);

    auto name_result1 = database::Person::search("Jack", "London");
    ASSERT_EQ(name_result1.at(0).get_login(), "123");
    ASSERT_EQ(name_result1.size(), 1);

    auto name_result2 = database::Person::search("Tom", "Sawyer");
    ASSERT_EQ(name_result2.size(), 2);
    ASSERT_EQ(name_result2.at(0).get_last_name(), "Sawyer");

    auto full_query = database::Person::read_all();
    ASSERT_EQ(full_query.size(), 100005);
    ASSERT_EQ(full_query.at(100000).get_login(), "123");
    ASSERT_EQ(full_query.at(100003).get_last_name(), "Saveliev");
    ASSERT_EQ(full_query.at(100004).get_last_name(), "Petrov");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}