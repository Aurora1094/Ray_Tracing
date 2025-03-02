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
    double x, y, z;

    Vector3() : x(0), y(0), z(0) {}//构造函数，防垃圾值

    Vector3(double newX, double newY,double newZ)
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
    Vector3 operator*(double a)const
    {
        return Vector3(x * a, y * a, z * a);
    }
    //向量数除
    Vector3 operator/(double a)const
    {
        return Vector3(x / a, y / a, z / a);
    }
    //向量点积
    double operator*(const Vector3& a)const
    {
        return ((x * a.x)+(y * a.y)+( z * a.z));
    }
    //向量模长
    static double getlen(const Vector3& a)
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
static double z=500.0;
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

    Ray(Vector3 newA, Vector3 newB, float newt) : A(newA), B(newB), t(newt) {
        A = newA;
        B = newB;
        B = Vector3::unitization(B);
        t = newt;
    }
    // Vector3 ProjectivePoint(float t)const//得到射影点
    // {
    //     return A +  B * t;
    // }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Sphere类的创建
class Sphere
{
public:
    Vector3 center;
    double radius;

    Sphere() : center(), radius(0.0) {};//构造函数，防垃圾值

    Sphere(Vector3 newCenter, double newRadius): center(newCenter), radius(newRadius)  //赋值构造函数
    {
        // center=newACenter;
        radius=newRadius;
    }

};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//光线撞击相关函数
//是否撞击的判断
static bool if_Hit(Vector3 center, double radius, Ray ray)
{
    double a = ray.B * ray.B;
    double b = -2.0 * (ray.B * (ray.A-center));
    double c = (ray.A-center) *(ray.A- center) - radius * radius;

    double delta = b * b - 4 * a * c;

    return delta >= 0;
}
//求解撞击法向量，并线性变换得到rgb编码(但是我就不判断撞不撞击了，使用时需要在前面加一个if（bool if_Hit）判断)
static Vector3 where_Hit(Vector3 center, double radius, Ray ray)
{
    float a = ray.B * ray.B;
    float b = 2.0 * (ray.B * (ray.A-center));
    float c = (ray.A-center) *(ray.A- center) - radius * radius;

    float delta = b * b - 4 * a * c;

    float x1=(-b+sqrt(delta))/(2*a);
    float x2=(-b-sqrt(delta))/(2*a);

    float t=(x1<x2?x1:x2);

    if(t<=0&&(x1>x2?x1:x2)>0)
    {
        t=(x1>x2?x1:x2);
    }

    if(t>=0)
    {
        Vector3 Lawline=ray.A+ray.B*t-center;

        Lawline=Vector3::unitization(Lawline);//单位化

        Vector3 Unit(1,1,1);
        Lawline=(Lawline+Unit)/2*255;//向0~255作线性映射(为了颜色好看，改了一下，可能以后变数据会出问题，注意一下)

        return Lawline;
    }
    else
    {
        Vector3 black=Vector3(0,0,0);
        return black;


        // Vector3 Lawline=ray.A+ray.B*t-center;

        // Lawline=Vector3::unitization(Lawline);//单位化

        // Vector3 Unit(1,1,1);
        // Lawline=(Lawline+Unit)/2*255;//向0~255作线性映射(为了颜色好看，改了一下，可能以后变数据会出问题，注意一下)

        // return Lawline;
    }

}
//多重取样 抗锯齿
static Vector3 multisample(double x,double y,Sphere R)
{
    const int sample=2;
    Vector3 sample_point[sample*sample];
    int n=0;
    for(int i=0;i<sample;i++)
    {
        for(int j=0;j<sample;j++)
        {
            // float newx=x-0.5+i;
            // float newy=y-0.5+j;
            float newx=x-0.5+4/sample/sample*i;
            float newy=y-0.5+4/sample/sample*j;
            Ray Hit_R(camera, Vector3(newx, newy, z),0);//0是随意赋的值，无实义

             bool flag=if_Hit(R.center,R.radius,Hit_R);

            if(flag)
            {
                sample_point[n]=where_Hit(R.center,R.radius,Hit_R);
                n++;
            }
            else

            {
                double px=(x-camera.x+800/2);
                double py=(y-camera.y+600/2);
                sample_point[n]=Vector3(135+120*py/601,206+50*py/601,255);
                n++;
            }//对2*2个样本点实现了采集
        }
    }//对2*2个样本点实现了采集
    Vector3 average=sample_point[0];
    for(int i=1;i<sample*sample;i++)
    {
        average=average+sample_point[i];
    }
    average=average/sample/sample;

    //ai搜到的，防止越界的函数
    average.x = std::clamp((float)average.x, 0.0f, 255.0f);
    average.y = std::clamp((float)average.y, 0.0f, 255.0f);
    average.z = std::clamp((float)average.z, 0.0f, 255.0f);

    return average;
}
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

        // 将每个像素点设置为渐变青
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
        //             QColor color2(Lawline.x,Lawline.y,Lawline.z);
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
                double true_x=(double)camera.x-(double)(image.width() - 1)/2.0f+(double)x;
                double true_y=(double)camera.y-(double)(image.height() - 1)/2.0f+(double)y;

                Ray Hit_R1(camera,Vector3(true_x,true_y,z),0);//0是随意赋的值，无实义
                bool flag=if_Hit(R1.center,R1.radius,Hit_R1);
                if(flag)
                {
                    Vector3 point=multisample(true_x,true_y,R1);




                    QColor color2(point.x,point.y,point.z);
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
