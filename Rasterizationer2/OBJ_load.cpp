#include "OBJ_load.h"
#include <fstream>
#include <sstream>
#include <iostream>


Model::Model(const std::string& root, const std::string& filename)
{
    // ���ļ�
    std::ifstream obj;
    std::string mtlfilename;
    obj.open(root + filename, std::ifstream::in);
    if (obj.fail())
    {
        // ����ļ��򲻿����򷵻ش�����Ϣ���˳�����
        std::cerr << "Cannot open:" << filename << std::endl;
        return;
    }

    std::string objLine;
    while (std::getline(obj, objLine))
    {
        // �ö�ȡ���д����ַ�����
        std::istringstream s(objLine);

        std::string header;
        char trash;
        int cur_obj = objects.size() - 1;
        int cur_mesh;
        if (!objects.empty())
            cur_mesh = objects[cur_obj].meshes.size() - 1;
        
        //��ȡ���ʿ��ļ���
        if (objLine.compare(0, 7, "mtllib ")==0)
        {
            s >> header;
            s >> mtlfilename;
        }
        //��ȡ����
        else if(objLine.compare(0,2,"o ") == 0)
        {
            s >> header >> header;
            objects.emplace_back(Object());
        }
        //��ȡ����
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
        //��ȡ��������
        else if (objLine.compare(0, 3, "vt ") == 0)
        {
            float x, y;
            s >> header;
            s >> x >> y;
            texcoords.emplace_back(glm::vec2(x, y));
        }
        //��ȡ����
        else if (objLine.compare(0, 3, "vn ") == 0)
        {
            float x, y, z;
            s >> header;
            s >> x >> y >> z;
            normals.emplace_back(glm::vec3(x, y, z));
        }
        // ���õ�ǰʹ�õĲ���
        else if (objLine.compare(0, 7, "usemtl ") == 0)
        {
            std::string materialName;
            s >> header;
            s >> materialName;
            Mesh mesh(mtlfilename, root, materialName);
            objects[cur_obj].meshes.push_back(mesh);
        }
        //��ȡ��
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
            // ������������ɵ���������ӵ�ģ����
            objects[cur_obj].meshes[cur_mesh].primitives.push_back(Triangle(vertex));
        }
    }
    // �ر��ļ�
    obj.close();
}
