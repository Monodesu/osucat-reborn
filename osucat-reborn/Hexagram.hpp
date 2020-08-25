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
        int8_t node_count; //�����м�������Ҫ����
        int32_t nodeMaxValue; //���û������ݵ����ֵ
        std::string abilityLineColor; //���������ߵ���ɫ
        std::string abilityFillColor; //���������ɫ
        int32_t sideLength; //ͼƬ�ı߳�=��������Բֱ��
        int32_t StrokeWidth; //�����߳ߴ� v1���Ĭ��3
        float nodesize; //������С v1���Ĭ��2.5f
        int mode;
    };

    // ������ת��ֱ������
    static void r82xy(const r8& _r8, xy& _xy) {
        _xy.x = _r8.r * sin(_r8._8 * M_PI / 180);
        _xy.y = _r8.r * cos(_r8._8 * M_PI / 180);
    }

    // ppd          pp_plus_data, ע��Ҫ�������multi��exp����������ͬ�Ҷ���
    // multi, exp   ��Ȩֵ y = multi * x ^ exp
    // pImage       ָ��InfoPanelͼƬ��ָ�룬��&InfoPanel����ʾָ��
    // x(y)_offset  pp+ͼƬ�����InfoPanelͼƬ������ƫ����
    // hi           pp+ͼƬ��һЩ���ò���, hi.node_countҪ����ppd������
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
        drawables.push_back(Magick::DrawableStrokeWidth(hi.StrokeWidth)); //�����߳ߴ�
        drawables.push_back(Magick::DrawablePolygon(point_list));
        im.draw(drawables);
        drawables.clear();
        drawables.push_back(Magick::DrawableFillColor(hi.abilityLineColor));
        for (int i = 0; i < hi.node_count; ++i) {
            drawables.push_back(Magick::DrawableCircle(point_list[i].x(),
                point_list[i].y(),
                point_list[i].x() + hi.nodesize,
                point_list[i].y())); //������С�����
        }
        im.draw(drawables);


        pImage->composite(im, (int)x_offset, (int)y_offset, MagickCore::OverCompositeOp);
    }
}; // namespace Hexagram
#endif