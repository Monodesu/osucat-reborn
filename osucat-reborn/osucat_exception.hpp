#pragma once
#ifndef OC_EXCEPTION
#define OC_EXCEPTION


namespace osucat {
    class exception {
    public:
        exception(const std::string& info, int code) {
            this->info = info;
            this->code = code;
        }

        int Code() {
            return this->code;
        }

        std::string Info() {
            return this->info;
        }

        // ����������������ʾ����ĸ�ʽ
        virtual std::string Show() {
            time_t tt = time(NULL);
            tm _tm = { 0 };
            localtime_s(&_tm, &tt);
            char time_str[128];
            strftime(time_str, 128, "%Y-%m-%d %H:%M:%S", &_tm);

            char result[1024];
            // [ʱ��] ������� info��������Ϣ
            sprintf_s(result, 1024, "[%s] %d - %s", time_str, this->code, this->info.c_str());
            return result;
        }

    private:
        std::string info;
        // error code
        int32_t code;
        // �������exception��target
        //cq::Target target;
        // target���͵���Ϣ
        std::string message;
    };
    class NetWork_Exception : public exception {
    public:
        NetWork_Exception(const std::string& info, int code) : exception(info, code) {
        }
        std::string Show() {
            char buffer[512];
            sprintf_s(buffer,
                512,
                "������ִ��󣬴�����룺%d��������Ϣ��%s",
                this->Code(),
                this->Info().c_str());
            return buffer;
        }
    };
    // ����������ʽ�̳��˸����info��code
    class database_exception : public exception {
    public:
        database_exception(const std::string& info, int code) : exception(info, code) {
        }

        // ���أ��滻�������Show����
        std::string Show() {
            char buffer[512];
            sprintf_s(buffer,
                512,
                "MySQL���ִ��󣬴�����룺%d��������Ϣ��%s",
                this->Code(),
                this->Info().c_str());
            return buffer;
        }
    };

} // namespace OsuCat
#endif // !1