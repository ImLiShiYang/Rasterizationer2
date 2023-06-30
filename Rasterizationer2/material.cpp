#include "material.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

Material::Material(const std::string& filename, const std::string& root, const std::string& materialName)
{
	name = materialName;  // 将传入的材质名称赋值给成员变量name

	std::ifstream mtl;  // 定义一个输入文件流对象mtl
	mtl.open(root + filename, std::ifstream::in);  // 打开文件，文件路径为根目录加上文件名
	if (mtl.fail())  // 如果打开文件失败
	{
		std::cerr << "Cannot open:" << filename << std::endl;  // 在错误输出流中打印错误信息
		return;  // 返回，结束函数
	}

	std::string mtlLine;  // 定义一个字符串，用来存储从文件中读取的每一行
	std::vector<std::string> mtlDescribe;  // 定义一个字符串向量，用来存储描述材质的所有行
	bool isWrite = false;  // 定义一个布尔值，初始为false，用来表示是否找到了匹配的材质
	while (!mtl.eof())  // 当没有到达文件末尾时
	{
		std::getline(mtl, mtlLine);  // 使用getline函数从文件中读取一行，存储到mtlLine中

		if (mtlLine.compare(0, 7, "newmtl ") == 0)  // 如果该行以"newmtl "开始（表示这是一个新材质的开始）
			if (mtlLine.compare(7, materialName.size(), materialName) == 0)  // 如果该行的材质名称与我们要找的材质名称相同
				isWrite = true;  // 设置isWrite为true，表示我们已经找到了匹配的材质

		if (isWrite)  // 如果我们找到了匹配的材质
		{
			if (mtlLine.size() == 0)
				break;  // 如果该行为空，则结束循环
			mtlDescribe.push_back(mtlLine);  // 否则，把该行添加到mtlDescribe中
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


