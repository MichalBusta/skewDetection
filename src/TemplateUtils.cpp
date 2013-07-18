/*
 * TemplateUtils.cpp
 *
 *  Created on: Jul 18, 2013
 *      Author: Michal Busta
 */

#include "TemplateUtils.h"
#include "IOUtils.h"

namespace cmp
{

TemplateUtils::TemplateUtils()
{
	// TODO Auto-generated constructor stub

}

TemplateUtils::~TemplateUtils()
{
	// TODO Auto-generated destructor stub
}

/**
 * Copy templates of index page to dst directory
 *
 * @param currentDir
 * @param dstDir
 */
void TemplateUtils::CopyIndexTemplates(std::string currentDir,
		std::string dstDir)
{
	std::string templatesDir = 	currentDir;
	templatesDir += "/templates";

	std::string index = templatesDir + "/index.htm";
	std::string indexDst = dstDir;
	indexDst += "/index.htm";
	IOUtils::CopyFile( index, indexDst );

	std::string jsDir = dstDir;
	jsDir += "/js";
	IOUtils::CreateDir( jsDir );

	std::string chartsSrc = templatesDir;
	chartsSrc += "/js/highcharts.js";
	std::string chartsDst = dstDir;
	chartsDst += "/js/highcharts.js";
	IOUtils::CopyFile( chartsSrc, chartsDst );

	chartsSrc = templatesDir;
	chartsSrc += "/js/jquery.min.js";
	chartsDst = dstDir;
	chartsDst += "/js/jquery.min.js";
	IOUtils::CopyFile( chartsSrc, chartsDst );

}

} /* namespace cmp */
