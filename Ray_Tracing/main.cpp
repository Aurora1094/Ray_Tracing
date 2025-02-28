#include <QApplication>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QColor>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//向量类的创建（运算法则）
class Vector3
{
public:
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}//构造函数，防垃圾值

    Vector3(float newX, float newY, float newZ)
    {
        x = newX;
        y = newY;
        z = newZ;
    }

    //向量加法
    Vector3 operator+(const Vector3 &a)const
    {
        return Vector3(x + a.x, y + a.y, z + a.z);
    }
    //向量减法
    Vector3 operator-(const Vector3& a)const
    {
        return Vector3(x - a.x, y - a.y, z - a.z);
    }
    //向量数乘
    Vector3 operator*(float a)const
    {
        return Vector3(x * a, y * a, z * a);
    }
    //向量数除
    Vector3 operator/(float a)const
    {
        return Vector3(x / a, y / a, z / a);
    }
    //向量点积
    float operator*(const Vector3& a)const
    {
        return ((x * a.x)+(y * a.y)+( z * a.z));
    }
    //向量模长
    static float getlen(const Vector3& a)
    {
        return  sqrt( a * a);
    }
    //向量单位化
    static Vector3 unitization(const Vector3& a)
    {
        return a / getlen(a);
    }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//z轴的位置
static float z=2.0;
//摄像机位置(A)
static Vector3 camera(0.0,0.0,0.0);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Ray类的建立 A+t*B
class Ray
{
public:
    Vector3 A=camera;
    Vector3 B;
    float t;

    Ray() : A(), B(), t(0) {}//构造函数，防垃圾值

    Ray(Vector3 newA, Vector3 newB, float newt)//赋值构造函数
    {
        A = newA;
        B = newB;
        t = newt;
    }

    Vector3 ProjectivePoint(float t)const//得到射影点
    {
        return A +  B * t;
    }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//光线撞击相关函数
static bool if_Hit(Vector3 center, float radius, Ray ray)
{
    float a = ray.B * ray.B;
    float b = ray.B * (ray.A - center) * 2;
    float c = (ray.A - center) * (ray.A - center) - radius * radius;

    float delta = b * b - 4 * a * c;

    return delta > 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Sphere类的创建
class Sphere
{
public:
    Vector3 center;
    float radius;

    Sphere() : center(), radius(0.0) {};//构造函数，防垃圾值

    Sphere(Vector3 newACenter(float newX, float newY, float newZ),float newRadius)//赋值构造函数
    {
        // center=newACenter;
        radius=newRadius;
    }

};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//窗口，输出一张图片
class ImageWidget : public QWidget
{
protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);

        // 创建一个 800x600 的 QImage，并初始化为白色
        QImage image(800, 600, QImage::Format_RGB32);
        image.fill(Qt::white);  // 可选：设置初始背景颜色

        // 将每个像素点设置为黑色
        for (int y = 0; y <image.height() ; ++y)
        {
            for (int x =0 ; x <image.width() ; ++x)
            {
                QColor color(135+120*y/image.height(),206+50*y/image.height(),255);  // 渐变青
                image.setPixel(x, y, color.rgb());
            }
        }
        // 绘制图像到窗口
        painter.drawImage(0, 0, image);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ImageWidget widget;
    widget.resize(800, 600); // 设置窗口大小
    widget.show();

    return app.exec();
}
