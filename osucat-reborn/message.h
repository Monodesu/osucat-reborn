#pragma once
#ifndef OSUCAT_MESSAGE_DEFINE
#define OSUCAT_MESSAGE_DEFINE

#define BOT_NAME "osuCat"
#define OSUCAT_SELF 834276213
#define MONO 451577581

#define 未绑定 u8"你还没有绑定你的账户，请使用!setid id来绑定~"
#define 他人未绑定 u8"他还没有绑定他的账户，请让他使用!setid id来绑定哦——"
#define 未从bancho检索到用户 u8"没这个人！"
#define 未找到最近游玩记录 u8"你最近在这个模式上还没有游玩记录~"
#define 未找到他人最近游玩记录 u8"他最近在这个模式上还没有游玩记录呢~"
#define 用户已被bancho封禁 u8"被办了。"
#define 获取谱面信息错误 u8"在获取Beatmap信息时发生了一些错误，请稍后再试。"
#define 没有查询到BP u8"你还没有这个bp！"
#define 没有查询到他人BP u8"他还没有这个bp！"
#define 没有查询到成绩 u8"你在这张谱面上还没有成绩呢。"
#define 被ban账户重新绑定提示 u8"你的账户已经被bancho ban掉了，请先解绑再绑定新的账户。"
#define 自定义Banner帮助                                                                                      \
    u8"请附上你想替换的背景！\n背景的尺寸为1200x340，所以建议上传的大小也为1200x340!\n" \
    u8"超出此大小可能会因为非常弱智的自动裁剪然后造成一些视觉体验上的问题..."
#define 自定义InfoPanel帮助                                                                                                      \
    u8"请附上你想替换的面板图像！面板图像模板的下载地址为：\nhttps://pan.baidu.com/s/"                        \
    u8"1FVkYD3Nr4iYZFrkuBb3m6g "                                                                                                        \
    u8"提取码s2d6\n请务必使用此模板进行替换！上传时请隐藏或删除模板中自带的test_"                        \
    u8"bg组，否则会覆盖掉已上传的banner，当然也可以自己设定bg而无视已上传的banner "                        \
    u8"~另外，请输出为PNG格式！请输出为PNG格式！请输出为PNG格式！\n输出为Jepg格式会由于没有透明通" \
    u8"道而导致banner被覆盖！ "
#define 接收图片出错 u8"接收图片失败，请稍后再试...\n如多次上传失败，请加群217451241联系管理员手动修改.."
#define 已上传待审核提示 u8"已成功上传，请耐心等待审核~"
#define InfoPanel数据错误提示 u8"请上传正确的Info面板！\n你是否使用了所提供的面板模板，且确保输出格式为png?"
#define 参数过长提示 u8"你在干嘛..."
#define 英文模式名提示 u8"0=std,1=taiko,2=ctb,3=mania"

#endif