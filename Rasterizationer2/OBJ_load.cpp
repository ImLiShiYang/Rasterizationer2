#include "OBJ_load.h"
#include <fstream>
#include <sstream>
#include <iostream>


Model::Model(const std::string& root, const std::string& filename)
{
    // 打开文件
    std::ifstream obj;
    std::string mtlfilename;
    obj.open(root + filename, std::ifstream::in);
    if (obj.fail())
    {
        // 如果文件打不开，则返回错误信息并退出函数
        std::cerr << "Cannot open:" << filename << std::endl;
        return;
    }

    std::string objLine;
    while (std::getline(obj, objLine))
    {
        // 用读取的行创建字符串流
        std::istringstream s(objLine);

        std::string header;
        char trash;
        int cur_obj = objects.size() - 1;
        int cur_mesh;
        if (!objects.empty())
            cur_mesh = objects[cur_obj].meshes.size() - 1;
        
        //读取材质库文件名
        if (objLine.compare(0, 7, "mtllib ")==0)
        {
            s >> header;
            s >> mtlfilename;
        }
        //读取对象
        else if(objLine.compare(0,2,"o ") == 0)
        {
            s >> header >> header;
            objects.emplace_back(Object());
        }
        //读取顶点
        else if (objLine.compare(0, 2, "v ") == 0)
        {
            if (objects.empty())
            {
                objects.push_back(Object());
            }
            float x, y, z;
            s >> header;
            s >> x >> y >> z;
            verts.emplace_back(glm::vec3(x, y, z));
        }
        //读取纹理坐标
        else if (objLine.compare(0, 3, "vt ") == 0)
        {
            float x, y;
            s >> header;
            s >> x >> y;
            texcoords.emplace_back(glm::vec2(x, y));
        }
        //读取法线
        else if (objLine.compare(0, 3, "vn ") == 0)
        {
            float x, y, z;
            s >> header;
            s >> x >> y >> z;
            normals.emplace_back(glm::vec3(x, y, z));
        }
        // 设置当前使用的材质
        else if (objLine.compare(0, 7, "usemtl ") == 0)
        {
            std::string materialName;
            s >> header;
            s >> materialName;
            Mesh mesh(mtlfilename, root, materialName);
            objects[cur_obj].meshes.push_back(mesh);
        }
        //读取面
        else if (objLine.compare(0, 2, "f ") == 0)
        {
            s >> header;
            int v[3], vt[3], vn[3];
            for (int i = 0; i < 3; i++)
                s >> v[i] >> trash >> vt[i] >> trash >> vn[i];
            Vertex vertex[3]
            {
                Vertex(verts[v[0] - 1], TGAColor(255, 255, 255), normals[vn[0] - 1], texcoords[vt[0] - 1]),
                Vertex(verts[v[1] - 1], TGAColor(255, 255, 255), normals[vn[1] - 1], texcoords[vt[1] - 1]),
                Vertex(verts[v[2] - 1], TGAColor(255, 255, 255), normals[vn[2] - 1], texcoords[vt[2] - 1])
            };
            // 将三个顶点组成的三角形添加到模型里
            objects[cur_obj].meshes[cur_mesh].primitives.push_back(Triangle(vertex));
        }
    }
    // 关闭文件
    obj.close();
}
