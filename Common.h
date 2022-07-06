#ifndef common_h
#define common_h

#define VER_SOFT_METR "01.00\0"
#define VER_SOFT_MAIN "01.00\0"

#pragma section="METER_REGION"
#define csizeofmeter (unsigned int)__section_size("METER_REGION")
#endif