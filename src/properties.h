#ifndef PROPERTIES_H
#define PROPERTIES_H

// TODO:

typedef uint32_t property;
//typedef uint32_t attrubute;

// THINK OF THIS AS BITS SO LIKE if last 4 are 0000 for empty 0001 is burnable 0010 is smth else 0011 is smth else and burnable
#define PROPERTY_EMPTY 0x0000
#define PROPERTY_BURNABLE 0x0001
#define PROPERTY_PLANTABLE 0x0010
#define PROPERTY_PLACEHOLDER2 0x0004


#endif
