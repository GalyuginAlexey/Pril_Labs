#include <gtest/gtest.h>
#include "config/config.h"
#include "database/database.h"
#include "database/person.h"
#include <Poco/Data/SessionFactory.h>
using Poco::Data::Session;
using Poco::Data::Statement;
using namespace Poco::Data::Keywords;

long get_ai(int shard_num) {

    int result;

    Poco::Data::Session session = database::Database::get().create_session();
    Statement get_ai(session);
    std::string sql_request = "Select count(*) from Person -- sharding:" + std::to_string(shard_num);
    get_ai << sql_request, into(result), now;

    return result;
}

void reset_ai(int shard_num) {
    Poco::Data::Session session = database::Database::get().create_session();
    Statement reset_ai(session);
    std::string sql_request = "ALTER TABLE Person AUTO_INCREMENT = " + std::to_string((shard_num) + 1) + " -- sharding:" + std::to_string(shard_num);
    reset_ai << sql_request, now;
}

void remove_person(std::string login, int shard_num) {
    Poco::Data::Session session = database::Database::get().create_session();
    Statement cleanup(session);
    std::string sql_request = "DELETE FROM Person where login = ? -- sharding:" + std::to_string(shard_num);
    cleanup << sql_request, use(login), now;
}

class TestApp : public ::testing::Test {
protected:
    TestApp() {
        Config::get().host() = "127.0.0.1";
        Config::get().database() = "sql_test";
        Config::get().port() = "6033";
        Config::get().login() = "test";
        Config::get().password() = "pzjqUkMnc7vfNHET";
    }
    
    ~TestApp() {
        remove_person("123", 0);
        remove_person("456", 1);
        remove_person("789", 1);
        remove_person("509", 2);
        remove_person("919", 0);

        reset_ai(0);
        reset_ai(1);
        reset_ai(2);
    }
    
    void SetUp() {}
    
    void TearDown() {}

protected:

};

TEST_F(TestApp, TestPerson) {

    database::Person person;
    person.login() = "123";
    person.first_name() = "Jack";
    person.last_name() = "London";
    person.age() = 34;
    testing::internal::CaptureStdout();
    person.save_to_mysql();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "-- sharding:0\n");

    person.login() = "456";
    person.first_name() = "Tom";
    person.last_name() = "Sawyer";
    testing::internal::CaptureStdout();
    person.save_to_mysql();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "-- sharding:1\n");

     person.login() = "789";
    person.first_name() = "Greg";
    person.last_name() = "Makeev";
    person.age() = 31;
    testing::internal::CaptureStdout();
    person.save_to_mysql();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "-- sharding:1\n");

    person.login() = "509";
    person.first_name() = "Andrey";
    person.last_name() = "Saveliev";
    person.age() = 27;
    testing::internal::CaptureStdout();
    person.save_to_mysql();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "-- sharding:2\n");

    person.login() = "919";
    person.first_name() = "Pavel";
    person.last_name() = "Petrov";
    person.age() = 18;
    testing::internal::CaptureStdout();
    person.save_to_mysql();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "-- sharding:0\n");

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
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}