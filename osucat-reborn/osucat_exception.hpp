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

        // 输出函数，你决定显示错误的格式
        virtual std::string Show() {
            time_t tt = time(NULL);
            tm _tm = { 0 };
            localtime_s(&_tm, &tt);
            char time_str[128];
            strftime(time_str, 128, "%Y-%m-%d %H:%M:%S", &_tm);

            char result[1024];
            // [时间] 错误代码 info：错误消息
            sprintf_s(result, 1024, "[%s] %d - %s", time_str, this->code, this->info.c_str());
            return result;
        }

    private:
        std::string info;
        // error code
        int32_t code;
        // 触发这个exception的target
        //cq::Target target;
        // target发送的消息
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
                "网络出现错误，错误代码：%d，错误信息：%s",
                this->Code(),
                this->Info().c_str());
            return buffer;
        }
    };
    // 这里子类隐式继承了父类的info和code
    class database_exception : public exception {
    public:
        database_exception(const std::string& info, int code) : exception(info, code) {
        }

        // 重载（替换）基类的Show函数
        std::string Show() {
            char buffer[512];
            sprintf_s(buffer,
                512,
                "MySQL出现错误，错误代码：%d，错误信息：%s",
                this->Code(),
                this->Info().c_str());
            return buffer;
        }
    };

} // namespace OsuCat
#endif // !1