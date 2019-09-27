project "assimp"
    kind "StaticLib"
    language "C++"
    
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"code/**.h",
		"code/**.cpp",
		"code/**.hpp",
		"include/assimp/**.h",
		"include/assimp/**.cpp",
		"include/assimp/**.hpp"
	}

    includedirs
    {
        "include/assimp",
		"include",
		"contrib/irrXML",
		"contrib/zlib",
		"contrib/rapidjson/include",
		"contrib/openddlparser/include",
		""
    }
    
	filter "system:windows"
        systemversion "latest"
        staticruntime "On"

		defines
		{
			"ASSIMP_BUILD_NO_C4D_IMPORTER"		,
			"ASSIMP_BUILD_NO_X_IMPORTER"        ,
			"ASSIMP_BUILD_NO_AMF_IMPORTER"      ,
			"ASSIMP_BUILD_NO_3DS_IMPORTER"      ,
			"ASSIMP_BUILD_NO_MD3_IMPORTER"      ,
			"ASSIMP_BUILD_NO_MDL_IMPORTER"      ,
			"ASSIMP_BUILD_NO_MD2_IMPORTER"      ,
			"ASSIMP_BUILD_NO_PLY_IMPORTER"      ,
			"ASSIMP_BUILD_NO_ASE_IMPORTER"      ,
			--"ASSIMP_BUILD_NO_OBJ_IMPORTER"      ,
			"ASSIMP_BUILD_NO_HMP_IMPORTER"      ,
			"ASSIMP_BUILD_NO_SMD_IMPORTER"      ,
			"ASSIMP_BUILD_NO_MDC_IMPORTER"      ,
			"ASSIMP_BUILD_NO_MD5_IMPORTER"      ,
			"ASSIMP_BUILD_NO_STL_IMPORTER"      ,
			"ASSIMP_BUILD_NO_LWO_IMPORTER"      ,
			"ASSIMP_BUILD_NO_DXF_IMPORTER"      ,
			"ASSIMP_BUILD_NO_NFF_IMPORTER"      ,
			"ASSIMP_BUILD_NO_RAW_IMPORTER"      ,
			"ASSIMP_BUILD_NO_SIB_IMPORTER"      ,
			"ASSIMP_BUILD_NO_OFF_IMPORTER"      ,
			"ASSIMP_BUILD_NO_AC_IMPORTER"       ,
			"ASSIMP_BUILD_NO_BVH_IMPORTER"      ,
			"ASSIMP_BUILD_NO_IRRMESH_IMPORTER"  ,
			"ASSIMP_BUILD_NO_IRR_IMPORTER"      ,
			"ASSIMP_BUILD_NO_Q3D_IMPORTER"      ,
			"ASSIMP_BUILD_NO_B3D_IMPORTER"      ,
			"ASSIMP_BUILD_NO_COLLADA_IMPORTER"  ,
			"ASSIMP_BUILD_NO_TERRAGEN_IMPORTER" ,
			"ASSIMP_BUILD_NO_CSM_IMPORTER"      ,
			"ASSIMP_BUILD_NO_3D_IMPORTER"       ,
			"ASSIMP_BUILD_NO_LWS_IMPORTER"      ,
			"ASSIMP_BUILD_NO_OGRE_IMPORTER"     ,
			"ASSIMP_BUILD_NO_OPENGEX_IMPORTER"  ,
			"ASSIMP_BUILD_NO_MS3D_IMPORTER"     ,
			"ASSIMP_BUILD_NO_COB_IMPORTER"      ,
			"ASSIMP_BUILD_NO_BLEND_IMPORTER"    ,
			"ASSIMP_BUILD_NO_Q3BSP_IMPORTER"    ,
			"ASSIMP_BUILD_NO_NDO_IMPORTER"      ,
			"ASSIMP_BUILD_NO_IFC_IMPORTER"      ,
			"ASSIMP_BUILD_NO_XGL_IMPORTER"      ,
			--"ASSIMP_BUILD_NO_FBX_IMPORTER"      ,
			"ASSIMP_BUILD_NO_ASSBIN_IMPORTER"   ,
			"ASSIMP_BUILD_NO_GLTF_IMPORTER"     ,
			"ASSIMP_BUILD_NO_C4D_IMPORTER"      ,
			"ASSIMP_BUILD_NO_3MF_IMPORTER"      ,
			"ASSIMP_BUILD_NO_X3D_IMPORTER"      ,
			"ASSIMP_BUILD_NO_EXPORT",
			--"ZLIB_WINAPI",
			"MINIZ_NO_ZLIB_COMPATIBLE_NAMES",
			"ASSIMP_BUILD_NO_OWN_ZLIB"
		}
		
    --filter { "system:windows", "configurations:Release" }
    --    buildoptions "/MT"

	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"
