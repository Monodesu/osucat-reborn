#pragma once
#include <stdint.h>
#include <string>

namespace osucat {
    struct Target {
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
        enum class RequestType {
            FRIEND,
            GROUP,
        };
        std::string GR_SubType;
        RequestType request_type;
        int64_t time;
        int64_t group_id;
        int64_t user_id;
        std::string flag;
        std::string message;//������Ϣ

    };

    struct SenderInfo {
        enum class Role {
            OWNER,
            ADMIN,
            MEMBER,
        };
        Role member_role;
        int age;
        std::string card; //Ⱥ�ǳ�
        std::string level;
        std::string nikename; //ԭ���û���
        std::string title; //ͷ��
        std::string sex;
    };
}