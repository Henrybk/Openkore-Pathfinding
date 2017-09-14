#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

#include "mylib/mylib.h"

#include "const-c.inc"

MODULE = Mytest3		PACKAGE = Mytest3		

INCLUDE: const-xs.inc

int
do_path(rawMap, width, height, startX, startY, endX, endY)
	SV *rawMap
	unsigned long width
	unsigned long height
	unsigned long startX
	unsigned long startY
	unsigned long endX
	unsigned long endY
	
	INIT:
		STRLEN len;
		unsigned char *c_rawMap, *data;
	
	CODE:
		if (!SvOK (rawMap))
			XSRETURN_UNDEF;

		c_rawMap = (unsigned char *) SvPV (rawMap, len);
		if ((int) len != width * height)
			XSRETURN_UNDEF;
		
		New (0, data, len, unsigned char);
		Copy (c_rawMap, data, len, unsigned char);
		
		Map* currentMap = GenerateMap(data, width, height);
		
		main(currentMap, startX, startY, endX, endY);
		RETVAL = 1;
	
	OUTPUT:
		RETVAL

		
SV *
makeDistMap(rawMap, width, height)
	SV *rawMap
	unsigned long width
	unsigned long height
	
	INIT:
		STRLEN len;
		unsigned char *c_rawMap, *data;
	
	CODE:
		if (!SvOK (rawMap))
			XSRETURN_UNDEF;

		c_rawMap = (unsigned char *) SvPV (rawMap, len);
		if ((int) len != width * height)
			XSRETURN_UNDEF;
		
		New (0, data, len, unsigned char);
		Copy (c_rawMap, data, len, unsigned char);
		
		unsigned char *result;
		
		result = printinfo(data, width, height);
		
		RETVAL = newSVpv ((const char *) result, (width * height));
	
	OUTPUT:
		RETVAL