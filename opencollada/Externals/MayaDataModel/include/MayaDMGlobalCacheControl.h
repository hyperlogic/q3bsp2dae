/*
    Copyright (c) 2008-2009 NetAllied Systems GmbH

    This file is part of MayaDataModel.

    Licensed under the MIT Open Source License,
    for details please see LICENSE file or the website
    http://www.opensource.org/licenses/mit-license.php
*/
#ifndef __MayaDM_GLOBALCACHECONTROL_H__
#define __MayaDM_GLOBALCACHECONTROL_H__
#include "MayaDMTypes.h"
#include "MayaDMConnectables.h"
#include "MayaDMDependNode.h"
namespace MayaDM
{
class GlobalCacheControl : public DependNode
{
public:
public:

	GlobalCacheControl():DependNode(){}
	GlobalCacheControl(FILE* file,const std::string& name,const std::string& parent="",bool shared=false,bool create=true)
		:DependNode(file, name, parent, "globalCacheControl", shared, create){}
	virtual ~GlobalCacheControl(){}

	void setEnableStatus(unsigned int ebls)
	{
		if(ebls == 0) return;
		fprintf(mFile,"\tsetAttr \".ebls\" %i;\n", ebls);
	}
	void setWriteEnable(bool webl)
	{
		if(webl == false) return;
		fprintf(mFile,"\tsetAttr \".webl\" %i;\n", webl);
	}
	void getEnableStatus()const
	{
		fprintf(mFile,"\"%s.ebls\"",mName.c_str());
	}
	void getWriteEnable()const
	{
		fprintf(mFile,"\"%s.webl\"",mName.c_str());
	}
protected:
	GlobalCacheControl(FILE* file,const std::string& name,const std::string& parent,const std::string& nodeType,bool shared=false,bool create=true)
		:DependNode(file, name, parent, nodeType, shared, create) {}

};
}//namespace MayaDM
#endif//__MayaDM_GLOBALCACHECONTROL_H__
