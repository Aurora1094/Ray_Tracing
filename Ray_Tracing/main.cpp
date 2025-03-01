#include <QApplication>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QColor>
#include <cmath>
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
//光屏平面的位置
static float z=500.0;
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

    Ray(Vector3 newA, Vector3 newB, float newt):A(newA),B(newB),t(newt)//赋值构造函数
    {
        A = newA;
        B = newB;
        B=Vector3::unitization(B);
        t = newt;
    }

    Vector3 ProjectivePoint(float t)const//得到射影点
    {
        return A +  B * t;
    }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//光线撞击相关函数
   //是否撞击的判断
static bool if_Hit(Vector3 center, float radius, Ray ray)
{
    float a = ray.B * ray.B;
    float b = -2.0 * (ray.B * (ray.A-center));
    float c = (ray.A-center) *(ray.A- center) - radius * radius;

    float delta = b * b - 4 * a * c;

    return delta > 0;
}
   //求解撞击法向量，并线性变换得到rgb编码(但是我就不判断撞不撞击了，使用时需要在前面加一个if（bool if_Hit）判断)
static Vector3 where_Hit(Vector3 center, float radius, Ray ray)
{
    float a = ray.B * ray.B;
    float b = -2.0 * (ray.B * (ray.A-center));
    float c = (ray.A-center) *(ray.A- center) - radius * radius;

    float delta = b * b - 4 * a * c;

    float x1=(-b+sqrt(delta))/(2*a);
    float x2=(-b-sqrt(delta))/(2*a);

    float t=(x1<x2?x1:x2);

    Vector3 Lawline=ray.A+ray.B*t-center;

    Lawline=Vector3::unitization(Lawline);//单位化

    Vector3 Unit(1,1,1);
    Lawline=(Lawline+Unit)/2*300;//向0~255作线性映射(为了颜色好看，改了一下，可能以后变数据会出问题，注意一下)

    return Lawline;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Sphere类的创建
class Sphere
{
public:
    Vector3 center;
    float radius;

    Sphere() : center(), radius(0.0) {};//构造函数，防垃圾值

     Sphere(Vector3 newCenter, float newRadius): center(newCenter), radius(newRadius)  //赋值构造函数
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

        // 创建一个 801x601 的 QImage，并初始化为白色
        QImage image(801, 601, QImage::Format_RGB32);
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

        // //建立地面球R0
        // Sphere R0(Vector3(0,1800,z),1800);

        // for (int y = 0; y <image.height() ; ++y)
        // {
        //     for (int x =0 ; x <image.width() ; ++x)
        //     {
        //         //世界坐标系转化
        //         int true_x=camera.x-(image.width()/2)+x;
        //         int true_y=camera.y-(image.height()/2)+y;

        //         Ray Hit_R1(camera,Vector3(true_x,true_y,z),0);//0是随意赋的值，无实义
        //         bool flag=if_Hit(R0.center,R0.radius,Hit_R1);
        //         if(flag)
        //         {
        //             Vector3 Lawline=where_Hit(R0.center,R0.radius,Hit_R1);
        //             QColor color2(Lawline.x-50,Lawline.y+80,Lawline.z-40);
        //             image.setPixel(x, y, color2.rgb());//渲染球体
        //         }
        //     }
        // }



        //建立球R1
        Sphere R1(Vector3(0,0,z),200.0);

        for (int y = 0; y <image.height() ; ++y)
        {
            for (int x =0 ; x <image.width() ; ++x)
            {
                //世界坐标系转化
                float true_x=(float)camera.x-(float)(image.width() - 1)/2.0f+(float)x;
                float true_y=(float)camera.y-(float)(image.height() - 1)/2.0f+(float)y;

                Ray Hit_R1(camera,Vector3(true_x,true_y,z),0);//0是随意赋的值，无实义
                bool flag=if_Hit(R1.center,R1.radius,Hit_R1);
                if(flag)
                {
                    Vector3 Lawline=where_Hit(R1.center,R1.radius,Hit_R1);
                    QColor color2(Lawline.x-20,Lawline.y-60,Lawline.z+150,200);
                    image.setPixel(x, y, color2.rgb());//渲染球体
                }
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
    widget.resize(801, 601); // 设置窗口大小
    widget.show();

    return app.exec();
}
