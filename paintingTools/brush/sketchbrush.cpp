#include "sketchbrush.h"

SketchBrush::SketchBrush(QObject *parent) :
    Brush(parent)
{

}

bool SketchBrush::loadStencil(const QString &)
{
    return true;
}

void SketchBrush::setColor(const QColor &color)
{
    mainColor = color;
}

int SketchBrush::width()
{
    return sketchPen.width();
}

void SketchBrush::setWidth(int w)
{
    sketchPen.setWidth(w);
}

void SketchBrush::start(const QPointF &st)
{
    clear();
    preparePen();
    leftOverDistance = 0;
    points.clear();
    points.push_back(st);
    lastPoint_ = st;
}

void SketchBrush::preparePen()
{
    QColor subColor = mainColor;
    subColor.setAlphaF(0.07);
    sketchPen.setColor(subColor);
    sketchPen.setCapStyle(Qt::RoundCap);
    sketchPen.setJoinStyle(Qt::RoundJoin);
}

void SketchBrush::lineTo(const QPointF &st)
{
    clear();
    if(lastPoint_.isNull()){
        start(st);
        return;
    }
    points.push_back(st);
    sketch();
    lastPoint_ = st;
}

void SketchBrush::setLastPoint(const QPointF &p)
{
    lastPoint_ = p;
    points.clear();
}

void SketchBrush::sketch()
{
    if(points.count() > 10){
        QPainterPath path;
        path.moveTo(points[0]);
        for(int i=0;i<points.count()-10;i+=4){
            path.cubicTo(points[i], points[i+2], points[i+9]);
        }
        points.pop_front();

        QPainter painter;
        if(directDraw_ && surface_){
            painter.begin(surface_);
        }else{
            painter.begin(&result);
        }
        painter.setRenderHint(QPainter::Antialiasing);
        painter.strokePath(path, sketchPen);
    }
}

QVariantMap SketchBrush::brushInfo()
{
    QVariantMap map;
    QVariantMap colorMap;
    colorMap.insert("red", sketchPen.color().red());
    colorMap.insert("green", sketchPen.color().green());
    colorMap.insert("blue", sketchPen.color().blue());
    colorMap.insert("alpha", sketchPen.color().alpha());
    map.insert("width", QVariant(sketchPen.width()));
    map.insert("color", colorMap);
    map.insert("name", QVariant("Sketch"));
    return map;
}

QVariantMap SketchBrush::defaultInfo()
{
    QVariantMap map;
    QVariantMap colorMap;
    colorMap.insert("red", 160);
    colorMap.insert("green", 160);
    colorMap.insert("blue", 164);
    colorMap.insert("alpha", 255);
    map.insert("width", QVariant(1));
    map.insert("color", colorMap);
    return map;
}
