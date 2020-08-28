#pragma once
#pragma once
#ifndef OSUCAT_HEXAGRAM_HPP
#define OSUCAT_HEXAGRAM_HPP
#define twodec(x) (al_round((x)*100.0f) / 100.0f)

#include <Magick++.h>
#include <cmath>

#define M_PI 3.14159265358979323846

//#define min(x, y) x < y ? x : y;
//#define max(x, y) x > y ? x : y;

using namespace std;

namespace Hexagram {
    struct xy {
        double x, y;
    };

    struct r8 {
        r8(double nr, double n8) : r(nr), _8(n8) {
        }
        double r, _8;
    };

    struct HexagramInfo {
        int64_t size;
        int8_t node_count; //设置有几个边需要绘制
        int32_t nodeMaxValue; //设置绘制数据的最大值
        std::string abilityLineColor; //设置轮廓线的颜色
        std::string abilityFillColor; //设置填充颜色
        int32_t sideLength; //图片的边长=多边形外接圆直径
        int32_t StrokeWidth; //连接线尺寸 v1面板默认3
        float nodesize; //坐标点大小 v1面板默认2.5f
        int mode;
    };

    // 极坐标转换直角坐标
    static void r82xy(const r8& _r8, xy& _xy) {
        _xy.x = _r8.r * sin(_r8._8 * M_PI / 180);
        _xy.y = _r8.r * cos(_r8._8 * M_PI / 180);
    }

    // ppd          pp_plus_data, 注意要与下面的multi和exp参数数量相同且对齐
    // multi, exp   加权值 y = multi * x ^ exp
    // pImage       指向InfoPanel图片的指针，用&InfoPanel来表示指针
    // x(y)_offset  pp+图片相对于InfoPanel图片的坐标偏移量
    // hi           pp+图片的一些设置参数, hi.node_count要等于ppd的数量
    static void draw_v1(const std::vector<long>& ppd, const std::vector<double>& multi,
        const std::vector<double>& exp, Magick::Image* pImage, double x_offset,
        double y_offset, const HexagramInfo& hi) {
        if (ppd.size() != multi.size() || ppd.size() != exp.size() || ppd.size() != hi.node_count) {
            return;
        }
        Magick::Image im(Magick::Geometry((int)hi.size, (int)hi.size), Magick::Color("none"));
        Magick::CoordinateList point_list;
        for (int i = 0; i < hi.node_count; ++i) {
            xy _xy;
            double r = pow(multi[i] * pow(ppd[i], exp[i]) / hi.nodeMaxValue, 0.8) * hi.size / 2;
            if (hi.mode == 1) {
                if (r > 100.00) r = 100.00;
            }
            else if (hi.mode == 2) {
                if (r > 395.00) r = 395.00;
            }
            else if (hi.mode == 3) {
                if (r > 495.00) r = 495.00;
            }
            r8 _r8(r, 360.0 / hi.node_count * i + 90);
            r82xy(_r8, _xy);
            point_list.push_back(Magick::Coordinate(_xy.x + hi.size / 2, _xy.y + hi.size / 2));
        }

        Magick::DrawableList drawables;
        drawables.push_back(Magick::DrawableFillColor(hi.abilityFillColor));
        drawables.push_back(Magick::DrawableStrokeColor(hi.abilityLineColor));
        drawables.push_back(Magick::DrawableStrokeWidth(hi.StrokeWidth)); //连接线尺寸
        drawables.push_back(Magick::DrawablePolygon(point_list));
        im.draw(drawables);
        drawables.clear();
        drawables.push_back(Magick::DrawableFillColor(hi.abilityLineColor));
        for (int i = 0; i < hi.node_count; ++i) {
            drawables.push_back(Magick::DrawableCircle(point_list[i].x(),
                point_list[i].y(),
                point_list[i].x() + hi.nodesize,
                point_list[i].y())); //坐标点大小这里改
        }
        im.draw(drawables);


        pImage->composite(im, (int)x_offset, (int)y_offset, MagickCore::OverCompositeOp);
    }
}; // namespace Hexagram
#endif