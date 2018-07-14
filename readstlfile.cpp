﻿#pragma once
#include <vector>
#include <fstream>
#include"Point3f.h"
#include <iostream>
#include <string>
#include"readstlfile.h"
#include <sstream>
#include <QDebug>

using namespace std;

bool ReadSTLFile::ReadStlFile(const char *cfilename)
{
    FILE * pFile;
    long lSize;
    char* buffer;
    size_t result;

    /* 若要一个byte不漏地读入整个文件，只能采用二进制方式打开 */
    fopen_s(&pFile,cfilename, "rb");//fopen_s(指针,文件名,使用文件方式);其中指针是用来接收<指向打开的文件的指针>的指针
    if (pFile == NULL)
    {
        fputs("File error", stderr);
        //exit(1);
    }

    /* 获取文件大小 */
    fseek(pFile, 0, SEEK_END);//控制件指针偏移
    lSize = ftell(pFile);//得到文件位置指针当前位置相对于文件首的偏移字节数
    rewind(pFile);//文件位置为给定流文件的开头

    /* 分配内存存储整个文件 */
    buffer = (char*)malloc(sizeof(char)*lSize);
    if (buffer == NULL)
    {
        fputs("Memory error", stderr);
        //exit(2);
    }

    /* 将文件拷贝到buffer中 */
    result = fread(buffer, 1, lSize, pFile);//从给定流pFile读取数据到Buffer所指向的数组中
    if (result != lSize)
    {
        fputs("Reading error", stderr);
        //exit(3);
    }


    /* 结束演示，关闭文件并释放内存 */
    fclose(pFile);

    ios::sync_with_stdio(false);
    if (buffer[0]=='s')//判断格式
    {
        std::cout<<"ASCII文件"<<endl;
        vector<Point3f>().swap(pointList);
        ReadASCII(buffer);
    }
    else
    {
        std::cout<<"Binary文件"<<endl;
        vector<Point3f>().swap(pointList);
        ReadBinary(buffer);
    }
    ios::sync_with_stdio(true);

    free(buffer);//释放内存
    return true;
}

bool ReadSTLFile::ReadASCII(const char *buffer)
{
    unTriangles = 0;
    float x, y, z;
    string name, useless;
    stringstream ss(buffer);
    getline(ss, name);//文件路径及文件名
    ss.get();
    //提取第一个面片，得到最大最小值初始值
    ss >> useless;//facet
    ss >> useless >> x >> y >>z;
    pointList.push_back(Point3f(x, y, z));
    getline(ss, useless);
    getline(ss, useless);//outer loop
    for (int i = 0; i < 3; i++)
    {
        ss >> useless >> x >> y >> z;
        pointList.push_back(Point3f(x, y, z));
    }
    surroundBox[0]=qMin(pointList.at(0).x,pointList.at(1).x);
    surroundBox[1]=qMax(pointList.at(0).x,pointList.at(1).x);
    surroundBox[2]=qMin(pointList.at(0).y,pointList.at(1).y);
    surroundBox[3]=qMax(pointList.at(0).y,pointList.at(1).y);
    surroundBox[4]=qMin(pointList.at(0).z,pointList.at(1).z);
    surroundBox[5]=qMax(pointList.at(0).z,pointList.at(1).z);
    unTriangles++;
    getline(ss, useless);//空行
    getline(ss, useless);//end loop
    getline(ss, useless);//end facet

    //读取其他面片
    do {
        ss >> useless;//facet
        if (useless != "facet")
            break;
        ss >> useless >> x >> y >>z;
        pointList.push_back(Point3f(x, y, z));
        getline(ss, useless);
        getline(ss, useless);//outer loop
        for (int i = 0; i < 3; i++)
        {
            ss >> useless >> x >> y >> z;
            surroundBox[0]=qMin(surroundBox[0],x);
            surroundBox[1]=qMax(surroundBox[1],x);
            surroundBox[2]=qMin(surroundBox[2],y);
            surroundBox[3]=qMax(surroundBox[3],y);
            surroundBox[4]=qMin(surroundBox[4],z);
            surroundBox[5]=qMax(surroundBox[5],z);
            pointList.push_back(Point3f(x, y, z));
        }
        unTriangles++;
        getline(ss, useless);//空行
        getline(ss, useless);//end loop
        getline(ss, useless);//end facet
    } while (1);
    return true;
}

bool ReadSTLFile::ReadBinary(const char *buffer)
{
    const char* p = buffer;
    char name[80];
    float x,y,z;
    memcpy(name, p, 80);//80字节文件头
    //cout<<name<<endl;
    p += 80;
    unTriangles= cpyint(p);//4字节三角面片个数
    //提取第一个面片，得到最大最小值初始值
    pointList.push_back(Point3f(cpyfloat(p), cpyfloat(p), cpyfloat(p)));//法向量
    for (int j = 0; j < 3; j++)//读取三顶点
    {
        x=cpyfloat(p);y=cpyfloat(p);z=cpyfloat(p);
        pointList.push_back(Point3f(x, y, z));
    }
    surroundBox[0]=qMin(pointList.at(0).x,pointList.at(1).x);
    surroundBox[1]=qMax(pointList.at(0).x,pointList.at(1).x);
    surroundBox[2]=qMin(pointList.at(0).y,pointList.at(1).y);
    surroundBox[3]=qMax(pointList.at(0).y,pointList.at(1).y);
    surroundBox[4]=qMin(pointList.at(0).z,pointList.at(1).z);
    surroundBox[5]=qMax(pointList.at(0).z,pointList.at(1).z);
    p += 2;//跳过尾部标志

    for (int i = 1; i<unTriangles; i++)
    {
        pointList.push_back(Point3f(cpyfloat(p), cpyfloat(p), cpyfloat(p)));//法向量
        for (int j = 0; j < 3; j++)//读取三顶点
        {
            x=cpyfloat(p);y=cpyfloat(p);z=cpyfloat(p);
            surroundBox[0]=qMin(surroundBox[0],x);
            surroundBox[1]=qMax(surroundBox[1],x);
            surroundBox[2]=qMin(surroundBox[2],y);
            surroundBox[3]=qMax(surroundBox[3],y);
            surroundBox[4]=qMin(surroundBox[4],z);
            surroundBox[5]=qMax(surroundBox[5],z);
            pointList.push_back(Point3f(x, y, z));
        }
        p += 2;//跳过尾部标志
    }
    return true;
}

int ReadSTLFile::NumTri()
{
    return unTriangles;
    cout<<unTriangles<<endl;
}

vector<Point3f>& ReadSTLFile::PointList()
{
    return pointList;
}

int ReadSTLFile::cpyint(const char*& p)
{
    int cpy;
    memwriter = (char*)&cpy;
    memcpy(memwriter, p, 4);
    p += 4;
    return cpy;
}
float ReadSTLFile::cpyfloat(const char*& p)
{
    float cpy;
    memwriter = (char*)&cpy;
    memcpy(memwriter, p, 4);
        p += 4;
    return cpy;
}
