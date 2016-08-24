// Copyright 2015 Tailgo

#ifndef __OBJ_LOADER_H__
#define __OBJ_LOADER_H__

#include "Include.h"

namespace tg // tg => tailgo
{
    typedef struct
    {
        std::vector<unsigned int>   indices;
        std::vector<float>          positions;
        std::vector<float>          normals;
        std::vector<float>          texcoords;
        std::vector<int>            material_ids;
    } mesh_r;

    typedef struct
    {
        std::string name;

        float ambient[3];
        float diffuse[3];
        float specular[3];
        float transmittance[3];
        float emission[3];
        float shininess;
        float ior;
        float dissolve;
        int illum;

        std::string ambient_texname;
        std::string diffuse_texname;
        std::string specular_texname;
        std::string normal_texname;
        std::map<std::string, std::string> unknown_parameter;
    } material_r;

    //加载的面的信息
    typedef struct
    {
        std::string name;
        mesh_r       mesh;
    } shape_r;

    class MaterialReader
    {
    public:
        MaterialReader(){}
        virtual ~MaterialReader(){}
        virtual std::string operator() (const std::string& matId, std::vector<material_r>& materials, std::map<std::string, int>& matMap) = 0;
    };

    class MaterialFileReader : public MaterialReader
    {
    public:
        MaterialFileReader(const std::string& mtl_basepath) : m_mtlBasePath(mtl_basepath) {}
        virtual ~MaterialFileReader() {}
        virtual std::string operator() (
            const std::string& matId,
            std::vector<material_r>& materials,
            std::map<std::string,
            int>& matMap);
    private:
        std::string m_mtlBasePath;
    };

    std::string LoadObj(
        std::vector<shape_r>& shapes,
        std::vector<material_r>& materials,
        const char* filename,
        const char* mtl_basepath = NULL);

    std::string LoadObj(std::vector<shape_r>& shapes,
        std::vector<material_r>& materials,
        std::istream& inStream,
        MaterialReader& readMatFn);
}

#endif