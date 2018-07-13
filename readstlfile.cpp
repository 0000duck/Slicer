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
    cfilename="F:/QT/Slicer/data/EiffelTower.stl";
    qDebug()<<cfilename<<endl;
    FILE * pFile;
    long lSize;
    char* buffer;
    size_t result;

    /* 若要一个byte不漏地读入整个文件，只能采用二进制方式打开 */
    fopen_s(&pFile,cfilename, "rb");
    if (pFile == NULL)
    {
        fputs("File error", stderr);
        exit(1);
    }

    /* 获取文件大小 */
    fseek(pFile, 0, SEEK_END);
    lSize = ftell(pFile);
    rewind(pFile);

    /* 分配内存存储整个文件 */
    buffer = (char*)malloc(sizeof(char)*lSize);
    if (buffer == NULL)
    {
        fputs("Memory error", stderr);
        exit(2);
    }

    /* 将文件拷贝到buffer中 */
    result = fread(buffer, 1, lSize, pFile);
    if (result != lSize)
    {
        fputs("Reading error", stderr);
        exit(3);
    }


    /* 结束演示，关闭文件并释放内存 */
    fclose(pFile);

    ios::sync_with_stdio(false);
    if (buffer[79]!='\0')//判断格式
    {
        ReadASCII(buffer);
    }
    else
    {
        ReadBinary(buffer);
    }
    ios::sync_with_stdio(true);

    free(buffer);
    return true;
}

bool ReadSTLFile::ReadASCII(const char *buffer)
{
    unTriangles = 0;
    float x, y, z;
    int i;
    string name, useless;
    stringstream ss(buffer);
    ss >> name >> name;
    ss.get();
    do {
        ss >> useless;
        if (useless != "facet")
            break;
        getline(ss, useless);
        getline(ss, useless);
        for (i = 0; i < 3; i++)
        {
            ss >> useless >> x >> y >> z;
            pointList.push_back(Point3f(x, y, z));
        }
        unTriangles++;
        getline(ss, useless);
        getline(ss, useless);
        getline(ss, useless);
    } while (1);
    return true;
}

bool ReadSTLFile::ReadBinary(const char *buffer)
{
    const char* p = buffer;
    char name[80];
    int i, j;
    memcpy(name, p, 80);
    p += 80;
    unTriangles= cpyint(p);
    for (i = 0; i < unTriangles; i++)
    {
        p += 12;//跳过头部法向量
        for (j = 0; j < 3; j++)//读取三顶点
            pointList.push_back(Point3f(cpyfloat(p), cpyfloat(p), cpyfloat(p)));
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
