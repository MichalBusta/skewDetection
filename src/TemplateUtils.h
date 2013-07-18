/*
 * TemplateUtils.h
 *
 *  Created on: Jul 18, 2013
 *      Author: Michal Busta
 */

#ifndef TEMPLATEUTILS_H_
#define TEMPLATEUTILS_H_

#include <string>

namespace cmp
{

/**
 * @class cmp::TemplateUtils
 * 
 * @brief TODO brief description
 *
 * TODO type description
 */
class TemplateUtils
{
public:
	TemplateUtils();
	virtual ~TemplateUtils();

	static void CopyIndexTemplates( std::string currentDir, std::string dstDir );

	static void CopyDetectorTemplates( std::string currentDir, std::string dstDir );
};

} /* namespace cmp */
#endif /* TEMPLATEUTILS_H_ */
