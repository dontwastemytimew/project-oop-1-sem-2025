#include <gtest/gtest.h>
#include <QFile>
#include "ChatManager.h"
#include "DatabaseManager.h"
#include "ContactInfo.h"
#include "Preference.h"
#include "ChatStructures.h"
#include "UserProfile.h"

// Unit Tests for Models
TEST(ModelTests, ContactInfo) {
    ContactInfo c("123", "email@test.com");
    EXPECT_EQ(c.getPhone(), "123");
    EXPECT_EQ(c.getEmail(), "email@test.com");

    c.setPhone("456");
    EXPECT_EQ(c.getPhone(), "456");
}

TEST(ModelTests, Preference) {
    Preference p(18, 99, "Kyiv", "Male", "Hetero");
    EXPECT_EQ(p.getMinAge(), 18);
    EXPECT_EQ(p.getCity(), "Kyiv");

    p.setCity("Lviv");
    EXPECT_EQ(p.getCity(), "Lviv");
}

TEST(ModelTests, ChatMessageStruct) {
    ChatMessage msg;
    msg.message = "Hello";
    msg.fromUserId = 1;
    msg.toUserId = 2;
    msg.timestamp = QDateTime::currentDateTime();

    EXPECT_EQ(msg.message, "Hello");
    EXPECT_TRUE(msg.timestamp.isValid());
}

// Integration Tests for ChatManager
class ChatManagerTests : public ::testing::Test {
protected:
    DatabaseManager* dbManager;
    ChatManager* chatManager;
    const QString DB_FILE = "dating_agency.db";

    void SetUp() override {
        QFile::remove(DB_FILE);
        dbManager = new DatabaseManager();
        dbManager->openDatabase();
        chatManager = new ChatManager(dbManager);
    }

    void TearDown() override {
        delete chatManager;
        delete dbManager;
        QFile::remove(DB_FILE);
    }
};

TEST_F(ChatManagerTests, SendAndReceiveMessage) {
    UserProfile u1(-1, "A", 20, "C", "B");
    u1.setContactInfo(ContactInfo("1", "a@chat.com"));
    dbManager->saveProfile(u1);

    UserProfile u2(-1, "B", 20, "C", "B");
    u2.setContactInfo(ContactInfo("2", "b@chat.com"));
    dbManager->saveProfile(u2);

    dbManager->loadProfileByEmail("a@chat.com", u1);
    dbManager->loadProfileByEmail("b@chat.com", u2);

    int idA = u1.getId();
    int idB = u2.getId();

    QString text = "Hello friend!";
    chatManager->sendMessage(idA, idB, text);

    QList<ChatMessage> history = chatManager->getMessages(idA, idB);

    ASSERT_EQ(history.size(), 1);
    EXPECT_EQ(history[0].message, text);
    EXPECT_EQ(history[0].fromUserId, idA);
    EXPECT_EQ(history[0].toUserId, idB);
}

TEST_F(ChatManagerTests, BotReply) {
    QString reply = chatManager->getBotReply();
    EXPECT_FALSE(reply.isEmpty());
}