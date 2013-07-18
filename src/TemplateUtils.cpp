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
	IOUtils::CpFile( index, indexDst );

	std::string jsDir = dstDir;
	jsDir += "/js";
	IOUtils::CreateDir( jsDir );

	std::string chartsSrc = templatesDir;
	chartsSrc += "/js/highcharts.js";
	std::string chartsDst = dstDir;
	chartsDst += "/js/highcharts.js";
	IOUtils::CpFile( chartsSrc, chartsDst );

	chartsSrc = templatesDir;
	chartsSrc += "/js/jquery.min.js";
	chartsDst = dstDir;
	chartsDst += "/js/jquery.min.js";
	IOUtils::CpFile( chartsSrc, chartsDst );

}

void TemplateUtils::CopyDetectorTemplates(std::string currentDir,
		std::string dstDir)
{
	std::string templatesDir = 	currentDir;
	templatesDir += "/templates/detector";

	std::string index = templatesDir + "/detector.html";
	std::string indexDst = dstDir;
	indexDst += "/index.htm";
	IOUtils::CpFile( index, indexDst );

	//copy js
	index = templatesDir + "/detector.js";
	indexDst = dstDir;
	indexDst += "/detector.js";
	IOUtils::CpFile( index, indexDst );

	index = templatesDir + "/jit.js";
	indexDst = dstDir;
	indexDst += "/jit.js";
	IOUtils::CpFile( index, indexDst );

	index = templatesDir + "/excanvas.js";
	indexDst = dstDir;
	indexDst += "/excanvas.js";
	IOUtils::CpFile( index, indexDst );

	std::string css = dstDir;
	css += "/css";
	IOUtils::CreateDir( css );

	index = templatesDir + "/css/base.css";
	indexDst = css;
	indexDst += "/base.css";
	IOUtils::CpFile( index, indexDst );

	index = templatesDir + "/css/Sunburst.css";
	indexDst = css;
	indexDst += "/Sunburst.css";
	IOUtils::CpFile( index, indexDst );
}

} /* namespace cmp */


