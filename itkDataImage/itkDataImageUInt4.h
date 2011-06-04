#ifndef _itk_DataImageUInt4_h_
#define _itk_DataImageUInt4_h_

#include "itkDataImage.h"
#include "itkDataImagePluginExport.h"

extern const char itkDataImageUInt4Name[] = "itkDataImageUInt4";
typedef ITKDATAIMAGEPLUGIN_EXPORT itkDataImage<4,unsigned int,itkDataImageUInt4Name> itkDataImageUInt4;

#endif
