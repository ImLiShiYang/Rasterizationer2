#include "material.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

Material::Material(const std::string& filename, const std::string& root, const std::string& materialName)
{
	name = materialName;  // ������Ĳ������Ƹ�ֵ����Ա����name

	std::ifstream mtl;  // ����һ�������ļ�������mtl
	mtl.open(root + filename, std::ifstream::in);  // ���ļ����ļ�·��Ϊ��Ŀ¼�����ļ���
	if (mtl.fail())  // ������ļ�ʧ��
	{
		std::cerr << "Cannot open:" << filename << std::endl;  // �ڴ���������д�ӡ������Ϣ
		return;  // ���أ���������
	}

	std::string mtlLine;  // ����һ���ַ����������洢���ļ��ж�ȡ��ÿһ��
	std::vector<std::string> mtlDescribe;  // ����һ���ַ��������������洢�������ʵ�������
	bool isWrite = false;  // ����һ������ֵ����ʼΪfalse��������ʾ�Ƿ��ҵ���ƥ��Ĳ���
	while (!mtl.eof())  // ��û�е����ļ�ĩβʱ
	{
		std::getline(mtl, mtlLine);  // ʹ��getline�������ļ��ж�ȡһ�У��洢��mtlLine��

		if (mtlLine.compare(0, 7, "newmtl ") == 0)  // ���������"newmtl "��ʼ����ʾ����һ���²��ʵĿ�ʼ��
			if (mtlLine.compare(7, materialName.size(), materialName) == 0)  // ������еĲ�������������Ҫ�ҵĲ���������ͬ
				isWrite = true;  // ����isWriteΪtrue����ʾ�����Ѿ��ҵ���ƥ��Ĳ���

		if (isWrite)  // ��������ҵ���ƥ��Ĳ���
		{
			if (mtlLine.size() == 0)
				break;  // �������Ϊ�գ������ѭ��
			mtlDescribe.push_back(mtlLine);  // ���򣬰Ѹ�����ӵ�mtlDescribe��
		}
	}
	for (auto& s : mtlDescribe)
	{
		std::istringstream iss(s);
		std::string header;
		char trash;
		if (s.compare(0, 3, "Ns ") == 0)
		{
			iss >> header;
			iss >> Ns;
		}
		else if (s.compare(0, 3, "Ke ") == 0)
		{
			iss >> header;
			iss >> Ke.x >> Ke.y >> Ke.z;
		}
		else if (s.compare(0, 3, "Ka ") == 0)
		{
			iss >> header;
			iss >> Ka.x >> Ka.y >> Ka.z;
		}
		else if (s.compare(0, 3, "Kd ") == 0)
		{
			iss >> header;
			iss >> Kd.x >> Kd.y >> Kd.z;
		}
		else if (s.compare(0, 3, "Ks ") == 0)
		{
			iss >> header;
			iss >> Ks.x >> Ks.y >> Ks.z;
		}
		else if (s.compare(0, 2, "d ") == 0)
		{
			iss >> header;
			iss >> d;
		}
		else if (s.compare(0, 3, "Ni ") == 0)
		{
			iss >> header;
			iss >> Ni;
		}
		else if (s.compare(0, 6, "illum ") == 0)
		{
			iss >> header;
			iss >> illum;
		}
		else if (s.compare(0, 7, "map_Ke ") == 0)
		{
			std::string textureName;
			iss >> header;
			iss >> textureName;
			TGAImage map = TGAImage();
			map.read_tga_file(root + textureName);
			map.flip_vertically();
			map_Ke = generateMipmap(map);
			//map_Ke.write_tga_file(root + "/read_result/" + textureName + ".tga");
		}
		else if (s.compare(0, 7, "map_Ka ") == 0)
		{
			std::string textureName;
			iss >> header;
			iss >> textureName;
			TGAImage map = TGAImage();
			map.read_tga_file(root + textureName);
			map.flip_vertically();
			map_Ka = generateMipmap(map);
			//map_Ka.write_tga_file(root + "/read_result/" + textureName + ".tga");
		}
		else if (s.compare(0, 7, "map_Kd ") == 0)
		{
			std::string textureName;
			iss >> header;
			iss >> textureName;
			TGAImage map = TGAImage();
			map.read_tga_file(root + textureName);
			map.flip_vertically();
			map_Kd = generateMipmap(map);
			//map_Kd.write_tga_file(root + "/read_result/" + textureName + ".tga");
		}
		else if (s.compare(0, 7, "map_Ks ") == 0)
		{
			std::string textureName;
			iss >> header;
			iss >> textureName;
			TGAImage map = TGAImage();
			map.read_tga_file(root + textureName);
			map.flip_vertically();
			map_Ks = generateMipmap(map);
			//map_Ks.write_tga_file(root + "/read_result/" + textureName + ".tga");
		}
		else if (s.compare(0, 7, "map_Ns ") == 0)
		{
			std::string textureName;
			iss >> header;
			iss >> textureName;
			TGAImage map = TGAImage();
			map.read_tga_file(root + textureName);
			map.flip_vertically();
			map_Ns = generateMipmap(map);
			//map_Ns.write_tga_file(root + "/read_result/" + textureName + ".tga");
		}
		else if (s.compare(0, 6, "map_d ") == 0)
		{
			std::string textureName;
			iss >> header;
			iss >> textureName;
			TGAImage map = TGAImage();
			map.read_tga_file(root + textureName);
			map.flip_vertically();
			map_d = generateMipmap(map);
			//map_d.write_tga_file(root + "/read_result/" + textureName + ".tga");
		}
		else if (s.compare(0, 9, "map_Bump ") == 0)
		{
			std::string textureName;
			iss >> header;
			iss >> textureName;
			TGAImage map = TGAImage();
			map.read_tga_file(root + textureName);
			map.flip_vertically();
			map_Bump = generateMipmap(map);
			//map_Bump.write_tga_file(root + "/read_result/" + textureName + ".tga");
		}
	}

}


