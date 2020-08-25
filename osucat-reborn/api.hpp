#pragma once
#include <stdint.h>
#include <string>

struct Target{
    enum class MessageType {
        PRIVATE,
        GROUP,
    };
    MessageType message_type;
    int64_t time;
    int64_t group_id;
    int64_t user_id;
    int64_t message_id;
    std::string message;
};

struct Request {
    //enum class RequestType {
    //    FRIEND,
    //    GROUP,
    //};
    //std::string GR_SubType;
    //RequestType request_type;
    int64_t time;
    int64_t group_id;
    int64_t user_id;
    std::string flag;
    std::string message;//附加信息

};

struct SenderInfo {
    enum class Role {
        OWNER,
        ADMIN,
        MEMBER,
    };
    Role member_role;
    int age;
    std::string card; //群昵称
    std::string level;
    std::string nikename; //原有用户名
    std::string title; //头衔
    std::string sex;
};